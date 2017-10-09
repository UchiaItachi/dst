#include "Tests.h"

#include <iostream>

const std::string Tests::circuitPathPrefix("/home/itachi/repositories/gatech/fall_2017/ece6140_dst/Test_Circuits/");

void Tests::setUp()
{
  {
    CircuitInfo c;
    c.circuitFile = circuitPathPrefix + "s27.txt";
    c.vectors_.push_back(TestVector("1110101", "1001"));
    c.vectors_.push_back(TestVector("0001010", "0100"));
    circuits_.push_back(c);
  }

  {
    CircuitInfo c;
    c.circuitFile = circuitPathPrefix + "s298f_2.txt";
    c.vectors_.push_back(TestVector("10101010101010101", "00000010101000111000"));
    c.vectors_.push_back(TestVector("01011110000000111", "00000000011000001000"));
    circuits_.push_back(c);
  }

  {
    CircuitInfo c;
    c.circuitFile = circuitPathPrefix + "s344f_2.txt";
    c.vectors_.push_back(TestVector("101010101010101011111111", "10101010101010101010101101"));
    c.vectors_.push_back(TestVector("010111100000001110000000", "00011110000000100001111100"));
    circuits_.push_back(c);
  }

  {
    CircuitInfo c;
    c.circuitFile = circuitPathPrefix + "s349f_2.txt";
    c.vectors_.push_back(TestVector("101010101010101011111111", "10101010101010101101010101"));
    c.vectors_.push_back(TestVector("010111100000001110000000", "00011110000000101011110000"));
    circuits_.push_back(c);
  }
}

Circuits_t& Tests::getCircuits()
{
  return circuits_;
}

TestVectors_t& Tests::getTestVectors(CircuitInfo &ci)
{
  return ci.vectors_;
}

void Tests::printSummary()
{
  std::cout << "-------------------------------------------------------------" << std::endl;
  std::cout << "Tests Summary for various circuits" << std::endl;
  std::cout << "-------------------------------------------------------------" << std::endl;

  for (auto& circuit: circuits_)
  {
    std::cout << "Circuit : " << circuit.circuitFile << std::endl;
    for (auto& testVector: circuit.vectors_)
    {
      bool pass = testVector.output == testVector.expectedOutput;
      std::cout << "\t" << "Result : " << (pass ? "PASS" : "FAIL")
                << "\t" << "Input : " << testVector.input << " "
                << "\t" << "Output : " << testVector.output << " "
                << "\t" << "Expected : " << testVector.expectedOutput
                << std::endl;
    }
    std::cout << std::endl;
  }
}
