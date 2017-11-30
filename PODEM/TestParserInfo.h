#pragma once

#include "SimulatorDefines.h"

#include <string>
#include <vector>

struct TestVector
{
  TestVector(const std::string& inputNet, const std::string& stuckAtValue, const std::string& expectedOutput = "")
    : inputNet(inputNet)
    , stuckAtValue(stuckAtValue)
    , expectedOutput(expectedOutput)
    , output("")
    , faultOutput("")
  {
  }

  std::string inputNet;
  std::string stuckAtValue;
  std::string expectedOutput;
  std::string output;
  std::string faultOutput;
};

struct CircuitInfo
{
  std::string circuitFile;
  std::string inputVectorFile;
  std::string outputFile;
  TestVectors_t vectors_;
};
