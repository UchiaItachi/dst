#pragma once

#include "ParsedGateInfo.h"

#include <vector>
#include <string>
#include <fstream>
#include <regex>


class CircuitParser
{
public:
  CircuitParser(std::string fileName);

  ParsedGateInfo_t& getParsedGates();
  PrimaryNodeList_t& getPrimaryInputs();
  PrimaryNodeList_t& getPrimaryOutputs();

  NodeValueMap_t getInputNodeValues(std::string& inputValueString);

private:
  void parseFile();
  void parseInput(std::string& line);
  void parseOutput(std::string& line);
  void parseGate(std::string& line);

  bool isInputLine(std::string& line);
  bool isOutputLine(std::string& line);

  void constructFaultSpace();

  std::string inputFile_;

  ParsedGateInfo_t gates_;
  PrimaryNodeList_t inputs_;
  PrimaryNodeList_t outputs_;

  static const std::regex INPUT_REGEX;
  static const std::regex OUTPUT_REGEX;
  static const std::regex GATE_REGEX;
};