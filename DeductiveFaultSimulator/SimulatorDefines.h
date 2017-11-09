#pragma once

#include <vector>
#include <map>

namespace SimulatorDefines
{

  typedef std::vector<uint32_t> StuckAtFaults_t;
  typedef uint32_t FaultNode_t;
  typedef std::map<FaultNode_t, StuckAtFaults_t> FaultList_t;
}