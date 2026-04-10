cmake_minimum_required (VERSION 3.10)

project ("MeshViewer")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_subdirectory(ext/SDL EXCLUDE_FROM_ALL)
add_executable (MeshViewer "src/main.cpp" "src/win.cpp" "src/mesh.cpp" "inc/point.h" "src/input.cpp"
"src/fileprocessor.cpp" "src/calc.cpp" "src/render.cpp" "inc/renderdata.h")

target_include_directories(${PROJECT_NAME} PRIVATE inc)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)