CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

# consistent handling of OPTION vars, removes need for if(not option_var) ... option() etc.
cmake_policy(VERSION 3.20)
#cmake_policy(SET CMP0077 NEW)
include(CMakePrintHelpers)

PROJECT(WT LANGUAGES C CXX)

# Was min 14, now set to 17 and use std::any
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

#
# Version Settings
#
SET(VERSION_SERIES 4)
SET(VERSION_MAJOR 5)
SET(VERSION_MINOR 0)

# Renamed and reorganised options, added WT prefix to all.
# Removed DEBUG_LIB_POSTFIX and use global CMAKE_DEBUG_POSTFIX.
# Removed unused vars, or vars with no effect:
#   APPLE USE_BOOST_FRAMEWORK
#   WT_WITH_OLD_INTERNALPATH_API, ditto.
#   BUILD_SHARED_LIBS, ditto
#   DEBUG (set WT_DEBUG_ENABLED directly)
#   USERLIB_ROOT, USERLIB_PREFIX, BOOST_PREFIX_DEFAULT
#   WT_NO_BOOST_INTRUSIVE
#   WT_NO_STD_LOCALE
#   WT_NO_STD_WSTRING
#   HAVE_SSL -> changed to WT_ENABLE_SSL in CMakeLists.txt files
#   HAVE_PANGO -> changed to WT_ENABLE_PANGO in CMakeLists.txt files
#   HAVE_UNWIND -> changed to WT_ENABLE_UNWIND in CMakeLists.txt files
#   MULTI_THREADED_BUILD -> changed to WT_MULTI_THREADED in CMakeLists.txt files
#   HAVE_HARU -> changed to WT_ENABLE_HARU in CMakeLists.txt files
# Removed support for Mingw, Cygwin, QT4
# Removed support for server-side OpenGL. All modern browsers have WebGL suport.
# Defined support libraries, expecially OpenSSL & Boost are required, removed "not found" logic
# Removed MT, etc logic from Boost.  Always link to static multi-threaded.
# Always use Boost asio.
# Always multi-threaded build.
# set *_STATIC in WConfig.h.in using WT_SHARED_LIBS directly (for "Windows declspec stuff")
# Expliticly link targets to support libraries, for above reason.
# Threads::Threads search not necessary, defined as dependency in Boost::threads
# WT_THREAD_LIB not necessary

# src/CMakeLists.txt
# 
# Removed unused vars, or vars with no effect:
#   HAVE_PDF_IMAGE
# Link in non-minfied versions of JS for Debug builds.


#
# Global CMAKE settings
#

#IF(NOT CMAKE_BUILD_TYPE)
#  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
#      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
#      FORCE)
#ENDIF(NOT CMAKE_BUILD_TYPE)


#########################################################################################
#  Configure with Conan
#
if (CMAKE_BUILD_TYPE)
  set(CONAN_BUILD_TYPES ${CMAKE_BUILD_TYPE})
  message(STATUS "Single Config CMAKE_BUILD_TYPE ${CONAN_BUILD_TYPES}")
else()
  set(CONAN_BUILD_TYPES ${CMAKE_CONFIGURATION_TYPES})
  message(STATUS "Multi Config CMAKE_CONFIGURATION_TYPES ${CONAN_BUILD_TYPES}")
endif()

find_program(CONAN_CMD conan)
if(NOT CONAN_CMD)
    message(FATAL_ERROR "Conan executable not found! Please install conan.")
endif()

file(TO_NATIVE_PATH "${CONAN_CMD}" CONAN_CMD)
file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}" CONAN_SOURCE_FOLDER)
file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}" CONAN_INSTALL_FOLDER)

