#pragma once

#include "SimulatorDefines.h"

class Gate;

class DeductiveSolver
{
public:
  DeductiveSolver(const SimulatorDefines::FaultList_t& faultSpace);
  ~DeductiveSolver() = default;

  SimulatorDefines::FaultList_t solve(Gate* gate);

  void reset();
  void setFaultSpace(SimulatorDefines::FaultList_t& fl);

  bool isFaultInFaultSpace(uint32_t node, uint32_t stuckAtFault);

private:
  bool hasAControllingValue(Gate* gate) const;

  SimulatorDefines::FaultList_t solveWithControllingValue(Gate* gate);
  SimulatorDefines::FaultList_t solveWithoutControllingValue(Gate* gate);

  SimulatorDefines::FaultList_t uniteFaultLists(const SimulatorDefines::FaultList_t& f1, const SimulatorDefines::FaultList_t& f2);
  SimulatorDefines::FaultList_t intersectFaultLists(const SimulatorDefines::FaultList_t& f1, const SimulatorDefines::FaultList_t& f2);
  SimulatorDefines::FaultList_t deductFaultLists(const SimulatorDefines::FaultList_t& from, const SimulatorDefines::FaultList_t& to);

  uint32_t xorAAndB(bool a, bool b);

  SimulatorDefines::FaultList_t faultSpace_;

};