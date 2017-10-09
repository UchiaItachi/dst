#pragma once

#include "GateType.h"
#include "GateBase.h"
#include "ParsedGateInfo.h"

#include <cstdint>
#include <map>

class Gate;

class CircuitSimulator
{
public:
  CircuitSimulator();
  ~CircuitSimulator();

  CircuitSimulator(const CircuitSimulator&) = delete;
  CircuitSimulator& operator =(const CircuitSimulator&) = delete;

  void addGate(const ParsedGateInfo_t& gateInfo);
  void addPrimaryInputs(const PrimaryNodeList_t& inputs);
  void addPrimaryOutputs(const PrimaryNodeList_t& outputs);

  std::string simulate(const NodeValueMap_t& inputNodeValues);

  void reset();

private:
  typedef uint32_t GateId_t;
  typedef std::map<GateId_t, Gate*> GateInfoMap_t;
  typedef std::vector<Gate*> GateList_t;

  Gate* getGate(const ParsedGateInfo& gi);
  GateList_t getGatesWithPrimaryInputNode(NodeId_t inputNode);
  void buildCircuit(GateInfoMap_t& gates, ParsedGateInfo_t& gateInfo);
  void propagateConnection(Gate* gate);
  void assignPrimaryInputs();


  bool isInputFannedIn(NodeId_t input);
  Gate* getFanInGate(NodeId_t input);

  GateInfoMap_t gates_;
  ParsedGateInfo_t gateInfo_;

  PrimaryNodeList_t inputs_;
  PrimaryNodeList_t outputs_;
  NodeValueMap_t inputValues_;
  NodeValueMap_t outputValues_;
};
