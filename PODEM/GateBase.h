#pragma once

#include "Gate.h"
#include "ParsedGateInfo.h"
#include "SimulatorDefines.h"
#include "BooleanValue.h"

#include <iostream>
#include <cstdint>

class GateBase: public Gate
{
public:
  GateBase(ParsedGateInfo gi);

  BooleanValue getOutput() override;
  void setInput1Gate(Gate* gate) override;
  void setInput2Gate(Gate* gate) override;
  BooleanValue getOutputReval() override;
  void markAsPrimaryGate();

  bool isPrimaryGate();

  BooleanValue getInput1() override;
  BooleanValue getInput2() override;

  bool isAtPrimaryInput() const;
  void setInput1Value(BooleanValue value) override;
  void setInput2Value(BooleanValue value) override;

  ParsedGateInfo& getInfo() override;

  void reset() override;

  bool getControllingValue() override;
  bool getInversionParity() override;
  bool hasInput1() const override;
  bool hasInput2() const override;

  void addOutputFanOutGate(Gate* fanoutGate) override;

  bool isGateInDFrontier() override;
  std::vector<Gate*>& getOutputFanoutGates() override;

protected:


  ParsedGateInfo gateInfo_;
  bool outputAvailable_;
  BooleanValue input1Value_;
  BooleanValue input2Value_;
  BooleanValue outputValue_;
  bool isPrimaryGate_;

  Gate* input1Gate_;
  Gate* input2Gate_;
  std::vector<Gate*> outputFanOut_;
};
