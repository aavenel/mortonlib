#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stdio.h>
#include <cstdint>

#include <iostream>
#include <string>
#include "time.h"
#include <chrono>

#include "grids.h"

struct Profiler
{
private:
  std::string name;
  std::chrono::high_resolution_clock::time_point t_start;
  bool exportCSV;
public:
  Profiler(std::string name, bool exportCSV = false) : name(name), exportCSV(exportCSV)
  {
    t_start = std::chrono::high_resolution_clock::now();
  };

  ~Profiler()
  {
    auto t_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    if (!exportCSV)
    {
      std::cout << name << " : " << duration << "ms" << std::endl;
    }
    else
    {
      std::cout << duration << ";";
    }
  };
};

#define BEGINPROFILE(name) { Profiler p(name, false);
#define ENDPROFILE }


void benchmark2d(const int gridsize = 64, const int iMax = 1e8)
{
  typedef uint64_t gridType;
  Grid2d<gridType> g = Grid2d<gridType>(gridsize);
  MortonGrid2d<gridType> gm = MortonGrid2d<gridType>(gridsize);

  BEGINPROFILE("Classic 2d grid get() linear")
  volatile gridType r;
  for (int it = 0; it < iMax/(gridsize*gridsize); ++it)
  {
    for (int i = 0; i < gridsize; i++)
      for (int j = 0; j < gridsize; ++j)
      {
        r = g.get(i, j);
      }
  }
  ENDPROFILE

  BEGINPROFILE("Morton  2d grid get() linear")
  volatile gridType r;
  for (int it = 0; it < iMax / (gridsize*gridsize); ++it)
  {
    for (int i = 0; i < gridsize; ++i)
      for (int j = 0; j < gridsize; ++j)
      {
        //XY convention for morton code, so (i,j) to iterate in a cache friendly way
        r = gm.get(i, j);
      }
  }
  ENDPROFILE

  srand(42);
  std::vector<int> random_pool;
  random_pool.resize(iMax * 2);
  std::generate(random_pool.begin(), random_pool.end(), [&](){ return rand() % gridsize; });

  BEGINPROFILE("Classic 2d grid get() random")
  volatile gridType r;
  int x, y;
  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];
    r = g.get(x, y);
  }
  ENDPROFILE

  BEGINPROFILE("Morton  2d grid get() random")
  volatile gridType r;
  int x, y;
  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];
    r = gm.get(x, y);
  }
  ENDPROFILE

  BEGINPROFILE("Classic 2d grid get() linear non cache friendly")
  volatile gridType r;
  for (int it = 0; it < iMax / (gridsize*gridsize); ++it)
  {
    for (int i = 0; i < gridsize; ++i)
    {
      for (int j = 0; j < gridsize; ++j)
      {
        r = g.get(j, i);
      }
    }
  }
  ENDPROFILE

  BEGINPROFILE("Morton  2d grid get() linear non cache friendly")
  volatile gridType r;
  for (int it = 0; it < iMax / (gridsize*gridsize); ++it)
  {
    for (int i = 0; i < gridsize; ++i)
    {
      for (int j = 0; j < gridsize; ++j)
      {
        //XY convention for morton code, so (j,i) to iterate in a non cache-friendly way
        r = gm.get(j, i);
      }
    }
  }
  ENDPROFILE

  BEGINPROFILE("Classic 2d grid get() random + 4 neighbors")
  volatile gridType r;
  int x, y;
  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];
    r = g.get(x, y);
    //Neighbors
    if (y+1 < gridsize)
      r = g.get(x, y + 1);
    if (y-1 >= 0)
      r = g.get(x, y - 1);
    if (x + 1 < gridsize)
      r = g.get(x + 1, y);
    if (x - 1 >= 0)
      r = g.get(x - 1, y);
  }
  ENDPROFILE

  BEGINPROFILE("Morton  2d grid get() random + 4 neighbors")
  volatile gridType r;
  int x, y;
  morton2 mkey(0);
  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];
    mkey = morton2(x, y);
    r = gm.get(mkey);
    //Neighbors
    if (y + 1 < gridsize)
      r = gm.get(mkey.incY());
    if (y - 1 >= 0)
      r = gm.get(mkey.decY());

    if (x + 1 < gridsize)
      r = gm.get(mkey.incX());
    if (x - 1 >= 0)
      r = gm.get(mkey.decX());
  }
  ENDPROFILE

  BEGINPROFILE("Classic 2d grid get() random + 8 neighbors")
  volatile gridType r;
  int x, y;
  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];

    for (int xx = ((x-1 >= 0) ? -1 : 0); xx < ((x+1 < gridsize) ? 2 : 1); ++xx)
      for (int yy = ((y-1 >=0) ? -1 : 0); yy < ((y+1 < gridsize) ? 2 : 1); ++yy)
        r = g.get(x + xx, y + yy);

  }
  ENDPROFILE

  BEGINPROFILE("Morton  2d grid get() random + 8 neighbors A")
  volatile gridType r;
  int x, y;
  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];

    for (int xx = ((x - 1 >= 0) ? -1 : 0); xx < ((x + 1 < gridsize) ? 2 : 1); ++xx)
      for (int yy = ((y - 1 >= 0) ? -1 : 0); yy < ((y + 1 < gridsize) ? 2 : 1); ++yy)
        r = gm.get(x + xx, y + yy);

  }
  ENDPROFILE

  BEGINPROFILE("Morton  2d grid get() random + 8 neighbors B")
  volatile gridType r;
  int x, y;
  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];
    morton2 mkey = morton2(x, y);
    //Neighbors

    //X-1
    if (x - 1 >= 0)
    {
      r = gm.get(mkey.decX());
      if (y-1 >= 0)
        r = gm.get(mkey.decX().decY());
      if (y+1 < gridsize)
        r = gm.get(mkey.decX().incY());
    }

    //X
    r = gm.get(mkey);
    if (y - 1 >= 0)
      r = gm.get(mkey.decY());
    if (y + 1 < gridsize)
      r = gm.get(mkey.incY());

    //X+1
    if (x + 1 < gridsize)
    {
      r = gm.get(mkey.incX());
      if (y - 1 >= 0)
        r = gm.get(mkey.incX().decY());
      if (y + 1 < gridsize)
        r = gm.get(mkey.incX().incY());
    }

  }
  ENDPROFILE

  BEGINPROFILE("Morton  2d grid get() random + 8 neighbors B' ")
  volatile gridType r;
  int x, y;
  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];

    morton2 xpart = morton2(x, 0);
    morton2 ypart = morton2(0, y);

    morton2 dxpart = xpart.decX();

    morton2 ixpart = xpart.incX();

    //Neighbors
    //Y-1
    if (y - 1 >= 0)
    {
      morton2 dypart = ypart.decY();

      if (x - 1 >= 0)
        r = gm.get(dxpart | dypart);

      r = gm.get(xpart | dypart);

      if (x+1 < gridsize)
        r = gm.get(ixpart | dypart);
    }

    //Y
    if (x - 1 >= 0)
      r = gm.get(dxpart | ypart);

    r = gm.get(xpart | ypart);

    if (x + 1 < gridsize)
      r = gm.get(ixpart | ypart);

    //Y+1
    if (y + 1 < gridsize)
    {
      morton2 iypart = ypart.incY();

      if (x - 1 >= 0)
        r = gm.get(dxpart | iypart);

      r = gm.get(xpart | iypart);

      if (x + 1 < gridsize)
        r = gm.get(ixpart | iypart);
    }

  }
  ENDPROFILE

