add_executable(app main.cpp) # TODO: your app name

target_link_libraries(app
  PRIVATE
    scy
    project_options
    project_sanitizers
    project_coverage
)

target_include_directories(scy
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
)