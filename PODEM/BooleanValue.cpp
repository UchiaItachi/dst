#include "BooleanValue.h"

#include <exception>

BooleanValue booleanvalue_or(const BooleanValue lhs, const BooleanValue rhs)
{
  if ((lhs == BooleanValue::_1) || (rhs == BooleanValue::_1))
  {
    return BooleanValue::_1;
  }

  if (lhs == BooleanValue:: _0)
  {
    return rhs;
  }

  if (rhs == BooleanValue::_0)
  {
    return lhs;
  }

  if ((rhs == BooleanValue::_X) || (lhs == BooleanValue::_X))
  {
    return BooleanValue::_X;
  }

  if ((lhs == BooleanValue::_D) && (rhs == BooleanValue::_D))
  {
    return BooleanValue::_D;
  }

  if ((lhs == BooleanValue::_D) && (rhs == BooleanValue::_D_Bar))
  {
    return BooleanValue::_1;
  }

  if ((lhs == BooleanValue::_D_Bar) && (rhs == BooleanValue::_D_Bar))
  {
    return BooleanValue::_D;
  }

  if ((lhs == BooleanValue::_D_Bar) && (rhs == BooleanValue::_D))
  {
    return BooleanValue::_1;
  }

  throw std::exception();
}

BooleanValue booleanvalue_and(const BooleanValue lhs, const BooleanValue rhs)
{
  const BooleanValue a_bar = !lhs;
  const BooleanValue b_bar = !rhs;

  const BooleanValue result = !booleanvalue_or(a_bar, b_bar);
  return result;
}

BooleanValue operator !(const BooleanValue operand)
{
  if (operand == BooleanValue::_0)
  {
    return BooleanValue::_1;
  }
  else if (operand == BooleanValue::_1)
  {
    return BooleanValue::_0;
  }
  else if (operand == BooleanValue::_D)
  {
    return BooleanValue::_D_Bar;
  }
  else if (operand == BooleanValue::_D_Bar)
  {
    return BooleanValue::_D;
  }
  else if (operand == BooleanValue::_X)
  {
    return BooleanValue::_X;
  }

  throw std::exception();
}

BooleanValue booleanvalue_xor(bool lhs, const BooleanValue rhs)
{
  if (!lhs)
  {
    return rhs;
  }
  else
  {
    return !rhs;
  }
}
std::string getBooleanValueString(BooleanValue value)
{
  switch (value)
  {
    case BooleanValue::_0: return "0";
    case BooleanValue::_1: return "1";
    case BooleanValue::_D: return "D";
    case BooleanValue::_D_Bar: return "D_Bar";
    case BooleanValue::_X: return "X";
    default:
      throw std::exception();
  }
  return "";
}
