cmake_minimum_required(VERSION 3.8)
project(PODEM)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp DeductiveFaultSimulator.h DeductiveFaultSimulator.cpp Gate.h GateBase.h GateType.h CircuitParser.h ParsedGateInfo.h CircuitParser.cpp TestParserInfo.h TestParser.h TestParser.cpp SimulatorDefines.h GateBase.cpp BooleanValue.cpp BooleanValue.h BasicGates.cpp BasicGates.h Podem.cpp Podem.h)
add_executable(PODEM ${SOURCE_FILES})
