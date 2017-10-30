#include "TestParser.h"

#include <fstream>
#include <iostream>

const std::regex TestParser::TEST_FILE_ENTRY_REGEX("\\s*(.+)\\,\\s*(.*)\\s*");
const std::regex TestParser::TEST_INPUT_REGEX("\\s*([0|1]+)\\s*");

std::string TestParser::PATH_SEP("/");

TestParser::TestParser(const std::string& testFile)
  : testFile_(testFile)
  , testFileDir_("")
  , circuitInfo_()
{
  parseTestFile();
}

void TestParser::parseTestInputFile(CircuitInfo& ci)
{
  std::ifstream file;
  file.open(ci.inputVectorFile, std::fstream::in);
  if (!file.is_open())
  {
    std::cout << "\nError: Couldn't open the input file : " << ci.inputVectorFile << std::endl;
    return;
  }

  for (std::string line; std::getline(file, line);)
  {
    std::smatch matches;
    if (std::regex_match(line, matches, TEST_INPUT_REGEX))
    {
      TestVector tv{matches[1]};
      ci.vectors_.push_back(tv);
    }
  }
}

void TestParser::parseTestFile()
{
  std::size_t sep = testFile_.rfind("\\");

  if (sep != std::string::npos)
  {
    testFileDir_ = std::string(testFile_.begin(), testFile_.begin() + sep);
    PATH_SEP = "\\";
  }
  else if (testFile_.rfind("/") != std::string::npos)
  {
    sep = testFile_.rfind("/");
    testFileDir_ = std::string(testFile_.begin(), testFile_.begin() + sep);
    PATH_SEP = "/";
  }

  std::ifstream file;
  file.open(testFile_, std::fstream::in);
  if (!file.is_open())
  {
    std::cout << "\nError: Couldn't open the Test file : " << testFile_ << std::endl;
    throw std::exception();
    return;
  }

  for (std::string line; std::getline(file, line);)
  {
    std::smatch matches;
    if (std::regex_match(line, matches, TEST_FILE_ENTRY_REGEX))
    {
      CircuitInfo ci;
      ci.circuitFile = testFileDir_ + PATH_SEP + matches[1].str();
      ci.inputVectorFile = testFileDir_ + PATH_SEP + matches[2].str();
      {
        std::string cf = matches[1];
        std::size_t sep = cf.find("txt");
        if (sep != std::string::npos)
        {
          std::string outputFile(cf.begin(), cf.begin() + sep - 1);
          ci.outputFile = testFileDir_ + PATH_SEP + outputFile + "_output.txt";
        }
        else
        {
          ci.outputFile = testFileDir_ + PATH_SEP + cf + "_output.txt";
        }
      }
      parseTestInputFile(ci);
      circuitInfo_.push_back(ci);
    }
  }
}

void TestParser::printSummary()
{
  std::cout << std::endl << "Test output written to corresponding test output files " << std::endl;
  for (auto& circuit: circuitInfo_)
  {
    std::ofstream output;
    output.open(circuit.outputFile, std::ofstream::out | std::ofstream::trunc);
    if (!output.is_open())
    {
      std::cout << "\nError: Couldn't open the output file : " << circuit.outputFile << std::endl;
      continue;
    }

    output << "Netlist File : " << circuit.circuitFile << std::endl;
    output << "Test Input File : " << circuit.inputVectorFile << std::endl;

    output << std::endl;
    for (auto& testVector: circuit.vectors_)
    {
      //bool pass = testVector.output == testVector.expectedOutput;
      output //<< "\t" << "Result : " << (pass ? "PASS" : "FAIL")
             << "Input : " << testVector.input << " "
             << "\t" << "Output : " << testVector.output << " "
             //<< "\t" << "Expected : " << testVector.expectedOutput
             << std::endl;
    }
    output << std::endl;
  }
}
