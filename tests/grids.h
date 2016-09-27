#ifndef GRIDS_H
#define GRIDS_H

#include <vector>
#include <assert.h>

#include "../include/morton2d.h"
#include "../include/morton3d.h"


template<typename T>
class Grid2d
{
public:

	Grid2d(int gridSize)
	{
		//reserve space
		storage.resize(gridSize*gridSize);
		this->gridSize = gridSize;

		//Fill with random values
		std::generate(storage.begin(), storage.end(), std::rand);
	}

	inline void push(const int x, const int y, T data)
	{
		assert(x < this->gridSize && y < this->gridSize);
		this->storage[x*gridSize + y] = data;
	}

	inline T& get(const size_t index)
	{
		return this->storage[index];
	}

	inline T& get(const int x, const int y)
	{
		assert(x < this->gridSize && y < this->gridSize);
		return this->storage[x*gridSize + y];
	}


public:
	int gridSize;

private:
	std::vector<T> storage;

};

template<typename T>
class MortonGrid2d
{
public:

	MortonGrid2d(int gridSize)
	{
		//reserve space
		storage.resize(gridSize*gridSize);
		this->gridSize = gridSize;

		//Fill with random values
		std::generate(storage.begin(), storage.end(), std::rand);
	}

	inline void push(const int x, const int y, T data)
	{
		assert(x < this->gridSize && y < this->gridSize);
		morton2 m = morton2(x, y);
		this->storage[m.key] = data;
	}

	inline T& get(const morton2 index)
	{
		return this->storage[index.key];
	}

	inline T& get(const int x, const int y)
	{
		assert(x < this->gridSize && y < this->gridSize);
		morton2 m = morton2(x, y);
		return this->storage[m.key];
	}


public:
	int gridSize;

private:
	std::vector<T> storage;

};

template<typename T>
class Grid3d
{
public:

	Grid3d(int gridSize)
	{
		//reserve space
		storage.resize(gridSize*gridSize*gridSize);
		this->gridSize = gridSize;

		//Fill with random values
		std::generate(storage.begin(), storage.end(), std::rand);
	}

	inline void push(const int x, const int y, const int z, T data)
	{
		assert(x < this->gridSize && y < this->gridSize && z < this->gridSize);
		this->storage[x*gridSize*gridSize + y*gridSize + z] = data;
	}

	inline T& get(const size_t index)
	{
		return this->storage[index];
	}

	inline T& get(const int x, const int y, const int z)
	{
		assert(x < this->gridSize && y < this->gridSize && z < this->gridSize);
		return this->storage[x*gridSize*gridSize + y*gridSize + z];
	}


public:
	int gridSize;

private:
	std::vector<T> storage;

};


template<typename T>
class MortonGrid3d
{
public:

	MortonGrid3d(int gridSize)
	{
		//reserve space
		storage.resize(gridSize*gridSize*gridSize);
		this->gridSize = gridSize;

		//Fill with random values
		std::generate(storage.begin(), storage.end(), std::rand);
	}

	inline void push(const int x, const int y, const int z, T data)
	{
		assert(x < this->gridSize && y < this->gridSize && z < this->gridSize);
		morton3 m = morton3(x, y, z);
		this->storage[m.key] = data;
	}

	inline T& get(const morton3 index)
	{
		return this->storage[index.key];
	}

	inline T& get(const int x, const int y, const int z)
	{
		assert(x < this->gridSize && y < this->gridSize && z < this->gridSize);
		morton3 m = morton3(x, y, z);
		return this->storage[m.key];
	}


public:
	int gridSize;

private:
	std::vector<T> storage;

};

#endif //GRIDS_H