foreach(CONAN_BUILD_TYPE IN LISTS CONAN_BUILD_TYPES)
    set(CONAN_ARGS "install")
    list(APPEND CONAN_ARGS "-if=${CONAN_INSTALL_FOLDER}")
    list(APPEND CONAN_ARGS "--build" "outdated")
    list(APPEND CONAN_ARGS "-s" "build_type=${CONAN_BUILD_TYPE}")
    list(APPEND CONAN_ARGS "${CONAN_SOURCE_FOLDER}")

    execute_process(COMMAND ${CONAN_CMD} ${CONAN_ARGS}
                    COMMAND_ECHO STDOUT
                    RESULT_VARIABLE return_code
                    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

    if(NOT "${return_code}" STREQUAL "0")
        message(FATAL_ERROR "Conan install failed='${return_code}'")
    endif()
endforeach()

# Add conan paths
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

# grab Ctl module from ctl-cmake conan package
#include(CtlDefaults)
#
#########################################################################################


SET(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${WT_SOURCE_DIR}
  ${WT_SOURCE_DIR}/cmake
)

SET(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE true)
set(CMAKE_DEBUG_POSTFIX "d")

# standard install paths
include(GNUInstallDirs)

# unconditionally use std::any, we are C++17 or higher.
SET(WT_ANY_IS_STD_ANY ON)

# unconditionally use boost asio and set required api level for windows (min 0x0600)
SET(WT_ASIO_IS_BOOST_ASIO ON)
add_compile_definitions($<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0A00>)

# suppress excess warnings
#add_compile_definitions($<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS>)

# IDE folders, no pdb
if(MSVC)
  # store debug info in objects so no pdb required for static libraries
  foreach(flag_var CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELWITHDEBINFO)
    string(REGEX REPLACE "/Zi" "/Z7" ${flag_var} "${${flag_var}}")
  endforeach()

  # BMD Not needed?
  #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
  SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

  SET(WT_BUILD_PARALLEL "/MP" CACHE STRING "MSVC option for parallel builds (/MP or /MPx)")
ENDIF(MSVC)


#
# Main Build Options
#
IF(WIN32)
  OPTION(WT_SHARED_LIBS "Compile shared libraries" OFF)
ELSE(WIN32)
  OPTION(WT_SHARED_LIBS "Compile shared libraries" ON)
ENDIF(WIN32)

if(WT_SHARED_LIBS)
  SET(CMAKE_C_VISIBILITY_PRESET hidden)
  SET(CMAKE_CXX_VISIBILITY_PRESET hidden)
  SET(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
ENDIF()


OPTION(WT_MULTI_THREADED "Build multi-threaded httpd deamon (if possible)" ON)
OPTION(WT_DEBUG_ENABLED "Support for debugging, must be enabled also in wt_config.xml" OFF)
OPTION(WT_ENABLE_SSL "Enable cryptography functions, using OpenSSL" ON)
OPTION(WT_ENABLE_HARU "Enable Haru Free PDF Library, which is used to provide support for painting to PDF (WPdfImage)" OFF)
OPTION(WT_ENABLE_PANGO "Enable Pango Library, which is used for improved font support (WPdfImage and WRasterImage)" OFF)
OPTION(WT_ENABLE_QT5 "Build Qt5 interworking library (libwtwithqt5)" OFF)
OPTION(WT_ENABLE_SAML "Build built-in SAML service provider for Wt::Auth" OFF)
OPTION(WT_ENABLE_UNWIND "Build Wt with stacktrace support using libunwind" OFF)

IF(WIN32)
  SET(WT_CONNECTOR_FCGI OFF)
  OPTION(WT_CONNECTOR_ISAPI "Compile in ISAPI connector (libwtisapi) ?" OFF)
ELSE(WIN32)
  OPTION(WT_CONNECTOR_FCGI "Compile in FCGI connector (libwtfcgi) ?" OFF)
  SET(WT_CONNECTOR_ISAPI OFF)
ENDIF(WIN32)
OPTION(WT_CONNECTOR_HTTP "Compile in stand-alone httpd connector (libwthttp) ?" ON)


#
# DBO Build Options
#
OPTION(WT_ENABLE_LIBWTDBO "Build Wt::Dbo" ON)
OPTION(WT_ENABLE_SQLITE "Build SQLite3 backend for Wt::Dbo" ON)
OPTION(WT_ENABLE_POSTGRES "Build PostgreSQL backend for Wt::Dbo" OFF)
OPTION(WT_ENABLE_FIREBIRD "Build FirebirdSQL backend for Wt::Dbo" OFF)
OPTION(WT_ENABLE_MYSQL "Build mariadb/mysql backend for Wt::Dbo" OFF)
OPTION(WT_ENABLE_MSSQLSERVER "Build Microsoft SQL Server backend for Wt::Dbo" ON)


#
# Examples
#
OPTION(WT_BUILD_EXAMPLES "Build examples" ON)


#
# Tests
#
OPTION(WT_ENABLE_LIBWTTEST "Build Wt::Test" ON)
OPTION(WT_BUILD_TESTS "Build Wt tests" ON)


#
# Documentation
#



#
# Install Options
#
OPTION(WT_INSTALL_DOCUMENTATION "Install documentation (tutorials and reference)" ON)
IF(NOT WT_DOCUMENTATION_DESTINATION)
  SET(WT_DOCUMENTATION_DESTINATION share/doc/Wt)
ENDIF(NOT WT_DOCUMENTATION_DESTINATION)
IF(WT_INSTALL_DOCUMENTATION)
  INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/doc/reference/html DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${WT_DOCUMENTATION_DESTINATION}/reference)
  INSTALL(FILES ${PROJECT_SOURCE_DIR}/doc/reference/wt.qch DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${WT_DOCUMENTATION_DESTINATION}/reference)
  INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/doc/examples/html DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${WT_DOCUMENTATION_DESTINATION}/examples)
  INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/doc/tutorial DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${WT_DOCUMENTATION_DESTINATION})
