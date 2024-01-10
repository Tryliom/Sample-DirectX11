#pragma once

#include <DirectXMath.h>
#include "math/Vec3.h"

#include <vector>


enum Direction { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

// Default camera values
constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float SPEED = 2.f;
constexpr float SENSITIVITY = 0.2f;
constexpr float ZOOM = 45.0f;

class Camera {
 public:
  Math::Vec3F position_;
  Math::Vec3F front_;
  Math::Vec3F up_;
  Math::Vec3F right_;
  Math::Vec3F world_up_;

  float yaw_;
  float pitch_;

  float speed_;
  float sensitivity_;
  float zoom_;

  explicit Camera(Math::Vec3F position = Math::Vec3F(0.0f, 0.0f, 0.0f),
         Math::Vec3F up = Math::Vec3F(0.0f, 1.0f, 0.0f), float yaw = YAW,
         float pitch = PITCH);

  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
         float yaw, float pitch);

  DirectX::XMMATRIX GetViewMatrix();

  void ProcessKeyboard(Direction direction, float deltaTime);

  void ProcessMouseMovement(float xoffset, float yoffset,
                            bool constrainPitch = true);

  void ProcessMouseScroll(float yoffset);

 private:
  void updateCameraVectors();
};