#ifdef USE_BMI2
  BEGINPROFILE("Morton  2d grid get() random + 8 neighbors C")
  volatile gridType r;
  int x, y;
  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];

    //Neighbors
    uint64_t xpart = _pdep_u64(x, x2_mask);
    uint64_t ypart = _pdep_u64(y, y2_mask);

    uint64_t ixpart = ((xpart | y2_mask) + 2) & x2_mask;
    uint64_t iypart = ((ypart | x2_mask) + 1) & y2_mask;

    uint64_t dxpart = (xpart - 2) & x2_mask;
    uint64_t dypart = (ypart - 1) & y2_mask;

    //Y-1
    if (y - 1 >= 0)
    {
      if (x - 1 >= 0)
        r = gm.get(morton2(dxpart | dypart));

      r = gm.get(morton2(xpart | dypart));

      if (x + 1 < gridsize)
        r = gm.get(morton2(ixpart | dypart));
    }

    //Y
    if (x - 1 >= 0)
      r = gm.get(morton2(dxpart | ypart));

    r = gm.get(morton2(xpart | ypart));

    if (x + 1 < gridsize)
      r = gm.get(morton2(ixpart | ypart));

    //Y+1
    if (y + 1 < gridsize)
    {
      if (x - 1 >= 0)
        r = gm.get(morton2(dxpart | iypart));

      r = gm.get(morton2(xpart | iypart));

      if (x + 1 < gridsize)
        r = gm.get(morton2(ixpart | iypart));
    }

  }
  ENDPROFILE
