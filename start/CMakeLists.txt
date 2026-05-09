cmake_minimum_required(VERSION 3.20.0)
project(Start VERSION 0.1.0)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

find_package(SQLite3 REQUIRED)

file(GLOB project
     src/*
)

add_executable(Start ${project}
     src/main.cpp
     src/open_bd.cpp
     src/edit_bd.cpp
     src/del_bd.cpp
     src/add_bd.cpp
     src/main.h)

target_link_libraries(Start ${wxWidgets_LIBRARIES})
target_link_libraries(Start SQLite3::SQLite3)
target_include_directories(Start PRIVATE ${wxWidgets_INCLUDE_DIRS})