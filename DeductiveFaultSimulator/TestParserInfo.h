#pragma once

#include <string>
#include <vector>

struct TestVector;

struct TestVector;
struct CircuitInfo;

typedef std::vector<TestVector> TestVectors_t;
typedef std::vector<CircuitInfo> Circuits_t;

struct TestVector
{
  TestVector(std::string input, std::string expectedOutput = "")
    : input(input)
    , expectedOutput(expectedOutput)
    , output("")
    , faultOutput("")
  {
  }

  std::string input;
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
