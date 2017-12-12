//
// Created by itachi on 11/29/17.
//

#include "Podem.h"
#include "FaultSimulator.h"
#include <algorithm>

Podem::Podem(FaultSimulator& simulator)
  : simulator_(simulator)
  , firstTime_(true)
  , triedObjectives_()
  , otherNodeObjectiveFailed_(false)
{

}

void Podem::reset()
{
  inputsForObjective_.clear();
  triedObjectives_.clear();
  otherNodeObjectiveFailed_ = false;\

  firstTime_ = true;
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

  firstTime_ = false;
  Objective_t objective = getObjective(faultyNode, stuckAtValue);

  PrimaryInputBacktrace_t piBackTrace = backtrace(std::get<0>(objective), std::get<1>(objective));
  BooleanValue implyValue = std::get<1>(piBackTrace) ? BooleanValue::_1 : BooleanValue::_0;
  NodeId_t piNode = std::get<0>(piBackTrace);
  triedNodeForObjetive(std::get<0>(objective), piNode);
  imply(piNode, implyValue);

  if (Podemize(faultyNode, stuckAtValue))
  {
    return true;
  }

  imply(piNode, !implyValue);
  triedNodeForObjetive(std::get<0>(objective), piNode);

  if (Podemize(faultyNode, stuckAtValue))
  {
    return true;
  }

  imply(piNode, BooleanValue::_X);

  failedNodeForObjetive(std::get<0>(objective), piNode);
  return false;
}

bool Podem::isValueOfLAnX(NodeId_t faultyLine)
{
  bool isPrimaryInputNode = !simulator_.isInputFannedIn(faultyLine);
  Gate *gate = nullptr;
  if (isPrimaryInputNode)
  {
    FaultSimulator::GateList_t gatelist = simulator_.getGatesWithPrimaryInputNode(faultyLine);
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

  if (simulator_.dFrontierGates_.empty() && (!firstTime_))
  {
    //std::cout << "D forntier empty ..... " << std::endl;
    throw std::exception();
  }

  Gate *dFrontierGate = nullptr;
  Objective_t objective;
  auto selectAnInputWithX = [&] (NodeId_t inputNode, BooleanValue inputNodeValue, bool cValue) -> bool
  {
    if (inputNodeValue == BooleanValue::_X)
    {
     // if (!triedObjectives_[inputNode])
      {
        objective = std::make_tuple(inputNode, !cValue);
        return true;
      }
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
  //return objective;
  std::cout << "No objective found ..... " << std::endl;
  throw std::exception();
}

Podem::PrimaryInputBacktrace_t Podem::backtrace(NodeId_t objectiveNode, bool value)
{
  bool finalValue = value;
  NodeId_t inputNode = objectiveNode;
  while (simulator_.isGate(inputNode))
  {
    //std::cout << "bt....\n";
    Gate* gate = simulator_.getGate(inputNode);
    bool inversionParity = gate->getInversionParity();
    GateType gateType = gate->getInfo().gateType;
    if (gate->getInput1() == BooleanValue::_X)
    {
      NodeId_t temp = gate->getInfo().input1;
     // if (!isFailedNodeForObjective(objectiveNode, temp))
      {
        finalValue = (finalValue && (!inversionParity)) || ((!finalValue) && inversionParity);
        inputNode = gate->getInfo().input1;
        continue;
      }
    }

    if ((gateType != GateType::BUFF) && (gateType != GateType::NOT))
    {
      if (gate->getInput2() == BooleanValue::_X)
      {
        NodeId_t temp = gate->getInfo().input2;
       // if (!isFailedNodeForObjective(objectiveNode, temp))
        {
          finalValue = (finalValue && (!inversionParity)) || ((!finalValue) && inversionParity);
          inputNode = gate->getInfo().input2;
          continue;
        }
      }
    }
  }

  if (simulator_.isGate(objectiveNode))
  {
    Gate *gate = simulator_.getGate(objectiveNode);
    auto& nodes = inputsForObjective_[gate];
    auto it = std::find (nodes.begin(), nodes.end(), inputNode);
    if (it == nodes.end())
    {
      nodes.push_back(inputNode);
    }
  }
  PrimaryInputBacktrace_t pi = std::make_tuple(inputNode, finalValue);

  return pi;
}

void Podem::imply(Podem::PrimaryInput_t piNode, BooleanValue value)
{
  FaultSimulator::GateList_t piGates = simulator_.getGatesWithPrimaryInputNode(piNode);

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
  if (firstTime_)
  {
    return true;
  }

  if (simulator_.dFrontierGates_.empty())
  {
     if (isValueOfLAnX(faultyNode))
     {
       return true;
     }
     return false;
  }

  if (arePrimrayInputsImplied(faultyNode))
  {
    return false;
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

bool Podem::arePrimrayInputsImplied(NodeId_t objective)
{
  PrimaryNodeList_t gl = simulator_.getPrimaryInputsForObjective(objective);

  for (auto& pi: gl)
  {
    if (!isFailedNodeForObjective(objective, pi))
    {
      return false;
    }
  }
  return true;
}