cmake_minimum_required(VERSION 3.25.0)
project(Lugaru)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Lugaru)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CXX_STANDARD_REQUIRED ON)

if (MSVC)
	# Ignore warnings about missing pdb
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /ignore:4099")
	set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} /ignore:4099")
endif()

find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(SDL2 REQUIRED)
find_package(vorbis REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(glext REQUIRED)
find_package(glu REQUIRED)


file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS 
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h 
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cxx
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.hxx
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

add_executable (${PROJECT_NAME} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
	CXX_STANDARD 17
)

target_link_libraries (${PROJECT_NAME}
    PRIVATE OpenGL::GL
    PRIVATE OpenAL::OpenAL
    PRIVATE JPEG::JPEG
    PRIVATE PNG::PNG
    PRIVATE SDL2::SDL2
    PRIVATE vorbis::vorbis
    PRIVATE JsonCpp::JsonCpp
    PRIVATE glext::glext
    PRIVATE glu::glu
)

# _CRT_SECURE_NO_WARNINGS
target_compile_definitions(${PROJECT_NAME} 
	PRIVATE $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
)

target_include_directories(${PROJECT_NAME}
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Source/
)

target_compile_options(${PROJECT_NAME} 
	PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/W4>
	#PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/WX /W3>
	PRIVATE $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Werror>
	# -Wextra -Wpedantic 
)

file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/Data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})