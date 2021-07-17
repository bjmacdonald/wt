CMAKE_MINIMUM_REQUIRED(VERSION 3.20)

# consistent handling of OPTION vars, removes need for if(not option_var) ... option() etc.
cmake_policy(SET CMP0077 NEW)


PROJECT(WT)

#
# Version Settings
#
SET(VERSION_SERIES 4)
SET(VERSION_MAJOR 5)
SET(VERSION_MINOR 0)

# Renamed and reorganised options, added WT prefix to all.
# Removed DEBUG_LIB_POSTFIX and use global CMAKE_DEBUG_POSTFIX.
# Removed APPLE USE_BOOST_FRAMEWORK, this was unused.
# Removed WT_WITH_OLD_INTERNALPATH_API, ditto.
# Removed support for Mingw, Cygwin


#
# Main Build Options
#
OPTION(WT_BUILD_EXAMPLES "Build examples" ON)
OPTION(WT_ENABLE_LIBWTTEST "Build Wt::Test" ON)
OPTION(WT_ENABLE_SSL "Enable cryptography functions, using OpenSSL" ON)
OPTION(WT_ENABLE_HARU "Enable Haru Free PDF Library, which is used to provide support for painting to PDF (WPdfImage)" ON)
OPTION(WT_ENABLE_PANGO "Enable Pango Library, which is used for improved font support (WPdfImage and WRasterImage)" ON)
OPTION(WT_ENABLE_QT4 "Build Qt4 interworking library (libwtwithqt)" OFF)
OPTION(WT_ENABLE_QT5 "Build Qt5 interworking library (libwtwithqt5)" OFF)
OPTION(WT_ENABLE_OPENGL "Build Wt with support for server-side opengl rendering" ON)
OPTION(WT_ENABLE_UNWIND "Build Wt with stacktrace support using libunwind" OFF)
OPTION(WT_NO_STD_LOCALE "Build Wt to run on a system without std::locale support" OFF)
OPTION(WT_NO_STD_WSTRING "Build Wt to run on a system without std::wstring support" OFF)

IF(WIN32)
  OPTION(WT_SHARED_LIBS "Compile shared libraries" OFF)
ELSE(WIN32)
  OPTION(WT_SHARED_LIBS "Compile shared libraries" ON)
ENDIF(WIN32)

#
# DBO Build Options
#
OPTION(WT_ENABLE_LIBWTDBO "Build Wt::Dbo" ON)
OPTION(WT_ENABLE_SQLITE "Build SQLite3 backend for Wt::Dbo" OFF)
OPTION(WT_ENABLE_POSTGRES "Build PostgreSQL backend for Wt::Dbo" OFF)
OPTION(WT_ENABLE_FIREBIRD "Build FirebirdSQL backend for Wt::Dbo" OFF)
OPTION(WT_ENABLE_MYSQL "Build mariadb/mysql backend for Wt::Dbo" OFF)
OPTION(WT_ENABLE_MSSQLSERVER "Build Microsoft SQL Server backend for Wt::Dbo" ON)

#
# Install Options
#
OPTION(WT_INSTALL_DOCUMENTATION "Install documentation (tutorials and reference)" OFF)
OPTION(WT_INSTALL_EXAMPLES "Install examples (binaries and source)" OFF)
OPTION(WT_INSTALL_RESOURCES "Install resources directory" ON)



IF(NOT WT_MULTI_THREADED)
  OPTION(WT_MULTI_THREADED "Build multi-threaded httpd deamon (if possible)" ON)
ENDIF(NOT WT_MULTI_THREADED)

  IF(MSVC)
    SET(BUILD_PARALLEL "/MP" CACHE STRING "MSVC option for parallel builds (/MP or /MPx)")
  ENDIF(MSVC)

OPTION(DEBUG "Support for debugging, must be enabled also in wt_config.xml" OFF)

OPTION(BUILD_TESTS "Build Wt tests" ON)

