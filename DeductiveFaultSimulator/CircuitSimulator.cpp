#include "CircuitSimulator.h"
#include "DeductiveSolver.h"

#include <sstream>
#include <algorithm>

using namespace SimulatorDefines;

CircuitSimulator::CircuitSimulator()
  : gates_()
  , gateInfo_()
  , inputs_()
  , outputs_()
  , inputValues_()
  , outputValues_()
  , faultSpace_()
  , defaultFaults_{0, 1}
  , solver_(FaultList_t())
  , restrictedFaultSpace_(false)
{
}

CircuitSimulator::~CircuitSimulator()
{
  for (auto& gi: gates_)
  {
    delete gi.second;
  }
  gates_.clear();
  inputs_.clear();
  outputs_.clear();
}

void CircuitSimulator::reset()
{
  for (auto& gi: gates_)
  {
    Gate* gate = gi.second;
    gate->reset();
  }
  solver_.reset();
}

void CircuitSimulator::setRestrictedFaultSpace(const SimulatorDefines::FaultList_t &fl)
{
  faultSpace_ = fl;
}

void CircuitSimulator::addGate(const ParsedGateInfo_t& gateInfo)
{
  gateInfo_ = gateInfo;
  for (auto& gate: gateInfo_)
  {
    gates_[gate.first] = getGate(gate.second);
    gates_[gate.first]->setDeductiveSolver(&solver_);

  }
  buildCircuit(gates_, gateInfo_);
}

void CircuitSimulator::addPrimaryInputs(const PrimaryNodeList_t& inputs)
{
  inputs_ = inputs;
}

void CircuitSimulator::addPrimaryOutputs(const PrimaryNodeList_t& outputs)
{
  outputs_ = outputs;
}

void CircuitSimulator::buildCircuit(GateInfoMap_t& gates, ParsedGateInfo_t& gateInfo)
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

bool CircuitSimulator::isInputFannedIn(NodeId_t input)
{
  return gates_.find(input) != gates_.end();
}

Gate* CircuitSimulator::getFanInGate(NodeId_t input)
{
  if (isInputFannedIn(input))
  {
    return gates_.at(input);
  }
  return nullptr;
}

void CircuitSimulator::propagateConnection(Gate* gate)
{
  ParsedGateInfo& gi = gate->getInfo();

  if (!restrictedFaultSpace_)
  {
    faultSpace_[gi.output] = defaultFaults_;
    faultSpace_[gi.input1] = defaultFaults_;
    faultSpace_[gi.input2] = defaultFaults_;
  }

  if (isInputFannedIn(gi.input1))
  {
    Gate* fanInGate1 = getFanInGate(gi.input1);
    gate->setInput1Gate(fanInGate1);
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
    propagateConnection(fanInGate2);
  }
  else
  {
    gate->setInput2Gate(nullptr);
  }
}

std::string CircuitSimulator::simulateGoodCircuit(const NodeValueMap_t& inputNodeValues)
{
  std::string circuitOutput = "";
  inputValues_ = inputNodeValues;
  assignPrimaryInputs();

  for (size_t oNodeIndex = 0; oNodeIndex < outputs_.size(); ++oNodeIndex)
  {
    NodeId_t outputNode = outputs_[oNodeIndex];
    Gate* outputGate = gates_.at(outputNode);

    bool output = outputGate->getOutput();
    outputValues_[outputNode] = output;
    circuitOutput += (output ? '1' : '0');
  }
  return circuitOutput;
}

std::string CircuitSimulator::simulateFaultyCircuit()
{
  std::stringstream circuitOutput;
  FaultList_t detectedFaultList;

  solver_.setFaultSpace(faultSpace_);
  for (size_t oNodeIndex = 0; oNodeIndex < outputs_.size(); ++oNodeIndex)
  {
    NodeId_t outputNode = outputs_[oNodeIndex];
    Gate* outputGate = gates_.at(outputNode);

    FaultList_t fl = outputGate->getOutputFaultList();

    detectedFaultList.insert(fl.begin(), fl.end());
  }

  for (auto& item: detectedFaultList)
  {
    StuckAtFaults_t& fl = item.second;
    for (auto& stuckAtValue: fl)
    {
      circuitOutput << item.first << " stuck at " << ((stuckAtValue == 0) ? "0" : "1") << "";
    }
    circuitOutput << std::endl;
  }
  return circuitOutput.str();
}

void CircuitSimulator::assignPrimaryInputs()
{
  for (auto& iN: inputValues_)
  {
    NodeId_t primaryInputNode = iN.first;
    GateList_t matchingGates = getGatesWithPrimaryInputNode(primaryInputNode);
    bool primaryInputNodeValue = inputValues_.at(primaryInputNode);
    for (auto& gate: matchingGates)
    {
      ParsedGateInfo& gi = gate->getInfo();
      if (gi.input1 == primaryInputNode)
      {
        gate->setInput1Value(primaryInputNodeValue);
      }
      if (gi.input2 == primaryInputNode)
      {
        gate->setInput2Value(primaryInputNodeValue);
      }
    }
  }
}

CircuitSimulator::GateList_t CircuitSimulator::getGatesWithPrimaryInputNode(NodeId_t inputNode)
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
    if (inputNode == gi.input2)
    {
      gates.push_back(gate);
    }
  }
  return gates;
}

Gate* CircuitSimulator::getGate(const ParsedGateInfo& gi)
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

void CircuitSimulator::enableRestrictedFaultSpaceSimualtion()
{
  restrictedFaultSpace_ = true;
}
