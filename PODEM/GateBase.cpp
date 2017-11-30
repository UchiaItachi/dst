#include "GateBase.h"

GateBase::GateBase(ParsedGateInfo gi)
  : gateInfo_(gi)
  , outputAvailable_(false)
  , input1Value_(BooleanValue::_X)
  , input2Value_(BooleanValue::_X)
  , outputValue_(BooleanValue::_X)
  , isPrimaryGate_(false)
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
}

BooleanValue GateBase::getOutput()
{
  if (outputAvailable_)
  {
    return outputValue_;
  }
  return getOutputReval();
}

BooleanValue GateBase::getOutputReval()
{
  if (input1Gate_)
  {
    input1Value_ = input1Gate_->getOutput();
  }

  if (input2Gate_)
  {
    input2Value_ = input2Gate_->getOutput();
  }
  return getValue();
}

void GateBase::addOutputFanOutGate(Gate* fanoutGate)
{
  outputFanOut_.push_back(fanoutGate);
}

bool GateBase::isAtPrimaryInput() const
{
  if ((gateInfo_.gateType == GateType::NOT) || (gateInfo_.gateType == GateType::BUFF))
  {
    return input1Gate_ == nullptr;
  }
  else
  {
    return (input1Gate_ == nullptr) && (input2Gate_ == nullptr);
  }
}

bool GateBase::isPrimaryGate()
{
  return isPrimaryGate_;
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
    std::ignore = getOutput();
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
}

void GateBase::setInput2Value(BooleanValue value)
{
  input2Value_ = value;
  outputAvailable_ = false;
}

void GateBase::markAsPrimaryGate()
{
  isPrimaryGate_ = true;
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