IF(WIN32)
  SET(BOOST_PREFIX_DEFAULT "c:/Program Files/Boost")
  OPTION(BOOST_DYNAMIC "Link to boost DLLs (OFF means static link)" OFF)
  OPTION(HARU_DYNAMIC "Link to libharu DLLs (OFF means static link)" OFF)
ELSE(WIN32)
  SET(BOOST_PREFIX_DEFAULT ${USERLIB_PREFIX})
ENDIF(WIN32)


#
# Global CMAKE settings
#
SET(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${WT_SOURCE_DIR}
  ${WT_SOURCE_DIR}/cmake
)

SET(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE true)

set(CMAKE_DEBUG_POSTFIX "d")

# Min 14(?), set to 17 to enable xxx
IF(NOT CMAKE_CXX_STANDARD)
  SET(CMAKE_CXX_STANDARD 14)
ENDIF(NOT CMAKE_CXX_STANDARD)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)


if(MSVC)
  # store debug info in objects so no pdb required for static libraries
  foreach(flag_var CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELWITHDEBINFO)
    string(REGEX REPLACE "/Zi" "/Z7" ${flag_var} "${${flag_var}}")
  endforeach()

  # BMD Not needed?
  #SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
  SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
ENDIF(MSVC)

IF(NOT WT_DOCUMENTATION_DESTINATION)
  SET(WT_DOCUMENTATION_DESTINATION share/doc/Wt)
ENDIF(NOT WT_DOCUMENTATION_DESTINATION)
IF(NOT WT_EXAMPLES_DESTINATION)
  SET(WT_EXAMPLES_DESTINATION lib/Wt/examples)
ENDIF(NOT WT_EXAMPLES_DESTINATION)

IF(WIN32)
  # Win10 or greater, min 0x0600 required for all that use boost.asio
  ADD_DEFINITIONS(-D_WIN32_WINNT=0x0A00)

  IF(WT_SHARED_LIBS)
    # See http://svn.boost.org/trac/boost/ticket/3465
    SET(WT_NO_BOOST_INTRUSIVE true)
  ENDIF(WT_SHARED_LIBS)
ENDIF(WIN32)

# Fixup Windows declspec stuff
IF(NOT WT_SHARED_LIBS)
  SET(WT_STATIC true)
  SET(WTHTTP_STATIC true)
  SET(WT_EXT_STATIC true)
  SET(WTDBO_STATIC true)
  SET(WTDBOSQLITE3_STATIC true)
  SET(WTDBOPOSTGRES_STATIC true)
  SET(WTDBOFIREBIRD_STATIC true)
  SET(WTDBOMYSQL_STATIC true)
  SET(WTDBOMSSQLSERVER_STATIC true)
ENDIF(NOT WT_SHARED_LIBS)


SET(BUILD_SHARED_LIBS ${WT_SHARED_LIBS})

SET(LIB_INSTALL_DIR "lib" CACHE STRING
  "Name for library directory within ${CMAKE_INSTALL_PREFIX}")

SET(CMAKE_INSTALL_DIR "${LIB_INSTALL_DIR}/cmake" CACHE STRING
  "Name for CMake package configuration directory within ${CMAKE_INSTALL_PREFIX}")

IF(WIN32)

  SET(RUNDIR "c:/witty") # Does not apply to win32

  IF(NOT DEFINED CONFIGDIR)
    SET(CONFIGDIR ${RUNDIR} CACHE STRING "Path for the configuration files")
  ENDIF(NOT DEFINED CONFIGDIR)

  SET(USERLIB_PREFIX_DEFAULT "c:/libraries")

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

  # If the user specifies the standard CMAKE_PREFIX_PATH to find packages,
  # honour it.
  IF(CMAKE_PREFIX_PATH)
    SET(USERLIB_PREFIX_DEFAULT ${CMAKE_PREFIX_PATH})
  ELSE()
    SET(USERLIB_PREFIX_DEFAULT "/usr")
  ENDIF()

