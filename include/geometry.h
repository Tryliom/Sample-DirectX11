#pragma once

#include "math/Vec2.h"

#include <windows.h>

#include <vector>

struct Vertex {
  float position[2];
  float uv[2];
  float color[3];
};

struct GeometryBuilder {
  std::vector<Vertex> vertices = {};
  std::vector<uint32_t> indices = {};

  [[nodiscard]] UINT GetVerticesSize() const;
  [[nodiscard]] UINT GetIndicesSize() const;

  void AddQuad(const std::vector<Math::Vec2F>& positions);
};