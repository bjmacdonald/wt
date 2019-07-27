# activate conane (if necessary)
#
#set PostgreSQL_ROOT=c:\app\pgsql
#
#rd /s /q c:\Y-Work\build\wt
#md c:\Y-Work\build\wt
#cd c:\Y-Work\build\wt
#conan install ..\..\wt
#cmake -Ax64 -G"Visual Studio 16 2019" -Thost=x64 -DCMAKE_INSTALL_PREFIX=c:\app\local\wt ..\..\wt
#cmake --build . --config Debug
#cmake --build . --config Release
CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

PROJECT(Wt VERSION 4.0.5)

# Debug Properties...
set(REPORT_UNDEFINED_PROPERTIES "cmake-undefined-properties.txt")

# BMD Conan Setup
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
include(CapitisDefaults)

SET(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE true)
SET(WT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
SET(WT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

SET(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${WT_SOURCE_DIR}/cmake
)

SET(VERSION_SERIES 4)
SET(VERSION_MAJOR 1)
SET(VERSION_MINOR 0)


# Options
OPTION(WT_BUILD_EXAMPLES "Build examples" OFF)
OPTION(WT_BUILD_TESTS "Build Wt tests" OFF)
OPTION(WT_BUILD_LIBWTTEST "Build Wt::Test" ON)
OPTION(WT_BUILD_LIBWTDBO "Build Wt::Dbo" OFF)

# Note: This is a special CMake variable that changes the default for add_library()
option(BUILD_SHARED_LIBS "Compile shared libraries" OFF)
if(BUILD_SHARED_LIBS)
  message("*** Building Shared Libraries")
else()
  message("*** Building Static Libraries")
endif(BUILD_SHARED_LIBS)

OPTION(WT_INSTALL_DOCUMENTATION "Install documentation (tutorials and reference)" OFF)
OPTION(WT_INSTALL_EXAMPLES "Install examples (binaries and source)" OFF)
OPTION(WT_INSTALL_RESOURCES "Install resources directory" ON)

# HARU is required for WPdfImage.
OPTION(WT_ENABLE_HARU "Enable Haru Free PDF Library, which is used to provide support for painting to PDF (WPdfImage)" OFF)
IF(WT_ENABLE_HARU)
  find_package(Haru REQUIRED)
  set(WT_HAS_WPDFIMAGE true)
ENDIF(WT_ENABLE_HARU)

OPTION(WT_ENABLE_SQLITE "Build SQLite3 backend for Wt::Dbo" OFF)
OPTION(WT_ENABLE_POSTGRES "Build PostgreSQL backend for Wt::Dbo" OFF)

if (WT_ENABLE_POSTGRES)
  find_package(PostgreSQL)
endif()

OPTION(WT_DEBUG "Support for debugging, must be enabled also in wt_config.xml" OFF)
IF(WT_DEBUG)
  SET(WT_DEBUG_ENABLED 1)
ELSE(WT_DEBUG)
  SET(WT_DEBUG_ENABLED 0)
ENDIF(WT_DEBUG)

OPTION(WT_CONNECTOR_HTTP "Compile in stand-alone httpd connector (libwthttp) ?" ON)


#
# External Dependencies
#
find_package(Boost REQUIRED CONFIG)
find_package(OpenSSL REQUIRED)
set(WT_WITH_SSL ON)
find_package(ZLIB REQUIRED CONFIG)

# Raster Image BackEnd
# Pango and SKIA are used on Linux, DirectWrite and Direct2D on Windows (built-in).
IF (WIN32 AND NOT MINGW)
  SET(WT_WRASTERIMAGE_IMPLEMENTATION "Direct2D")
ELSE (WIN32 AND NOT MINGW)
  SET(WT_WRASTERIMAGE_IMPLEMENTATION "skia")
  find_package(skia REQUIRED)
  find_package(pango REQUIRED)
  SET(HAVE_PANGO ON)
ENDIF (WIN32 AND NOT MINGW)
SET(WT_HAS_WRASTERIMAGE true)

# OpenGL
INCLUDE(cmake/WtFindGL.txt)
IF(GL_FOUND)
  message("** OpenGL configured")
  SET(WT_USE_OPENGL TRUE)
  SET(HAVE_GL TRUE)
ELSE()
  message("!!! OpenGL missing, skipping support") 
ENDIF(GL_FOUND)

# Sockets, etc.
INCLUDE(cmake/WtFindSystemLibraries.txt)

# Threads
MESSAGE("** Enabling multi threading.")
find_package(Threads REQUIRED)
set(WT_THREAD_LIB Threads::Threads)
SET(MULTI_THREADED_BUILD true)
SET(WT_THREADED true)
ADD_DEFINITIONS(-D_REENTRANT -DBOOST_SPIRIT_THREADSAFE)

# C++17
SET(WT_ANY_IS_STD_ANY ON)
SET(WT_ASIO_IS_BOOST_ASIO ON)

#
# Destination Folders
#
SET(WT_DOCUMENTATION_DESTINATION share/doc/Wt)
SET(WT_EXAMPLES_DESTINATION lib/Wt/examples)

IF(WIN32)
  SET(RUNDIR "") # Does not apply to win32

  IF(NOT DEFINED CONFIGDIR)
    SET(CONFIGDIR "${CMAKE_INSTALL_PREFIX}/etc" CACHE STRING "Path for the configuration files")
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
SET(EXAMPLES_CONNECTOR wthttp CACHE STRING "Connector used for examples")

#
# Build Projects
#
add_subdirectory(src)

IF(WT_BUILD_EXAMPLES)
IF(WIN32)
    add_subdirectory(examples)
ELSE(WIN32)
    IF (WT_INSTALL_EXAMPLES)
      add_subdirectory(examples)
    ELSE (WT_INSTALL_EXAMPLES)
      add_subdirectory(EXCLUDE_FROM_ALL examples)
    ENDIF (WT_INSTALL_EXAMPLES)
ENDIF(WIN32)
ENDIF(WT_BUILD_EXAMPLES)

IF(WT_BUILD_TESTS)
  add_subdirectory(test)
ENDIF(WT_BUILD_TESTS)


#
# Documentation
#
FIND_PACKAGE(Doxygen)

IF(INSTALL_DOCUMENTATION)
  ADD_CUSTOM_TARGET(doc ALL)
ELSE(INSTALL_DOCUMENTATION)
  ADD_CUSTOM_TARGET(doc)
ENDIF(INSTALL_DOCUMENTATION)

IF (ASCIIDOC_FOUND)
  MACRO (ASCIIDOC_FILE target infile outfile)
    ADD_CUSTOM_TARGET(${target}
       ${ASCIIDOC_EXECUTABLE} -a toc -a numbered -a icons -a  theme=emweb -a pygments -a linkcss -o ${outfile} ${infile}
       COMMENT "Asciidoc ${infile}")
    ADD_DEPENDENCIES(doc ${target})
  ENDMACRO (ASCIIDOC_FILE)
ENDIF (ASCIIDOC_FOUND)

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


#
# Installation
#

IF(INSTALL_RESOURCES)
INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/resources DESTINATION
    ${CMAKE_INSTALL_PREFIX}/share/Wt/)
ENDIF(INSTALL_RESOURCES)

IF(INSTALL_EXAMPLES)
  INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/examples/ DESTINATION
      ${CMAKE_INSTALL_PREFIX}/${EXAMPLES_DESTINATION} PATTERN "examples/*")
