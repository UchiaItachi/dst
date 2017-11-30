#pragma once

#include "SimulatorDefines.h"
#include "BooleanValue.h"

struct ParsedGateInfo;
class DeductiveSolver;

class Gate
{
public:
  virtual ~Gate() {}

  virtual BooleanValue getOutput() = 0;
  virtual BooleanValue getInput1() = 0;
  virtual BooleanValue getInput2() = 0;
  virtual bool getControllingValue() = 0;
  virtual bool getInversionParity() = 0;
  virtual BooleanValue getOutputReval() = 0;

  virtual bool hasInput1() const = 0;
  virtual bool hasInput2() const = 0;
  virtual bool isAtPrimaryInput() const = 0;

  virtual ParsedGateInfo& getInfo() = 0;

  virtual void setInput1Gate(Gate* gate) = 0;
  virtual void setInput2Gate(Gate* gate) = 0;

  virtual void setInput1Value(BooleanValue value) = 0;
  virtual void setInput2Value(BooleanValue value) = 0;

  virtual void markAsPrimaryGate() = 0;
  virtual bool isPrimaryGate() = 0;
  virtual bool isGateInDFrontier() = 0;
  virtual void addOutputFanOutGate(Gate* fanoutGate) = 0;
  virtual std::vector<Gate*>& getOutputFanoutGates() = 0;
  virtual void reset() = 0;

protected:
  virtual BooleanValue getValue() = 0;
};

