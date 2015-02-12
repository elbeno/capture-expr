//#include "capture.h"
#include "capture-et.h"

#include <iostream>
#include <string>
using namespace std;

int main(int, char* [])
{
  REQUIRE(5);
  REQUIRE(5 + 2 == 7);
  REQUIRE(5 - 2 == 3);
  REQUIRE(5 + 2 * 2 == 9);
  REQUIRE((6 / 2) % 2 == 1);
  REQUIRE((16 >> 2) == 4);
  REQUIRE((16 << 2) == 64);
  REQUIRE(7 + 2 != 7);
  REQUIRE(7 + 2 > 7);
  REQUIRE(7 - 2 < 7);
  REQUIRE(7 + 2 >= 7);
  REQUIRE(7 - 2 <= 7);

  string s("Hello");
  REQUIRE(s + ", world" + " debug" == "Hello, world");

  /*
  REQUIRE(3 * 5 > 3 * 4);
  REQUIRE(3 * 5 >= 3 * 4);
  REQUIRE(3 * 5 != 3 * 4);
  REQUIRE(3 * 3 < 3 * 4);
  REQUIRE(3 * 3 <= 3 * 4);
  */
}
