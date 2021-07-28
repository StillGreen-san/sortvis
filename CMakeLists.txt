cmake_minimum_required( VERSION 3.20 )

project( SortVis CXX )

option( WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON )
option( ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF )
option( ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" ON )
option( ENABLE_INCLUDE_WHAT_YOU_USE "Enable static analysis with include-what-you-use" OFF )

include( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerWarnings.cmake )
include( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectSettings.cmake )
include( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/StaticAnalyzers.cmake )

set( INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc )
set( EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext )
set( SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src )

set( TARGET_MAIN ${PROJECT_NAME} )

set( SOURCES_MAIN
	${SRC_DIR}/${PROJECT_NAME}.cpp
)

add_executable( ${TARGET_MAIN} ${SOURCES_MAIN} )
target_include_directories( ${TARGET_MAIN} PUBLIC ${INC_DIR} )
target_include_directories( ${TARGET_MAIN} SYSTEM PUBLIC ${EXT_DIR} )
target_link_libraries( ${TARGET_MAIN} PUBLIC cppcoro )
target_compile_features( ${TARGET_MAIN} PUBLIC cxx_std_20 )