ENDIF(WIN32)

IF(DEFINED USERLIB_ROOT) # Deprecated <= 3.1.3
  SET(USERLIB_PREFIX ${USERLIB_ROOT} CACHE PATH
    "Installation prefix of dependency libraries (by USERLIB_ROOT)")
ELSE(DEFINED USERLIB_ROOT)
  SET(USERLIB_PREFIX ${USERLIB_PREFIX_DEFAULT} CACHE PATH
    "Installation prefix of dependency libraries")
ENDIF(DEFINED USERLIB_ROOT)





MARK_AS_ADVANCED( CONFIGDIR )

SET(CONFIGURATION "${CONFIGDIR}/wt_config.xml" CACHE PATH "Path to the wt configuration file")
SET(WTHTTP_CONFIGURATION "${CONFIGDIR}/wthttpd" CACHE PATH "Path for the wthttpd configuration file")

SET(WEBUSER apache CACHE STRING "Webserver username (e.g. apache or www)")
SET(WEBGROUP apache CACHE STRING "Webserver groupname (e.g. apache or www or users)")
IF(WIN32)
  SET(CONNECTOR_FCGI FALSE)
  OPTION(CONNECTOR_ISAPI "Compile in ISAPI connector (libwtisapi) ?" ON)
ELSE(WIN32)
  OPTION(CONNECTOR_FCGI "Compile in FCGI connector (libwtfcgi) ?" ON)
  SET(CONNECTOR_ISAPI OFF)
ENDIF(WIN32)
OPTION(CONNECTOR_HTTP "Compile in stand-alone httpd connector (libwthttp) ?" ON)
SET(EXAMPLES_CONNECTOR wthttp CACHE STRING "Connector used for examples")

include (CheckSymbolExists)

find_package(PNG)

set(ZLIB_ROOT ${ZLIB_PREFIX})
find_package(ZLIB)

INCLUDE(cmake/WtFindBoost.txt)
INCLUDE(cmake/WtFindFcgi.txt)
INCLUDE(cmake/WtFindMysql.txt)
INCLUDE(cmake/WtFindPostgresql.txt)
INCLUDE(cmake/WtFindOdbc.txt)
INCLUDE(cmake/WtFindAsciidoc.txt)
INCLUDE(cmake/WtFindHaru.txt)
INCLUDE(cmake/WtFindGm.txt)
INCLUDE(cmake/WtFindGL.txt)
INCLUDE(cmake/WtFindSkia.txt)

set(OPENSSL_ROOT_DIR ${SSL_PREFIX})
find_package(OpenSSL)

IF (WT_ENABLE_UNWIND)
  INCLUDE(cmake/WtFindUnwind.txt)
ENDIF (WT_ENABLE_UNWIND)

IF (WT_ENABLE_PANGO)
  INCLUDE(cmake/WtFindPangoFt2.txt)
ENDIF (WT_ENABLE_PANGO)

INCLUDE(cmake/WtFindSystemLibraries.txt)

IF(WT_ENABLE_QT4)
  FIND_PACKAGE(Qt4)

  IF(QT_FOUND)
    INCLUDE(${QT_USE_FILE})
  ENDIF(QT_FOUND)
ENDIF(WT_ENABLE_QT4)

IF(WT_ENABLE_QT5)
  FIND_PACKAGE(Qt5Core)
ENDIF(WT_ENABLE_QT5)

set(WT_THREAD_LIB "")
IF(WT_MULTI_THREADED)
  find_package(Threads)
  set(WT_THREAD_LIB Threads::Threads)
ENDIF(WT_MULTI_THREADED)

