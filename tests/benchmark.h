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
public:
  Profiler(std::string name) : name(name)
  {
    t_start = std::chrono::high_resolution_clock::now();
  };

  ~Profiler()
  {
    auto t_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    std::cout << name << " : " << duration << "ms" << std::endl;
  };
};

#define BEGINPROFILE(name) { Profiler p(name);
#define ENDPROFILE }


void benchmark2d()
{
  const int gridsize = 1024;
  typedef int gridType;
  Grid2d<gridType> g = Grid2d<gridType>(gridsize);
  MortonGrid2d<gridType> gm = MortonGrid2d<gridType>(gridsize);

  BEGINPROFILE("Classic 2d grid get() linear")
  volatile gridType r;
  for (int i = 0; i < gridsize; i++)
    for (int j = 0; j < gridsize; ++j)
    {
      r = g.get(i, j);
    }
  ENDPROFILE

  BEGINPROFILE("Morton  2d grid get() linear")
  volatile gridType r;
  for (int i = 0; i < gridsize; ++i)
    for (int j = 0; j < gridsize; ++j)
    {
      //XY convention for morton code, so (j,i) to iterate in a cache friendly way
      r = gm.get(i, j);
    }
  ENDPROFILE

  srand(42);
  std::vector<int> random_pool;
  random_pool.resize(gridsize * gridsize * 2);
  std::generate(random_pool.begin(), random_pool.end(), [&](){ return rand() % gridsize; });

  BEGINPROFILE("Classic 2d grid get() random")
  volatile gridType r;
  int x, y;
  for (int i = 0; i < gridsize * gridsize; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];
    r = g.get(x, y);
  }
  ENDPROFILE

  BEGINPROFILE("Morton  2d grid get() random")
  volatile gridType r;
  int x, y;
  for (int i = 0; i < gridsize * gridsize; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];
    r = gm.get(x, y);
  }
  ENDPROFILE

  BEGINPROFILE("Classic 2d grid get() linear non cache friendly")
  volatile gridType r;
  for (int i = 0; i < gridsize; ++i)
  {
    for (int j = 0; j < gridsize; ++j)
    {
      r = g.get(j, i);
    }
  }
  ENDPROFILE

  BEGINPROFILE("Morton  2d grid get() linear non cache friendly")
  volatile gridType r;
  for (int i = 0; i < gridsize; ++i)
  {
    for (int j = 0; j < gridsize; ++j)
    {
      r = gm.get(j, i);
    }
  }
  ENDPROFILE

  srand(42);
  std::generate(random_pool.begin(), random_pool.end(), [&](){ return (rand() % (gridsize - 2)) + 1; });

  BEGINPROFILE("Classic 2d grid get() random + 4 neighbors")
  volatile gridType r;
  int x, y;
  for (int i = 0; i < gridsize * gridsize; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];
    r = g.get(x, y);
    //Neighbors
    r = g.get(x, y + 1);
    r = g.get(x, y - 1);
    r = g.get(x + 1, y);
    r = g.get(x - 1, y);
  }
  ENDPROFILE

  BEGINPROFILE("Morton  2d grid get() random + 4 neighbors")
  volatile gridType r;
  int x, y;
  morton2 mkey(0);
  for (int i = 0; i < gridsize * gridsize; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];
    mkey = morton2(x, y);
    r = gm.get(mkey);
    //Neighbors
    r = gm.get(mkey.incY());
    r = gm.get(mkey.decY());
    r = gm.get(mkey.incX());
    r = gm.get(mkey.decX());
  }
  ENDPROFILE

  BEGINPROFILE("Classic 2d grid get() random + 8 neighbors")
  volatile gridType r;
  int x, y;
  for (int i = 0; i < gridsize * gridsize; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];
    for (int xx = -1; xx < 2; ++xx)
      for (int yy = -1; yy < 2; ++yy)
        r = g.get(x + xx, y + yy);

  }
  ENDPROFILE

  BEGINPROFILE("Morton  2d grid get() random + 8 neighbors A")
  volatile gridType r;
  int x, y;
  for (int i = 0; i < gridsize * gridsize; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];

    for (int xx = -1; xx < 2; ++xx)
      for (int yy = -1; yy < 2; ++yy)
        r = gm.get(x + xx, y + yy);

  }
  ENDPROFILE

  BEGINPROFILE("Morton  2d grid get() random + 8 neighbors B")
  volatile gridType r;
  int x, y;
  for (int i = 0; i < gridsize * gridsize; i++)
  {
    x = random_pool[i * 2];
    y = random_pool[i * 2 + 1];
    morton2 mkey = morton2(x, y);
    //Neighbors

    //X-1
    r = gm.get(mkey.decX());
    r = gm.get(mkey.decX().decY());
    r = gm.get(mkey.decX().incY());

    //X
    r = gm.get(mkey);
    r = gm.get(mkey.decY());
    r = gm.get(mkey.incY());

    //X+1
    r = gm.get(mkey.incX());
    r = gm.get(mkey.incX().decY());
    r = gm.get(mkey.incX().incY());

  }
  ENDPROFILE


}

