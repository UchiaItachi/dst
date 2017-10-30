#pragma once

#include "TestParserInfo.h"

#include <regex>
#include <string>

class TestParser
{
public:
  TestParser(const std::string& testFile);

  Circuits_t& getCircuits() { return circuitInfo_; }
  void printSummary();

private:
  void parseTestFile();
  void parseTestInputFile(CircuitInfo& ci);

  std::string testFile_;
  std::string testFileDir_;
  Circuits_t circuitInfo_;

  static std::string PATH_SEP;
  static const std::regex TEST_FILE_ENTRY_REGEX;
  static const std::regex TEST_INPUT_REGEX;
};

