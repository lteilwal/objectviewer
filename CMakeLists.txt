cmake_minimum_required (VERSION 3.10)

project ("MeshViewerCUDA" LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_ARCHITECTURES 89)
set(CMAKE_CUDA_HOST_COMPILER "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe")

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_subdirectory(ext/SDL EXCLUDE_FROM_ALL)
add_executable (${PROJECT_NAME} "src/main.cpp" "src/win.cpp" "src/mesh.cpp" "inc/point.h" "src/input.cpp"
"src/fileprocessor.cpp"  "src/render.cpp" "inc/renderdata.h" "src/kernelCalc.cu")

target_include_directories(${PROJECT_NAME} PRIVATE inc)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)