#pragma once

#include <ostream>
#include <utility>

// -----------------------------------------------------------------------------
// We will build up the expression as a list of types.

template <typename L>
struct Capture
{
  typedef L type;
  L m_list;
};

struct Nothing {};

// -----------------------------------------------------------------------------
// We will recurse into the expression to extract the LHS, RHS and the RelOp
// linking the two sides.

template <typename T>
struct LHSRecur
{
  constexpr static auto value(T&& t) noexcept
  {
    return std::forward<T>(t);
  }
};

template <typename T>
struct RHSRecur
{
  constexpr static auto value(T&& t) noexcept
  {
    return std::forward<T>(t);
  }
};

template <typename T>
struct RelOpRecur
{
  constexpr static auto value(T&&)
  {
    return "";
  }
};

// -----------------------------------------------------------------------------
// The first op we use is just to capture the top level on the RHS of the ->*

struct SecondOp
{
  template <typename T, typename U>
  constexpr static U eval(T&&, U&& u) noexcept
  {
    return std::forward<U>(u);
  }
};

// -----------------------------------------------------------------------------
// Now, operations that we can eval by recursion into the expression.

struct AddOp
{
  template <typename T, typename U>
  constexpr static auto eval(T&& t, U&& u)
  {
    return LHSRecur<T>::value(std::forward<T>(t)) + std::forward<U>(u);
  }
};

struct SubOp
{
  template <typename T, typename U>
  constexpr static auto eval(T&& t, U&& u)
  {
    return LHSRecur<T>::value(std::forward<T>(t)) - std::forward<U>(u);
  }
};

struct MulOp
{
  template <typename T, typename U>
  constexpr static auto eval(T&& t, U&& u)
  {
    return LHSRecur<T>::value(std::forward<T>(t)) * std::forward<U>(u);
  }
};

struct DivOp
{
  template <typename T, typename U>
  constexpr static auto eval(T&& t, U&& u)
  {
    return LHSRecur<T>::value(std::forward<T>(t)) / std::forward<U>(u);
  }
};

struct ModOp
{
  template <typename T, typename U>
  constexpr static auto eval(T&& t, U&& u)
  {
    return LHSRecur<T>::value(std::forward<T>(t)) % std::forward<U>(u);
  }
};

struct LShiftOp
{
  template <typename T, typename U>
  constexpr static auto eval(T&& t, U&& u)
  {
    return LHSRecur<T>::value(std::forward<T>(t)) << std::forward<U>(u);
  }
};

struct RShiftOp
{
  template <typename T, typename U>
  constexpr static auto eval(T&& t, U&& u)
  {
    return LHSRecur<T>::value(std::forward<T>(t)) >> std::forward<U>(u);
  }
};

// -----------------------------------------------------------------------------
// Here are relational operations that link the LHS and RHS.

// -----------------------------------------------------------------------------
struct EQOp
{
  template <typename T, typename U>
  constexpr static auto eval(T&& t, U&& u)
  {
    return LHSRecur<T>::value(std::forward<T>(t)) == std::forward<U>(u);
  }
};

struct NEOp
{
  template <typename T, typename U>
  constexpr static auto eval(T&& t, U&& u)
  {
    return LHSRecur<T>::value(std::forward<T>(t)) != std::forward<U>(u);
  }
};

struct LTOp
{
  template <typename T, typename U>
  constexpr static auto eval(T&& t, U&& u)
  {
    return LHSRecur<T>::value(std::forward<T>(t)) < std::forward<U>(u);
  }
};

struct GTOp
{
  template <typename T, typename U>
  constexpr static auto eval(T&& t, U&& u)
  {
    return LHSRecur<T>::value(std::forward<T>(t)) > std::forward<U>(u);
  }
};

struct LTEOp
{
  template <typename T, typename U>
  constexpr static auto eval(T&& t, U&& u)
  {
    return LHSRecur<T>::value(std::forward<T>(t)) <= std::forward<U>(u);
  }
};

struct GTEOp
{
  template <typename T, typename U>
  constexpr static auto eval(T&& t, U&& u)
  {
    return LHSRecur<T>::value(std::forward<T>(t)) >= std::forward<U>(u);
  }
};

// -----------------------------------------------------------------------------
// OpPair uses the phantom operator type and stores the pair of operands

template <class H, class Op, class T>
struct OpPair
{
  constexpr OpPair(H&& h, T&& t)
    : m_pair(std::forward<H>(h), std::forward<T>(t))
  {}

  std::pair<H,T> m_pair;
};

// -----------------------------------------------------------------------------
// Now we have the operator overloads to capture each type.

#define OP_OVERLOAD(OP, OP_NAME)                                        \
  template <typename H, typename T>                                     \
  constexpr inline Capture<OpPair<H, OP, T>> OP_NAME(                   \
      Capture<H>&& head, T&& tail)                                      \
  {                                                                     \
    return {{ std::forward<H>(head.m_list), std::forward<T>(tail) }};   \
  }

OP_OVERLOAD(SecondOp, operator->*)
OP_OVERLOAD(AddOp, operator+)
OP_OVERLOAD(SubOp, operator-)
OP_OVERLOAD(MulOp, operator*)
OP_OVERLOAD(DivOp, operator/)
OP_OVERLOAD(ModOp, operator%)