ENDIF(WT_INSTALL_DOCUMENTATION)

OPTION(WT_INSTALL_EXAMPLES "Install examples (binaries and source)" OFF)
IF(WT_INSTALL_EXAMPLES)
  INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/examples/ DESTINATION
      ${CMAKE_INSTALL_PREFIX}/${WT_EXAMPLES_DESTINATION} PATTERN "examples/*")
ENDIF(WT_INSTALL_EXAMPLES)

OPTION(WT_INSTALL_RESOURCES "Install resources directory" ON)
IF(WT_INSTALL_RESOURCES)
  INSTALL(DIRECTORY   ${PROJECT_SOURCE_DIR}/resources
          DESTINATION ${CMAKE_INSTALL_PREFIX}/share/Wt/)
ENDIF(WT_INSTALL_RESOURCES)

SET(LIB_INSTALL_DIR "lib" CACHE STRING
  "Name for library directory within ${CMAKE_INSTALL_PREFIX}")

SET(CMAKE_INSTALL_DIR "${LIB_INSTALL_DIR}/cmake" CACHE STRING
  "Name for CMake package configuration directory within ${CMAKE_INSTALL_PREFIX}")



# target_compile_options(libbiogears_cdm PRIVATE $<$<PLATFORM_ID:Windows>:/bigobj>  PRIVATE $<$<PLATFORM_ID:Windows>:/MP${Biogears_BUILD_PARALLEL_LEVEL}>)
# add_definitions() ??



# Boost is used nearly everywhere, so we can put these here
#INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIRS})
#LINK_DIRECTORIES(${BOOST_LIB_DIRS})



#
# Runtime Configuration
#
IF(WIN32)
  SET(RUNDIR "c:/app/wt") # Does not apply to win32

  IF(NOT DEFINED CONFIGDIR)
    SET(CONFIGDIR ${RUNDIR} CACHE STRING "Path for the configuration files")
  ENDIF(NOT DEFINED CONFIGDIR)
ELSE(WIN32)
  IF("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr")
    SET(CONFIG_PREFIX "")
  ELSEIF("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr/local")
    SET(CONFIG_PREFIX "")
  ELSE()
    SET(CONFIG_PREFIX ${CMAKE_INSTALL_PREFIX})
  ENDIF()

  SET(RUNDIR "${CONFIG_PREFIX}/var/run/wt" CACHE PATH
    "Default path for wt session management (only used by FCGI connector)")

  IF(NOT DEFINED CONFIGDIR)
    SET(CONFIGDIR "${CONFIG_PREFIX}/etc/wt" CACHE STRING "Path for the configuration files")
  ENDIF( NOT DEFINED CONFIGDIR )
ENDIF(WIN32)
MARK_AS_ADVANCED( CONFIGDIR )


