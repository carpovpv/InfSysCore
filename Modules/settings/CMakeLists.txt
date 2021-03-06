SET(CMAKE_SHARED_LIBRARY_PREFIX "")
SET (PROJECT
	LAP_Settings)

SET (SOURCES
        settings.cpp
        ../../lap_abstractmodule.cpp
)

SET (HEADERS
        settings.h
        ../../lap_abstractmodule.h
)

SET (UI
    settings.ui
)

project (${PROJECT})
                         
find_package( Qt4 REQUIRED)
include (${QT_USE_FILE})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${QT_QTSQL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/Components/LAP_DataGrid)

qt4_wrap_cpp (MOC_SOURCES ${HEADERS})
qt4_wrap_ui (UIS ${UI})

add_library ( ${PROJECT} SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UIS})
target_link_libraries ( ${PROJECT} ${QT_LIBRARIES} ${QT_QTSQL_LIBRARY} LAP_DataGrid)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}${CMAKE_SHARED_MODULE_SUFFIX} DESTINATION "${CMAKE_BINARY_DIR}/bin" )
