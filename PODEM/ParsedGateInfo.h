#pragma once

#include "GateType.h"
#include "BooleanValue.h"

#include <cstdint>
#include <vector>
#include <map>

struct ParsedGateInfo
{
  GateType gateType;
  uint32_t output;
  uint32_t input1;
  uint32_t input2;
};
