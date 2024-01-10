#pragma once
#pragma once

#include <stdint.h>

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <numeric>
#include <random>
#include <vector>

#include "../../../../Downloads/DirectX11_Learning-master/DirectX11_Learning-master/DirectX11_Learning/Math.h"
struct Vertex {
  Vec3 position;
  Vec2 uv;
  Vec3 color;
};

static void Shuffle(std::vector<int>::iterator begin,
                    std::vector<int>::iterator end) {
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle(begin, end, g);
}

class Perlin {
 public:
  std::vector<int> hash_;

  Perlin();
  Perlin(std::vector<int> hash);

  int noise(int x, int y);
  float lin_inter(float x, float y, float s);
  float smooth_inter(float x, float y, float s);
  float noise2d(float x, float y);
  float perlin2d(float x, float y, float freq, int depth);
};

class GeometryBuilder {
 public:
  std::vector<Vertex> vertices_;
  std::vector<uint32_t> indices_;

  void PushQuad(float scale = 1, Vec3 pos = {0, 0, 0}, Vec3 color = {0, 0, 1});

  void PushCube(float scale = 1, Vec3 pos = {0, 0, 0}, Vec3 color = {0, 0, 1});
};
