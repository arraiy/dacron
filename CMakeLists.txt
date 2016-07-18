cmake_minimum_required(VERSION 3.0)
project(Dacron)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(FindUBSan)
include(FindTSan)
include(FindASan)
include(FindMSan)
include(CTest)
enable_testing()

set(Dacron_MAJOR_VERSION 0)
set(Dacron_MINOR_VERSION 0)
set(Dacron_PATCH_VERSION 0)

set(Dacron_VERSION
  ${Dacron_MAJOR_VERSION}.${Dacron_MINOR_VERSION}.${Dacron_MINOR_VERSION})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")


find_package(Boost REQUIRED program_options filesystem system regex)
find_package(GTest REQUIRED)
find_package(gflags REQUIRED)
find_package(glog REQUIRED)

include_directories(${Boost_INCLUDE_DIRS} SYSTEM)
include_directories(${GTEST_INCLUDE_DIRS} SYSTEM)
include_directories(${glog_INCLUDE_DIRS} SYSTEM)
include_directories(${gflags_INCLUDE_DIRS} SYSTEM)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

set(Dacron_SOURCES)
set(Dacron_HEADERS)
set(Dacron_TEST_SOURCES)
foreach(name
    context
    component
    io
    tcp
    )
  LIST(APPEND Dacron_SOURCES dacron/${name}.cc)
  LIST(APPEND Dacron_HEADERS dacron/${name}.h)
  LIST(APPEND Dacron_TEST_SOURCES dacron/${name}_test.cc)
endforeach()

add_library(Dacron SHARED
  ${Dacron_SOURCES}
  ${Dacron_HEADERS})

target_link_libraries(Dacron
  ${Boost_LIBRARIES}
  ${glog_LIBRARIES})

add_executable(DacronTest
  dacron/test_main.cc
  ${Dacron_TEST_SOURCES})

target_link_libraries(DacronTest Dacron ${GTEST_LIBRARY} ${glog_LIBRARIES} glog ${gflags_LIBRARIES} -pthread)

gtest_add_tests(DacronTest "" ${Dacron_TEST_SOURCES})


set_property(TARGET Dacron PROPERTY VERSION ${Dacron_VERSION})
set_property(TARGET Dacron PROPERTY SOVERSION ${Dacron_MAJOR_VERSION})
set_property(TARGET Dacron PROPERTY
  INTERFACE_Dacron_MAJOR_VERSION ${Dacron_MAJOR_VERSION})
set_property(TARGET Dacron APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING Dacron_MAJOR_VERSION
)

install(TARGETS Dacron EXPORT DacronTargets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

install(
  FILES
     ${Dacron_HEADERS}
  DESTINATION
    include/dacron
  COMPONENT
    Devel
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/Dacron/DacronConfigVersion.cmake"
  VERSION ${Dacron_VERSION}
  COMPATIBILITY AnyNewerVersion
)

export(EXPORT DacronTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/Dacron/DacronTargets.cmake"
  NAMESPACE Dacron::
)
configure_file(cmake/DacronConfig.cmake
  "${CMAKE_CURRENT_BINARY_DIR}/Dacron/DacronConfig.cmake"
  COPYONLY
)

set(ConfigPackageLocation lib/cmake/Dacron)
install(EXPORT DacronTargets
  FILE
    DacronTargets.cmake
  NAMESPACE
    Dacron::
  DESTINATION
    ${ConfigPackageLocation}
)
install(
  FILES
    cmake/DacronConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/Dacron/DacronConfigVersion.cmake"
  DESTINATION
    ${ConfigPackageLocation}
  COMPONENT
    Devel
)
