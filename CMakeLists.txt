cmake_minimum_required(VERSION 2.8)
project(kodi-qrcode-client)
add_definitions(-std=c++0x -Wall -Wextra -Wattributes -Wno-packed-bitfield-compat -D_REENTRANT -O3 -fPIC -g)

FILE(GLOB CURRENT_SRC "./*.cpp")

find_package(OpenCV REQUIRED highgui core imgproc)

include_directories( 
                  ${CMAKE_BINARY_DIR}
                  SYSTEM ${OpenCV_INCLUDE_DIRS}
                )

add_custom_target(common_stubs
	DEPENDS ${CMAKE_BINARY_DIR}/gen/abstractstubserver.h ${CMAKE_BINARY_DIR}/gen/stubclient.h
)
                
add_custom_target(
        gen
	COMMAND jsonrpcstub ${CMAKE_CURRENT_SOURCE_DIR}/kodi.json --cpp-client=XbmcRemoteClient --cpp-client-file=${CMAKE_BINARY_DIR}/xbmcremote.h
	COMMENT "Generating Client XBMC Stubfile"
)



set( libs jsoncpp jsonrpccpp-common jsonrpccpp-client ${OpenCV_LIBS} zbar)

ADD_EXECUTABLE(kodi-qrcode-client ${CURRENT_SRC}    )
TARGET_LINK_LIBRARIES(kodi-qrcode-client ${libs})
add_dependencies(kodi-qrcode-client gen)
