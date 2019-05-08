# activate conane (if necessary)
#
# rd /s /q c:\Y-Work\wt-build
# md c:\Y-Work\wt-build
# cd c:\Y-Work\wt-build
# conan install ..\wt
# cmake -G"Visual Studio 15 2017 Win64" -Thost=x64 ..\wt
# cmake -Ax64 -G"Visual Studio 16 2019" -Thost=x64 ..\wt


CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

PROJECT(Wt VERSION 4.0.5)

# BMD Conan Setup
include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
include(CapitisDefaults)

SET(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE true)
SET(WT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
SET(WT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

SET(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${WT_SOURCE_DIR}
  ${WT_SOURCE_DIR}/cmake
)

SET(VERSION_SERIES 4)
SET(VERSION_MAJOR 0)
SET(VERSION_MINOR 5)

SET(WT_SOVERSION 51)
SET(WTHTTP_SOVERSION 51)
SET(WTFCGI_SOVERSION 51)
SET(WTISAPI_SOVERSION 27)
SET(WTDBO_SOVERSION 51)
SET(WTDBOSQLITE3_SOVERSION 51)
SET(WTDBOPOSTGRES_SOVERSION 51)
SET(WTDBOFIREBIRD_SOVERSION 51)
SET(WTDBOMYSQL_SOVERSION 51)
SET(WTDBOMSSQLSERVER_SOVERSION 51)
SET(WTTEST_SOVERSION 21)


# Options
OPTION(BUILD_EXAMPLES "Build examples" OFF)
OPTION(INSTALL_DOCUMENTATION "Install documentation (tutorials and reference)" OFF)
OPTION(INSTALL_EXAMPLES "Install examples (binaries and source)" OFF)
OPTION(INSTALL_RESOURCES "Install resources directory" ON)
OPTION(ENABLE_SSL "Enable cryptography functions, using OpenSSL" ON)
OPTION(ENABLE_HARU "Enable Haru Free PDF Library, which is used to provide support for painting to PDF (WPdfImage)" OFF)
OPTION(ENABLE_PANGO "Enable Pango Library, which is used for improved font support (WPdfImage and WRasterImage)" ON)
OPTION(ENABLE_EXT "Build Wt Ext library with JavaScript-only widgets (http://extjs.com/) DEPRECATED" OFF)
OPTION(ENABLE_SQLITE "Build SQLite3 backend for Wt::Dbo" OFF)
OPTION(ENABLE_POSTGRES "Build PostgreSQL backend for Wt::Dbo" ON)
OPTION(ENABLE_FIREBIRD "Build FirebirdSQL backend for Wt::Dbo" OFF)
OPTION(ENABLE_MYSQL "Build mariadb/mysql backend for Wt::Dbo" OFF)
OPTION(ENABLE_MSSQLSERVER "Build Microsoft SQL Server backend for Wt::Dbo" OFF)
OPTION(ENABLE_QT4 "Build Qt4 interworking library (libwtwithqt" OFF)
OPTION(ENABLE_LIBWTTEST "Build Wt::Test" ON)
OPTION(ENABLE_LIBWTDBO "Build Wt::Dbo" ON)
OPTION(ENABLE_OPENGL "Build Wt with support for server-side opengl rendering" ON)

OPTION(WT_NO_STD_LOCALE "Build Wt to run on a system without std::locale support" OFF)
OPTION(WT_NO_STD_WSTRING "Build Wt to run on a system without std::wstring support" OFF)
OPTION(WT_WARN_HEADER_MISSING_H "Output a warning if a Wt header without .h extension is included (deprecated since Wt 4.0.0)" ON)

OPTION(ENABLE_UNWIND "Build Wt with stacktrace support using libunwind" OFF)
OPTION(MULTI_THREADED "Build multi-threaded httpd deamon (if possible)" ON)
OPTION(SHARED_LIBS "Compile shared libraries" OFF)

OPTION(BOOST_DYNAMIC "Link to boost DLLs (OFF means static link)" OFF)
OPTION(HARU_DYNAMIC "Link to libharu DLLs (OFF means static link)" OFF)

OPTION(DEBUG "Support for debugging, must be enabled also in wt_config.xml" OFF)
OPTION(BUILD_TESTS "Build Wt tests" OFF)

OPTION(CONNECTOR_HTTP "Compile in stand-alone httpd connector (libwthttp) ?" ON)

SET(DOCUMENTATION_DESTINATION share/doc/Wt)
SET(EXAMPLES_DESTINATION lib/Wt/examples)


IF(WIN32)
  # BMD check this.
  # required for all that uses boost.asio
  #ADD_DEFINITIONS(-D_WIN32_WINNT=0x0600)

  IF(SHARED_LIBS)
    # See http://svn.boost.org/trac/boost/ticket/3465
    SET(WT_NO_BOOST_INTRUSIVE true)
  ENDIF(SHARED_LIBS)
ENDIF(WIN32)

IF(WIN32)
  SET(RUNDIR "c:/app/wt") # Does not apply to win32

  IF(NOT DEFINED CONFIGDIR)
    SET(CONFIGDIR "${RUNDIR}/etc" CACHE STRING "Path for the configuration files")
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



# BMD
SET(POSTGRES_PREFIX "c:/app/pgsql" CACHE PATH
  "Installation prefix of postgresql library (overrides USERLIB_PREFIX)")
SET(MYSQL_PREFIX ${USERLIB_PREFIX} CACHE PATH
  "Installation prefix of mariadb client libraries (overrides USERLIB_PREFIX)")
SET(SQLITE3_PREFIX ${USERLIB_PREFIX} CACHE PATH
  "Installation prefix of sqlite3 library (overrides USERLIB_PREFIX)")
SET(HARU_PREFIX ${USERLIB_PREFIX} CACHE PATH
  "Installation prefix of libharu library (overrides USERLIB_PREFIX)")
SET(SSL_PREFIX ${USERLIB_PREFIX} CACHE PATH
  "Installation prefix of SSL library (overrides USERLIB_PREFIX)")
SET(GM_PREFIX ${USERLIB_PREFIX} CACHE PATH
  "Installation prefix of GraphicsMagick library (overrides USERLIB_PREFIX)")
SET(SKIA_PREFIX ${USERLIB_PREFIX} CACHE PATH
  "Prefix of skia library (overrides USERLIB_PREFIX)")
SET(UNWIND_PREFIX ${UNWIND_PREFIX} CACHE PATH
  "Prefix of unwind library (overrides USERLIB_PREFIX)")

MARK_AS_ADVANCED( CONFIGDIR )
SET(CONFIGURATION "${CONFIGDIR}/wt_config.xml" CACHE PATH "Path to the wt configuration file")
SET(WTHTTP_CONFIGURATION "${CONFIGDIR}/wthttpd" CACHE PATH "Path for the wthttpd configuration file")

SET(WEBUSER apache CACHE STRING "Webserver username (e.g. apache or www)")
SET(WEBGROUP apache CACHE STRING "Webserver groupname (e.g. apache or www or users)")
SET(EXAMPLES_CONNECTOR wthttp CACHE STRING "Connector used for examples")

# BMD
set(DEBUG_LIB_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# BMD
find_package(Boost REQUIRED CONFIG)
find_package(OpenSSL REQUIRED)
SET(HAVE_SSL ON)
SET(WT_WITH_SSL true)

find_package(ZLIB REQUIRED CONFIG)

# include (CheckSymbolExists)
# INCLUDE(cmake/WtFindBoost.txt)
# INCLUDE(cmake/WtFindFcgi.txt)
# INCLUDE(cmake/WtFindPng.txt)
# INCLUDE(cmake/WtFindSsl.txt)
# INCLUDE(cmake/WtFindMysql.txt)
# INCLUDE(cmake/WtFindPostgresql.txt)
# INCLUDE(cmake/WtFindOdbc.txt)
# INCLUDE(cmake/WtFindAsciidoc.txt)
# INCLUDE(cmake/WtFindHaru.txt)
# INCLUDE(cmake/WtFindGm.txt)
# INCLUDE(cmake/WtFindGL.txt)
# INCLUDE(cmake/WtFindSkia.txt)

IF (ENABLE_UNWIND)
  INCLUDE(cmake/WtFindUnwind.txt)
ENDIF (ENABLE_UNWIND)

IF (ENABLE_PANGO)
  INCLUDE(cmake/WtFindPangoFt2.txt)
ENDIF (ENABLE_PANGO)

INCLUDE(cmake/WtFindSystemLibraries.txt)

set(WT_THREAD_LIB "")
IF(MULTI_THREADED)
  find_package(Threads)
  set(WT_THREAD_LIB Threads::Threads)
ENDIF(MULTI_THREADED)

MESSAGE("** Enabling multi threading.")
SET(MULTI_THREADED_BUILD true)
SET(WT_THREADED true)
ADD_DEFINITIONS(-D_REENTRANT -DBOOST_SPIRIT_THREADSAFE)

# decide on GraphicsMagick vs skia
# todo: set default to whatever was found
IF (WIN32 AND NOT MINGW)
  SET(WT_WRASTERIMAGE_DEFAULT_IMPLEMENTATION "Direct2D")
ELSE (WIN32 AND NOT MINGW)
  SET(WT_WRASTERIMAGE_DEFAULT_IMPLEMENTATION "none")
ENDIF (WIN32 AND NOT MINGW)

SET(WT_WRASTERIMAGE_IMPLEMENTATION ${WT_WRASTERIMAGE_DEFAULT_IMPLEMENTATION} CACHE STRING "Implementation for WRasterImage server-side rendering")
SET_PROPERTY(CACHE WT_WRASTERIMAGE_IMPLEMENTATION PROPERTY STRINGS GraphicsMagick skia Direct2D none)

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
  IF (WIN32 AND NOT MINGW)
    SET(WT_HAS_WRASTERIMAGE true)
  ENDIF (WIN32 AND NOT MINGW)
ELSE (${WT_WRASTERIMAGE_IMPLEMENTATION} STREQUAL "GraphicsMagick")
    MESSAGE("** Disabled WRasterimage support. Set WT_WRASTERIMAGE_IMPLEMENTATION to GraphicsMagick, skia, or Direct2D.")
ENDIF (${WT_WRASTERIMAGE_IMPLEMENTATION} STREQUAL "GraphicsMagick")


# C++17
SET(WT_ANY_IS_STD_ANY ON)
SET(WT_ASIO_IS_BOOST_ASIO ON)

FIND_PACKAGE(Doxygen)

IF(WIN32)
  IF(HARU_DYNAMIC)
    ADD_DEFINITIONS(-DHPDF_DLL)
  ENDIF(HARU_DYNAMIC)
  ADD_DEFINITIONS(
      -D_CRT_SECURE_NO_WARNINGS
      -D_SCL_SECURE_NO_WARNINGS
      )
ENDIF(WIN32)

IF(DEBUG)
  SET(WT_DEBUG_ENABLED 1)
ELSE(DEBUG)
  SET(WT_DEBUG_ENABLED 0)
ENDIF(DEBUG)

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

add_subdirectory(src)

IF(BUILD_EXAMPLES)
  IF(WIN32)
    add_subdirectory(examples)
  ELSE(WIN32)
    IF (INSTALL_EXAMPLES)
      add_subdirectory(examples)
    ELSE (INSTALL_EXAMPLES)
      add_subdirectory(EXCLUDE_FROM_ALL examples)
    ENDIF (INSTALL_EXAMPLES)
  ENDIF(WIN32)
ENDIF(BUILD_EXAMPLES)

IF(BUILD_TESTS)
  add_subdirectory(test)
ENDIF(BUILD_TESTS)

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
    ${CMAKE_INSTALL_PREFIX}/${DOCUMENTATION_DESTINATION}/reference)
  INSTALL(FILES ${PROJECT_SOURCE_DIR}/doc/reference/wt.qch DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${DOCUMENTATION_DESTINATION}/reference)
  INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/doc/examples/html DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${DOCUMENTATION_DESTINATION}/examples)
  INSTALL(DIRECTORY ${PROJECT_SOURCE_DIR}/doc/tutorial DESTINATION
    ${CMAKE_INSTALL_PREFIX}/${DOCUMENTATION_DESTINATION})
ENDIF(INSTALL_DOCUMENTATION)

IF(NOT EXISTS ${CONFIGDIR}/wt_config.xml)
  INSTALL(FILES ${WT_BINARY_DIR}/wt_config.xml DESTINATION ${CONFIGDIR})
ENDIF (NOT EXISTS ${CONFIGDIR}/wt_config.xml)

IF(ENABLE_HARU AND HARU_FOUND)
  SET(HAVE_HARU ON)
  SET(WT_HAS_WPDFIMAGE true)
ENDIF(ENABLE_HARU AND HARU_FOUND)


IF(ENABLE_PANGO AND PANGO_FT2_FOUND)
  SET(HAVE_PANGO ON)
ENDIF(ENABLE_PANGO AND PANGO_FT2_FOUND)

IF(ENABLE_OPENGL AND GL_FOUND AND WT_HAS_WRASTERIMAGE)
  SET(WT_USE_OPENGL TRUE)
  SET(HAVE_GL TRUE)
ENDIF(ENABLE_OPENGL AND GL_FOUND AND WT_HAS_WRASTERIMAGE)

IF(ENABLE_UNWIND AND UNWIND_FOUND)
  SET(HAVE_UNWIND ON)
ENDIF(ENABLE_UNWIND AND UNWIND_FOUND)

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
SET(_WTCONFIG_CMAKE_FIND_BOOST TRUE)
CONFIGURE_FILE(
  ${WT_SOURCE_DIR}/wt-config.cmake.in
  ${WT_BINARY_DIR}/wt-config.cmake
  @ONLY
)
UNSET(_WTCONFIG_CMAKE_FIND_BOOST)
INSTALL(FILES
  ${WT_BINARY_DIR}/wt-config.cmake
  DESTINATION ${CMAKE_INSTALL_DIR}/wt)