IF(NOT BOOST_WT_FOUND)
  SET(ERR
     "Could not find a boost installation in " ${BOOST_PREFIX} ".\n\n"
      "There are two methods in Wt to find boost:\n\n"
      "1. Find boost through cmake (recommended)\n"
      "This requires cmake 2.6, and is in "
      "that case the default (unless BOOST_COMPILER or BOOST_VERSION is "
      "defined). This method requires a multi-threaded boost installation.\n"
      "You may need to add your boost version number to "
      "Boost_ADDITIONAL_VERSIONS, and/or set BOOST_PREFIX to the location "
      "where boost is installed.\n\n"
      "2. Use the Wt-proprietary method to find boost.\n"
      "This requires you to define three variables:\n"
      "BOOST_PREFIX: set to the directory where you installed boost\n"
      "BOOST_COMPILER: set to the compiler signature as you find them in "
      "library names (e.g.: gcc42)\n"
      "BOOST_VERSION: set to the boost version, again as you find them in "
      "the library names (e.g.: 1_38)\n\n"
      "Note 1: the code to discover boost is located in cmake/WtFindBoost.txt\n"
      "Note 2: on windows, set BOOST_PREFIX to the full path, eg "
      "c:/Program Files/boost/boost_1_65_1")
  MESSAGE(FATAL_ERROR ${ERR})
ENDIF(NOT BOOST_WT_FOUND)

IF(BOOST_WT_MT_FOUND)
  IF(WT_MULTI_THREADED)
    MESSAGE("** Enabling multi threading.")
    SET(MULTI_THREADED_BUILD true)
    SET(WT_THREADED true)
    ADD_DEFINITIONS(-D_REENTRANT -DBOOST_SPIRIT_THREADSAFE)
  ELSE(WT_MULTI_THREADED)
    MESSAGE("** Disabling multi threading.")
    SET(MULTI_THREADED_BUILD false)

    ADD_DEFINITIONS(-DBOOST_DISABLE_THREADS -DSQLITE_THREADSAFE=0)
  ENDIF(WT_MULTI_THREADED)
ELSE(BOOST_WT_MT_FOUND)
  SET(MULTI_THREADED_BUILD false)
  MESSAGE("** Disabling multi threading: could not find multi-threaded boost libraries")
  ADD_DEFINITIONS(-DBOOST_DISABLE_THREADS -DSQLITE_THREADSAFE=0)
ENDIF(BOOST_WT_MT_FOUND)

# decide on GraphicsMagick vs skia
# todo: set default to whatever was found
IF (WIN32)
  SET(WT_WRASTERIMAGE_DEFAULT_IMPLEMENTATION "Direct2D")
ELSE (WIN32)
  SET(WT_WRASTERIMAGE_DEFAULT_IMPLEMENTATION "none")
ENDIF (WIN32)

SET(WT_WRASTERIMAGE_IMPLEMENTATION ${WT_WRASTERIMAGE_DEFAULT_IMPLEMENTATION} CACHE STRING "Implementation for WRasterImage server-side rendering")
IF (CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION LESS 8)
  MESSAGE(STATUS "Informational: WT_WRASTERIMAGE_IMPLEMENTATION should be either GraphicsMagick, skia, or Direct2D")
ELSE (CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION LESS 8)
  SET_PROPERTY(CACHE WT_WRASTERIMAGE_IMPLEMENTATION PROPERTY STRINGS GraphicsMagick skia Direct2D none)
ENDIF (CMAKE_MAJOR_VERSION EQUAL 2 AND CMAKE_MINOR_VERSION LESS 8)

IF (${WT_WRASTERIMAGE_IMPLEMENTATION} STREQUAL "GraphicsMagick")
  IF (NOT GM_FOUND)
    MESSAGE(FATAL_ERROR "WT_WRASTERIMAGE_IMPLEMENTATION set to GraphicsMagick but GM is not found. Indicate the location of your graphicsmagick library using -DGM_PREFIX=...")
  ENDIF (NOT GM_FOUND)
  SET(WT_HAS_WRASTERIMAGE true)
ELSEIF (${WT_WRASTERIMAGE_IMPLEMENTATION} STREQUAL "skia")
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

