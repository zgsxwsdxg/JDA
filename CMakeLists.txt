project(JDA)
cmake_minimum_required(VERSION 2.8)

set(OpenCV_DIR /usr/local/share/OpenCV)
find_package(OpenCV REQUIRED)

include(libjda.cmake)

file(GLOB SRC src/*.cpp)

add_executable(jda ${SRC})
target_link_libraries(jda libjda ${OpenCV_LIBS})
