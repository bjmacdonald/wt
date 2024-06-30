IF(NOT CONNECTOR_HTTP)
  MESSAGE("** Disabling built-in httpd.")
  return()
endif()

MESSAGE("** Enabling built-in httpd.")

add_library(wthttp)
target_compile_definitions(wthttp PRIVATE WT_BUILDING)

target_sources(wthttp PRIVATE
    Android.h Android.C
    Configuration.h Configuration.C
    Connection.h Connection.C
    ConnectionManager.h ConnectionManager.C
    HTTPRequest.h HTTPRequest.C
    MimeTypes.h MimeTypes.C
    ProxyReply.h ProxyReply.C
    Reply.h Reply.C
    Request.h Request.C
    RequestHandler.h RequestHandler.C
    RequestParser.h RequestParser.C
    Server.h Server.C
    SessionProcess.h SessionProcess.C
    SessionProcessManager.h SessionProcessManager.C
    SslConnection.h SslConnection.C
    StaticReply.h StaticReply.C
    StockReply.h StockReply.C
    TcpConnection.h TcpConnection.C
    WServer.C
    WtReply.h WtReply.C
)

# Wt uses strstrcase for case insensitive ASCII string comparisons on Linux for performance reasons. This function is however
# not supported on many platforms (windows, solaris).
# BMD This is ignored for Windows and only applies to Request.C
if(NOT WIN32)
  if(NOT WIN32)
    INCLUDE(CheckFunctionExists)
    set(WT_REQUEST_C_DEFS "")
    foreach(_STRFN strcasestr strcasecmp strncasecmp)
      CHECK_FUNCTION_EXISTS(${_STRFN} _HAS_STRFN)
      if(_HAS_STRFN)
        string(TOUPPER ${_STRFN} _STRFNU)
        list(APPEND WT_REQUEST_C_DEFS "WT_HAVE_${_STRFNU}")
      endif()
    endforeach()
  ENDIF()
  set_source_files_properties(Request.C PROPERTIES COMPILE_DEFINITIONS ${WT_REQUEST_C_DEFS})
endif()

# Boost
target_link_libraries(wthttp PRIVATE Boost::date_time Boost::filesystem Boost::program_options Boost::thread)

# SSL
IF(WT_WITH_SSL)
  target_link_libraries(wthttp PRIVATE OpenSSL::SSL)
ELSE(WT_WITH_SSL)
  MESSAGE("** Disabling crypto support (Auth::SHA1HashFunction, HTTPS support): requires OpenSSL.")
ENDIF(WT_WITH_SSL)


# ZLib (find_package in top level CMakeLists.txt)
IF(ZLIB_FOUND)
  target_link_libraries(wthttp PRIVATE ZLIB::ZLIB)
  target_compile_definitions(wthttp PUBLIC WTHTTP_WITH_ZLIB)
ENDIF()

if(NOT APPLE OR SHARED_LIBS)
  SET_PROPERTY(TARGET wthttp PROPERTY C_VISIBILITY_PRESET hidden)
  SET_PROPERTY(TARGET wthttp PROPERTY CXX_VISIBILITY_PRESET hidden)
  SET_PROPERTY(TARGET wthttp PROPERTY VISIBILITY_INLINES_HIDDEN YES)
endif()

TARGET_LINK_LIBRARIES(wthttp PUBLIC wt)
#TARGET_LINK_LIBRARIES(wthttp PRIVATE ${WT_SOCKET_LIBRARY})


IF(WIN32 AND SHARED_LIBS)
  CONFIGURE_FILE(wthttp-version.rc.in
                 ${CMAKE_CURRENT_BINARY_DIR}/wthttp-version.rc)
  target_sources(wthttp ${CMAKE_CURRENT_BINARY_DIR}/wthttp-version.rc)
ENDIF()


SET_TARGET_PROPERTIES(
  wthttp
PROPERTIES
  EXPORT_NAME HTTP
  VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
)

IF(MSVC)
  SET_TARGET_PROPERTIES(
      wthttp
    PROPERTIES
      COMPILE_FLAGS "${BUILD_PARALLEL} /wd4251 /wd4275 /wd4355 /wd4800 /wd4996 /wd4101 /wd4267 /wd4267"
  )
ENDIF(MSVC)

ADD_DEPENDENCIES(wthttp wt ${EXAMPLES_CONNECTOR})

INSTALL(TARGETS wthttp EXPORT wt-targets)
