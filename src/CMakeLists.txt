add_library(scy
  lexer.cpp
  parser.cpp
  ast_visualizer.cpp
)

target_link_libraries(scy
  PRIVATE
    project_options
    project_sanitizers
    project_coverage
)

target_include_directories(scy
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
)