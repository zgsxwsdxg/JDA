file(GLOB src_files ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

find_package(OpenCV REQUIRED)

foreach(source ${src_files})
  get_filename_component(file_name ${source} NAME_WE)
  set(tool_name "${file_name}.bin")
  # target
  add_executable(${tool_name} ${source})
  target_link_libraries(${tool_name} ${OpenCV_LIBS} libjda)
endforeach(source)