#endif

}

void benchmark3d(const int gridsize = 64, const int iMax= 1e8)
{ 
  typedef uint64_t gridType;
  Grid3d<gridType> g = Grid3d<gridType>(gridsize);
  MortonGrid3d<gridType> gm = MortonGrid3d<gridType>(gridsize);

  BEGINPROFILE("Classic 3d grid get() linear")
  volatile gridType r;
  for (int it = 0; it < iMax / (gridsize*gridsize*gridsize); ++it)
  {
    for (int i = 0; i < gridsize; ++i)
      for (int j = 0; j < gridsize; ++j)
        for (int k = 0; k < gridsize; ++k)
        {
          r = g.get(i, j, k);
        }
  }
  ENDPROFILE

  BEGINPROFILE("Morton  3d grid get() linear")
  volatile gridType r;
  for (int it = 0; it < iMax / (gridsize*gridsize*gridsize); ++it)
  {
    for (int i = 0; i < gridsize; ++i)
      for (int j = 0; j < gridsize; ++j)
        for (int k = 0; k < gridsize; ++k)
        {
          //XYZ convention for morton code, so (i,j,k) to iterate in a cache friendly way
          r = gm.get(i, j, k);
        }
  }
  ENDPROFILE

  srand(42);
  std::vector<int> random_pool;
  random_pool.resize(iMax * 3);
  std::generate(random_pool.begin(), random_pool.end(), [&](){ return rand() % gridsize; });

  BEGINPROFILE("Classic 3d grid get() random")
  volatile gridType r;
  int x, y, z;

  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 3];
    y = random_pool[i * 3 + 1];
    z = random_pool[i * 3 + 2];
    r = g.get(x, y, z);
  }
  ENDPROFILE

  BEGINPROFILE("Morton  3d grid get() random")
  volatile gridType r;
  int x, y, z;
  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 3];
    y = random_pool[i * 3 + 1];
    z = random_pool[i * 3 + 2];
    r = gm.get(x, y, z);
  }
  ENDPROFILE

  BEGINPROFILE("Classic 3d grid get() linear non cache friendly")
  volatile gridType r;
  for (int it = 0; it < iMax / (gridsize*gridsize*gridsize); ++it)
  {
    for (int i = 0; i < gridsize; ++i)
    {
      for (int j = 0; j < gridsize; ++j)
      {
        for (int k = 0; k < gridsize; ++k)
        {
          r = g.get(k, j, i);
        }
      }
    }
  }
  ENDPROFILE

  BEGINPROFILE("Morton  3d grid get() linear non cache friendly")
  volatile gridType r;
  for (int it = 0; it < iMax / (gridsize*gridsize*gridsize); ++it)
  {
    for (int i = 0; i < gridsize; ++i)
    {
      for (int j = 0; j < gridsize; ++j)
      {
        for (int k = 0; k < gridsize; ++k)
        {
          r = gm.get(k, j, i);
        }
      }
    }
  }
  ENDPROFILE

  BEGINPROFILE("Classic 3d grid get() random + 6 neighbors")
  volatile gridType r;
  int x, y, z;
  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 3];
    y = random_pool[i * 3 + 1];
    z = random_pool[i * 3 + 2];
    r = g.get(x, y, z);
    //Neighbors
    if (z+1 < gridsize)
      r = g.get(x, y, z + 1);
    if (z-1 >= 0)
      r = g.get(x, y, z - 1);
    if (y + 1 < gridsize)
      r = g.get(x, y+1, z);
    if (y - 1 >= 0)
      r = g.get(x, y - 1, z);
    if (x + 1 < gridsize)
      r = g.get(x + 1, y, z);
    if (x - 1 >= 0)
      r = g.get(x - 1, y, z);
  }
  ENDPROFILE

  BEGINPROFILE("Morton  3d grid get() random + 6 neighbors")
  volatile gridType r;
  int x, y, z;
  morton3 mkey(0);
  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 3];
    y = random_pool[i * 3 + 1];
    z = random_pool[i * 3 + 2];
    mkey = morton3(x, y, z);
    r = gm.get(mkey);
    //Neighbors
    if (z - 1 >= 0)
      r = gm.get(mkey.decZ());
    if (z + 1 < gridsize)
      r = gm.get(mkey.incZ());
    if (y + 1 < gridsize)
      r = gm.get(mkey.incY());
    if (y - 1 >= 0)
      r = gm.get(mkey.decY());
    if (x + 1 < gridsize)
      r = gm.get(mkey.incX());
    if (x - 1 >= 0)
      r = gm.get(mkey.decX());
  }
  ENDPROFILE

  BEGINPROFILE("Classic 3d grid get() random + 26 neighbors")
  volatile gridType r;
  int x, y, z;
  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 3];
    y = random_pool[i * 3 + 1];
    z = random_pool[i * 3 + 2];

    /*
    for (int xx = -1; xx < 2; ++xx)
      for (int yy = -1; yy < 2; ++yy)
        for (int zz = -1; zz < 2; ++zz)
          r = g.get(x+xx, y+yy, z+zz);
    */
    int xkey, ykey, zkey;
    for (int xx = ((x - 1 >= 0) ? -1 : 0); xx < ((x + 1 < gridsize) ? 2 : 1); ++xx)
    {
      xkey = (x + xx) * gridsize*gridsize;
      for (int yy = ((y - 1 >= 0) ? -1 : 0); yy < ((y + 1 < gridsize) ? 2 : 1); ++yy)
      {
        ykey = xkey + (y + yy) * gridsize;
        for (int zz = ((z - 1 >= 0) ? -1 : 0); zz < ((z + 1 < gridsize) ? 2 : 1); ++zz)
        {
          zkey = ykey + (z + zz);
          r = g.get(zkey);
        }
      }
    }

  }
  ENDPROFILE

  BEGINPROFILE("Morton  3d grid get() random + 26 neighbors A")
  volatile gridType r;
  int x, y, z;
  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 3];
    y = random_pool[i * 3 + 1];
    z = random_pool[i * 3 + 2];

    for (int xx = ((x - 1 >= 0) ? -1 : 0); xx < ((x + 1 < gridsize) ? 2 : 1); ++xx)
      for (int yy = ((y - 1 >= 0) ? -1 : 0); yy < ((y + 1 < gridsize) ? 2 : 1); ++yy)
        for (int zz = ((z - 1 >= 0) ? -1 : 0); zz < ((z + 1 < gridsize) ? 2 : 1); ++zz)
          r = gm.get(x + xx, y + yy, z + zz);

  }
  ENDPROFILE

  BEGINPROFILE("Morton  3d grid get() random + 26 neighbors B ")
  volatile gridType r;
  int x, y, z;
  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 3];
    y = random_pool[i * 3 + 1];
    z = random_pool[i * 3 + 2];

    morton3 xpart = morton3(x, 0, 0);
    morton3 ypart = morton3(0, y, 0);
    morton3 zpart = morton3(0, 0, z);
    //Neighbors

    morton3 ixpart = xpart.incX();
    morton3 iypart = ypart.incY();
    morton3 izpart = zpart.incZ();

    morton3 dxpart = xpart.decX();
    morton3 dypart = ypart.decY();
    morton3 dzpart = zpart.decZ();

    //X-1
    if (x - 1 >= 0)
    {
      if (y - 1 >= 0)
      {
        if (z - 1 >= 0)
          r = gm.get(morton3(dxpart | dypart | dzpart));

        r = gm.get(morton3(dxpart | dypart | zpart));

        if (z + 1 < gridsize)
          r = gm.get(morton3(dxpart | dypart | izpart));
      }

      if (z - 1 >= 0)
        r = gm.get(morton3(dxpart | ypart | dzpart));

      r = gm.get(morton3(dxpart | ypart | zpart));

      if (z + 1 < gridsize)
        r = gm.get(morton3(dxpart | ypart | izpart));

      if (y + 1 < gridsize)
      {
        if (z - 1 >= 0)
          r = gm.get(morton3(dxpart | iypart | dzpart));

        r = gm.get(morton3(dxpart | iypart | zpart));

        if (z + 1 < gridsize)
          r = gm.get(morton3(dxpart | iypart | izpart));
      }
    }

    //X
    if (y - 1 >= 0)
    {
      if (z - 1 >= 0)
        r = gm.get(morton3(xpart | dypart | dzpart));

      r = gm.get(morton3(xpart | dypart | zpart));

      if (z + 1 < gridsize)
        r = gm.get(morton3(xpart | dypart | izpart));
    }

    if (z - 1 >= 0)
      r = gm.get(morton3(xpart | ypart | dzpart));

    r = gm.get(morton3(xpart | ypart | zpart));

    if (z + 1 < gridsize)
      r = gm.get(morton3(xpart | ypart | izpart));

    if (y + 1 < gridsize)
    {
      if (z - 1 >= 0)
        r = gm.get(morton3(xpart | iypart | dzpart));

      r = gm.get(morton3(xpart | iypart | zpart));

      if (z + 1 < gridsize)
        r = gm.get(morton3(xpart | iypart | izpart));
    }

    //X+1
    if (x+1 < gridsize)
    {
      if (y - 1 >= 0)
      {
        if (z - 1 >= 0)
          r = gm.get(morton3(ixpart | dypart | dzpart));

        r = gm.get(morton3(ixpart | dypart | zpart));

        if (z + 1 < gridsize)
          r = gm.get(morton3(ixpart | dypart | izpart));
      }

      if (z - 1 >= 0)
        r = gm.get(morton3(ixpart | ypart | dzpart));

      r = gm.get(morton3(ixpart | ypart | zpart));

      if (z + 1 < gridsize)
        r = gm.get(morton3(ixpart | ypart | izpart));

      if (y + 1 < gridsize)
      {
        if (z - 1 >= 0)
          r = gm.get(morton3(ixpart | iypart | dzpart));

        r = gm.get(morton3(ixpart | iypart | zpart));

        if (z + 1 < gridsize)
          r = gm.get(morton3(ixpart | iypart | izpart));
      }
    }

    

  }
  ENDPROFILE

