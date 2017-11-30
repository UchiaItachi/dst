#pragma once

#include <string>

enum class GateType
{
  AND,
  OR,
  NOT,
  NOR,
  NAND,
  XOR,
  XNOR,
  BUFF
};

inline GateType getGateType(const std::string& gateStr)
{
  if (gateStr == "AND")
  {
    return GateType::AND;
  }
  else if (gateStr == "OR")
  {
    return GateType::OR;
  }
  else if (gateStr == "INV")
  {
    return GateType::NOT;
  }
  else if (gateStr == "NOR")
  {
    return GateType::NOR;
  }
  else if (gateStr == "NAND")
  {
    return GateType::NAND;
  }
  else if (gateStr == "XOR")
  {
    return GateType::XOR;
  }
  else if (gateStr == "XNOR")
  {
    return GateType::XNOR;
  }
  else if (gateStr == "BUF")
  {
    return GateType::BUFF;
  }
  else
  {
    throw std::exception();
  }
}