IF(NOT WT_CONNECTOR_HTTP)
  MESSAGE("** Disabling built-in httpd.")
  return()
endif()

MESSAGE("** Enabling built-in httpd.")

# The target
ADD_LIBRARY(wthttp)

# Wt uses strstrcase for case insensitive ASCII string
# comparisons on Linux for performance reasons. This function is however
# not supported on many platforms (windows, solaris)
INCLUDE(CheckFunctionExists)
CHECK_FUNCTION_EXISTS(strcasestr HAVE_STRCASESTR)
IF (HAVE_STRCASESTR)
  target_compile_definitions(wthttp PRIVATE HAVE_STRCASESTR)
ENDIF (HAVE_STRCASESTR)

CHECK_FUNCTION_EXISTS(strcasecmp HAVE_STRCASECMP)
IF (HAVE_STRCASECMP)
  target_compile_definitions(wthttp PRIVATE HAVE_STRCASECMP)
ENDIF (HAVE_STRCASECMP)

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

target_link_libraries(wthttp
  PUBLIC
    wt
    Boost::program_options
  PRIVATE
    Threads::Threads
    ${WT_SOCKET_LIBRARY}
)

target_compile_definitions(wthttp PRIVATE WT_BUILDING)
target_compile_definitions(wthttp PRIVATE _WIN32_WINNT=0x0A00)

# SSL
target_link_libraries(wthttp PRIVATE OpenSSL::api)
target_compile_definitions(wthttp PRIVATE HTTP_WITH_SSL)

# ZLib
target_link_libraries(wthttp PUBLIC ZLIB::ZLIB)
target_compile_definitions(wthttp PUBLIC WTHTTP_WITH_ZLIB)

SET_TARGET_PROPERTIES(
  wthttp
PROPERTIES
  EXPORT_NAME HTTP
  VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
  SOVERSION ${WTHTTP_SOVERSION}
)

#IF(MSVC)
#  SET_TARGET_PROPERTIES(
#      wthttp
#    PROPERTIES
#      COMPILE_FLAGS "${BUILD_PARALLEL} /wd4251 /wd4275 /wd4355 /wd4800 /wd4996 /wd4101 /wd4267 /wd4267"
#  )
#ENDIF(MSVC)


INSTALL(TARGETS wthttp
  EXPORT "${PROJECT_NAME}-targets"
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION ${LIB_INSTALL_DIR}
  ARCHIVE DESTINATION ${LIB_INSTALL_DIR})