void benchmark3d()
{ 
  const int gridsize = 256;
  typedef int gridType;
  Grid3d<gridType> g = Grid3d<gridType>(gridsize);
  MortonGrid3d<gridType> gm = MortonGrid3d<gridType>(gridsize);

  BEGINPROFILE("Classic 3d grid get() linear")
  volatile gridType r;
  for (int i = 0; i < gridsize; i++)
    for (int j = 0; j < gridsize; ++j)
      for (int k = 0; k < gridsize; ++k)
      {
        r = g.get(i, j, k);
      }
  ENDPROFILE

  BEGINPROFILE("Morton  3d grid get() linear")
  volatile gridType r;
  for (int i = 0; i < gridsize; ++i)
    for (int j = 0; j < gridsize; ++j)
      for (int k = 0; k < gridsize; ++k)
      {
        //XYZ convention for morton code, so (i,j,z) to iterate in a cache friendly way
        r = gm.get(i, j, k);
      }
  ENDPROFILE

  srand(42);
  std::vector<int> random_pool;
  random_pool.resize(gridsize * gridsize * gridsize * 3);
  std::generate(random_pool.begin(), random_pool.end(), [&](){ return rand() % gridsize; });

  BEGINPROFILE("Classic 3d grid get() random")
  volatile gridType r;
  int x, y, z;
  for (int i = 0; i < gridsize * gridsize * gridsize; i++)
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
  for (int i = 0; i < gridsize * gridsize * gridsize; i++)
  {
    x = random_pool[i * 3];
    y = random_pool[i * 3 + 1];
    z = random_pool[i * 3 + 2];
    r = gm.get(x, y, z);
  }
  ENDPROFILE

  BEGINPROFILE("Classic 3d grid get() linear non cache friendly")
  volatile gridType r;
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
  ENDPROFILE

  BEGINPROFILE("Morton  3d grid get() linear non cache friendly")
  volatile gridType r;
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
  ENDPROFILE


  srand(42);
  std::generate(random_pool.begin(), random_pool.end(), [&](){ return (rand() % (gridsize - 2)) + 1; } );

  BEGINPROFILE("Classic 3d grid get() random + 6 neighbors")
  volatile gridType r;
  int x, y, z;
  for (int i = 0; i < gridsize * gridsize * gridsize; i++)
  {
    x = random_pool[i * 3];
    y = random_pool[i * 3 + 1];
    z = random_pool[i * 3 + 2];
    r = g.get(x, y, z);
    //Neighbors
    r = g.get(x, y, z + 1);
    r = g.get(x, y, z - 1);
    r = g.get(x, y+1, z);
    r = g.get(x, y - 1, z);
    r = g.get(x + 1, y, z);
    r = g.get(x - 1, y, z);
  }
  ENDPROFILE

  BEGINPROFILE("Morton  3d grid get() random + 6 neighbors")
  volatile gridType r;
  int x, y, z;
  morton3 mkey(0);
  for (int i = 0; i < gridsize * gridsize * gridsize; i++)
  {
    x = random_pool[i * 3];
    y = random_pool[i * 3 + 1];
    z = random_pool[i * 3 + 2];
    mkey = morton3(x, y, z);
    r = gm.get(mkey);
    //Neighbors
    r = gm.get(mkey.decZ());
    r = gm.get(mkey.incZ());
    r = gm.get(mkey.incY());
    r = gm.get(mkey.decY());
    r = gm.get(mkey.incX());
    r = gm.get(mkey.decX());
  }
  ENDPROFILE

  BEGINPROFILE("Classic 3d grid get() random + 26 neighbors")
  volatile gridType r;
  int x, y, z;
  for (int i = 0; i < gridsize * gridsize * gridsize; i++)
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
    for (int xx = -1; xx < 2; ++xx)
    {
      xkey = (x + xx) * gridsize*gridsize;
      for (int yy = -1; yy < 2; ++yy)
      {
        ykey = xkey + (y + yy) * gridsize;
        for (int zz = -1; zz < 2; ++zz)
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
  for (int i = 0; i < gridsize * gridsize * gridsize; i++)
  {
    x = random_pool[i * 3];
    y = random_pool[i * 3 + 1];
    z = random_pool[i * 3 + 2];

    for (int xx = -1; xx < 2; ++xx)
      for (int yy = -1; yy < 2; ++yy)
        for (int zz = -1; zz < 2; ++zz)
          r = gm.get(x + xx, y + yy, z + zz);

  }
  ENDPROFILE

  BEGINPROFILE("Morton  3d grid get() random + 26 neighbors B")
  volatile gridType r;
  int x, y, z;
  for (int i = 0; i < gridsize * gridsize * gridsize; i++)
  {
    x = random_pool[i * 3];
    y = random_pool[i * 3 + 1];
    z = random_pool[i * 3 + 2];
    morton3 mkey = morton3(x, y, z);
    //Neighbors

    //X-1
    auto dxkey = mkey.decX();
    r = gm.get(dxkey);
    r = gm.get(dxkey.decZ());
    r = gm.get(dxkey.incZ());

    auto dxdykey = dxkey.decY();
    r = gm.get(dxdykey);
    r = gm.get(dxdykey.decZ());
    r = gm.get(dxdykey.incZ());

    auto dxiykey = dxkey.incY();
    r = gm.get(dxiykey);
    r = gm.get(dxiykey.decZ());
    r = gm.get(dxiykey.incZ());

    //X
    r = gm.get(mkey);
    r = gm.get(mkey.decZ());
    r = gm.get(mkey.incZ());

    r = gm.get(mkey.decY());
    r = gm.get(mkey.decY().decZ());
    r = gm.get(mkey.decY().incZ());

    r = gm.get(mkey.incY());
    r = gm.get(mkey.incY().decZ());
    r = gm.get(mkey.incY().incZ());

    //X+1
    auto ixkey = mkey.incX();
    r = gm.get(ixkey);
    r = gm.get(ixkey.decZ());
    r = gm.get(ixkey.incZ());

    r = gm.get(ixkey.decY());
    r = gm.get(ixkey.decY().decZ());
    r = gm.get(ixkey.decY().incZ());

    r = gm.get(ixkey.incY());
    r = gm.get(ixkey.incY().decZ());
    r = gm.get(ixkey.incY().incZ());

  }
  ENDPROFILE


}

#endif