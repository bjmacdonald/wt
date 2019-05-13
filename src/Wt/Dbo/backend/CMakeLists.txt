IF(ENABLE_SQLITE)

  OPTION(USE_SYSTEM_SQLITE3 "Use system-wide Sqlite3 instead of Wt's version" OFF)
  OPTION(USE_SQLITE3_BDB "Use BerkeleyDB storage with Sqlite3 API" OFF)

  IF(USE_SYSTEM_SQLITE3)

    FIND_PACKAGE( Sqlite3 REQUIRED)

    IF (USE_SQLITE3_BDB)
      ADD_DEFINITIONS(-DSQLITE3_BDB)
    ENDIF (USE_SQLITE3_BDB)

  ELSE(USE_SYSTEM_SQLITE3)

    IF (NOT WIN32)
      ADD_DEFINITIONS(-DHAVE_USLEEP)
    ENDIF (NOT WIN32)
    SET(Sqlite3_SRCS amalgamation/sqlite3.c)

  ENDIF(USE_SYSTEM_SQLITE3)

  FIND_PACKAGE(Threads)

  set(WT_SQLITE3_THREAD_LIB "")
  IF (NOT Threads_FOUND)
    ADD_DEFINITIONS(-DSQLITE3_THREAD_SAFE=0)
  ELSE (NOT Threads_FOUND)
    set(WT_SQLITE3_THREAD_LIB Threads::Threads)
  ENDIF (NOT Threads_FOUND)

  SET(HAVE_SQLITE ON CACHE INTERNAL "building sqlite3 backend" FORCE)

  IF(WIN32)
    IF(SHARED_LIBS)
      CONFIGURE_FILE(wtdbosqlite3-version.rc.in
                     ${CMAKE_CURRENT_BINARY_DIR}/wtdbosqlite3-version.rc)
      SET(Sqlite3_SRCS ${Sqlite3_SRCS} 
          ${CMAKE_CURRENT_BINARY_DIR}/wtdbosqlite3-version.rc)
    ENDIF(SHARED_LIBS)
  ENDIF(WIN32)

  ADD_LIBRARY(wtdbosqlite3
    Sqlite3.C
    ${Sqlite3_SRCS}
    )

  MESSAGE("** Wt::Dbo: building SQLite3 backend.")

  TARGET_LINK_LIBRARIES(wtdbosqlite3
    PUBLIC
      wtdbo
    PRIVATE
      ${SQLITE3_LIBRARIES}
      ${WT_SQLITE3_THREAD_LIB}
      ${WT_MATH_LIBRARY}
      )

  IF(NOT WIN32)
    FIND_LIBRARY(DL_LIB
      NAMES
      dl
      )
    IF(DL_LIB)
      TARGET_LINK_LIBRARIES(wtdbosqlite3 PRIVATE ${DL_LIB})
    ENDIF(DL_LIB)
  ENDIF(NOT WIN32)

  INSTALL(TARGETS wtdbosqlite3
    EXPORT wt-target-dbosqlite3
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR})

  INSTALL(EXPORT wt-target-dbosqlite3
          DESTINATION ${CMAKE_INSTALL_DIR}/wt
          NAMESPACE Wt::)

  SET_TARGET_PROPERTIES(
    wtdbosqlite3
  PROPERTIES
    EXPORT_NAME DboSqlite3
    VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
    SOVERSION ${WTDBOSQLITE3_SOVERSION} 
    )

  IF(MSVC)
    SET_TARGET_PROPERTIES(
      wtdbosqlite3
    PROPERTIES
      COMPILE_FLAGS "${BUILD_PARALLEL} /wd4251 /wd4275 /wd4267"
    )
	SET_TARGET_PROPERTIES(wtdbosqlite3 PROPERTIES FOLDER "dbo")
  ENDIF(MSVC)

  INSTALL_FILES(/include/Wt/Dbo/backend "^.*Sqlite3.*h$")

ELSE(ENABLE_SQLITE)
  SET(HAVE_SQLITE OFF CACHE INTERNAL "building sqlite3 backend" FORCE)
ENDIF(ENABLE_SQLITE)

