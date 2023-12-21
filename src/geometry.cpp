#include "../include/geometry.h"

UINT GeometryBuilder::GetVerticesSize() const {
  return static_cast<UINT>(vertices.size() * sizeof(vertices[0]));
}

UINT GeometryBuilder::GetIndicesSize() const {
  return static_cast<UINT>(indices.size() * sizeof(indices[0]));
}

void GeometryBuilder::AddQuad(const std::vector<Math::Vec2F> &positions) {
  const auto index = static_cast<uint32_t>(vertices.size());

  for (const auto &position : positions) {
    vertices.push_back({position.X, position.Y, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
  }

  indices.push_back(index + 0);
  indices.push_back(index + 1);
  indices.push_back(index + 3);
  indices.push_back(index + 1);
  indices.push_back(index + 2);
  indices.push_back(index + 3);
}

