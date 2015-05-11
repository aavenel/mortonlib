#ifndef MORTON3D_H
#define MORTON3D_H

#include <cstdint>
#include <array>
#include <assert.h>
#include <immintrin.h>


#define USE_BMI2

typedef std::array<uint64_t, 256> mortonLUT;
mortonLUT morton3dLUT;

//Tesseral arithmetic
const uint64_t x3_mask = 0x9249249249249249; // 0b...01001001
const uint64_t y3_mask = 0x2492492492492492; // 0b...10010010
const uint64_t z3_mask = 0x4924924924924924; // 0b...00100100
const uint64_t xy3_mask = x3_mask | y3_mask;
const uint64_t xz3_mask = x3_mask | z3_mask;
const uint64_t yz3_mask = y3_mask | z3_mask;

struct morton3d
{
public:
	uint64_t key;

public:

	inline morton3d(const uint64_t _key) : key(_key){};

	inline morton3d(const uint32_t x, const uint32_t y, const uint32_t z) : key(0){
#ifdef USE_BMI2
		key = _pdep_u64(z, z3_mask) | _pdep_u64(y, y3_mask) | _pdep_u64(x, x3_mask);
#else
		key = morton3dLUT[(z >> 16) & 0xFF] << 2 |
			morton3dLUT[(y >> 16) & 0xFF] << 1 |
			morton3dLUT[(x >> 16) & 0xFF];
		key = key << 48 |
			morton3dLUT[(z >> 8) & 0xFF] << 2 |
			morton3dLUT[(y >> 8) & 0xFF] << 1 |
			morton3dLUT[(x >> 8) & 0xFF];
		key = key << 24 |
			morton3dLUT[z & 0xFF] << 2 |
			morton3dLUT[y & 0xFF] << 1 |
			morton3dLUT[x & 0xFF];
#endif
	}

  inline void decode(uint64_t& x, uint64_t& y, uint64_t& z)
  {
#ifdef USE_BMI2
    x = _pext_u64(this->key, x3_mask);
    y = _pext_u64(this->key, y3_mask);
    z = _pext_u64(this->key, z3_mask);
#endif
  }

	//Binary operators
	inline bool operator==(const morton3d m1) const
	{
		return this->key == m1.key;
	}

	inline bool operator!=(const morton3d m1) const
	{
		return !operator==(m1);
	}

	inline morton3d operator|(const morton3d m1) const
	{
		return this->key | m1.key;
	}

	inline morton3d operator&(const morton3d m1) const
	{
		return this->key & m1.key;
	}

	inline morton3d operator>>(const uint64_t d) const
	{
		assert(d<22);
		return this->key >> (3 * d);
	}

	inline morton3d operator<<(const uint64_t d) const
	{
		assert(d<22);
		return this->key << (3 * d);
	}

#ifndef USE_BMI2
	static inline morton3d morton3d_256(const uint32_t x, const uint32_t y, const uint32_t z)
	{
		assert(x < 256 && y < 256 && z < 256);
		uint64_t key = morton3dLUT[z] << 2 |
			morton3dLUT[y] << 1 |
			morton3dLUT[x];
		return morton3d(key);
	}
#endif

	static inline morton3d incX(const morton3d m1)
	{
		uint64_t x_sum = (m1.key | yz3_mask) + 1;
		return (x_sum & x3_mask) | (m1.key & yz3_mask);
	}

	static inline morton3d incY(const morton3d m1)
	{
		uint64_t y_sum = (m1.key | xz3_mask) + 2;
		return (y_sum & y3_mask) | (m1.key & xz3_mask);
	}

	static inline morton3d incZ(const morton3d m1)
	{
		uint64_t z_sum = (m1.key | xy3_mask) + 4;
		return (z_sum & z3_mask) | (m1.key & xy3_mask);
	}

	static inline morton3d decX(const morton3d m1)
	{
		uint64_t x_diff = (m1.key & x3_mask) - 1;
		return (x_diff & x3_mask) | (m1.key & yz3_mask);
	}

	static inline morton3d decY(const morton3d m1)
	{
		uint64_t y_diff = (m1.key & y3_mask) - 2;
		return (y_diff & y3_mask) | (m1.key & xz3_mask);
	}

	static inline morton3d decZ(const morton3d m1)
	{
		uint64_t z_diff = (m1.key & z3_mask) - 4;
		return (z_diff & z3_mask) | (m1.key & xy3_mask);
	}

	static inline void decode(const morton3d m1, uint64_t& x, uint64_t& y, uint64_t& z)
	{
#ifdef USE_BMI2
		x = _pext_u64(m1.key, x3_mask);
		y = _pext_u64(m1.key, y3_mask);
		z = _pext_u64(m1.key, z3_mask);
#endif
	}

};


/* Add two morton keys (xyz interleaving) */
morton3d operator+(const morton3d m1, const morton3d m2)
{
	uint64_t x_sum = (m1.key | yz3_mask) + (m2.key & x3_mask);
	uint64_t y_sum = (m1.key | xz3_mask) + (m2.key & y3_mask);
	uint64_t z_sum = (m1.key | xy3_mask) + (m2.key & z3_mask);
	return (x_sum & x3_mask) | (y_sum & y3_mask) | (z_sum & z3_mask);
}

/* Substract two morton keys (xyz interleaving) */
morton3d operator-(const morton3d m1, const morton3d m2)
{
	uint64_t x_diff = (m1.key & x3_mask) - (m2.key & x3_mask);
	uint64_t y_diff = (m1.key & y3_mask) - (m2.key & y3_mask);
	uint64_t z_diff = (m1.key & z3_mask) - (m2.key & z3_mask);
	return (x_diff & x3_mask) | (y_diff & y3_mask) | (z_diff & z3_mask);
}


static const uint64_t MORTON3DXMAX = 1317624576693539401; //21 bits MAX morton key

/*Compute a 256 array of morton keys at compile time*/
template <uint32_t i>
struct morton3dtable
{
	enum
	{
		//Use a little trick to calculate next morton key
		//mortonkey(x+1) = (mortonkey(x) - MAXMORTONKEY) & MAXMORTONKEY
		value = (morton3dtable<i - 1>::value - MORTON3DXMAX) & MORTON3DXMAX
	};
	static void add_values(mortonLUT& v)
	{
		morton3dtable<i - 1>::add_values(v);
		v[i] = value;
	}
};

template<>
struct morton3dtable<0>
{
	enum
	{
		value = 0
	};
	static void add_values(mortonLUT& v)
	{
		v[0] = value;
	}
};

#endif