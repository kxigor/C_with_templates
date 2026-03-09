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
# 3. SUBPROJECTS
# =========================================================
set(_BACKUP_UNITTESTS ${ENABLE_UNITTESTS})
set(_BACKUP_FORMAT    ${ENABLE_FORMAT_CHECK})
set(_BACKUP_TIDY      ${ENABLE_TIDY_CHECK})

# 2. Выключаем тестирование для дочернего проекта локально
# Используем обычный set (без CACHE), чтобы LRK увидел это значение
set(ENABLE_TESTING OFF)
add_subdirectory(third_party/LRK)
unset(ENABLE_TESTING) # Убираем локальную переменную, чтобы вернуться к глобальной

# 3. ПРИНУДИТЕЛЬНО возвращаем наши настройки в кэш
set(ENABLE_UNITTESTS ${_BACKUP_UNITTESTS} CACHE BOOL "" FORCE)
set(ENABLE_FORMAT_CHECK ${_BACKUP_FORMAT} CACHE BOOL "" FORCE)
set(ENABLE_TIDY_CHECK ${_BACKUP_TIDY} CACHE BOOL "" FORCE)

# =========================================================
# 4. SUBDIRECTORIES
# =========================================================
add_subdirectory(src)
add_subdirectory(app)

if(ENABLE_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
