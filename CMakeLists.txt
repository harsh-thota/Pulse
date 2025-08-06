cmake_minimum_required(VERSION 3.27)
project(Pulse)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# source files
add_executable(pulse
	src/main.cpp
	src/ui/application.cpp
	external/clay/clayman.cpp
	src/platform/windows/windows_system_monitor.cpp
	src/monitoring/data_collector.cpp
	src/ui/screens/base_screen.cpp
	src/ui/screens/performance_screen.cpp
	src/ui/components/graph_component.cpp
	src/ui/components/ui_card.cpp
	src/ui/components/text_components.cpp
	src/ui/components/layout_components.cpp
	src/ui/components/table_component.cpp
	src/ui/components/metric_card.cpp
)

# header files
target_include_directories(pulse PRIVATE
	${CMAKE_SOURCE_DIR}/external/SDL2/include
	${CMAKE_SOURCE_DIR}/external/clay
	${CMAKE_SOURCE_DIR}/src #internal headers
)

# Link directories
target_link_directories(pulse PRIVATE 
	${CMAKE_SOURCE_DIR}/external/SDL2/lib
)

# Fixed library names to match actual .lib files
target_link_libraries(pulse
	SDL2main.lib
	SDL2.lib
	SDL2_ttf.lib
)

# Copy assets to build directory
add_custom_command(TARGET pulse POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:pulse>/assets
	COMMENT "Copying assets to build directory"
)

# Copy SDL2 DLLs to the executable directory for runtime
add_custom_command(TARGET pulse POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${CMAKE_SOURCE_DIR}/external/SDL2/lib/SDL2.dll
	$<TARGET_FILE_DIR:pulse>/SDL2.dll
	COMMENT "Copying SDL2.dll to build directory"
)

add_custom_command(TARGET pulse POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${CMAKE_SOURCE_DIR}/external/SDL2/lib/SDL2_ttf.dll
	$<TARGET_FILE_DIR:pulse>/SDL2_ttf.dll
	COMMENT "Copying SDL2_ttf.dll to build directory"
)