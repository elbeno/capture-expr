#include "capture.h"

#include <iostream>
#include <string>
using namespace std;

#define REQUIRE(x)                                              \
  {                                                             \
  cout << STRINGIFY(x) << endl;                                 \
  auto cap = LHSCapture() ->* x;                                \
  cout << cap.first.m_val;                                      \
  if (cap.second.m_op != Op::NOOP) {                            \
    cout << ' '                                                 \
         << opstr[static_cast<int>(cap.second.m_op) - 1] << ' ' \
         << cap.second.m_val;                                   \
  }                                                             \
  cout << endl;                                                 \
  }

int main(int, char* [])
{
  string s("Hello");
  REQUIRE(s + ", world" == "Hello, world");

  REQUIRE(3 * 5 > 3 * 4);
  REQUIRE(3 * 5 >= 3 * 4);
  REQUIRE(3 * 5 != 3 * 4);
  REQUIRE(3 * 3 < 3 * 4);
  REQUIRE(3 * 3 <= 3 * 4);
}
