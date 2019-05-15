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
  ptr.C
  Call.C
  DbAction.C
  Exception.C
  FixedSqlConnectionPool.C
  Json.C
  Query.C
  QueryColumn.C
  SqlQueryParse.C
  Session.C
  SqlConnection.C
  SqlConnectionPool.C
  SqlStatement.C
  SqlTraits.C
  StdSqlTraits.C
  Transaction.C
  WtSqlTraits.C
  EscapeOStream.C
  StringStream.C
)

target_include_directories(wtdbo PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>)
target_include_directories(wtdbo PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/web>)
target_include_directories(wtdbo PUBLIC $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)
message("Wt::DBo Current Binary Dir ${CMAKE_CURRENT_BINARY_DIR}")
message("Wt::DBo Binary Dir ${CMAKE_BINARY_DIR}")
message("Wt::DBo Current Source Dir ${CMAKE_CURRENT_SOURCE_DIR}")
message("Wt::DBo Source Dir ${CMAKE_SOURCE_DIR}")
target_include_directories(wtdbo INTERFACE $<INSTALL_INTERFACE:include>)
target_compile_features(wtdbo PUBLIC cxx_std_17)
target_compile_definitions(wtdbo PRIVATE WT_BUILDING)
target_compile_definitions(wtdbo PRIVATE _WIN32_WINNT=0x0A00)

target_link_libraries(wtdbo PUBLIC ${WT_THREAD_LIB} Boost::boost)

set_target_properties(
  wtdbo
PROPERTIES
  FOLDER "dbo"
  EXPORT_NAME Dbo
  VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
  SOVERSION ${WTDBO_SOVERSION}
)

IF(MSVC)
  SET_TARGET_PROPERTIES(
    wtdbo
  PROPERTIES
    COMPILE_FLAGS "${BUILD_PARALLEL} /wd4251 /wd4275 /wd4267"
  )
ENDIF(MSVC)

add_subdirectory(backend)

INSTALL(TARGETS wtdbo
    EXPORT wt-target-dbo
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR})

INSTALL(EXPORT wt-target-dbo
        DESTINATION ${CMAKE_INSTALL_DIR}/wt
        NAMESPACE Wt::)

INSTALL_FILES(/include/Wt/Dbo "^[^.]+\\.h$")
INSTALL_FILES(/include/Wt FILES ${WCONFIG_H_PATH})

IF (ASCIIDOC_FOUND)
  ASCIIDOC_FILE(dbo-tutorial
    ${CMAKE_SOURCE_DIR}/doc/tutorial/dbo.doc
    ${CMAKE_SOURCE_DIR}/doc/tutorial/dbo.html)
ENDIF (ASCIIDOC_FOUND)
