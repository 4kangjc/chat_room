cmake_minimum_required(VERSION 3.2)
project(chat_room)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/flexy)

include(flexy/cmake/utils.cmake)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O3 -ggdb -std=c++17 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

include_directories(./)
include_directories(flexy)
link_directories(./flexy/lib)


set(LIB_SRC
    chat/protocol.cc
    chat/resource_servlet.cc
    chat/chat_servlet.cc
)


option(BUILD_SHARED_LIBS "Build flexy as a shared lib" ON)
option(BUILD_STATIC_LIBS "Build flext as a static lib" OFF)

if (BUILD_STATIC_LIBS)
add_library(chat_room STATIC ${LIB_SRC})
target_link_libraries(chat_room flexy)
elseif (BUILD_SHARED_LIBS)
add_library(chat_room SHARED ${LIB_SRC})
target_link_libraries(chat_room flexy)
endif(BUILD_STATIC_LIBS)

redefine_file_macro(chat_room)

flexy_add_executable(chat "chat/main.cc" chat_room)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
