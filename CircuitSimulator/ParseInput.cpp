
#include "ParseInput.h"

#include <sstream>
#include <cstdlib>

const std::regex ParseInput::INPUT_REGEX("INPUT.*");
const std::regex ParseInput::OUTPUT_REGEX("OUTPUT.*");
const std::regex ParseInput::GATE_REGEX("^\\s*(AND|OR|NOT|XOR|XNOR|INV|BUF)\\s+(\\d+)\\s+(\\d+)\\s*(\\d+)?\\s*$");


ParseInput::ParseInput(std::string fileName)
  : inputFile_(fileName)
  , inputs_()
  , outputs_()
{
  parseFile();
}

ParsedGateInfo_t& ParseInput::getParsedGates()
{
  return gates_;
}

PrimaryNodeList_t& ParseInput::getPrimaryInputs()
{
  return inputs_;
}

PrimaryNodeList_t&ParseInput::getPrimaryOutputs()
{
  return outputs_;
}

bool ParseInput::isInputLine(std::string& line)
{
  return std::regex_search(line, INPUT_REGEX);
}

bool ParseInput::isOutputLine(std::string& line)
{
  return std::regex_search(line, OUTPUT_REGEX);
}

void ParseInput::parseInput(std::string& line)
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

void ParseInput::parseOutput(std::string& line)
{
  std::istringstream iss(line);
  std::string token;
  iss >> token;

  while(iss >> token)
  {
    uint32_t value = static_cast<uint32_t>(std::strtoul(token.c_str(), nullptr, 10));
    if (value == -1)
    {
      break;
    }
    outputs_.push_back(value);
  }
}

void ParseInput::parseGate(std::string& line)
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
}

void ParseInput::parseFile()
{
  std::ifstream file(inputFile_);

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

NodeValueMap_t ParseInput::getInputNodeValues(std::string& inputValueString)
{
  if (inputValueString.size() != inputs_.size())
  {
    throw std::exception();
  }

  NodeValueMap_t nodeValues;
  for (size_t iNodeIndex = 0; iNodeIndex < inputs_.size(); ++iNodeIndex)
  {
    nodeValues[inputs_[iNodeIndex]] = (inputValueString[iNodeIndex] == '1');
  }

  return nodeValues;
}