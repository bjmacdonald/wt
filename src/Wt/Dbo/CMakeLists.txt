IF(NOT WT_BUILD_LIBWTDBO)
  MESSAGE("** libwtdbo disabled, not building Wt::Dbo.")
  return()
ENDIF(NOT WT_BUILD_LIBWTDBO)

MESSAGE("** Building Wt::Dbo")

IF(WIN32 AND BUILD_SHARED_LIBS)
  CONFIGURE_FILE(wtdbo-version.rc.in
                 ${CMAKE_CURRENT_BINARY_DIR}/wtdbo-version.rc)
  SET(wtdboversionrc ${CMAKE_CURRENT_BINARY_DIR}/wtdbo-version.rc)
ENDIF()

add_library(wtdbo
    ${wtdboversionrc}
    ptr.h ptr_impl.h ptr.C
    Call.h Call_impl.h Call.C
    DbAction.h DbAction_impl.h DbAction.C
    Exception.h Exception.C
    FixedSqlConnectionPool.h FixedSqlConnectionPool.C
    Json.h Json.C
    Query.h Query_impl.h Query.C
    QueryColumn.h QueryColumn.C
    SqlQueryParse.C
    Session.h Session_impl.h Session.C
    SqlConnection.h SqlConnection.C
    SqlConnectionPool.h SqlConnectionPool.C
    SqlStatement.h SqlStatement.C
    SqlTraits.h SqlTraits_impl.h SqlTraits.C
    StdSqlTraits.h StdSqlTraits.C
    Transaction.h Transaction.C
    WtSqlTraits.h WtSqlTraits.C
    EscapeOStream.h EscapeOStream.C
    StringStream.h StringStream.C
)

target_compile_features(wtdbo PUBLIC cxx_std_17)
target_compile_definitions(wtdbo PRIVATE WT_BUILDING)

if (MSVC)
  target_compile_options(wtdbo
    PUBLIC 
      /permissive-
      /Zc:__cplusplus
  )
  target_compile_definitions(wtdbo
    PUBLIC
      _CRT_NONSTDC_NO_WARNINGS # Don't deprecate posix names of functions.
      _CRT_SECURE_NO_WARNINGS # Don't deprecate the non _s versions of various standard library functions, because safety is for chumps.
      _SCL_SECURE_NO_WARNINGS # Don't deprecate the non _s versions of various standard library functions, because safety is for chumps.
    PRIVATE 
      _WIN32_WINNT=0x0A00
  )
endif(MSVC)

target_include_directories(wtdbo PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>)
target_include_directories(wtdbo PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/web>)
target_include_directories(wtdbo PUBLIC $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
target_include_directories(wtdbo INTERFACE $<INSTALL_INTERFACE:include>)

target_link_libraries(wtdbo PUBLIC ${WT_THREAD_LIB} Boost::boost)

set_target_properties(
  wtdbo
PROPERTIES
  FOLDER "dbo"
  EXPORT_NAME Dbo
  VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
)

add_subdirectory(backend)

INSTALL(TARGETS wtdbo
    EXPORT "${PROJECT_NAME}-targets"
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR})

INSTALL_FILES(/include/Wt/Dbo "^[^.]+\\.h$")
INSTALL_FILES(/include/Wt FILES ${WCONFIG_H_PATH})

IF (ASCIIDOC_FOUND)
  ASCIIDOC_FILE(dbo-tutorial
    ${CMAKE_SOURCE_DIR}/doc/tutorial/dbo.doc
    ${CMAKE_SOURCE_DIR}/doc/tutorial/dbo.html)
ENDIF (ASCIIDOC_FOUND)
