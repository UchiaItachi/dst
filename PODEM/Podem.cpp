//
// Created by itachi on 11/29/17.
//

#include "Podem.h"
#include "DeductiveFaultSimulator.h"
#include <algorithm>

Podem::Podem(DeductiveFaultSimulator& simulator)
  : simulator_(simulator)
{

}

void Podem::reset()
{
  inputsForObjective_.clear();
}

bool Podem::Podemize(NodeId_t faultyNode, bool stuckAtValue)
{
  if (isErrorAtPrimaryOutput())
  {
    return true;
  }

  if (!testPossible(faultyNode))
  {
    return false;
  }

  Objective_t objective = getObjective(faultyNode, stuckAtValue);
  PrimaryInputBacktrace_t piBackTrace = backtrace(std::get<0>(objective), std::get<1>(objective));

  BooleanValue implyValue = std::get<1>(piBackTrace) ? BooleanValue::_1 : BooleanValue::_0;
  NodeId_t piNode = std::get<0>(piBackTrace);
  imply(piNode, implyValue);

  if (Podemize(faultyNode, stuckAtValue))
  {
    return true;
  }

  imply(piNode, !implyValue);

  if (Podemize(faultyNode, stuckAtValue))
  {
    return true;
  }

  imply(piNode, BooleanValue::_X);

  return false;
}

bool Podem::isValueOfLAnX(NodeId_t faultyLine)
{
  bool isPrimaryInputNode = !simulator_.isInputFannedIn(faultyLine);
  Gate *gate = nullptr;
  if (isPrimaryInputNode)
  {
    DeductiveFaultSimulator::GateList_t gatelist = simulator_.getGatesWithPrimaryInputNode(faultyLine);
    gate = gatelist[0];
  }
  else
  {
    gate = simulator_.getFanInGate(faultyLine);
  }

  ParsedGateInfo& info = gate->getInfo();
  if (isPrimaryInputNode)
  {
    if (info.input1 == faultyLine)
    {
      return gate->getInput1() == BooleanValue::_X;
    }
    else if (info.input2 == faultyLine)
    {
      return gate->getInput2() == BooleanValue::_X;
    }
    else
    {
      throw std::exception();
    }
  }
  else
  {
    return gate->getOutputReval() == BooleanValue::_X;
  }

  return false;
}

Podem::Objective_t Podem::getObjective(NodeId_t faultyNode, bool stuckAtValue)
{
  if (isValueOfLAnX(faultyNode))
  {
    return std::make_tuple(faultyNode, !stuckAtValue);
  }

  if (simulator_.dFrontierGates_.empty())
  {
    throw std::exception();
  }

  Gate *dFrontierGate = nullptr;
  Objective_t objective;
  auto selectAnInputWithX = [&] (NodeId_t inputNode, BooleanValue inputNodeValue, bool cValue) -> bool
  {
    if (inputNodeValue == BooleanValue::_X)
    {
      objective = std::make_tuple(inputNode, !cValue);
      return true;
      auto& takenInputsOfGate = inputsForObjective_[dFrontierGate];
      auto posIt = std::find(takenInputsOfGate.begin(), takenInputsOfGate.end(), inputNode);
      if (posIt == takenInputsOfGate.end())
      {
        takenInputsOfGate.push_back(inputNode);
        objective = std::make_tuple(inputNode, !cValue);
        return true;
      }
      return true;
    }
    return false;
  };


  for (auto& gate: simulator_.dFrontierGates_)
  {
    if (inputsForObjective_.find(gate) == inputsForObjective_.end())
    {
      inputsForObjective_[gate] = {};
    }

    dFrontierGate = gate;
    const auto& info = dFrontierGate->getInfo();
    bool cValue = dFrontierGate->getControllingValue();
    NodeId_t inputNode;
    BooleanValue inputValue;

    inputNode = info.input1;
    inputValue = dFrontierGate->getInput1();
    if (selectAnInputWithX(inputNode, inputValue, cValue))
    {
      return objective;
    }

    inputNode = info.input2;
    inputValue = dFrontierGate->getInput2();
    if (selectAnInputWithX(inputNode, inputValue, cValue))
    {
      return objective;
    }
  }

  throw std::exception();
}

