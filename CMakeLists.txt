cmake_minimum_required(VERSION 3.1)
project(Main_Path_Analysis)

# set c++ standard to c++17
set(CMAKE_CXX_STANDARD 17)

# set compiler flags to treat warnings as errors etc.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic-errors -Werror=return-type -Wuninitialized -Wmaybe-uninitialized")

# find boost library
find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})

# add fmt library
add_subdirectory(libs/fmt)

# setup sets of files
set (CPP_FILES
	src/graph/graph.cpp
	src/graph/io/readFromGml.cpp
	src/graph/io/writeToGml.cpp

	src/model/model.cpp
	src/model/modelProperties.cpp
	src/model/performOneStep.cpp
)

set (TEST_FILES
)

# CLI programs
add_executable(simulation src/main_simulation.cpp ${CPP_FILES})
target_link_libraries(simulation fmt)
target_link_libraries(simulation ${Boost_LIBRARIES})
# target_link_libraries(compute_weights Boost::filesystem)
