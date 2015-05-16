#include <stdio.h>
#include <tchar.h>
#include <assert.h>
#include <cstdint>

#include <iostream>

#include "../include/morton2d.h"
#include "../include/morton3d.h"


void test_morton2d()
{
	morton2(0, 0);

  uint32_t x, y;
  x = 15; y = 79;
  morton2 m1 = morton2(x, y);

  //Decode
  uint64_t x1, y1;
  m1.decode(x1, y1);
  assert(x1 == x && y1 == y);

  //operator+
  auto m2 = m1 + morton2(4, 5);
  assert(m2 == morton2(x + 4, y + 5));
  assert(m1 == m1 + morton2(0, 0));

  //operator-
  auto m3 = m1 - morton2(4, 5);
  assert(m3 == morton2(x - 4, y - 5));
  assert(m1 == m1 - morton2(0, 0));

  //increments
  assert(m1 == morton2::decX(morton2::incX(m1)));
  assert(m1 == morton2::decY(morton2::incY(m1)));

  assert(m1 + morton2(1, 0) == morton2::incX(m1));
  assert(m1 + morton2(0, 1) == morton2::incY(m1));
  assert(m1 - morton2(1, 0) == morton2::decX(m1));
  assert(m1 - morton2(0, 1) == morton2::decY(m1));

  //Min & Max
  morton2 m4 = morton2(75, 15);
  morton2 m5 = morton2(48, 79);
  assert(morton2::min(m4, m5) == morton2(48, 15));
  assert(morton2::max(m4, m5) == morton2(75, 79));

  //Comp
  assert(m4 < m5);
  assert(m5 > m4);

}

void test_morton3d()
{
	morton3(0, 0, 0);

	uint32_t x, y, z;
	x = 15; y = 79; z = 74;
	morton3 m1 = morton3(x, y, z);

  assert(morton3(0x1fffff, 0x1fffff, 0x1fffff) == morton3(0x7fffffffffffffff));
  
  //Decode
	uint64_t x1, y1, z1;
	m1.decode(x1, y1, z1);
	assert(x1 == x && y1 == y && z1 == z);

  //operator+
  auto m2 = m1 + morton3(4, 5, 6);
  assert(m2 == morton3(x + 4, y + 5, z + 6));
  assert(m1 == m1 + morton3(0, 0, 0));

  //operator-
  auto m3 = m1 - morton3(4, 5, 6);
  assert(m3 == morton3(x - 4, y - 5, z - 6));
  assert(m1 == m1 - morton3(0, 0, 0));

  //increments
  assert(m1 == morton3::decX(morton3::incX(m1)));
  assert(m1 == morton3::decY(morton3::incY(m1)));
  assert(m1 == morton3::decZ(morton3::incZ(m1)));

  assert(m1 + morton3(1, 0, 0) == morton3::incX(m1));
  assert(m1 + morton3(0, 1, 0) == morton3::incY(m1));
  assert(m1 + morton3(0, 0, 1) == morton3::incZ(m1));
  assert(m1 - morton3(1, 0, 0) == morton3::decX(m1));
  assert(m1 - morton3(0, 1, 0) == morton3::decY(m1));
  assert(m1 - morton3(0, 0, 1) == morton3::decZ(m1));

  //Min & Max
  morton3 m4 = morton3(75, 15, 16);
  morton3 m5 = morton3(48, 79, 26);
  assert(morton3::min(m4, m5) == morton3(48, 15, 16));
  assert(morton3::max(m4, m5) == morton3(75, 79, 26));

  //Comp
  assert(m4 < m5);
  assert(m5 > m4);

}

int main(int argc, char *argv[])
{
	test_morton2d();
	test_morton3d();
	return 0;
}

