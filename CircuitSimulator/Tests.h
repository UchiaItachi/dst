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
  {
  }

  std::string input;
  std::string expectedOutput;
  std::string output;
};

struct CircuitInfo
{
  std::string circuitFile;
  TestVectors_t vectors_;
};

class Tests
{
public:

  void setUp();
  Circuits_t& getCircuits();
  TestVectors_t& getTestVectors(CircuitInfo&);
  void printSummary();

private:
  Circuits_t circuits_;
  static const std::string circuitPathPrefix;
};