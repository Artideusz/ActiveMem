cmake_minimum_required(VERSION 3.31.0)
project(activemem LANGUAGES CXX)

# find_library(dbghelp_LIBRARY_PATH PATH DbgHelp)
# add_subdirectory(srclib)
include_directories(include)

add_library(main SHARED src/main.cpp src/main.def)
target_compile_options(main PRIVATE /D_AMD64_=1)
target_link_libraries(main ${PROJECT_SOURCE_DIR}/lib/detours.lib ${PROJECT_SOURCE_DIR}/lib/syelog.lib ${PROJECT_SOURCE_DIR}/lib/dbghelp.lib)

add_executable(withdll srclib/withdll/withdll.cpp)
target_link_libraries(withdll ${PROJECT_SOURCE_DIR}/lib/detours.lib)

add_executable(syelogd srclib/syelogd/syelogd.cpp)
# target_link_libraries(syelogd ${PROJECT_SOURCE_DIR}/lib/detours.lib)

add_executable(test_valloc test/VirtualAllocTest/main.cpp)