#pragma once

#include <string>

enum class BooleanValue
{
  _0,
  _1,
  _D,
  _D_Bar,
  _X
};

BooleanValue booleanvalue_or(const BooleanValue lhs, const BooleanValue rhs);
BooleanValue booleanvalue_and(const BooleanValue lhs, const BooleanValue rhs);
BooleanValue  operator !(const BooleanValue operand);
BooleanValue booleanvalue_xor(bool lhs, const BooleanValue rhs);

std::string getBooleanValueString(BooleanValue value);