cmake_minimum_required( VERSION 3.20 )

project( SortVis CXX )

option( WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON )
option( ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF )
option( ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" ON )
option( ENABLE_INCLUDE_WHAT_YOU_USE "Enable static analysis with include-what-you-use" OFF )

include( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerWarnings.cmake )
include( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectSettings.cmake )
include( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake )

find_package( cppcoro CONFIG REQUIRED )
add_compile_definitions( _SILENCE_CLANG_COROUTINE_MESSAGE )

find_package( imgui CONFIG REQUIRED )
find_package( SFML COMPONENTS system window graphics CONFIG REQUIRED )
find_package( ImGui-SFML CONFIG REQUIRED )
find_package( implot CONFIG REQUIRED )

find_package( Catch2 CONFIG REQUIRED )

set( INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc )
# set( EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext )
set( SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src )
set( TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test )

set( TARGET_MAIN ${PROJECT_NAME} )
set( TARGET_TEST ${PROJECT_NAME}-test )
set( TARGET_SFML sfmltrial )

set( SOURCES_MAIN
	${SRC_DIR}/${TARGET_MAIN}.cpp
)
set( SOURCES_TEST
	${TEST_DIR}/${TARGET_TEST}.cpp
	${TEST_DIR}/sorting-test.cpp
)
set( SOURCES_SFML
	${SRC_DIR}/${TARGET_SFML}.cpp
)

add_executable( ${TARGET_MAIN} ${SOURCES_MAIN} )
target_include_directories( ${TARGET_MAIN} PRIVATE ${INC_DIR} )
# target_include_directories( ${TARGET_MAIN} SYSTEM PRIVATE ${EXT_DIR} )
target_link_libraries( ${TARGET_MAIN} PRIVATE cppcoro sfml-system sfml-graphics sfml-window ImGui-SFML::ImGui-SFML imgui::imgui implot::implot )
target_compile_features( ${TARGET_MAIN} PRIVATE cxx_std_20 )

add_executable( ${TARGET_TEST} ${SOURCES_TEST} )
target_include_directories( ${TARGET_TEST} SYSTEM PRIVATE ${INC_DIR} ${Catch2_INCLUDE_DIRS} )
target_link_libraries( ${TARGET_TEST} PRIVATE cppcoro Catch2::Catch2 )
target_compile_features( ${TARGET_TEST} PRIVATE cxx_std_20 )
