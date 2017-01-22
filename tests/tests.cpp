#include <stdio.h>
#include <assert.h>
#include <cstdint>

#include <iostream>
#include "../include/morton2d.h"
#include "../include/morton3d.h"
#include "benchmark.h"


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

	//64 bits morton code
	{
		uint64_t x64, y64;
		morton2 m64 = morton2(0xffffffffffffffff);
		assert(morton2(0xffffffff, 0xffffffff) == m64);
		m64.decode(x64, y64);
		assert(x64 == 0xffffffff);
		assert(y64 == 0xffffffff);

		m64 = morton2(0xaaaaaaaaaaaaaaaa);
		assert(morton2(0xffffffff, 0x0) == m64);
		m64.decode(x64, y64);
		assert(x64 == 0xffffffff);
		assert(y64 == 0x0);
	}

	//operator+
	auto m2 = m1 + morton2(4, 5);
	assert(m2 == morton2(x + 4, y + 5));
	assert(m1 == m1 + morton2(0, 0));

	auto m2a = m2;
	m2a += morton2(0, 0);
	assert(m2a == m2);

	//operator-
	auto m3 = m1 - morton2(4, 5);
	assert(m3 == morton2(x - 4, y - 5));
	assert(m1 == m1 - morton2(0, 0));

	auto m3a = m2;
	m3a -= morton2(0, 0);
	assert(m3a == m2);

	//increments
	assert(m1 == m1.incX().decX());
	assert(m1 == m1.incY().decY());

	assert(m1 + morton2(1, 0) == m1.incX());
	assert(m1 + morton2(0, 1) == m1.incY());
	assert(m1 - morton2(1, 0) == m1.decX());
	assert(m1 - morton2(0, 1) == m1.decY());

	//Min & Max
	morton2 m4 = morton2(75, 15);
	morton2 m5 = morton2(48, 79);
	assert(morton2::min(m4, m5) == morton2(48, 15));
	assert(morton2::max(m4, m5) == morton2(75, 79));

	//Comp
	assert(m4 > m5);
	assert(m5 < m4);

	//16 bits morton code
	morton2d<uint16_t> m16 = morton2d<uint16_t>(0, 0);
	assert(m16.incX().incY() == morton2d<uint16_t>(1, 1));
	assert(morton2d<uint16_t>(1, 1).decX().decY() == morton2d<uint16_t>(0, 0));

}

void test_morton3d()
{
	morton3(0, 0, 0);

	uint32_t x, y, z;
	x = 15; y = 79; z = 74;
	morton3 m1 = morton3(x, y, z);

	//Decode
	uint64_t x1, y1, z1;
	m1.decode(x1, y1, z1);
	assert(x1 == x && y1 == y && z1 == z);

	//64 bits morton code
	{
		uint64_t x64, y64, z64;
		morton3 m64 = morton3(0x7fffffffffffffff);
		assert(morton3(0x1fffff, 0x1fffff, 0x1fffff) == m64);


		m64.decode(x64, y64, z64);
		assert(x64 == 0x1fffff);
		assert(y64 == 0x1fffff);
		assert(z64 == 0x1fffff);

		m64 = morton3(0x1249249249249249);
		assert(morton3(0x0, 0x0, 0x1fffff) == m64);
		m64.decode(x64, y64, z64);
		assert(x64 == 0x0);
		assert(y64 == 0x0);
		assert(z64 == 0x1fffff);

	}

	//operator+
	auto m2 = m1 + morton3(4, 5, 6);
	assert(m2 == morton3(x + 4, y + 5, z + 6));
	assert(m1 == m1 + morton3(0, 0, 0));

	auto m2a = m2;
	m2a += morton3(0, 0, 0);
	assert(m2a == m2);


	//operator-
	auto m3 = m1 - morton3(4, 5, 6);
	assert(m3 == morton3(x - 4, y - 5, z - 6));
	assert(m1 == m1 - morton3(0, 0, 0));

	auto m3a = m3;
	m3a -= morton3(0, 0, 0);
	assert(m3a == m3);

	//increments
	assert(m1 == m1.incX().decX());
	assert(m1 == m1.incY().decY());
	assert(m1 == m1.incZ().decZ());

	assert(m1 + morton3(1, 0, 0) == m1.incX());
	assert(m1 + morton3(0, 1, 0) == m1.incY());
	assert(m1 + morton3(0, 0, 1) == m1.incZ());
	assert(m1 - morton3(1, 0, 0) == m1.decX());
	assert(m1 - morton3(0, 1, 0) == m1.decY());
	assert(m1 - morton3(0, 0, 1) == m1.decZ());

	//Min & Max
	morton3 m4 = morton3(75, 15, 16);
	morton3 m5 = morton3(48, 79, 26);
	assert(morton3::min(m4, m5) == morton3(48, 15, 16));
	assert(morton3::max(m4, m5) == morton3(75, 79, 26));

	//Comp
	assert(m5 < m4);
	assert(m4 > m5);

	//16 bits morton code
	morton3d<uint16_t> m16 = morton3d<uint16_t>(0, 0, 0);
	assert(m16.incX().incY().incZ() == morton3d<uint16_t>(1, 1, 1));
	assert(morton3d<uint16_t>(1, 1, 1).decX().decY().decZ() == morton3d<uint16_t>(0, 0, 0));

}

int main(int argc, char *argv[])
{
	test_morton2d();
	test_morton3d();
	benchmark2d();
	benchmark3d();
	return 0;
}