IF(ENABLE_POSTGRES AND POSTGRES_FOUND)

  SET(HAVE_POSTGRES ON CACHE INTERNAL "building postgres backend" FORCE)

  MESSAGE("** Wt::Dbo: building Postgres backend.")

  IF(WIN32)
    IF(SHARED_LIBS)
      CONFIGURE_FILE(wtdbopostgres-version.rc.in
                     ${CMAKE_CURRENT_BINARY_DIR}/wtdbopostgres-version.rc)
      SET(postgres_SRCS ${postgres_SRCS} 
          ${CMAKE_CURRENT_BINARY_DIR}/wtdbopostgres-version.rc)
    ENDIF(SHARED_LIBS)
  ENDIF(WIN32)

  ADD_LIBRARY(wtdbopostgres
    Postgres.C
    ${postgres_SRCS}
    )
  TARGET_LINK_LIBRARIES(wtdbopostgres
    PUBLIC
      wtdbo
    PRIVATE
      ${POSTGRES_LIBRARIES}
  )

  INCLUDE_DIRECTORIES(${POSTGRES_INCLUDE})

  IF(MSVC)
    SET_TARGET_PROPERTIES(wtdbopostgres PROPERTIES FOLDER "dbo")
  ENDIF(MSVC)

  INSTALL(TARGETS wtdbopostgres
    EXPORT wt-target-dbopostgres
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR})

  INSTALL(EXPORT wt-target-dbopostgres
          DESTINATION ${CMAKE_INSTALL_DIR}/wt
          NAMESPACE Wt::)

  SET_TARGET_PROPERTIES(
    wtdbopostgres
  PROPERTIES
    EXPORT_NAME DboPostgres
    VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
    SOVERSION ${WTDBOPOSTGRES_SOVERSION}
  )

  INSTALL_FILES(/include/Wt/Dbo/backend "^.*Postgres.*h$")
ELSE(ENABLE_POSTGRES AND POSTGRES_FOUND)
  MESSAGE("** Wt::Dbo: not building Postgres backend.")

  IF(ENABLE_POSTGRES)
    MESSAGE("    Indicate the location of your postgres installation using ")
    MESSAGE("    -DPOSTGRES_PREFIX=...")
  ENDIF(ENABLE_POSTGRES)

  SET(HAVE_POSTGRES OFF CACHE INTERNAL "building postgres backend" FORCE)
ENDIF(ENABLE_POSTGRES AND POSTGRES_FOUND)

IF(ENABLE_MSSQLSERVER AND ODBC_FOUND)

  SET(HAVE_MSSQLSERVER ON CACHE INTERNAL "building Microsoft SQL Server backend" FORCE)

  MESSAGE("** Wt::Dbo: building Microsoft SQL Server backend.")

  IF(WIN32)
    IF(SHARED_LIBS)
      CONFIGURE_FILE(wtdbomssqlserver-version.rc.in
                     ${CMAKE_CURRENT_BINARY_DIR}/wtdbomssqlserver-version.rc)
      SET(mssqlserver_SRCS ${mssqlserver_SRCS}
          ${CMAKE_CURRENT_BINARY_DIR}/wtdbomssqlserver-version.rc)
    ENDIF(SHARED_LIBS)
  ENDIF(WIN32)

  ADD_LIBRARY(wtdbomssqlserver
    MSSQLServer.C
    ${mssqlserver_SRCS}
  )
  TARGET_LINK_LIBRARIES(wtdbomssqlserver
    PUBLIC
      wtdbo
    PRIVATE
      ${ODBC_LIBRARIES}
  )

  INCLUDE_DIRECTORIES(${ODBC_INCLUDE})

  INSTALL(TARGETS wtdbomssqlserver
    EXPORT wt-target-dbomssqlserver
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR})

  INSTALL(EXPORT wt-target-dbomssqlserver
          DESTINATION ${CMAKE_INSTALL_DIR}/wt
          NAMESPACE Wt::)

  SET_TARGET_PROPERTIES(
    wtdbomssqlserver
  PROPERTIES
    EXPORT_NAME DboMSSQLServer
    VERSION ${VERSION_SERIES}.${VERSION_MAJOR}.${VERSION_MINOR}
    SOVERSION ${WTDBOMSSQLSERVER_SOVERSION}
    )

  IF(MSVC)
    SET_TARGET_PROPERTIES(
      wtdbomssqlserver
    PROPERTIES
      COMPILE_FLAGS "${BUILD_PARALLEL} /wd4251 /wd4275 /wd4267"
    )
    SET_TARGET_PROPERTIES(wtdbomssqlserver PROPERTIES FOLDER "dbo")
  endif(MSVC)

  INSTALL_FILES(/include/Wt/Dbo/backend "^.*MSSQLServer.*h$")
ELSE(ENABLE_MSSQLSERVER AND ODBC_FOUND)
  MESSAGE("** Wt::Dbo: not building Microsoft SQL Server backend.")
  SET(HAVE_MSSQLSERVER OFF CACHE INTERNAL "building Microsoft SQL Server backend" FORCE)
ENDIF(ENABLE_MSSQLSERVER AND ODBC_FOUND)
