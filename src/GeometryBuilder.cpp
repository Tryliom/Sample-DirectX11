#include "GeometryBuilder.h"

void GeometryBuilder::PushQuad(float scale, Vec3 pos, Vec3 color) {
  uint32_t offset = vertices_.size();

  vertices_.push_back(
      {(Vec3(-0.5f, -0.5f, 0) + pos) * scale, Vec2(0.0f, 0.0f), color});
  vertices_.push_back(
      {(Vec3(+0.5f, -0.5f, 0) + pos) * scale, Vec2(0.0f, 1.0f), color});
  vertices_.push_back(
      {(Vec3(-0.5f, +0.5f, 0) + pos) * scale, Vec2(1.0f, 0.0f), color});
  vertices_.push_back(
      {(Vec3(+0.5f, +0.5f, 0) + pos) * scale, Vec2(1.0f, 1.0f), color});

  indices_.push_back(0 + offset);
  indices_.push_back(1 + offset);
  indices_.push_back(2 + offset);
  indices_.push_back(1 + offset);
  indices_.push_back(2 + offset);
  indices_.push_back(3 + offset);
}

void GeometryBuilder::PushCube(float scale, Vec3 pos, Vec3 color) {
  uint32_t offset = vertices_.size();

  const std::vector<Vec3> vertices = {
      Vec3(0.5f, 0.5f, 0.5f),    Vec3(0.5f, -0.5f,
                                      0.5f),  // front
      Vec3(-0.5f, -0.5f, 0.5f),  Vec3(-0.5f, 0.5f, 0.5f),

      Vec3(0.5f, 0.5f, -0.5f),   Vec3(0.5f, 0.5f,
                                      0.5f),  // up
      Vec3(-0.5f, 0.5f, 0.5f),   Vec3(-0.5f, 0.5f, -0.5f),

      Vec3(0.5f, 0.5f, -0.5f),   Vec3(0.5f, -0.5f,
                                      -0.5f),  // back
      Vec3(-0.5f, -0.5f, -0.5f), Vec3(-0.5f, 0.5f, -0.5f),

      Vec3(0.5f, -0.5f, -0.5f),  Vec3(0.5f, -0.5f,
                                      0.5f),  // down
      Vec3(-0.5f, -0.5f, 0.5f),  Vec3(-0.5f, -0.5f, -0.5f),

      Vec3(0.5f, 0.5f, -0.5f),   Vec3(0.5f, -0.5f,
                                      -0.5f),  // right
      Vec3(0.5f, -0.5f, 0.5f),   Vec3(0.5f, 0.5f, 0.5f),

      Vec3(-0.5f, 0.5f, 0.5f),   Vec3(-0.5f, -0.5f,
                                      0.5f),  // left
      Vec3(-0.5f, -0.5f, -0.5f), Vec3(-0.5f, 0.5f, -0.5f)};

  const std::vector<uint32_t> indices = {
      0,  1,  3,  1,  2,  3,  4,  5,  7,  5,  6,  7,  8,  9,  11, 9,  10, 11,
      12, 13, 15, 13, 14, 15, 16, 17, 19, 17, 18, 19, 20, 21, 23, 21, 22, 23,
  };

  for (int i = 0; i < vertices.size(); i++) {
    vertices_.push_back({(vertices[i] + pos) * scale, Vec2(),
                         i >= 4 && i < 8 ? color : Vec3(0.34, 0.22, 0.24)});
  }

  for (int i = 0; i < indices.size(); ++i) {
    indices_.push_back(indices[i] + offset);
  }
}

Perlin::Perlin() {
  for (int i = 0; i < 256; ++i) {
    hash_.push_back(i);
  }
  Shuffle(hash_.begin(), hash_.end());
  // hash_ = {208, 34,  231, 213, 32,  248, 233, 56,  161, 78,  24,  140, 71,
  // 48,
  //          140, 254, 245, 255, 247, 247, 40,  185, 248, 251, 245, 28,  124,
  //          204, 204, 76,  36,  1,   107, 28,  234, 163, 202, 224, 245, 128,
  //          167, 204, 9,   92,  217, 54,  239, 174, 173, 102, 193, 189, 190,
  //          121, 100, 108, 167, 44,  43,  77,  180, 204, 8,   81,  70,  223,
  //          11,  38,  24,  254, 210, 210, 177, 32,  81,  195, 243, 125, 8,
  //          169, 112, 32,  97,  53, 195, 13,  203, 9,   47,  104, 125, 117,
  //          114, 124, 165, 203, 181, 235, 193, 206, 70,  180, 174, 0,   167,
  //          181, 41,  164, 30,  116, 127, 198, 245, 146, 87,  224, 149, 206,
  //          57,  4,   192, 210, 65,  210, 129, 240, 178, 105, 228, 108, 245,
  //          148, 140, 40,  35,  195, 38,  58,  65,  207, 215, 253, 65,  85,
  //          208, 76,  62,  3,   237, 55,  89,  232, 50,  217, 64,  244, 157,
  //          199, 121, 252, 90,  17,  212, 203, 149, 152, 140, 187, 234, 177,
  //          73,  174, 193, 100, 192, 143, 97,  53,  145, 135, 19,  103, 13,
  //          90,  135, 151, 199, 91,  239, 247, 33,  39,  145, 101, 120, 99, 3,
  //          186, 86,  99,  41,  237, 203, 111, 79,  220, 135, 158, 42,  30,
  //          154, 120, 67,  87,  167, 135, 176, 183, 191, 253, 115, 184, 21,
  //          233, 58,  129, 233, 142, 39,  128, 211, 118, 137, 139, 255, 114,
  //          20,  218, 113, 154, 27,  127, 246, 250, 1,   8,   198, 250, 209,
  //          92,  222, 173, 21,  88,  102, 219};
}

Perlin::Perlin(std::vector<int> hash) { hash_ = hash; }

int Perlin::noise(int x, int y) {
  int tmp = hash_[(y) % 256];
  return hash_[(tmp + x) % 256];
}
float Perlin::lin_inter(float x, float y, float s) { return x + s * (y - x); }
float Perlin::smooth_inter(float x, float y, float s) {
  return lin_inter(x, y, s * s * (3 - 2 * s));
}
float Perlin::noise2d(float x, float y) {
  int x_int = x;
  int y_int = y;
  float x_frac = x - x_int;
  float y_frac = y - y_int;
  int s = noise(x_int, y_int);
  int t = noise(x_int + 1, y_int);
  int u = noise(x_int, y_int + 1);
  int v = noise(x_int + 1, y_int + 1);
  float low = smooth_inter(s, t, x_frac);
  float high = smooth_inter(u, v, x_frac);
  return smooth_inter(low, high, y_frac);
}
float Perlin::perlin2d(float x, float y, float freq, int depth) {
  float xa = x * freq;
  float ya = y * freq;
  float amp = 1.0;
  float fin = 0;
  float div = 0.0;
  int i;
  for (i = 0; i < depth; i++) {
    div += 256 * amp;
    fin += noise2d(xa, ya) * amp;
    amp /= 2;
    xa *= 2;
    ya *= 2;
  }
  return fin / div;
}
