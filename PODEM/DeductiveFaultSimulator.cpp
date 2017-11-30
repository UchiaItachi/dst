#include "DeductiveFaultSimulator.h"
#include "BasicGates.h"
#include "Podem.h"

#include <sstream>
#include <algorithm>

DeductiveFaultSimulator::DeductiveFaultSimulator()
  : gates_()
  , gateInfo_()
  , inputs_()
  , outputs_()
  , podem_(*this)
{
}

DeductiveFaultSimulator::~DeductiveFaultSimulator()
{
  for (auto& gi: gates_)
  {
    delete gi.second;
  }
  gates_.clear();
  inputs_.clear();
  outputs_.clear();
}

void DeductiveFaultSimulator::reset()
{
  for (auto& gi: gates_)
  {
    Gate* gate = gi.second;
    gate->reset();
  }
}

void DeductiveFaultSimulator::addGate(const ParsedGateInfo_t& gateInfo)
{
  gateInfo_ = gateInfo;
  for (auto& gate: gateInfo_)
  {
    gates_[gate.first] = getGate(gate.second);
  }
  buildCircuit(gates_, gateInfo_);
}

void DeductiveFaultSimulator::addPrimaryInputs(const PrimaryNodeList_t& inputs)
{
  inputs_ = inputs;
}

void DeductiveFaultSimulator::addPrimaryOutputs(const PrimaryNodeList_t& outputs)
{
  outputs_ = outputs;
}

void DeductiveFaultSimulator::buildCircuit(GateInfoMap_t& gates, ParsedGateInfo_t& gateInfo)
{
  if (outputs_.empty())
  {
    std::cout << "Error: first populate the output list " << std::endl;
    throw std::exception();
  }

  for (auto outputNode: outputs_)
  {
    Gate *outputGate = gates_[outputNode];
    propagateConnection(outputGate);
  }
}

bool DeductiveFaultSimulator::isInputFannedIn(NodeId_t input)
{
  return gates_.find(input) != gates_.end();
}

bool DeductiveFaultSimulator::isGate(NodeId_t node)
{
  return isInputFannedIn(node);
}

Gate* DeductiveFaultSimulator::getGate(NodeId_t node)
{
  return getFanInGate(node);
}

Gate* DeductiveFaultSimulator::getFanInGate(NodeId_t input)
{
  if (isInputFannedIn(input))
  {
    return gates_.at(input);
  }
  return nullptr;
}

bool DeductiveFaultSimulator::isPrimaryOutput(NodeId_t node)
{
  auto it = std::find(outputs_.begin(), outputs_.end(), node);
  return it != outputs_.end();
}

void DeductiveFaultSimulator::propagateConnection(Gate* gate)
{
  ParsedGateInfo& gi = gate->getInfo();

  if (isInputFannedIn(gi.input1))
  {
    Gate* fanInGate1 = getFanInGate(gi.input1);
    gate->setInput1Gate(fanInGate1);
    fanInGate1->addOutputFanOutGate(gate);
    propagateConnection(fanInGate1);
  }
  else
  {
    gate->setInput1Gate(nullptr);
  }

  if (isInputFannedIn(gi.input2) && (gi.gateType != GateType::NOT) && (gi.gateType != GateType::BUFF))
  {
    Gate* fanInGate2 = getFanInGate(gi.input2);
    gate->setInput2Gate(fanInGate2);
    fanInGate2->addOutputFanOutGate(gate);
    propagateConnection(fanInGate2);
  }
  else
  {
    gate->setInput2Gate(nullptr);
  }
}

std::string DeductiveFaultSimulator::orchestratePodem(NodeId_t faultyNode, bool stuckAtValue)
{
  std::string circuitOutput = "";
  assignPrimaryInputs();
  if (podem_.Podemize(faultyNode, stuckAtValue))
  {
    circuitOutput = getTestVector();
  }
  else
  {
    circuitOutput = "Undetectable Fault";
  }
  return circuitOutput;
}

void DeductiveFaultSimulator::assignPrimaryInputs()
{
  for (auto& inputNode: inputs_)
  {
    NodeId_t primaryInputNode = inputNode;
    GateList_t matchingGates = getGatesWithPrimaryInputNode(primaryInputNode);
    // bool primaryInputNodeValue = inputValues_.at(primaryInputNode); TODO: revisit & clean-up
    BooleanValue initialPrimaryInputValue = BooleanValue::_X;
    for (auto& gate: matchingGates)
    {
      ParsedGateInfo& gi = gate->getInfo();
      if (gi.input1 == primaryInputNode)
      {
        gate->setInput1Value(initialPrimaryInputValue);
      }
      if (gi.input2 == primaryInputNode)
      {
        gate->setInput2Value(initialPrimaryInputValue);
      }
    }
  }
}

DeductiveFaultSimulator::GateList_t DeductiveFaultSimulator::getGatesWithPrimaryInputNode(NodeId_t inputNode)
{
  GateList_t gates;
  for (auto& g: gates_)
  {
    Gate* gate = g.second;
    ParsedGateInfo& gi = gate->getInfo();
    if (inputNode == gi.input1)
    {
      gates.push_back(gate);
    }
    else if (inputNode == gi.input2)
    {
      gates.push_back(gate);
    }
  }
  return gates;
}

Gate* DeductiveFaultSimulator::getGate(const ParsedGateInfo& gi)
{
  Gate *gate = nullptr;

  switch (gi.gateType)
  {
    case GateType::AND:
      gate = new AndGate(gi);
      break;
    case GateType::OR:
      gate = new OrGate(gi);
      break;
    case GateType::NOT:
      {
        ParsedGateInfo info = gi;
        info.input2 = 0;
        gate = new NotGate(info);
      }
      break;
    case GateType::NAND:
      gate = new NandGate(gi);
      break;
    case GateType::NOR:
      gate = new NorGate(gi);
      break;
    case GateType::XOR:
      gate = new XorGate(gi);
      break;
    case GateType::XNOR:
      gate = new XnorGate(gi);
      break;
    case GateType::BUFF:
      {
        ParsedGateInfo info = gi;
        info.input2 = 0;
        gate = new BufGate(info);
      }
      break;
    default:
      throw std::exception();
  }
  if ((gi.gateType == GateType::BUFF) || (gi.gateType == GateType::NOT))
  {
    auto it = std::find(inputs_.begin(), inputs_.end(), gi.input1);
    if (it != inputs_.end())
    {
      gate->markAsPrimaryGate();
    }
  }
  else
  {
    auto it1 = std::find(inputs_.begin(), inputs_.end(), gi.input1);
    auto it2 = std::find(inputs_.begin(), inputs_.end(), gi.input2);
    if ((it1 != inputs_.end()) && (it2 != inputs_.end()))
    {
      gate->markAsPrimaryGate();
    }
  }
  return gate;
}

void DeductiveFaultSimulator::evaluateDFrontier()
{
  dFrontierGates_.clear();
  for (auto& gate: gates_)
  {
    if (gate.second->isGateInDFrontier())
    {
      dFrontierGates_.push_back(gate.second);
    }
  }
}

std::string DeductiveFaultSimulator::getTestVector()
{
  std::string testVector = "";

  for (auto& inputNode: inputs_)
  {
    GateList_t gates = getGatesWithPrimaryInputNode(inputNode);
    Gate* gate = gates[0];
    auto info = gate->getInfo();

    if (inputNode == info.input1)
    {
      testVector += getBooleanValueString(gate->getInput1());
    }
    else if (inputNode == info.input2)
    {
      testVector += getBooleanValueString(gate->getInput2());
    }
    else
    {
      throw std::exception();
    }
  }
  return testVector;
}