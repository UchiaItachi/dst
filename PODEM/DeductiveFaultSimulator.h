#pragma once

#include "GateType.h"
#include "GateBase.h"
#include "ParsedGateInfo.h"
#include "Podem.h"

#include <cstdint>
#include <map>

class Gate;

class DeductiveFaultSimulator
{
public:
  DeductiveFaultSimulator();
  ~DeductiveFaultSimulator();

  DeductiveFaultSimulator(const DeductiveFaultSimulator&) = delete;
  DeductiveFaultSimulator& operator =(const DeductiveFaultSimulator&) = delete;

  void addGate(const ParsedGateInfo_t& gateInfo);
  void addPrimaryInputs(const PrimaryNodeList_t& inputs);
  void addPrimaryOutputs(const PrimaryNodeList_t& outputs);

  void reset();

  std::string orchestratePodem(NodeId_t faultyNode, bool stuckAtValue);

private:
  friend class Podem;
  typedef uint32_t GateId_t;
  typedef std::map<GateId_t, Gate*> GateInfoMap_t;
  typedef std::vector<Gate*> GateList_t;

  void evaluateDFrontier();
  Gate* getGate(const ParsedGateInfo& gi);
  GateList_t getGatesWithPrimaryInputNode(NodeId_t inputNode);
  void buildCircuit(GateInfoMap_t& gates, ParsedGateInfo_t& gateInfo);
  void propagateConnection(Gate* gate);
  void assignPrimaryInputs();
  std::string getTestVector();

  bool isInputFannedIn(NodeId_t input);
  Gate* getFanInGate(NodeId_t input);
  Gate* getGate(NodeId_t node);
  bool isGate(NodeId_t node);
  bool isPrimaryOutput(NodeId_t node);
  GateInfoMap_t gates_;
  ParsedGateInfo_t gateInfo_;

  PrimaryNodeList_t inputs_;
  PrimaryNodeList_t outputs_;

  std::vector<Gate*> dFrontierGates_;
  Podem podem_;
};
