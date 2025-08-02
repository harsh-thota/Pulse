cmake_minimum_required(VERSION 3.27)
project(Pulse)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(pulse
	src/main.cpp
 "src/ui/application.h" "src/ui/application.cpp")

target_include_directories(pulse PRIVATE
	${CMAKE_SOURCE_DIR}/external/SDL2/include
	${CMAKE_SOURCE_DIR}/external/clay
)

target_link_directories(pulse PRIVATE ${CMAKE_SOURCE_DIR}/external/SDL2/lib)

target_link_libraries(pulse
	SDL2main
	SDL2
)