SET(CMAKE_SHARED_LIBRARY_PREFIX "")
SET (PROJECT
	LAP_DataGrid)

SET (SOURCES
        lap_datagrid.cpp
	lap_combobox.cpp
	lap_dateedit.cpp
	lap_lineedit.cpp
	lap_sqlmodel.cpp
)

SET (HEADERS
        lap_datagrid.h
	lap_combobox.h
	lap_checkbox.h
	lap_dateedit.h
	lap_lineedit.h
)

project (${PROJECT})

find_package( Qt4 REQUIRED)
include (${QT_USE_FILE})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${QT_QTSQL_INCLUDE_DIR})

qt4_wrap_cpp (MOC_SOURCES ${HEADERS})

add_library ( ${PROJECT} SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} )
target_link_libraries ( ${PROJECT} ${QT_LIBRARIES} ${QT_QTSQL_LIBRARY})

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}${CMAKE_SHARED_MODULE_SUFFIX} DESTINATION "${CMAKE_BINARY_DIR}/bin" )
