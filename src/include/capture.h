#pragma once

// -----------------------------------------------------------------------------
#include <utility>

struct Capture {};

template <typename T>
struct LHS
{
  LHS(T&& t)
    : m_val(std::forward<T>(t))
  {}

  T m_val;
};

enum class Op
{
  NOOP,
  NE,
  EQ,
  GT,
  LT,
  GTE,
  LTE
};

static const char* opstr[] =
{
  "!=",
  "==",
  ">",
  "<",
  ">=",
  "<="
};

template <typename T>
struct RHS
{
  RHS(Op op, T&& t)
    : m_op(op)
    , m_val(std::forward<T>(t))
  {}

  Op m_op;
  T m_val;
};

template <typename T>
std::pair<LHS<T>, RHS<int>> operator->*(const Capture&, T&& t)
{
  return std::make_pair(LHS<T>(std::forward<T>(t)), RHS<int>(Op::NOOP, 0));
}

// -----------------------------------------------------------------------------
#define LHS_ACCUM(OP)                                                   \
  template <typename T, typename U>                                     \
  auto operator OP(std::pair<LHS<T>, RHS<int>>&& t, U&& u)              \
    -> std::pair<LHS<decltype(std::forward<T>(t.first.m_val) OP         \
                         std::forward<U>(u))>, RHS<int>>                \
  {                                                                     \
    using V = decltype(std::forward<T>(t.first.m_val) OP                \
                       std::forward<U>(u));                             \
    return std::make_pair(LHS<V>(std::forward<T>(t.first.m_val) OP      \
                            std::forward<U>(u)),                        \
                     RHS<int>(Op::NOOP, 0));                            \
  }

LHS_ACCUM(*)
LHS_ACCUM(/)
LHS_ACCUM(%)
LHS_ACCUM(+)
LHS_ACCUM(-)
LHS_ACCUM(<<)
LHS_ACCUM(>>)

// -----------------------------------------------------------------------------
#define LHS_COMP(OP, OPVAL)                                             \
  template <typename T, typename U>                                     \
  auto operator OP(std::pair<LHS<T>, RHS<int>>&& t, U&& u)              \
      -> std::pair<LHS<T>, RHS<U>>                                      \
  {                                                                     \
    return std::make_pair(std::forward<LHS<T>>(t.first),                \
                     RHS<U>(Op::OPVAL, std::forward<U>(u)));            \
  }

LHS_COMP(!=, NE)
LHS_COMP(==, EQ)
LHS_COMP(>, GT)
LHS_COMP(<, LT)
LHS_COMP(>=, GTE)
LHS_COMP(<=, LTE)

// -----------------------------------------------------------------------------
#include <iostream>

#define STRINGIFY(a) #a

#define REQUIRE(x)                                              \
  {                                                             \
    std::cout << STRINGIFY(x) << std::endl;                     \
    auto cap = Capture() ->* x;                                 \
    std::cout << cap.first.m_val << std::endl;                  \
  }

#if 0
    if (cap.second.m_op != Op::NOOP) {                          \
      std::cout << ' '                                          \
                << opstr[static_cast<int>(cap.second.m_op) - 1] \
                << ' '                                          \
                << cap.second.m_val;                            \
    }                                                           \
    std::cout << std::endl;                                     \
  }
#endif
