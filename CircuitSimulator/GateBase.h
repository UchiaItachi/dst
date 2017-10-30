#pragma once

#include "Gate.h"
#include "ParsedGateInfo.h"

#include <iostream>
#include <cstdint>


class GateBase: public Gate
{
public:
  GateBase(ParsedGateInfo gi)
    : gateInfo_(gi)
    , outputAvailable_(false)
    , input1Value_(false)
    , input2Value_(false)
    , outputValue_(false)
    , input1Gate_(nullptr)
    , input2Gate_(nullptr)
  {
  }

  virtual bool getOutput() override
  {
    if (outputAvailable_)
    {
      return outputValue_;
    }

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

  virtual void setInput1Gate(Gate* gate) override
  {
    input1Gate_ = gate;
  }

  virtual void setInput2Gate(Gate* gate) override
  {
    input2Gate_ = gate;
  }

  void setInput1Value(bool value) override
  {
    input1Value_ = value;
  }

  void setInput2Value(bool value) override
  {
    input2Value_ = value;
  }

  virtual ParsedGateInfo& getInfo() override
  {
    return gateInfo_;
  }

  void reset() override
  {
    outputAvailable_ = false;
  }

protected:
  ParsedGateInfo gateInfo_;
  bool outputAvailable_;
  bool input1Value_;
  bool input2Value_;
  bool outputValue_;

  Gate* input1Gate_;
  Gate* input2Gate_;
};

class AndGate: public GateBase
{
public:
  AndGate(ParsedGateInfo gi)
    : GateBase(gi)
  {
  }

  bool getValue() override
  {
    outputValue_ = input1Value_ && input2Value_;
    outputAvailable_ = true;
    return outputValue_;
  }
};

class OrGate: public GateBase
{
public:
  OrGate(ParsedGateInfo gi)
    : GateBase(gi)
  {
  }

  bool getValue() override
  {
    outputValue_ = input1Value_ || input2Value_;
    outputAvailable_ = true;
    return outputValue_;
  }

};

class NotGate: public GateBase
{
public:
  NotGate(ParsedGateInfo gi)
    : GateBase(gi)
  {
  }

  bool getValue() override
  {
    outputValue_ = !input1Value_;
    outputAvailable_ = true;
    return outputValue_;
  }

};


class NandGate: public GateBase
{
public:
  NandGate(ParsedGateInfo gi)
  : GateBase(gi)
  {
  }

  bool getValue() override
  {
    outputValue_ = !(input1Value_ && input2Value_);
    outputAvailable_ = true;
    return outputValue_;
  }

};


class NorGate: public GateBase
{
public:
  NorGate(ParsedGateInfo gi)
  : GateBase(gi)
  {
  }

  bool getValue() override
  {
    outputValue_ = !(input1Value_ || input2Value_);
    outputAvailable_ = true;
    return outputValue_;
  }

};


class XorGate: public GateBase
{
public:
  XorGate(ParsedGateInfo gi)
  : GateBase(gi)
  {
  }

  bool getValue() override
  {
    outputValue_ = (input1Value_ && (!input2Value_)) || ((!input1Value_) && input2Value_);
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

  bool getValue() override
  {
    outputValue_ = (input1Value_ && input2Value_) || ((!input1Value_) && (!input2Value_));
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

  bool getValue() override
  {
    outputValue_ = input1Value_;
    outputAvailable_ = true;
    return outputValue_;
  }
};