// -----------------------------------------------------------------------------
// Overloading << and >> lowers their precedence to below relational ops. If we
// don't overload them, it's an error, but if we overload them, the compiler
// complains about the lowered precedence wrt relational ops and requires parens
// around the << operands, which means these functions doesn't actually get
// called. But they need to exist.

OP_OVERLOAD(LShiftOp, operator<<)
OP_OVERLOAD(RShiftOp, operator>>)

// -----------------------------------------------------------------------------
// Now, the relational ops.

OP_OVERLOAD(EQOp, operator==)
OP_OVERLOAD(NEOp, operator!=)
OP_OVERLOAD(LTOp, operator<)
OP_OVERLOAD(GTOp, operator>)
OP_OVERLOAD(LTEOp, operator<=)
OP_OVERLOAD(GTEOp, operator>=)

#undef OP_OVERLOAD

// -----------------------------------------------------------------------------
// Recursing into the LHS. eval the binary ops, and return the LHS for any
// relational ops.

template <typename H, typename Op, typename T>
struct LHSRecur<OpPair<H, Op, T>>
{
  constexpr static auto value(OpPair<H, Op, T>&& t)
  {
    return Op::eval(std::forward<H>(t.m_pair.first),
                    LHSRecur<T>::value(std::forward<T>(t.m_pair.second)));
  }
};

#define LHS_RECUR_SPECIALIZE(RELOP)                               \
  template <typename H, typename T>                               \
  struct LHSRecur<OpPair<H, RELOP, T>>                            \
  {                                                               \
    constexpr static auto value(OpPair<H, RELOP, T>&& t)          \
    {                                                             \
      return LHSRecur<H>::value(std::forward<H>(t.m_pair.first)); \
    }                                                             \
  };

LHS_RECUR_SPECIALIZE(EQOp)
LHS_RECUR_SPECIALIZE(NEOp)
LHS_RECUR_SPECIALIZE(LTOp)
LHS_RECUR_SPECIALIZE(GTOp)
LHS_RECUR_SPECIALIZE(LTEOp)
LHS_RECUR_SPECIALIZE(GTEOp)

#undef LHS_RECUR_SPECIALIZE

// -----------------------------------------------------------------------------
// Recursing into the RHS. Just return the RHS.

template <typename H, typename Op, typename T>
struct RHSRecur<OpPair<H, Op, T>>
{
  constexpr static auto value(OpPair<H, Op, T>&& t)
  {
    return RHSRecur<T>::value(std::forward<T>(t.m_pair.second));
  }
};

// -----------------------------------------------------------------------------
// Recursing to find the relational op. Discard the LHS until we hit a
// relational op.

template <typename H, typename Op, typename T>
struct RelOpRecur<OpPair<H, Op, T>>
{
  constexpr static auto value(OpPair<H, Op, T>&& t)
  {
    return RelOpRecur<T>::value(std::forward<T>(t.m_pair.second));
  }
};

#define RHS_RECUR_SPECIALIZE(RELOP, OPSTR)             \
  template <typename H, typename T>                    \
  struct RelOpRecur<OpPair<H, RELOP, T>>               \
  {                                                    \
    constexpr static auto value(OpPair<H, RELOP, T>&&) \
    {                                                  \
      return OPSTR;                                    \
    }                                                  \
  };

RHS_RECUR_SPECIALIZE(EQOp, "==")
RHS_RECUR_SPECIALIZE(NEOp, "!=")
RHS_RECUR_SPECIALIZE(LTOp, "<")
RHS_RECUR_SPECIALIZE(GTOp, ">")
RHS_RECUR_SPECIALIZE(LTEOp, "<=")
RHS_RECUR_SPECIALIZE(GTEOp, ">=")

#undef RHS_RECUR_SPECIALIZE

// -----------------------------------------------------------------------------
// Functions to extract the LHS, RHS and the relational op linking the sides.

template <typename T>
constexpr inline auto LHS(T&& c)
{
  using L = typename std::remove_reference_t<T>::type;
  return LHSRecur<L>::value(std::forward<L>(c.m_list));
}

template <typename T>
constexpr inline auto RHS(T&& c)
{
  using L = typename std::remove_reference_t<T>::type;
  return RHSRecur<L>::value(std::forward<L>(c.m_list));
}

template <typename T>
constexpr inline auto RelOp(T&& c)
{
  using L = typename std::remove_reference_t<T>::type;
  return RelOpRecur<L>::value(std::forward<L>(c.m_list));
}

// -----------------------------------------------------------------------------
#define STRINGIFY(a) #a

#define REQUIRE(x)                                                      \
  {                                                                     \
    std::cout << STRINGIFY(x) << std::endl;                             \
    auto cap = Capture<Nothing>() ->* x;                                \
    std::cout << LHS(cap);                                              \
    auto relop = RelOp(cap);                                            \
    if (relop[0] != 0)                                                  \
      std::cout << ' ' << relop << ' ' << RHS(cap);                     \
    std::cout << std::endl;                                             \
  }
