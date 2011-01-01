cmake_minimum_required (VERSION 2.6)
SET(CMAKE_BUILD_TYPE "Release")

set (PROJECT 
        Base)

SET (SOURCES
        lap_core.cpp
        main.cpp
        style03.cpp
        login.cpp
        about.cpp
        lap_abstractmodule.cpp
)

SET (HEADERS
        login.h
        lap_core.h
        style03.h
        about.h
        globals.h
        lap_abstractmodule.h
)

SET (UI
        login.ui
        lap_core.ui
        about.ui
    )

set (RESOURCES
        resources.qrc)

project (${PROJECT})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${QT_QTSQL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${QT_QTXML_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${QT_QTSCRIPT_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/Components/iisTaskPanel)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/Components)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(${PROJECT_SOURCE_DIR}/Components/iisTaskPanel)
add_subdirectory(${PROJECT_SOURCE_DIR}/Components/LAP_DataGrid)
add_subdirectory(${PROJECT_SOURCE_DIR}/Modules/info)
add_subdirectory(${PROJECT_SOURCE_DIR}/Modules/dictionary)
add_subdirectory(${PROJECT_SOURCE_DIR}/Modules/menu)
add_subdirectory(${PROJECT_SOURCE_DIR}/Modules/roles)
add_subdirectory(${PROJECT_SOURCE_DIR}/Modules/settings)
add_subdirectory(${PROJECT_SOURCE_DIR}/Modules/sql)
add_subdirectory(${PROJECT_SOURCE_DIR}/Modules/users)

find_package( Qt4 REQUIRED)
include (${QT_USE_FILE})

qt4_wrap_cpp (MOC_SOURCES ${HEADERS})
qt4_wrap_ui (UIS ${UI})
qt4_add_resources(QRC_SOURCES ${RESOURCES})

add_executable ( ${PROJECT} WIN32 ${UIS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES} )
target_link_libraries ( ${PROJECT} ${QT_LIBRARIES} ${QT_QTSQL_LIBRARY} ${QT_QTXML_LIBRARY} LAP_Info iisTaskPanel0)

install (TARGETS ${PROJECT} DESTINATION "${CMAKE_BINARY_DIR}/bin")
