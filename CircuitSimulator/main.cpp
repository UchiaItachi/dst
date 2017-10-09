#include <iostream>

#include "ParseInput.h"
#include "CircuitSimulator.h"
#include "Tests.h"

int main(int argc, char* argv[])
{

  Tests tests;
  tests.setUp();

  Circuits_t& circuits = tests.getCircuits();
  for (auto& circuitInfo: circuits)
  {
    TestVectors_t& testVectors = circuitInfo.vectors_;
    ParseInput parser(circuitInfo.circuitFile);
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

  tests.printSummary();
  return 0;
}