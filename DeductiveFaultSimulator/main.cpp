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

  std::string faultListFile;
  bool simulateAllFaults = true;
  if (argc == 3)
  {
    faultListFile = argv[2];
    simulateAllFaults = false;
    std::cout << "\nSimulating for given set of fault inputs from input fault list file : " << faultListFile << std::endl;
  }
  else
  {
    std::cout << "\n Simulating for all fault at every net in the circuit (pass the fault list file(2nd cmd arg.) to restrict fault space) " << std::endl;
  }

  std::string testFile(argv[1]);

  TestParser testParser(testFile);
  Circuits_t& circuits = testParser.getCircuits();
  for (auto& circuitInfo: circuits)
  {
    TestVectors_t& testVectors = circuitInfo.vectors_;
    CircuitParser parser(circuitInfo.circuitFile);

    CircuitSimulator simulator;
    if (!simulateAllFaults)
    {
      simulator.enableRestrictedFaultSpaceSimualtion();
      simulator.setRestrictedFaultSpace(testParser.getFaultSpace(faultListFile));
    }

    simulator.addPrimaryInputs(parser.getPrimaryInputs());
    simulator.addPrimaryOutputs(parser.getPrimaryOutputs());
    simulator.addGate(parser.getParsedGates());

    for (auto& testVector: testVectors)
    {
      simulator.reset();
      testVector.output = simulator.simulateGoodCircuit(parser.getInputNodeValues(testVector.input));
      testVector.faultOutput = simulator.simulateFaultyCircuit();
    }
  }
  testParser.printSummary();

  return 0;
}