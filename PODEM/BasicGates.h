#pragma once

#include "GateBase.h"
#include "Gate.h"
#include "ParsedGateInfo.h"
#include "SimulatorDefines.h"

#include <iostream>
#include <cstdint>

class AndGate: public GateBase
{
public:
  AndGate(ParsedGateInfo gi)
    : GateBase(gi)
  {
  }

  BooleanValue getValue() override
  {
    outputValue_ = booleanvalue_and(input1Value_ , input2Value_);
    outputAvailable_ = true;
    return outputValue_;
  }

  bool getControllingValue() override
  {
    return false;
  }

  bool getInversionParity() override
  {
    return false;
  }
};

class OrGate: public GateBase
{
public:
  OrGate(ParsedGateInfo gi)
    : GateBase(gi)
  {
  }

  BooleanValue getValue() override
  {
    outputValue_ = booleanvalue_or(input1Value_, input2Value_);
    outputAvailable_ = true;
    return outputValue_;
  }

  bool getControllingValue() override
  {
    return true;
  }

  bool getInversionParity() override
  {
    return false;
  }
};

class NotGate: public GateBase
{
public:
  NotGate(ParsedGateInfo gi)
    : GateBase(gi)
  {
  }

  BooleanValue getValue() override
  {
    outputValue_ = !input1Value_;
    outputAvailable_ = true;
    return outputValue_;
  }

  bool getControllingValue() override
  {
    if ((input1Value_ == BooleanValue::_0) || (input1Value_ == BooleanValue::_D_Bar))
    {
      return false;
    }
    else
    {
      return true;
    }
  }

  bool getInversionParity() override
  {
    return true;
  }

  FaultList_t getPrimaryInput2FaultList()
  {
    return FaultList_t();
  }

  bool hasInput2() const
  {
    return false;
  }

};


class NandGate: public GateBase
{
public:
  NandGate(ParsedGateInfo gi)
    : GateBase(gi)
  {
  }

  BooleanValue getValue() override
  {
    outputValue_ = !booleanvalue_and(input1Value_ , input2Value_);
    outputAvailable_ = true;
    return outputValue_;
  }
  bool getControllingValue() override
  {
    return false;
  }

  bool getInversionParity() override
  {
    return true;
  }
};


class NorGate: public GateBase
{
public:
  NorGate(ParsedGateInfo gi)
    : GateBase(gi)
  {
  }

  BooleanValue getValue() override
  {
    outputValue_ = !booleanvalue_or(input1Value_, input2Value_);
    outputAvailable_ = true;
    return outputValue_;
  }

  bool getControllingValue() override
  {
    return true;
  }

  bool getInversionParity() override
  {
    return true;
  }
};


class XorGate: public GateBase
{
public:
  XorGate(ParsedGateInfo gi)
    : GateBase(gi)
  {
  }

  BooleanValue getValue() override
  {
    BooleanValue a = booleanvalue_and(input1Value_, (!input2Value_));
    BooleanValue b = booleanvalue_and((!input1Value_), input2Value_);
    outputValue_ =  booleanvalue_or(a, b);
    outputAvailable_ = true;
    return outputValue_;
  }
};


class XnorGate: public GateBase
{
public:
  XnorGate(ParsedGateInfo gi)
    : GateBase(gi)
  {
  }

  BooleanValue getValue() override
  {
    BooleanValue a = booleanvalue_and(input1Value_, input2Value_);
    BooleanValue b = booleanvalue_and(!input1Value_, !input2Value_);
    outputValue_ =  booleanvalue_or(a, b);
    outputAvailable_ = true;
    return outputValue_;
  }
};


class BufGate: public GateBase
{
public:
  BufGate(ParsedGateInfo gi)
    : GateBase(gi)
  {
  }

  BooleanValue getValue() override
  {
    outputValue_ = input1Value_;
    outputAvailable_ = true;
    return outputValue_;
  }

  FaultList_t getPrimaryInput2FaultList()
  {
    return FaultList_t();
  }

  bool hasInput2() const
  {
    return false;
  }

  bool getControllingValue() override
  {
    return false;
  }

  bool getInversionParity() override
  {
    return false;
  }
};
