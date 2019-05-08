IF(CONNECTOR_HTTP)
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
    Android.C
    Configuration.C
    Connection.C
    ConnectionManager.C
    HTTPRequest.C
    MimeTypes.C
    ProxyReply.C
    Reply.C
    Request.C
    RequestHandler.C
    RequestParser.C
    Server.C
    SessionProcess.C
    SessionProcessManager.C
    SslConnection.C
    StaticReply.C
    StockReply.C
    TcpConnection.C
    WServer.C
    WtReply.C
  )

  target_link_libraries(wthttp
    PUBLIC
      wt
      Boost::program_options
    PRIVATE
      Threads::Threads
      ${WT_SOCKET_LIBRARY}
  )    

  IF (HAVE_SSL)
    target_link_libraries(wthttp PRIVATE OpenSSL::api)
    target_compile_definitions(wthttp PRIVATE HTTP_WITH_SSL)
  ENDIF (HAVE_SSL)

  # ZLib
  target_link_libraries(wthttp PUBLIC ZLIB::ZLIB)
  target_compile_definitions(wthttp PUBLIC WTHTTP_WITH_ZLIB)

  #INCLUDE_DIRECTORIES(
    #${BOOST_INCLUDE_DIRS}
    #${CMAKE_CURRENT_SOURCE_DIR}/../web
    #${CMAKE_CURRENT_SOURCE_DIR}/../wt
    #${CMAKE_CURRENT_BINARY_DIR}/
  #)

  INSTALL(TARGETS wthttp
    EXPORT wt-target-http
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR})

  INSTALL(EXPORT wt-target-http
          DESTINATION ${CMAKE_INSTALL_DIR}/wt
          NAMESPACE Wt::)

  SET_TARGET_PROPERTIES(
    wthttp
  PROPERTIES
    EXPORT_NAME HTTP
    VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
    SOVERSION ${WTHTTP_SOVERSION}
    DEBUG_POSTFIX ${DEBUG_LIB_POSTFIX}
  )

  IF(MSVC)
    SET_TARGET_PROPERTIES(
        wthttp
      PROPERTIES
        COMPILE_FLAGS "${BUILD_PARALLEL} /wd4251 /wd4275 /wd4355 /wd4800 /wd4996 /wd4101 /wd4267 /wd4267"
    )
  ENDIF(MSVC)

  ADD_DEPENDENCIES(wthttp wt ${EXAMPLES_CONNECTOR})

ELSE(CONNECTOR_HTTP)
  MESSAGE("** Disabling built-in httpd.")
ENDIF(CONNECTOR_HTTP)