#ifdef USE_BMI2
  BEGINPROFILE("Morton  3d grid get() random + 26 neighbors C")
  volatile gridType r;
  int x, y, z;

  for (int i = 0; i < iMax; i++)
  {
    x = random_pool[i * 3];
    y = random_pool[i * 3 + 1];
    z = random_pool[i * 3 + 2];

    //Neighbors
    uint64_t xpart = _pdep_u64(x, x3_mask);
    uint64_t ypart = _pdep_u64(y, y3_mask);
    uint64_t zpart = _pdep_u64(z, z3_mask);

    uint64_t ixpart = ((xpart | yz3_mask) + 4) & x3_mask;
    uint64_t iypart = ((ypart | xz3_mask) + 2) & y3_mask;
    uint64_t izpart = ((zpart | xy3_mask) + 1) & z3_mask;

    uint64_t dxpart = (xpart - 4) & x3_mask;
    uint64_t dypart = (ypart - 2) & y3_mask;
    uint64_t dzpart = (zpart - 1) & z3_mask;

    //X-1
    if (x - 1 >= 0)
    {
      if (y - 1 >= 0)
      {
        if (z - 1 >= 0)
          r = gm.get(morton3(dxpart | dypart | dzpart));

        r = gm.get(morton3(dxpart | dypart | zpart));

        if (z + 1 < gridsize)
          r = gm.get(morton3(dxpart | dypart | izpart));
      }

      if (z - 1 >= 0)
        r = gm.get(morton3(dxpart | ypart | dzpart));

      r = gm.get(morton3(dxpart | ypart | zpart));

      if (z + 1 < gridsize)
        r = gm.get(morton3(dxpart | ypart | izpart));

      if (y + 1 < gridsize)
      {
        if (z - 1 >= 0)
          r = gm.get(morton3(dxpart | iypart | dzpart));

        r = gm.get(morton3(dxpart | iypart | zpart));

        if (z + 1 < gridsize)
          r = gm.get(morton3(dxpart | iypart | izpart));
      }
    }

    //X
    if (y - 1 >= 0)
    {
      if (z - 1 >= 0)
        r = gm.get(morton3(xpart | dypart | dzpart));

      r = gm.get(morton3(xpart | dypart | zpart));

      if (z + 1 < gridsize)
        r = gm.get(morton3(xpart | dypart | izpart));
    }

    if (z - 1 >= 0)
      r = gm.get(morton3(xpart | ypart | dzpart));

    r = gm.get(morton3(xpart | ypart | zpart));

    if (z + 1 < gridsize)
      r = gm.get(morton3(xpart | ypart | izpart));

    if (y + 1 < gridsize)
    {
      if (z - 1 >= 0)
        r = gm.get(morton3(xpart | iypart | dzpart));

      r = gm.get(morton3(xpart | iypart | zpart));

      if (z + 1 < gridsize)
        r = gm.get(morton3(xpart | iypart | izpart));
    }

    //X+1
    if (x + 1 < gridsize)
    {
      if (y - 1 >= 0)
      {
        if (z - 1 >= 0)
          r = gm.get(morton3(ixpart | dypart | dzpart));

        r = gm.get(morton3(ixpart | dypart | zpart));

        if (z + 1 < gridsize)
          r = gm.get(morton3(ixpart | dypart | izpart));
      }

      if (z - 1 >= 0)
        r = gm.get(morton3(ixpart | ypart | dzpart));

      r = gm.get(morton3(ixpart | ypart | zpart));

      if (z + 1 < gridsize)
        r = gm.get(morton3(ixpart | ypart | izpart));

      if (y + 1 < gridsize)
      {
        if (z - 1 >= 0)
          r = gm.get(morton3(ixpart | iypart | dzpart));

        r = gm.get(morton3(ixpart | iypart | zpart));

        if (z + 1 < gridsize)
          r = gm.get(morton3(ixpart | iypart | izpart));
      }
    }

  }
  ENDPROFILE
#endif

}

#endif