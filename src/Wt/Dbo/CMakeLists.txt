IF(NOT ENABLE_LIBWTDBO)
  MESSAGE("** libwtdbo disabled, not building Wt::Dbo.")
  return()
ENDIF()

ADD_LIBRARY(wtdbo)
target_compile_definitions(wtdbo PRIVATE WT_BUILDING)

IF(WIN32 AND SHARED_LIBS)
  CONFIGURE_FILE(wtdbo-version.rc.in
                  ${CMAKE_CURRENT_BINARY_DIR}/wtdbo-version.rc)
  target_sources(wtdbo PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/wtdbo-version.rc)
ENDIF()

target_sources(wtdbo PRIVATE
  ptr.h ptr_impl.h ptr.C
  Call.h Call_impl.h Call.C
  DbAction.h DbAction_impl.h DbAction.C
  Exception.h Exception.C
  FixedSqlConnectionPool.h FixedSqlConnectionPool.C
  Json.h Json.C
  Query.h Query_impl.h Query.C
  QueryColumn.h
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
  Logger.h Logger.C
)

if(NOT APPLE OR SHARED_LIBS)
  SET_PROPERTY(TARGET wtdbo PROPERTY C_VISIBILITY_PRESET hidden)
  SET_PROPERTY(TARGET wtdbo PROPERTY CXX_VISIBILITY_PRESET hidden)
  SET_PROPERTY(TARGET wtdbo PROPERTY VISIBILITY_INLINES_HIDDEN YES)
endif()

IF(HAVE_UNWIND)
  TARGET_LINK_LIBRARIES(wtdbo PRIVATE ${UNWIND_LIBRARIES})
ENDIF(HAVE_UNWIND)

TARGET_LINK_LIBRARIES(wtdbo PRIVATE Boost::boost)

target_include_directories(wtdbo PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>)
target_include_directories(wtdbo PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/web>)
#target_include_directories(wtdbo PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/Wt/Date/include>)
target_include_directories(wtdbo PUBLIC $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)

target_include_directories(wtdbo INTERFACE $<INSTALL_INTERFACE:include>)

INSTALL(TARGETS wtdbo
    EXPORT wt-targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR})

SET_TARGET_PROPERTIES(
  wtdbo
PROPERTIES
  EXPORT_NAME Dbo
  VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
)
IF(MSVC)
  SET_TARGET_PROPERTIES(
    wtdbo
  PROPERTIES
    COMPILE_FLAGS "${BUILD_PARALLEL} /wd4251 /wd4275 /wd4267"
  )
  SET_TARGET_PROPERTIES(wtdbo PROPERTIES FOLDER "dbo")
ENDIF(MSVC)

SUBDIRS(backend)

INSTALL_FILES(/include/Wt/Dbo "^[^.]+\\.h$")
INSTALL_FILES(/include/Wt FILES ${WCONFIG_H_PATH})

  if(ASCIIDOCTOR_FOUND)
    ASCIIDOC_FILE(dbo-tutorial
      ${WT_SOURCE_DIR}/doc/tutorial/dbo.adoc
      ${WT_SOURCE_DIR}/doc/tutorial/dbo.html)
  endif()

