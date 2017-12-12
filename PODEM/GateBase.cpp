#include "GateBase.h"

#include <algorithm>

GateBase::GateBase(ParsedGateInfo gi)
  : gateInfo_(gi)
  , outputAvailable_(false)
  , input1Value_(BooleanValue::_X)
  , input2Value_(BooleanValue::_X)
  , outputValue_(BooleanValue::_X)
  , isPrimaryGate_(false)
  , isInputFaulty_(false)
  , isOutputFaulty_(false)
  , faultyNode_(~0x0)
  , input1Gate_(nullptr)
  , input2Gate_(nullptr)
{
}

void GateBase::reset()
{
  outputAvailable_ = false;
  input1Value_ = BooleanValue::_X;
  input2Value_ = BooleanValue::_X;
  outputValue_ = BooleanValue::_X;
  isInputFaulty_ = false;
  isOutputFaulty_ = false;
  faultyNode_ = ~0x0;
}

BooleanValue GateBase::getOutputReval()
{
  if (input1Gate_)
  {
    input1Value_ = input1Gate_->getOutputReval();
  }

  if (input2Gate_)
  {
    input2Value_ = input2Gate_->getOutputReval();
  }

  std::ignore = getValue();
  if (isFaultyGate() && (!isAtPrimaryInput()))
  {
    BooleanValue faultFreeValue = outputValue_;
    if ((faultFreeValue == BooleanValue::_D) || (faultFreeValue == BooleanValue::_D_Bar))
    {
      throw std::exception();
    }
  }

  if (isOutputFaulty_)
  {
    outputValue_ = evaluateDOrDBar(outputValue_);
  }

  return outputValue_;
}

void GateBase::addOutputFanOutGate(Gate* fanoutGate)
{
  auto it = std::find(outputFanOut_.begin(), outputFanOut_.end(), fanoutGate);
  if (it == outputFanOut_.end())
  {
    outputFanOut_.push_back(fanoutGate);
  }
}

bool GateBase::isAtPrimaryInput() const
{
  if ((gateInfo_.gateType == GateType::NOT) || (gateInfo_.gateType == GateType::BUFF))
  {
    return input1Gate_ == nullptr;
  }
  else
  {
    return (input1Gate_ == nullptr) || (input2Gate_ == nullptr);
  }
}

bool GateBase::isPrimaryGate()
{
  return isPrimaryGate_;
}

bool GateBase::isFaultyGate() const
{
  return isInputFaulty_ || isOutputFaulty_;
}

bool GateBase::hasInput1() const
{
  return true;
}

bool GateBase::hasInput2() const
{
  return true;
}

bool GateBase::isGateInDFrontier()
{
  if (!outputAvailable_)
  {
    std::ignore = getOutputReval();
  }

  if (outputValue_ != BooleanValue::_X)
  {
    return false;
  }

  if ((input1Value_ == BooleanValue::_D) || (input1Value_ == BooleanValue::_D_Bar))
  {
    return true;
  }

  if ((input2Value_ == BooleanValue::_D) || (input2Value_ == BooleanValue::_D_Bar))
  {
    return true;
  }

  return false;
}

// ------------------- setters -------------------------------------
void GateBase::setInput1Gate(Gate *gate)
{
  input1Gate_ = gate;
}

void GateBase::setInput2Gate(Gate *gate)
{
  input2Gate_ = gate;
}

void GateBase::setInput1Value(BooleanValue value)
{
  input1Value_ = value;
  outputAvailable_ = false;
  if (isInputFaulty_ && (gateInfo_.input1 == faultyNode_) && isAtPrimaryInput())
  {
    input1Value_ = evaluateDOrDBar(input1Value_);
  }
}

void GateBase::setInput2Value(BooleanValue value)
{
  input2Value_ = value;
  outputAvailable_ = false;
  if (isInputFaulty_ && (gateInfo_.input2 == faultyNode_) && isAtPrimaryInput())
  {
    input2Value_ = evaluateDOrDBar(input2Value_);
  }
}

BooleanValue GateBase::evaluateDOrDBar(const BooleanValue& evalItem)
{
  BooleanValue evaluatedValue = evalItem;
  if (stuckAtValue_)
  {
    if (evalItem == BooleanValue::_0)
    {
      evaluatedValue = BooleanValue::_D_Bar;
    }
  }
  else
  {
    if (evalItem == BooleanValue::_1)
    {
      evaluatedValue = BooleanValue::_D;
    }
  }
  return evaluatedValue;
}

void GateBase::markAsPrimaryGate()
{
  isPrimaryGate_ = true;
}

void GateBase::markAsFaultyGate(NodeId_t faultyNode, bool stuckAtValue)
{
  stuckAtValue_ = stuckAtValue;
  faultyNode_ = faultyNode;
  if (gateInfo_.output == faultyNode)
  {
    isOutputFaulty_ = true;
    isInputFaulty_ = false;
  }
  else if ((gateInfo_.input1 == faultyNode) && isAtPrimaryInput())
  {
    isOutputFaulty_ = false;
    isInputFaulty_ = true;
  }
  else if ((gateInfo_.input2 == faultyNode) && isAtPrimaryInput())
  {
    isOutputFaulty_ = false;
    isInputFaulty_ = true;
  }
  else
  {
    throw std::exception();
  }
}

// ------------------- getters -------------------------------------
ParsedGateInfo &GateBase::getInfo()
{
  return gateInfo_;
}

bool GateBase::getControllingValue() 
{
  return false;
}

bool GateBase::getInversionParity() 
{
  return false;
}

BooleanValue GateBase::getInput1()
{
  return input1Value_;
}

BooleanValue GateBase::getInput2()
{
  return input2Value_;
}

std::vector<Gate*>& GateBase::getOutputFanoutGates()
{
  return outputFanOut_;
}

PrimaryNodeList_t GateBase::getPrimaryInputsOfGate()
{
  PrimaryNodeList_t gl;
  if (input1Gate_)
  {
    PrimaryNodeList_t gl1 = input1Gate_->getPrimaryInputsOfGate();
    gl.insert(gl.begin(), gl1.begin(), gl1.end());
  }
  else
  {
    gl.push_back(gateInfo_.input1);
  }

  if (input2Gate_)
  {
    PrimaryNodeList_t gl1 = input2Gate_->getPrimaryInputsOfGate();
    gl.insert(gl.begin(), gl1.begin(), gl1.end());
  }
  else
  {
    if( (gateInfo_.gateType != GateType::NOT) && (gateInfo_.gateType != GateType::BUFF))
    {
      gl.push_back(gateInfo_.input2);
    }
  }
  return gl;
}