ENDIF(INSTALL_EXAMPLES)

IF(INSTALL_DOCUMENTATION)
  INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/doc/reference/html DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${WT_DOCUMENTATION_DESTINATION}/reference)
  INSTALL(FILES ${PROJECT_SOURCE_DIR}/doc/reference/wt.qch DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${WT_DOCUMENTATION_DESTINATION}/reference)
  INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/doc/examples/html DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${WT_DOCUMENTATION_DESTINATION}/examples)
  INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/doc/tutorial DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${WT_DOCUMENTATION_DESTINATION})
ENDIF(INSTALL_DOCUMENTATION)

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

INSTALL_FILES(/include/Wt FILES ${WCONFIG_H_PATH})

# Generate wt_config.xml from wt_config.xml.in
CONFIGURE_FILE(
  ${WT_SOURCE_DIR}/wt_config.xml.in
  ${WT_BINARY_DIR}/wt_config.xml
)

# CMake package configuration
SET(_WTCONFIG_CMAKE_FIND_BOOST TRUE)
CONFIGURE_FILE(
  ${WT_SOURCE_DIR}/wt-config.cmake.in
  ${WT_BINARY_DIR}/wt-config.cmake
  @ONLY
)
UNSET(_WTCONFIG_CMAKE_FIND_BOOST)
INSTALL(FILES
  ${WT_BINARY_DIR}/wt-config.cmake
  DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake)

INSTALL(EXPORT "${PROJECT_NAME}-targets"
        DESTINATION ${CMAKE_INSTALL_PREFIX}/cmake
        NAMESPACE Wt::)

export(EXPORT "${PROJECT_NAME}-targets" NAMESPACE Wt:: FILE ${CMAKE_BINARY_DIR}/wt-export.cmake)
