#pragma once

#include "GateType.h"

#include <cstdint>
#include <vector>
#include <map>

typedef uint32_t NodeId_t;
typedef std::vector<NodeId_t> PrimaryNodeList_t;
typedef std::map<NodeId_t, bool> NodeValueMap_t;

struct ParsedGateInfo;
typedef std::map<NodeId_t , ParsedGateInfo> ParsedGateInfo_t;


struct ParsedGateInfo
{
  GateType gateType;
  uint32_t output;
  uint32_t input1;
  uint32_t input2;
};


