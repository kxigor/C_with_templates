cmake_minimum_required(VERSION 3.25)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

project(cpp-project-template LANGUAGES CXX) # TODO: your project name

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =========================================================
# 1. QA/TESTING OPTIONS
# =========================================================
option(ENABLE_TESTING "Enable CTest checks (GTest, Format, Tidy, Coverage)." ON)

option(ENABLE_UNITTESTS "Enable unit tests GTest." ON)
option(ENABLE_FORMAT_CHECK "Register the Clang-Format check as a CTest target." ON)
option(ENABLE_TIDY_CHECK "Register the Clang-Tidy check as a CTest target." ON)
option(ENABLE_COVERAGE "Enable code coverage generation (GCOV/LCOV)." OFF)

set(ENABLE_UNITTESTS ON CACHE BOOL "Enable unit tests" FORCE)
set(ENABLE_FORMAT_CHECK ON CACHE BOOL "Enable format check" FORCE)
set(ENABLE_TIDY_CHECK ON CACHE BOOL "Enable tidy check" FORCE)

if(NOT ENABLE_TESTING)
  set(ENABLE_UNITTESTS OFF CACHE BOOL "Unit tests requires ENABLE_TESTING=ON." FORCE)
  set(ENABLE_FORMAT_CHECK OFF CACHE BOOL "Format check requires ENABLE_TESTING=ON." FORCE)
  set(ENABLE_TIDY_CHECK OFF CACHE BOOL "Tidy check requires ENABLE_TESTING=ON." FORCE)
  set(ENABLE_COVERAGE OFF CACHE BOOL "Coverage requires ENABLE_TESTING=ON." FORCE)
endif()

# =========================================================
# 2. ENABLING THE MAIN MODULES
# =========================================================
include(ProjectOptions)
include(ProjectSanitizers)
include(ProjectCoverage)

# =========================================================
# 3. SUBDIRECTORIES
# =========================================================
add_subdirectory(src)
add_subdirectory(app)

# =========================================================
# 4. TESTS
# =========================================================
if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()

# =========================================================
# 5. SUBPROJECTS
# =========================================================
add_subdirectory(third_party/LRK)
