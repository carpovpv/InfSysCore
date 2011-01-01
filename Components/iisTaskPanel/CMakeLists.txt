SET(CMAKE_SHARED_LIBRARY_PREFIX "")
set (PROJECT
        iisTaskPanel0)

set (HEADERS
	iisiconlabel.h             	
	iisrect.h                  
	iistaskbox.h               
	iistaskgroup.h             
	iistaskheader.h            
	iistaskpanel.h             
	iistaskpanelscheme.h       
	iistaskpanel_global.h      
	iisvistataskpanelscheme.h  
	iiswinxptaskpanelscheme.h  	
	)
		
set (SOURCES
	iisiconlabel.cpp           
	iisrect.cpp                
	iistaskbox.cpp             
	iistaskgroup.cpp           
	iistaskheader.cpp          
	iistaskpanel.cpp           
	iistaskpanelscheme.cpp     
	iisvistataskpanelscheme.cpp
	iiswinxptaskpanelscheme.cpp
        )

set (RESOURCES
        iisTaskPanel.qrc)

project (${PROJECT})
find_package( Qt4 REQUIRED)
include (${QT_USE_FILE})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES($(CMAKE_SOURCE_DIR))

qt4_wrap_cpp (MOC_SOURCES ${HEADERS})
qt4_add_resources(QRC_SOURCES ${RESOURCES})

add_library ( ${PROJECT} SHARED  ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${QRC_SOURCES} )

target_link_libraries ( ${PROJECT} ${QT_LIBRARIES} )

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT}${CMAKE_SHARED_MODULE_SUFFIX} DESTINATION "${CMAKE_BINARY_DIR}/bin" )
