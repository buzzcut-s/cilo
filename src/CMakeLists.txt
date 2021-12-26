add_library(cilo_editor)

target_include_directories(
  cilo_editor
  PUBLIC ${cilo_SOURCE_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(
  cilo_editor
  PUBLIC terminal.c
         error.c
         editor/state.c
         string_buffer.c
         window_size.c
         editor/input.c
         editor/row.c
         file_io.c
         editor/operations.c)

add_executable(cilo)

target_include_directories(
  cilo
  PUBLIC ${cilo_SOURCE_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(cilo PRIVATE main.c)

target_link_libraries(cilo PRIVATE cilo_editor)
