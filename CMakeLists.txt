cmake_minimum_required(VERSION 3.5)

project(simulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.81 REQUIRED CONFIG HINTS /usr/local/Cellar/boost/1.81.0_1/lib/cmake)
if(Boost_FOUND)
    message("Boost_INCLUDE_DIRS=${Boost_INCLUDE_DIRS}")
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_executable(simulator
    Simulator.cpp
    Circuit.h
    Gate.h
    Simulation.h
    TruthTable.h
    GateType.h
    Circuit.cpp
    Gate.cpp
    Simulation.cpp
    TruthTable.cpp
)

install(TARGETS simulator
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
