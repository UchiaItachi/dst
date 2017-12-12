#pragma once

#include "SimulatorDefines.h"
#include <map>
#include <tuple>
#include <vector>
#include <algorithm>

class Gate;
class FaultSimulator;

class Podem
{
public:
  Podem(FaultSimulator& simulator);

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

  FaultSimulator& simulator_;
  std::map<Gate*, std::vector<NodeId_t>> inputsForObjective_;
  void triedNodeForObjetive(NodeId_t objectiveNode, NodeId_t inputNode)
  {
    auto& nodes = triedObjectives_[objectiveNode];
    int32_t index = nodes.getIndex(inputNode);
    if (index == -1)
    {
      nodes.nodes.push_back(inputNode);
      nodes.failed.push_back(false);
      return;
    }

    if (!nodes.failed[index])
    {
      nodes.failed[index] = false;
    }
  }

  void failedNodeForObjetive(NodeId_t objectiveNode, NodeId_t inputNode)
  {
    auto& nodes = triedObjectives_[objectiveNode];
    int32_t index = nodes.getIndex(inputNode);
    if (index == -1)
    {
      throw std::exception();
    }
    nodes.failed[index] = true;
  }

  bool isFailedNodeForObjective(NodeId_t objectiveNode, NodeId_t inputNode)
  {
    auto& nodes = triedObjectives_[objectiveNode];
    int32_t index = nodes.getIndex(inputNode);
    if (index == -1)
    {
      return false;
    }
    return nodes.failed[index];
  }
  struct NodesForObjective
  {
    int32_t getIndex(uint32_t inputNode)
    {
      auto it = std::find(nodes.begin(), nodes.end(), inputNode);
      if (it == nodes.end())
      {
        return -1;
      }
      int32_t pos = it - nodes.begin();
    }
    std::vector<NodeId_t> nodes;
    std::vector<bool> failed;
  };

  bool arePrimrayInputsImplied(NodeId_t objective);
  std::map<NodeId_t, NodesForObjective> triedObjectives_;
  bool otherNodeObjectiveFailed_;
  bool firstTime_;
};