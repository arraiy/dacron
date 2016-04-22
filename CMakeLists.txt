cmake_minimum_required(VERSION 3.0)
project(Splice)
include(CTest)
enable_testing()

set(Splice_MAJOR_VERSION 0)
set(Splice_MINOR_VERSION 0)
set(Splice_PATCH_VERSION 0)

set(Splice_VERSION
  ${Splice_MAJOR_VERSION}.${Splice_MINOR_VERSION}.${Splice_MINOR_VERSION})

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")


find_package(Boost REQUIRED program_options filesystem system regex)
find_package(GTest REQUIRED)
find_package(glog REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} SYSTEM)
include_directories(${GTEST_INCLUDE_DIRS} SYSTEM)
include_directories(${glog_INCLUDE_DIRS} SYSTEM)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

set(Splice_SOURCES)
set(Splice_HEADERS)
set(Splice_TEST_SOURCES)
foreach(name
    context
    component
    io
    )
  LIST(APPEND Splice_SOURCES opencv_ai/splice/${name}.cc)
  LIST(APPEND Splice_HEADERS opencv_ai/splice/${name}.h)
  LIST(APPEND Splice_TEST_SOURCES opencv_ai/splice/${name}_test.cc)
endforeach()

add_library(Splice SHARED
  ${Splice_SOURCES}
  ${Splice_HEADERS})

target_link_libraries(Splice
  ${Boost_LIBRARIES}
  ${glog_LIBRARIES})

add_executable(SpliceTest
  opencv_ai/splice/test_main.cc
  ${Splice_TEST_SOURCES})

target_link_libraries(SpliceTest Splice ${GTEST_LIBRARY} -pthread)

gtest_add_tests(SpliceTest "" ${Splice_TEST_SOURCES})


set_property(TARGET Splice PROPERTY VERSION ${Splice_VERSION})
set_property(TARGET Splice PROPERTY SOVERSION ${Splice_MAJOR_VERSION})
set_property(TARGET Splice PROPERTY
  INTERFACE_Splice_MAJOR_VERSION ${Splice_MAJOR_VERSION})
set_property(TARGET Splice APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING Splice_MAJOR_VERSION
)

install(TARGETS Splice EXPORT SpliceTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install(
  FILES
     ${Splice_HEADERS}
  DESTINATION
    include/opencv_ai/splice
  COMPONENT
    Devel
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/Splice/SpliceConfigVersion.cmake"
  VERSION ${Splice_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT SpliceTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/Splice/SpliceTargets.cmake"
  NAMESPACE Splice::
)
configure_file(cmake/SpliceConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/Splice/SpliceConfig.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/Splice)
install(EXPORT SpliceTargets
  FILE
    SpliceTargets.cmake
  NAMESPACE
    Splice::
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    cmake/SpliceConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/Splice/SpliceConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)
