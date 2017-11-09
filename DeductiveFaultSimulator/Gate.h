#pragma once

#include "SimulatorDefines.h"

struct ParsedGateInfo;
class DeductiveSolver;

class Gate
{
public:
  virtual ~Gate() {}

  virtual bool getOutput() = 0;
  virtual bool getInput1() = 0;
  virtual bool getInput2() = 0;
  virtual bool getControllingValue() = 0;
  virtual bool getInversionParity() = 0;

  virtual SimulatorDefines::FaultList_t getInput1FaultList() = 0;
  virtual SimulatorDefines::FaultList_t getInput2FaultList() = 0;
  virtual SimulatorDefines::FaultList_t getOutputFaultList() = 0;

  virtual void setOutputFaultList(SimulatorDefines::FaultList_t& fl) = 0;
  virtual void setDeductiveSolver(DeductiveSolver* solver) = 0;

  virtual bool hasInput1() const = 0;
  virtual bool hasInput2() const = 0;
  virtual bool isAtPrimaryInput() const = 0;

  virtual ParsedGateInfo& getInfo() = 0;

  virtual void setInput1Gate(Gate* gate) = 0;
  virtual void setInput2Gate(Gate* gate) = 0;

  virtual void setInput1Value(bool value) = 0;
  virtual void setInput2Value(bool value) = 0;

  virtual void markAsPrimaryGate() = 0;
  virtual bool isPrimaryGate() = 0;

  virtual void reset() = 0;

protected:
  virtual bool getValue() = 0;
};