Podem::PrimaryInputBacktrace_t Podem::backtrace(NodeId_t objectiveNode, bool value)
{
  bool finalValue = value;
  while (simulator_.isGate(objectiveNode))
  {
    Gate* gate = simulator_.getGate(objectiveNode);
    bool inversionParity = gate->getInversionParity();

    if (gate->getInput1() == BooleanValue::_X)
    {
      finalValue = (finalValue && (!inversionParity)) || ((!finalValue) && inversionParity);
      objectiveNode = gate->getInfo().input1;
    }
    else if (gate->getInput2() == BooleanValue::_X)
    {
      finalValue = (finalValue && (!inversionParity)) || ((!finalValue) && inversionParity);
      objectiveNode = gate->getInfo().input2;
    }
  }

  PrimaryInputBacktrace_t pi = std::make_tuple(objectiveNode, finalValue);

  return pi;
}

void Podem::imply(Podem::PrimaryInput_t piNode, BooleanValue value)
{
  DeductiveFaultSimulator::GateList_t piGates = simulator_.getGatesWithPrimaryInputNode(piNode);

  for (auto& gate: piGates)
  {
    if (gate->getInfo().input1 == piNode)
    {
      gate->setInput1Value(value);
    }

    if (gate->getInfo().input2 == piNode)
    {
      gate->setInput2Value(value);
    }
  }

  PrimaryNodeList_t& poNodes = simulator_.outputs_;
  for (auto& poNode: poNodes)
  {
    Gate* poGate = simulator_.getGate(poNode);
    poGate->getOutputReval();
  }
  simulator_.evaluateDFrontier();
}

bool Podem::testPossible(NodeId_t faultyNode)
{
  if (simulator_.dFrontierGates_.empty())
  {
     return xpathCheck(faultyNode);
  }

  for (auto& dGate: simulator_.dFrontierGates_)
  {
    NodeId_t xpathCheckFromNode = ~0x0;

    BooleanValue input1 = dGate->getInput1();
    BooleanValue input2 = dGate->getInput2();

    if ((input1 == BooleanValue::_D) || (input1 == BooleanValue::_D_Bar))
    {
      xpathCheckFromNode = dGate->getInfo().input1;
    }
    else if ((input2 == BooleanValue::_D) || (input2 == BooleanValue::_D_Bar))
    {
      xpathCheckFromNode = dGate->getInfo().input2;
    }
    else
    {
      throw std::exception();
    }

    if (xpathCheck(xpathCheckFromNode))
    {
      return true;
    }
  }
  return false;
}

bool Podem::xpathCheck(NodeId_t checkFromNode)
{
  Gate* faultyGate = simulator_.getGate(checkFromNode);
  std::vector<Gate*> fanoutGates;
  if (faultyGate)
  {
    fanoutGates = faultyGate->getOutputFanoutGates();
  }
  else
  {
    fanoutGates = simulator_.getGatesWithPrimaryInputNode(checkFromNode);
  }

  if (fanoutGates.empty() && faultyGate)
  {
    NodeId_t oNode = faultyGate->getInfo().output;
    bool isPrimaryOutputNode = simulator_.isPrimaryOutput(oNode);
    bool isOutputX = (BooleanValue::_X) == faultyGate->getOutputReval();

    return isOutputX && isPrimaryOutputNode;
  }
  for (auto &gate: fanoutGates)
  {
    const BooleanValue output = gate->getOutputReval();
    if (output == BooleanValue::_X)
    {
      NodeId_t nextCheckNode = gate->getInfo().output;
      if (xpathCheck(nextCheckNode))
      {
        return true;
      }
    }
  }
  return false;
}

bool Podem::isErrorAtPrimaryOutput()
{
  for (auto& outputNode: simulator_.outputs_)
  {
    Gate* poGate = simulator_.getGate(outputNode);
    BooleanValue poValue = poGate->getOutputReval();
    if ((poValue == BooleanValue::_D) || (poValue == BooleanValue::_D_Bar))
    {
      return true;
    }
  }
  return false;
}