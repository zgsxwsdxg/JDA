project(JDA)
cmake_minimum_required(VERSION 2.8)

set(OpenCV_DIR /usr/local/share/OpenCV)
find_package(OpenCV REQUIRED)

include(libjda.cmake)

include_directories(${PROJECT_SOURCE_DIR}/include)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
link_directories(${PROJECT_SOURCE_DIR}/lib)

file(GLOB SRC src/*.cpp)

add_executable(jda ${SRC})
target_link_libraries(jda libjda ${OpenCV_LIBS})

add_subdirectory(examples)
