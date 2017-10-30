#include <iostream>

#include "CircuitParser.h"
#include "CircuitSimulator.h"
#include "TestParserInfo.h"

#include "TestParser.h"

int main(int argc, char* argv[])
{

  if (argc <= 1)
  {
    std::cout << "Error: Please pass the test file containing a list of circuit file & circuit input files per line" << std::endl;
    return -1;
  }
  std::string testFile(argv[1]);

  TestParser testParser(testFile);
  Circuits_t& circuits = testParser.getCircuits();
  for (auto& circuitInfo: circuits)
  {
    TestVectors_t& testVectors = circuitInfo.vectors_;
    CircuitParser parser(circuitInfo.circuitFile);
    CircuitSimulator simulator;

    simulator.addPrimaryInputs(parser.getPrimaryInputs());
    simulator.addPrimaryOutputs(parser.getPrimaryOutputs());
    simulator.addGate(parser.getParsedGates());

    for (auto& testVector: testVectors)
    {
      simulator.reset();
      testVector.output = simulator.simulate(parser.getInputNodeValues(testVector.input));
    }
  }
  testParser.printSummary();

  return 0;
}