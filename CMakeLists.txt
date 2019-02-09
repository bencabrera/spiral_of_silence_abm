cmake_minimum_required(VERSION 3.1)
project(Main_Path_Analysis)

# set c++ standard to c++17
set(CMAKE_CXX_STANDARD 17)

# set compiler flags to treat warnings as errors etc.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic-errors -Werror=return-type -Wuninitialized -Wmaybe-uninitialized")

# find boost library
find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})

# find threading libraries
find_package( Threads )

# add fmt library
add_subdirectory(libs/fmt)

# setup sets of files
set (CPP_FILES
	src/graph/graph.cpp
	src/graph/io/readFromGml.cpp
	src/graph/io/writeToGml.cpp
	src/graph/graphHelper.cpp

	src/model/model.cpp
	src/model/modelProperties.cpp
	src/model/modelToGraphviz.cpp

	src/generation/generation.cpp
	src/generation/generationParams.cpp
	src/generation/botAdding.cpp
	src/generation/helpers.cpp

	src/generation/preferentialAttachment.cpp
	src/generation/erdoesRenyi.cpp
	src/generation/stochasticBlockModel.cpp

	src/simulation/performOneStep.cpp
	src/simulation/simulateUntilStable.cpp
	src/simulation/writeSimulationResultsToCsv.cpp
)

set (TEST_FILES
	src/graph/tests/testGraphIo.cpp
	src/model/tests/testModelIo.cpp
	src/generation/tests/testGenerationParams.cpp
	src/generation/tests/testHelpers.cpp
	src/generation/tests/testGenerate.cpp
	src/simulation/tests/testPerformOneStep.cpp
)

# programs
add_executable(simulation src/main_simulation.cpp ${CPP_FILES})
target_link_libraries(simulation fmt)
target_link_libraries(simulation ${Boost_LIBRARIES})

add_executable(extended_simulation src/main_extendedSimulation.cpp ${CPP_FILES})
target_link_libraries(extended_simulation fmt)
target_link_libraries(extended_simulation ${Boost_LIBRARIES})
target_link_libraries(extended_simulation ${Boost_LIBRARIES})
target_link_libraries(extended_simulation ${CMAKE_THREAD_LIBS_INIT})

add_executable(generation src/main_generation.cpp ${CPP_FILES})
target_link_libraries(generation fmt)
target_link_libraries(generation ${Boost_LIBRARIES})

add_executable(compute_degree_dists src/main_computeDegreeDists.cpp ${CPP_FILES})
target_link_libraries(compute_degree_dists fmt)
target_link_libraries(compute_degree_dists ${Boost_LIBRARIES})

# tests
add_executable(tests src/tests/main_tests.cpp ${TEST_FILES} ${CPP_FILES})
target_link_libraries(tests fmt)
target_link_libraries(tests ${Boost_LIBRARIES})
target_compile_definitions(tests PRIVATE TEST_MODELS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/models/")