SET(CONFIGURATION "${CONFIGDIR}/wt_config.xml" CACHE PATH "Path to the wt configuration file")
SET(WTHTTP_CONFIGURATION "${CONFIGDIR}/wthttpd" CACHE PATH "Path for the wthttpd configuration file")

SET(WEBUSER apache CACHE STRING "Webserver username (e.g. apache or www)")
SET(WEBGROUP apache CACHE STRING "Webserver groupname (e.g. apache or www or users)")



#
# Multi Threaded
#
IF(WT_MULTI_THREADED)
  MESSAGE("** Enabling multi threading.")
  SET(WT_THREADED ON)
  add_compile_definitions(_REENTRANT BOOST_SPIRIT_THREADSAFE)
ELSE(WT_MULTI_THREADED)
  MESSAGE("** Disabling multi threading.")
  SET(WT_THREADED OFF)
  add_compile_definitions(BOOST_DISABLE_THREADS SQLITE_THREADSAFE=0)
ENDIF(WT_MULTI_THREADED)


#
# Boost, use  static, multi-threaded, min version 1.75.0
#
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.76.0 REQUIRED
             COMPONENTS date_time filesystem program_options thread
             CONFIG)
message("Boost found at ${Boost_DIR}")

#
# SSL
#
IF(WT_ENABLE_SSL)
  find_package(OpenSSL REQUIRED CONFIG)
	message("OpenSSL found at ${OpenSSL_DIR}")
  SET(WT_WITH_SSL ON)
ENDIF(WT_ENABLE_SSL)


#
# SAML
#
IF (WT_ENABLE_SAML)
  find_package(OpenSAML)
  if(OpenSAML_FOUND)
    set(WT_HAS_SAML TRUE)
  endif()
ENDIF (WT_ENABLE_SAML)

#
# ZLib
#
find_package(zlib REQUIRED CONFIG)
if(zlib_FOUND)
  message("ZLIB found at ${zlib_DIR}")
endif()

#
# Haru
#
# ToDo: Haru requires porting libharu which requires libpng as well.
IF(WT_ENABLE_HARU)
  INCLUDE(cmake/WtFindHaru.txt)
  SET(WT_HAS_WPDFIMAGE true)

  IF(WIN32)
    OPTION(HARU_DYNAMIC "Link to libharu DLLs (OFF means static link)" OFF)
    IF(HARU_DYNAMIC)
      add_compile_definitions(HPDF_DLL)
    ENDIF(HARU_DYNAMIC)
  ENDIF(WIN32)
ENDIF(WT_ENABLE_HARU)

#
# Pango
#
# ToDo: Pango has loads of dependencies.  Nightmare.
IF (WT_ENABLE_PANGO)
  INCLUDE(cmake/WtFindPangoFt2.txt)
ENDIF(WT_ENABLE_PANGO)

#
# QT5
#
# This has no effect outside of getting the package.
IF(WT_ENABLE_QT5)
  FIND_PACKAGE(Qt5Core)
ENDIF(WT_ENABLE_QT5)

#
# Raster Image
#
# decide on GraphicsMagick vs skia
# todo: set default to whatever was found
IF (WIN32)
	SET(WT_WRASTERIMAGE_IMPLEMENTATION Direct2D CACHE STRING "Implementation for WRasterImage server-side rendering")
	SET_PROPERTY(CACHE WT_WRASTERIMAGE_IMPLEMENTATION PROPERTY STRINGS GraphicsMagick skia Direct2D none)
ELSE (WIN32)
	SET(WT_WRASTERIMAGE_IMPLEMENTATION none CACHE STRING "Implementation for WRasterImage server-side rendering")
	SET_PROPERTY(CACHE WT_WRASTERIMAGE_IMPLEMENTATION PROPERTY STRINGS GraphicsMagick skia none)
ENDIF (WIN32)

IF (${WT_WRASTERIMAGE_IMPLEMENTATION} STREQUAL "GraphicsMagick")
  #INCLUDE(cmake/WtFindGm.txt)
  IF (NOT GM_FOUND)
    MESSAGE(FATAL_ERROR "WT_WRASTERIMAGE_IMPLEMENTATION set to GraphicsMagick but GM is not found. Indicate the location of your graphicsmagick library using -DGM_PREFIX=...")
  ENDIF (NOT GM_FOUND)
  SET(WT_HAS_WRASTERIMAGE true)
