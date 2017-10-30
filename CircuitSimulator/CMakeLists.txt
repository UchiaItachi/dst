cmake_minimum_required(VERSION 3.8)
project(CircuitSimulator)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp CircuitSimulator.h CircuitSimulator.cpp Gate.h GateBase.h GateType.h CircuitParser.h ParsedGateInfo.h CircuitParser.cpp TestParserInfo.h TestParser.h TestParser.cpp)
add_executable(CircuitSimulator ${SOURCE_FILES})