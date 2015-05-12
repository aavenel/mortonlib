#include <stdio.h>
#include <tchar.h>
#include <assert.h>
#include <cstdint>

#include <iostream>

#include "../include/morton2d.h"
#include "../include/morton3d.h"


void test_morton2d()
{
	morton2d(0, 0);

  uint32_t x, y;
  x = 15; y = 79;
  morton2d m1 = morton2d(x, y);

  //Decode
  uint64_t x1, y1;
  morton2d::decode(m1, x1, y1);
  assert(x1 == x && y1 == y);

  //operator+
  auto m2 = m1 + morton2d(4, 5);
  assert(m2 == morton2d(x + 4, y + 5));
  assert(m1 == m1 + morton2d(0, 0));

  //operator-
  auto m3 = m1 - morton2d(4, 5);
  assert(m3 == morton2d(x - 4, y - 5));
  assert(m1 == m1 - morton2d(0, 0));

  //increments
  assert(m1 == morton2d::decX(morton2d::incX(m1)));
  assert(m1 == morton2d::decY(morton2d::incY(m1)));

  assert(m1 + morton2d(1, 0) == morton2d::incX(m1));
  assert(m1 + morton2d(0, 1) == morton2d::incY(m1));
  assert(m1 - morton2d(1, 0) == morton2d::decX(m1));
  assert(m1 - morton2d(0, 1) == morton2d::decY(m1));

  //Min & Max
  morton2d m4 = morton2d(75, 15);
  morton2d m5 = morton2d(48, 79);
  assert(morton2d::min(m4, m5) == morton2d(48, 15));
  assert(morton2d::max(m4, m5) == morton2d(75, 79));

}

void test_morton3d()
{
	morton3d(0, 0, 0);

	uint32_t x, y, z;
	x = 15; y = 79; z = 74;
	morton3d m1 = morton3d(x, y, z);
  
  //Decode
	uint64_t x1, y1, z1;
	morton3d::decode(m1, x1, y1, z1);
	assert(x1 == x && y1 == y && z1 == z);

  //operator+
  auto m2 = m1 + morton3d(4, 5, 6);
  assert(m2 == morton3d(x + 4, y + 5, z + 6));
  assert(m1 == m1 + morton3d(0, 0, 0));

  //operator-
  auto m3 = m1 - morton3d(4, 5, 6);
  assert(m3 == morton3d(x - 4, y - 5, z - 6));
  assert(m1 == m1 - morton3d(0, 0, 0));

  //increments
  assert(m1 == morton3d::decX(morton3d::incX(m1)));
  assert(m1 == morton3d::decY(morton3d::incY(m1)));
  assert(m1 == morton3d::decZ(morton3d::incZ(m1)));

  assert(m1 + morton3d(1, 0, 0) == morton3d::incX(m1));
  assert(m1 + morton3d(0, 1, 0) == morton3d::incY(m1));
  assert(m1 + morton3d(0, 0, 1) == morton3d::incZ(m1));
  assert(m1 - morton3d(1, 0, 0) == morton3d::decX(m1));
  assert(m1 - morton3d(0, 1, 0) == morton3d::decY(m1));
  assert(m1 - morton3d(0, 0, 1) == morton3d::decZ(m1));

}

int main(int argc, char *argv[])
{
	test_morton2d();
	test_morton3d();
	return 0;
}

