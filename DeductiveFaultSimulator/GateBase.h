#pragma once

#include "Gate.h"
#include "ParsedGateInfo.h"
#include "SimulatorDefines.h"
#include "DeductiveSolver.h"

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
    , outputFaultList_()
    , isPrimaryGate_(false)
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

  SimulatorDefines::FaultList_t getOutputFaultList() override
  {
     if (outputFaultListAvaiable_)
     {
       return outputFaultList_;
     }

   // std::cout << "gate : " << gateInfo_.output << std::endl;

    input1FaultList_ = input1Gate_ ? input1Gate_->getOutputFaultList() : getPrimaryInput1FaultList();
    input2FaultList_ = input2Gate_ ? input2Gate_->getOutputFaultList() : getPrimaryInput2FaultList();

    outputFaultList_ = solver_->solve(this);
    outputFaultListAvaiable_ = true;

    return outputFaultList_;
  }

  virtual SimulatorDefines::FaultList_t getPrimaryInput1FaultList()
  {
    if (input1Gate_)
    {
      throw std::exception();
    }

    SimulatorDefines::FaultList_t fl;
    uint32_t stuckAtValue = input1Value_ ? 0 : 1;
    if (solver_->isFaultInFaultSpace(gateInfo_.input1, stuckAtValue))
    {
      fl[gateInfo_.input1].push_back(stuckAtValue);
    }
    return fl;
  }

  virtual SimulatorDefines::FaultList_t getPrimaryInput2FaultList()
  {
    if (input2Gate_)
    {
      throw std::exception();
    }

    SimulatorDefines::FaultList_t fl;
    uint32_t stuckAtValue = input2Value_ ? 0 : 1;
    if (solver_->isFaultInFaultSpace(gateInfo_.input2, stuckAtValue))
    {
      fl[gateInfo_.input2].push_back(stuckAtValue);
    }
    return fl;
  }

  SimulatorDefines::FaultList_t getInput1FaultList() override
  {
    return input1FaultList_;
  }

  SimulatorDefines::FaultList_t getInput2FaultList() override
  {
    return input2FaultList_;
  }

  void setOutputFaultList(SimulatorDefines::FaultList_t& fl) override
  {
    outputFaultList_ = fl;
  }

  void setDeductiveSolver(DeductiveSolver* solver)
  {
    solver_ = solver;
  }

  virtual void setInput1Gate(Gate* gate) override
  {
    input1Gate_ = gate;
  }

  virtual void setInput2Gate(Gate* gate) override
  {
    input2Gate_ = gate;
  }

  void markAsPrimaryGate()
  {
    isPrimaryGate_ = true;
  }

  bool isPrimaryGate()
  {
    return isPrimaryGate_;
  }

  bool getInput1() override
  {
    return input1Value_;
  }

  bool getInput2() override
  {
    return input2Value_;
  }

  virtual bool isAtPrimaryInput() const
  {
    if ((gateInfo_.gateType == GateType::NOT) || (gateInfo_.gateType == GateType ::BUFF))
    {
      return input1Gate_ == nullptr;
    }
    else
    {
      return (input1Gate_ == nullptr) && (input2Gate_ == nullptr);
    }
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
    outputFaultListAvaiable_ = false;
  }

  bool getControllingValue() override
  {
    return false;
  }

  bool getInversionParity() override
  {
    return false;
  }

  bool hasInput1() const override
  {
    return true;
  }

  bool hasInput2() const override
  {
    return true;
  }

protected:
  ParsedGateInfo gateInfo_;
  bool outputAvailable_;
  bool outputFaultListAvaiable_;
  bool input1Value_;
  bool input2Value_;
  bool outputValue_;
  bool isPrimaryGate_;

  Gate* input1Gate_;
  Gate* input2Gate_;

  SimulatorDefines::FaultList_t outputFaultList_;
  SimulatorDefines::FaultList_t input1FaultList_;
  SimulatorDefines::FaultList_t input2FaultList_;

  DeductiveSolver* solver_;
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

  bool getValue() override
  {
    outputValue_ = input1Value_ || input2Value_;
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

  bool getValue() override
  {
    outputValue_ = !input1Value_;
    outputAvailable_ = true;
    return outputValue_;
  }

  bool getControllingValue() override
  {
    return input1Value_;
  }

  bool getInversionParity() override
  {
    return true;
  }

  SimulatorDefines::FaultList_t getPrimaryInput2FaultList()
  {
    return SimulatorDefines::FaultList_t();
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

  bool getValue() override
  {
    outputValue_ = !(input1Value_ && input2Value_);
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

  bool getValue() override
  {
    outputValue_ = !(input1Value_ || input2Value_);
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

  SimulatorDefines::FaultList_t getPrimaryInput2FaultList()
  {
    return SimulatorDefines::FaultList_t();
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
