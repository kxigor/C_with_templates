add_gtest(BasicLexerTest
  SOURCES Basic.cpp
  LIBRARIES 
    scy
    project_options
    project_coverage
)

target_include_directories(scy
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
)