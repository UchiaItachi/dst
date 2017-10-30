#pragma once

struct ParsedGateInfo;

class Gate
{
public:
  virtual ~Gate() {}
  virtual bool getOutput() = 0;
  virtual ParsedGateInfo& getInfo() = 0;

  virtual void setInput1Gate(Gate* gate) = 0;
  virtual void setInput2Gate(Gate* gate) = 0;

  virtual void setInput1Value(bool value) = 0;
  virtual void setInput2Value(bool value) = 0;

  virtual void reset() = 0;

protected:
  virtual bool getValue() = 0;
};

