#ifndef MORTON2D_H
#define MORTON2D_H

#include <cstdint>
#include <array>
#include <assert.h>
#include <immintrin.h>

#define USE_BMI2

typedef std::array<uint64_t, 256> mortonLUT;
mortonLUT morton2dLUT;

const uint64_t x2_mask = 0x55555555; //0b...01010101
const uint64_t y2_mask = 0xAAAAAAAA; //0b...10101010

struct morton2d
{
public:
	uint64_t key;

public:

	morton2d() : key(0){};
	morton2d(uint64_t _key) : key(_key){};

	inline morton2d(const uint32_t x, const uint32_t y) : key(0){

#ifdef USE_BMI2
		key = _pdep_u64(y, y2_mask) | _pdep_u64(x, x2_mask);
#else
		//TODO
#endif
	}

	inline void decode(uint64_t& x, uint64_t& y)
	{
#ifdef USE_BMI2
		x = _pext_u64(this->key, x2_mask);
		y = _pext_u64(this->key, y2_mask);
#else
		//TODO
#endif
	}

	//Binary operators
  inline bool operator==(const morton2d m1) const
  {
    return this->key == m1.key;
  }

  inline bool operator!=(const morton2d m1) const
  {
    return !operator==(m1);
  }

	inline morton2d operator|(const morton2d m1) const
	{
		return this->key | m1.key;
	}

	inline morton2d operator&(const morton2d m1) const
	{
		return this->key & m1.key;
	}

	inline morton2d operator>>(const uint64_t d) const
	{
		return this->key >> (2 * d);
	}

	inline morton2d operator<<(const uint64_t d) const
	{
		return this->key << (2 * d);
	}

  static inline morton2d incX(morton2d m1)
  {
    uint64_t x_sum = (m1.key | y2_mask) + 1;
    return (x_sum & x2_mask) | (m1.key & y2_mask);
  }

  static inline morton2d incY(morton2d m1)
  {
    uint64_t y_sum = (m1.key | x2_mask) + 2;
    return (y_sum & y2_mask) | (m1.key & x2_mask);
}

  static inline morton2d decX(morton2d m1)
  {
    uint64_t x_diff = (m1.key & x2_mask) - 1;
    return (x_diff & x2_mask) | (m1.key & y2_mask);
  }

  static inline morton2d decY(morton2d m1)
  {
    uint64_t y_diff = (m1.key & y2_mask) - 2;
    return (y_diff & y2_mask) | (m1.key & x2_mask);
  }

  static inline void decode(const morton2d m1, uint64_t& x, uint64_t& y)
  {
#ifdef USE_BMI2
    x = _pext_u64(m1.key, x2_mask);
    y = _pext_u64(m1.key, y2_mask);
#else
    //TODO
#endif
  }

#ifndef USE_BMI2
	static inline morton2d morton2d_256(const uint32_t x, const uint32_t y)
	{
		assert(x < 256 && y < 256);
		uint64_t key = morton2dLUT[y] << 1 |
			morton2dLUT[x];
		return morton2d(key);
	}
#endif

};

/* Add two morton keys (xy interleaving) */
morton2d operator+(const morton2d m1, const morton2d m2)
{
	uint64_t x_sum = (m1.key | y2_mask) + (m2.key & x2_mask);
	uint64_t y_sum = (m1.key | x2_mask) + (m2.key & y2_mask);
	return (x_sum & x2_mask) | (y_sum & y2_mask);
}

/* Substract two mortons keys (xy interleaving) */
morton2d operator-(const morton2d m1, const morton2d m2)
{
	uint64_t x_diff = (m1.key & x2_mask) - (m2.key & x2_mask);
	uint64_t y_diff = (m1.key & y2_mask) - (m2.key & y2_mask);
	return (x_diff & x2_mask) | (y_diff & y2_mask);
}

static const uint32_t MORTON2DXMAX = 0x55555555;

/*Compute a 256 array of morton keys at compile time*/
template <uint32_t i>
struct morton2dtable
{
	enum
	{
		//Use a little trick to calculate next morton key
		//mortonkey(x+1) = (mortonkey(x) - MAXMORTONKEY) & MAXMORTONKEY
		value = (morton2dtable<i - 1>::value - MORTON2DXMAX) & MORTON2DXMAX
	};
	static void add_values(mortonLUT& v)
	{
		morton2dtable<i - 1>::add_values(v);
		v[i] = value;
	}
};

template<>
struct morton2dtable<0>
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