ELSEIF (${WT_WRASTERIMAGE_IMPLEMENTATION} STREQUAL "skia")
  #INCLUDE(cmake/WtFindSkia.txt)
  IF (NOT SKIA_FOUND)
    MESSAGE(FATAL_ERROR "WT_WRASTERIMAGE_IMPLEMENTATION set to skia but skia is not found. Indicate the location of your skia library using -DSKIA_PREFIX=...")
  ENDIF (NOT SKIA_FOUND)
  SET(WT_HAS_WRASTERIMAGE true)
ELSEIF (${WT_WRASTERIMAGE_IMPLEMENTATION} STREQUAL "Direct2D")
  IF (WIN32)
    SET(WT_HAS_WRASTERIMAGE true)
  ENDIF (WIN32)
ELSE (${WT_WRASTERIMAGE_IMPLEMENTATION} STREQUAL "GraphicsMagick")
    MESSAGE("** Disabled WRasterimage support. Set WT_WRASTERIMAGE_IMPLEMENTATION to GraphicsMagick, skia, or Direct2D.")
ENDIF (${WT_WRASTERIMAGE_IMPLEMENTATION} STREQUAL "GraphicsMagick")

#
# Unwind
#
IF (WT_ENABLE_UNWIND)
  INCLUDE(cmake/WtFindUnwind.txt)
ENDIF(WT_ENABLE_UNWIND)


#
# C++20 date or Hinnant date
#
SET(WT_CPP20_DATE_TZ_DEFAULT_IMPLEMENTATION "date")
SET(WT_CPP20_DATE_TZ_IMPLEMENTATION ${WT_CPP20_DATE_TZ_DEFAULT_IMPLEMENTATION} CACHE STRING
  "Implementation for Wt::cpp20::date. Defaults to \"date\": an included implementation of date and timezone support by Howard Hinnant. Use \"std\" for C++20's \"std::chrono\" implementation.")
SET_PROPERTY(CACHE WT_CPP20_DATE_TZ_IMPLEMENTATION PROPERTY STRINGS date std)

IF (${WT_CPP20_DATE_TZ_IMPLEMENTATION} STREQUAL "date")
  SET(WT_DATE_TZ_USE_DATE ON)
ELSEIF(${WT_CPP20_DATE_TZ_IMPLEMENTATION} STREQUAL "std")
  SET(WT_DATE_TZ_USE_STD ON)
ELSE()
  MESSAGE(FATAL_ERROR "WT_CPP20_DATE_TZ_IMPLEMENTATION must be date or std")
ENDIF()



#include (CheckSymbolExists)
#INCLUDE(cmake/WtFindFcgi.txt)
#INCLUDE(cmake/WtFindMysql.txt)
#INCLUDE(cmake/WtFindPostgresql.txt)
#INCLUDE(cmake/WtFindOdbc.txt)
#INCLUDE(cmake/WtFindSystemLibraries.txt)




IF(WT_INSTALL_DOCUMENTATION)
  ADD_CUSTOM_TARGET(doc ALL)
ELSE(WT_INSTALL_DOCUMENTATION)
  #ADD_CUSTOM_TARGET(doc)
ENDIF(WT_INSTALL_DOCUMENTATION)

#INCLUDE(cmake/WtFindAsciidoc.txt)
IF (ASCIIDOC_FOUND)
  MACRO (ASCIIDOC_FILE target infile outfile)
    ADD_CUSTOM_TARGET(${target}
       ${ASCIIDOC_EXECUTABLE} -a toc -a numbered -a icons -a  theme=emweb -a pygments -a linkcss -o ${outfile} ${infile}
       COMMENT "Asciidoc ${infile}")
    ADD_DEPENDENCIES(doc ${target})
  ENDMACRO (ASCIIDOC_FILE)
ENDIF (ASCIIDOC_FOUND)


