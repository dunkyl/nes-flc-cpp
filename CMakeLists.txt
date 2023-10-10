cmake_minimum_required(VERSION 3.21)

set (CMAKE_CXX_STANDARD 23)

set(LLVM_MOS_PLATFORM nes-nrom)

set(CMAKE_PREFIX_PATH "llvm-mos")
find_package(llvm-mos-sdk REQUIRED)

project(FLCXXNES LANGUAGES C CXX ASM)

set(CMAKE_EXECUTABLE_SUFFIX .NES)

add_executable(FLCXXNES main.cpp chr-rom.s)
target_include_directories(FLCXXNES PRIVATE .)
target_link_libraries(FLCXXNES neslib nesdoug)
set_property(SOURCE chr-rom.s PROPERTY
  OBJECT_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Alpha.chr)

target_compile_options(FLCXXNES PRIVATE
  -Wall -Wshadow -Wpedantic
  -Wnull-dereference
  -Wformat=2
  -Wimplicit-fallthrough
  -Oz
)