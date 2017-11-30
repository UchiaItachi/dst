#include "CircuitParser.h"

#include <iostream>

const std::regex CircuitParser::INPUT_REGEX("INPUT.*");
const std::regex CircuitParser::OUTPUT_REGEX("OUTPUT.*");
const std::regex CircuitParser::GATE_REGEX("^\\s*(AND|OR|NOT|XOR|XNOR|INV|BUF)\\s+(\\d+)\\s+(\\d+)\\s*(\\d+)?\\s*$");


CircuitParser::CircuitParser(std::string fileName)
  : inputFile_(fileName)
  , inputs_()
  , outputs_()
{
  parseFile();
}

ParsedGateInfo_t& CircuitParser::getParsedGates()
{
  return gates_;
}

PrimaryNodeList_t& CircuitParser::getPrimaryInputs()
{
  return inputs_;
}

PrimaryNodeList_t&CircuitParser::getPrimaryOutputs()
{
  return outputs_;
}

bool CircuitParser::isInputLine(std::string& line)
{
  return std::regex_search(line, INPUT_REGEX);
}

bool CircuitParser::isOutputLine(std::string& line)
{
  return std::regex_search(line, OUTPUT_REGEX);
}

void CircuitParser::parseInput(std::string& line)
{
  std::istringstream iss(line);
  std::string token;
  iss >> token;

  while(iss >> token)
  {
    int32_t value = static_cast<int32_t>(std::strtol(token.c_str(), nullptr, 10));
    if (value == -1)
    {
      break;
    }
    inputs_.push_back(value);
  }
}

void CircuitParser::parseOutput(std::string& line)
{
  std::istringstream iss(line);
  std::string token;
  iss >> token;

  while(iss >> token)
  {
    int32_t value = static_cast<int32_t>(std::strtoul(token.c_str(), nullptr, 10));
    if (value == -1)
    {
      break;
    }
    outputs_.push_back(value);
  }
}

void CircuitParser::parseGate(std::string& line)
{
  std::istringstream iss(line);
  std::string token;
  ParsedGateInfo gi;

  iss >> token;
  gi.gateType = getGateType(token);

  if (iss >> token)
  {
    gi.input1 = static_cast<uint32_t>(std::strtoul(token.c_str(), nullptr, 10));
  }

  if ((gi.gateType != GateType::NOT) && (gi.gateType != GateType::BUFF))
  {
    iss >> token;
    gi.input2 = static_cast<uint32_t>(std::strtoul(token.c_str(), nullptr, 10));
  }

  if (iss >> token)
  {
    gi.output = static_cast<uint32_t>(std::strtoul(token.c_str(), nullptr, 10));
  }

  gates_[gi.output] = gi;
  numOfNodes_[gi.output] = BooleanValue::_X;
  numOfNodes_[gi.input1] = BooleanValue::_X;
  if ((gi.gateType != GateType::NOT) && (gi.gateType != GateType::BUFF))
  {
    numOfNodes_[gi.input2] = BooleanValue::_X;
  }
}

void CircuitParser::parseFile()
{
  std::ifstream file(inputFile_);
  if (!file.is_open())
  {
    std::cout << "\nError: Couldn't open the Circuit file : " << inputFile_ << std::endl;
    return;
  }

  for (std::string line; std::getline(file, line);)
  {

    if (line == "\r")
    {
      continue;
    }

    if (isInputLine(line))
    {
      parseInput(line);
    }
    else if(isOutputLine(line))
    {
      parseOutput(line);
    }
    else
    {
      parseGate(line);
    }
  }
}
