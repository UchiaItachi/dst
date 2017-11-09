#include "DeductiveSolver.h"

#include "Gate.h"
#include "ParsedGateInfo.h"

#include <algorithm>
#include <iostream>

using namespace SimulatorDefines;

DeductiveSolver::DeductiveSolver(const FaultList_t& faultSpace)
  : faultSpace_(faultSpace)
{
}

void DeductiveSolver::reset()
{
  faultSpace_.clear();
}
void DeductiveSolver::setFaultSpace(SimulatorDefines::FaultList_t& fl)
{
  faultSpace_ = fl;
}

bool DeductiveSolver::hasAControllingValue(Gate* gate) const
{
  const bool controllingValue = gate->getControllingValue();
  if (gate->hasInput1() && (controllingValue == gate->getInput1()))
  {
    return true;
  }
  else if (gate->hasInput2() && (controllingValue == gate->getInput2()))
  {
    return true;
  }
  else
  {
    return false;
  }
}

FaultList_t DeductiveSolver::uniteFaultLists(const FaultList_t& f1, const FaultList_t& f2)
{
  FaultList_t output(f1);
  for (auto& item_fl2: f2)
  {
    const StuckAtFaults_t& fl2 = item_fl2.second;
    for (auto value_2: fl2)
    {
      StuckAtFaults_t& fl1 = output[item_fl2.first];
      auto it = std::find(fl1.begin(), fl1.end(), value_2);
      if (it == fl1.end())
      {
        fl1.push_back(value_2);
      }
    }
  }
  return output;
}

FaultList_t DeductiveSolver::intersectFaultLists(const FaultList_t& f1, const FaultList_t& f2)
{
  FaultList_t output;
  for (auto& item: f1)
  {
    if (f2.end() == f2.find(item.first))
    {
      continue;
    }
    StuckAtFaults_t& op = output[item.first];
    const StuckAtFaults_t& fl_1 = item.second;
    const StuckAtFaults_t& fl_2 = f2.at(item.first);

    for (auto& value_2: fl_2)
    {
      auto it = std::find(fl_1.begin(), fl_1.end(), value_2);
      if (it != fl_1.end())
      {
        op.push_back(value_2);
      }
    }
  }
  return output;
}

FaultList_t DeductiveSolver::deductFaultLists(const FaultList_t& from, const FaultList_t& to)
{
  FaultList_t output(from);

  for (auto& item_to: to)
  {
    auto it = output.find(item_to.first);
    if (it != output.end())
    {
      const StuckAtFaults_t& fl_to = item_to.second;
      StuckAtFaults_t& fl_op = output[item_to.first];
      for (auto& value_to: fl_to)
      {
        auto it = std::find(fl_op.begin(), fl_op.end(), value_to);
        if (it != fl_op.end())
        {
          fl_op.erase(it);
        }
      }
      if (fl_op.empty())
      {
        output.erase(it);
      }
    }
  }
  return output;
}

FaultList_t DeductiveSolver::solveWithControllingValue(Gate* gate)
{

  FaultList_t output;
  const FaultList_t& i1Fl = gate->getInput1FaultList();
  const FaultList_t& i2Fl = gate->getInput2FaultList();

  const bool input1Value = gate->getInput1();
  const bool input2Value = gate->getInput2();
  const bool cv = gate->getControllingValue();

  if (gate->hasInput2())
  {
    if ((input1Value == cv) && (input2Value == cv))
    {
      output = intersectFaultLists(gate->getInput1FaultList(), gate->getInput2FaultList());
    }
    else if (input1Value == cv)
    {
      output = deductFaultLists(gate->getInput1FaultList(), gate->getInput2FaultList());
    }
    else if (input2Value == cv)
    {
      output = deductFaultLists(gate->getInput2FaultList(), gate->getInput1FaultList());
    }
    else
    {
      throw std::exception();
    }
  }
  else
  {
    if (input1Value != cv)
    {
      throw std::exception();
    }
    output = gate->getInput1FaultList();
  }

  ParsedGateInfo& info = gate->getInfo();

  uint32_t outputNode = info.output;
  uint32_t stuckAtOutput = xorAAndB(!gate->getControllingValue(), gate->getInversionParity());
  FaultList_t opFaultList;
  if (!isFaultInFaultSpace(outputNode, stuckAtOutput))
  {
    return output;
  }

  StuckAtFaults_t& fl = opFaultList[outputNode];
  fl.push_back(stuckAtOutput);
  output = uniteFaultLists(output, opFaultList);
  return output;
}

FaultList_t DeductiveSolver::solveWithoutControllingValue(Gate* gate)
{
  FaultList_t output;
  const FaultList_t& i1Fl = gate->getInput1FaultList();
  const FaultList_t& i2Fl = gate->getInput2FaultList();

  if (gate->hasInput2())
  {
    output = uniteFaultLists(i1Fl, i2Fl);
  }
  else
  {
    output = i1Fl;
  }

  ParsedGateInfo& info = gate->getInfo();

  uint32_t outputNode = info.output;
  uint32_t outputStuckAt = xorAAndB(gate->getControllingValue(), gate->getInversionParity());
  if (isFaultInFaultSpace(outputNode, outputStuckAt))
  {
    FaultList_t op;
    StuckAtFaults_t& fl = op[outputNode];
    auto it = std::find(fl.begin(), fl.end(), outputStuckAt);
    if (it == fl.end())
    {
      fl.push_back(outputStuckAt);
    }
    output = uniteFaultLists(output, op);
  }

  return output;
}

uint32_t DeductiveSolver::xorAAndB(bool a, bool b)
{
  uint32_t result = ((a && (!b)) || ((!a) && b)) ? 1 : 0;
  return result;
}

FaultList_t DeductiveSolver::solve(Gate *gate)
{
  if (hasAControllingValue(gate))
  {
    return solveWithControllingValue(gate);
  }
  else
  {
    return solveWithoutControllingValue(gate);
  }
}

bool DeductiveSolver::isFaultInFaultSpace(uint32_t node, uint32_t stuckAtFault)
{
  {
    auto it = faultSpace_.find(node);
    if (it == faultSpace_.end())
    {
      return false;
    }
  }

  StuckAtFaults_t& fl = faultSpace_[node];
  auto it = std::find(fl.begin(), fl.end(), stuckAtFault);
  return it != fl.end();
}