FIND_PACKAGE(Doxygen)
IF (DOXYGEN_FOUND)
  ADD_CUSTOM_TARGET(doxygen
    ${DOXYGEN_EXECUTABLE}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMENT "Doxygen ...")
  ADD_DEPENDENCIES(doc doxygen)
  ADD_CUSTOM_TARGET(doxygen-examples
    ${DOXYGEN_EXECUTABLE}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/examples
    COMMENT "Doxygen for examples ...")
  ADD_DEPENDENCIES(doc doxygen-examples)
ENDIF (DOXYGEN_FOUND)

# we enable rpath support for APPLE, this probably goes against policy
# linux distributions?
IF(APPLE)
  SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
  IF("${isSystemDir}" STREQUAL "-1")
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  ENDIF("${isSystemDir}" STREQUAL "-1")
  SET(CMAKE_MACOSX_RPATH TRUE)
ENDIF(APPLE)

#
# Include source for build.
#
add_subdirectory(src)
IF(WT_BUILD_TESTS)
  add_subdirectory(test)
ENDIF(WT_BUILD_TESTS)

IF(NOT WT_EXAMPLES_DESTINATION)
  SET(WT_EXAMPLES_DESTINATION lib/Wt/examples)
ENDIF(NOT WT_EXAMPLES_DESTINATION)

SET(EXAMPLES_CONNECTOR wthttp CACHE STRING "Connector used for examples")
IF(WT_BUILD_EXAMPLES)
  IF(WIN32)
    add_subdirectory(examples)
  ELSE(WIN32)
    IF (WT_INSTALL_EXAMPLES)
      add_subdirectory(examples)
    ELSE (WT_INSTALL_EXAMPLES)
      add_subdirectory(examples EXCLUDE_FROM_ALL)
    ENDIF (WT_INSTALL_EXAMPLES)
  ENDIF(WIN32)
ENDIF(WT_BUILD_EXAMPLES)



IF(NOT EXISTS ${CONFIGDIR}/wt_config.xml)
  INSTALL(FILES ${WT_BINARY_DIR}/wt_config.xml DESTINATION ${CONFIGDIR})
ENDIF (NOT EXISTS ${CONFIGDIR}/wt_config.xml)


# Compile time constants & make sure our build finds it
FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Wt)
SET(WCONFIG_H_PATH ${CMAKE_CURRENT_BINARY_DIR}/Wt/WConfig.h)
CONFIGURE_FILE(
  ${WT_SOURCE_DIR}/WConfig.h.in
  ${WCONFIG_H_PATH}
)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INSTALL_FILES(/include/Wt FILES ${WCONFIG_H_PATH})

# Generate wt_config.xml from wt_config.xml.in
CONFIGURE_FILE(
  ${WT_SOURCE_DIR}/wt_config.xml.in
  ${WT_BINARY_DIR}/wt_config.xml
)

# CMake package configuration
IF(${Boost_USE_STATIC_LIBS})
  SET(_WTCONFIG_BOOST_STATIC "ON")
ELSE()
  SET(_WTCONFIG_BOOST_STATIC "OFF")
ENDIF()
IF (${WT_ASIO_IS_BOOST_ASIO})
  SET(_WTCONFIG_CMAKE_FIND_BOOST TRUE)
ELSEIF(Boost_USE_STATIC_LIBS AND NOT WT_SHARED_LIBS)
  SET(_WTCONFIG_CMAKE_FIND_BOOST TRUE)
ELSE()
  SET(_WTCONFIG_CMAKE_FIND_BOOST FALSE)
ENDIF()
CONFIGURE_FILE(
  ${WT_SOURCE_DIR}/wt-config.cmake.in
  ${WT_BINARY_DIR}/wt-config.cmake
  @ONLY
)
UNSET(_WTCONFIG_CMAKE_FIND_BOOST)
UNSET(_WTCONFIG_BOOST_STATIC)
CONFIGURE_FILE(
  ${WT_SOURCE_DIR}/wt-config-version.cmake.in
  ${WT_BINARY_DIR}/wt-config-version.cmake
  @ONLY
)
INSTALL(FILES
  ${WT_BINARY_DIR}/wt-config.cmake
  ${WT_BINARY_DIR}/wt-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_DIR}/wt)


cmake_print_properties(TARGETS wt wthttp wtdbo PROPERTIES INCLUDE_DIRECTORIES INTERFACE_INCLUDE_DIRECTORIES)
