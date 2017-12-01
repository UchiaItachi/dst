#pragma once

#include "SimulatorDefines.h"
#include <map>
#include <tuple>
#include <vector>
class Gate;
class DeductiveFaultSimulator;

class Podem
{
public:
  Podem(DeductiveFaultSimulator& simulator);

  bool Podemize(NodeId_t faultyNode, bool stuckAtValue);
  void reset();

private:
  typedef bool ControllingValue_t;
  typedef bool NodeValue_t;
  typedef ControllingValue_t ControllingValueBar_t;
  typedef std::tuple<NodeId_t, ControllingValueBar_t> Objective_t;

  typedef NodeId_t PrimaryInput_t;
  typedef std::tuple<PrimaryInput_t, NodeValue_t > PrimaryInputBacktrace_t;

  Objective_t getObjective(NodeId_t faultyNode, bool stuckAtValue);
  PrimaryInputBacktrace_t backtrace(NodeId_t objectiveNode, bool value);
  void imply(PrimaryInput_t piNode, BooleanValue value);
  bool testPossible(NodeId_t faultyNode);
  bool xpathCheck(NodeId_t faultNode);
  bool isValueOfLAnX(NodeId_t line);
  bool isErrorAtPrimaryOutput();

  DeductiveFaultSimulator& simulator_;
  std::map<Gate*, std::vector<NodeId_t>> inputsForObjective_;
};