# since cmake 3.7
IF (CMAKE_CXX_STANDARD GREATER_EQUAL 17)
  SET(WT_CPP17_ANY_DEFAULT_IMPLEMENTATION "std")
ELSE()
  SET(WT_CPP17_ANY_DEFAULT_IMPLEMENTATION "thelink2012")
ENDIF()

SET(WT_CPP17_ANY_IMPLEMENTATION ${WT_CPP17_ANY_DEFAULT_IMPLEMENTATION} CACHE STRING
  "Implementation for Wt::cpp17::any. Defaults to \"thelink2012\": an included implementation of any by GitHub user thelink2012. Use \"std\" for std::any, or \"experimental\" for std::experimental::any, if compiler support is available. You may have to change CMAKE_CXX_STANDARD to enable C++17 support in the compiler.")
SET_PROPERTY(CACHE WT_CPP17_ANY_IMPLEMENTATION PROPERTY STRINGS thelink2012 experimental std)

IF (${WT_CPP17_ANY_IMPLEMENTATION} STREQUAL "thelink2012")
  SET(WT_ANY_IS_THELINK2012_ANY ON)
ELSEIF (${WT_CPP17_ANY_IMPLEMENTATION} STREQUAL "experimental")
  SET(WT_ANY_IS_EXPERIMENTAL_ANY ON)
ELSEIF (${WT_CPP17_ANY_IMPLEMENTATION} STREQUAL "std")
  SET(WT_ANY_IS_STD_ANY ON)
ELSE (${WT_CPP17_ANY_IMPLEMENTATION} STREQUAL "thelink2012")
  MESSAGE(FATAL_ERROR "WT_CPP17_ANY_IMPLEMENTATION must be one of thelink2012, experimental, or std")
ENDIF (${WT_CPP17_ANY_IMPLEMENTATION} STREQUAL "thelink2012")

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

SET(WT_ASIO_DEFAULT_IMPLEMENTATION "boost")
SET(WT_ASIO_IMPLEMENTATION ${WT_ASIO_DEFAULT_IMPLEMENTATION} CACHE STRING
"Implementation of Asio to use. Defaults to \"boost\". Use \"standalone\" and provide its path in ASIO_PREFIX to use the standalone version.")
SET_PROPERTY(CACHE WT_ASIO_IMPLEMENTATION PROPERTY STRINGS boost standalone)

IF (${WT_ASIO_IMPLEMENTATION} STREQUAL "boost")
  SET(WT_ASIO_IS_BOOST_ASIO ON)
ELSEIF(${WT_ASIO_IMPLEMENTATION} STREQUAL "standalone")
  SET(WT_ASIO_IS_STANDALONE_ASIO ON)
ELSE(${WT_ASIO_IMPLEMENTATION} STREQUAL "boost")
  MESSAGE(FATAL_ERROR "WT_ASIO_IMPLEMENTATION must be boost or standalone")
ENDIF(${WT_ASIO_IMPLEMENTATION} STREQUAL "boost")

IF(WT_ASIO_IS_STANDALONE_ASIO)
  IF(NOT EXISTS "${ASIO_PREFIX}/include/asio.hpp")
    MESSAGE(FATAL_ERROR "WT_ASIO_IMPLEMENTATION is set to standalone, but Asio was not found in ASIO_PREFIX")
  ENDIF(NOT EXISTS "${ASIO_PREFIX}/include/asio.hpp")
  INCLUDE_DIRECTORIES("${ASIO_PREFIX}/include")
ENDIF(WT_ASIO_IS_STANDALONE_ASIO)


