add_library(cilo_editor)

target_include_directories(
  cilo_editor
  PUBLIC ${cilo_SOURCE_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(cilo_editor PUBLIC dummy.c)

add_executable(cilo)

target_include_directories(
  cilo
  PUBLIC ${cilo_SOURCE_DIR}/include
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(cilo PRIVATE main.c)

target_link_libraries(cilo PRIVATE cilo_editor)
