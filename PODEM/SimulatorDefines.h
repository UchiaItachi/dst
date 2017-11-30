#pragma once

#include "BooleanValue.h"

#include <vector>
#include <map>

typedef std::vector<uint32_t> StuckAtFaults_t;
typedef uint32_t FaultNode_t;
typedef std::map<FaultNode_t, StuckAtFaults_t> FaultList_t;

typedef uint32_t NodeId_t;
typedef std::vector<NodeId_t> PrimaryNodeList_t;
typedef std::map<NodeId_t, BooleanValue> NodeValueMap_t;

struct ParsedGateInfo;
typedef std::map<NodeId_t , ParsedGateInfo> ParsedGateInfo_t;

struct TestVector;
struct CircuitInfo;

typedef std::vector<TestVector> TestVectors_t;
typedef std::vector<CircuitInfo> Circuits_t;