# Boost is used nearly everywhere, so we can put these here
INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIRS})
LINK_DIRECTORIES(${BOOST_LIB_DIRS})
IF(WIN32)
  IF(BOOST_DYNAMIC)
    ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK)
  ELSE(BOOST_DYNAMIC)
    # You could expect that this is the default when BOOST_ALL_DYN_LINK
    # is not set, but this is problematic for cygwin
    ADD_DEFINITIONS(-DBOOST_THREAD_USE_LIB)
  ENDIF(BOOST_DYNAMIC)
  IF(HARU_DYNAMIC)
    ADD_DEFINITIONS(-DHPDF_DLL)
  ENDIF(HARU_DYNAMIC)
  ADD_DEFINITIONS(
      -D_CRT_SECURE_NO_WARNINGS
      -D_SCL_SECURE_NO_WARNINGS
      )
ENDIF(WIN32)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

IF(DEBUG)
  SET(WT_DEBUG_ENABLED 1)
ELSE(DEBUG)
  SET(WT_DEBUG_ENABLED 0)
ENDIF(DEBUG)

IF(WT_INSTALL_DOCUMENTATION)
  ADD_CUSTOM_TARGET(doc ALL)
ELSE(WT_INSTALL_DOCUMENTATION)
  ADD_CUSTOM_TARGET(doc)
ENDIF(WT_INSTALL_DOCUMENTATION)

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

SUBDIRS(src)

IF(WT_BUILD_EXAMPLES)
  IF(WIN32)
    SUBDIRS(examples)
  ELSE(WIN32)
    IF (WT_INSTALL_EXAMPLES)
      SUBDIRS(examples)
    ELSE (WT_INSTALL_EXAMPLES)
      SUBDIRS(EXCLUDE_FROM_ALL examples)
    ENDIF (WT_INSTALL_EXAMPLES)
  ENDIF(WIN32)
ENDIF(WT_BUILD_EXAMPLES)

IF(BUILD_TESTS)
  SUBDIRS(test)
ENDIF(BUILD_TESTS)

IF(WT_INSTALL_RESOURCES)
INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/resources DESTINATION
    ${CMAKE_INSTALL_PREFIX}/share/Wt/)
ENDIF(WT_INSTALL_RESOURCES)

IF(WT_INSTALL_EXAMPLES)
  INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/examples/ DESTINATION
      ${CMAKE_INSTALL_PREFIX}/${WT_EXAMPLES_DESTINATION} PATTERN "examples/*")
ENDIF(WT_INSTALL_EXAMPLES)

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

IF(NOT EXISTS ${CONFIGDIR}/wt_config.xml)
  INSTALL(FILES ${WT_BINARY_DIR}/wt_config.xml DESTINATION ${CONFIGDIR})
ENDIF (NOT EXISTS ${CONFIGDIR}/wt_config.xml)

IF(WT_ENABLE_HARU AND HARU_FOUND)
  SET(HAVE_HARU ON)
  SET(WT_HAS_WPDFIMAGE true)
ENDIF(WT_ENABLE_HARU AND HARU_FOUND)

IF(WT_ENABLE_SSL AND OPENSSL_FOUND)
  SET(HAVE_SSL ON)
  SET(WT_WITH_SSL true)
ENDIF(WT_ENABLE_SSL AND OPENSSL_FOUND)

IF(WT_ENABLE_PANGO AND PANGO_FT2_FOUND)
  SET(HAVE_PANGO ON)
ENDIF(WT_ENABLE_PANGO AND PANGO_FT2_FOUND)

IF(WT_ENABLE_OPENGL AND GL_FOUND AND WT_HAS_WRASTERIMAGE)
  SET(WT_USE_OPENGL TRUE)
  SET(HAVE_GL TRUE)
ENDIF(WT_ENABLE_OPENGL AND GL_FOUND AND WT_HAS_WRASTERIMAGE)

IF(WT_ENABLE_UNWIND AND UNWIND_FOUND)
  SET(HAVE_UNWIND ON)
ENDIF(WT_ENABLE_UNWIND AND UNWIND_FOUND)

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
IF (${WT_ASIO_IMPLEMENTATION} STREQUAL "boost")
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
