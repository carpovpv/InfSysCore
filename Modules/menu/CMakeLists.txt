SET(CMAKE_SHARED_LIBRARY_PREFIX "")
SET (PROJECT
	LAP_Menu)

SET (SOURCES
        menu.cpp
        addmenu.cpp
        addcategory.cpp
        ../../lap_abstractmodule.cpp
)

SET (HEADERS
        menu.h
	addmenu.h
	addcategory.h
        ../../lap_abstractmodule.h
)

SET (UI
    menu.ui
    addmenu.ui
    addcategory.ui
)

project (${PROJECT})

set (RESOURCES
        resources.qrc)

find_package( Qt4 REQUIRED)
include (${QT_USE_FILE})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${QT_QTSQL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/Components/LAP_DataGrid)

qt4_wrap_cpp (MOC_SOURCES ${HEADERS})
qt4_add_resources(QRC_SOURCES ${RESOURCES})
qt4_wrap_ui (UIS ${UI})

add_library ( ${PROJECT} SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES} ${UIS})
target_link_libraries ( ${PROJECT} ${QT_LIBRARIES} ${QT_QTSQL_LIBRARY} LAP_DataGrid)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}${CMAKE_SHARED_MODULE_SUFFIX} DESTINATION "${CMAKE_BINARY_DIR}/bin" )
