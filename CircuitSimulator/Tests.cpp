#include "Tests.h"

#include <iostream>
#include <fstream>

const std::string Tests::circuitPathPrefix("/home/itachi/repositories/gatech/fall_2017/ece6140_dst/Test_Circuits/");

void Tests::setUp()
{
  {
    CircuitInfo c;
    c.circuitFile = circuitPathPrefix + "s27.txt";
    c.vectors_.push_back(TestVector("1110101", "1001"));
    c.vectors_.push_back(TestVector("0001010", "0100"));
    c.vectors_.push_back(TestVector("1110101"));
    c.vectors_.push_back(TestVector("0001010"));
    c.vectors_.push_back(TestVector("1010101"));
    c.vectors_.push_back(TestVector("0110111"));
    c.vectors_.push_back(TestVector("1010001"));

    circuits_.push_back(c);
  }

  {
    CircuitInfo c;
    c.circuitFile = circuitPathPrefix + "s298f_2.txt";
    c.vectors_.push_back(TestVector("10101010101010101", "00000010101000111000"));
    c.vectors_.push_back(TestVector("01011110000000111", "00000000011000001000"));
    c.vectors_.push_back(TestVector("10101010101010101"));
    c.vectors_.push_back(TestVector("01011110000000111"));
    c.vectors_.push_back(TestVector("11111000001111000"));
    c.vectors_.push_back(TestVector("11100001110001100"));
    c.vectors_.push_back(TestVector("01111011110000000"));
    circuits_.push_back(c);
  }

  {
    CircuitInfo c;
    c.circuitFile = circuitPathPrefix + "s344f_2.txt";
    c.vectors_.push_back(TestVector("101010101010101011111111", "10101010101010101010101101"));
    c.vectors_.push_back(TestVector("010111100000001110000000", "00011110000000100001111100"));
    c.vectors_.push_back(TestVector("101010101010101011111111"));
    c.vectors_.push_back(TestVector("010111100000001110000000"));
    c.vectors_.push_back(TestVector("111110000011110001111111"));
    c.vectors_.push_back(TestVector("111000011100011000000000"));
    c.vectors_.push_back(TestVector("011110111100000001111111"));
    circuits_.push_back(c);
  }

  {
    CircuitInfo c;
    c.circuitFile = circuitPathPrefix + "s349f_2.txt";
    c.vectors_.push_back(TestVector("101010101010101011111111", "10101010101010101101010101"));
    c.vectors_.push_back(TestVector("010111100000001110000000", "00011110000000101011110000"));
    c.vectors_.push_back(TestVector("101010101010101011111111"));
    c.vectors_.push_back(TestVector("010111100000001110000000"));
    c.vectors_.push_back(TestVector("111110000011110001111111"));
    c.vectors_.push_back(TestVector("111000011100011000000000"));
    c.vectors_.push_back(TestVector("011110111100000001111111"));
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
  std::string outputFile = circuitPathPrefix + "TestSummary.txt";
  std::ofstream output;
  output.open(outputFile, std::ofstream::out | std::ofstream::trunc);

  output << "-------------------------------------------------------------" << std::endl;
  output << "Tests Summary for various circuits" << std::endl;
  output << "-------------------------------------------------------------" << std::endl;

  for (auto& circuit: circuits_)
  {
    output << "Circuit : " << circuit.circuitFile << std::endl;
    for (auto& testVector: circuit.vectors_)
    {
      bool pass = testVector.output == testVector.expectedOutput;
      output << "\t" << "Result : " << (pass ? "PASS" : "FAIL")
                << "\t" << "Input : " << testVector.input << " "
                << "\t" << "Output : " << testVector.output << " "
                << "\t" << "Expected : " << testVector.expectedOutput
                << std::endl;
    }
    output << std::endl;
  }
}
