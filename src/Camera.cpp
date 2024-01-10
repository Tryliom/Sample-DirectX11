#include "Camera.h"

Camera::Camera(Math::Vec3F position, Math::Vec3F up, float yaw, float pitch)
    : front_(Math::Vec3F(0.0f, 0.0f, 1.0f)),
      speed_(SPEED),
      sensitivity_(SENSITIVITY),
      zoom_(ZOOM) {
  position_ = position;
  world_up_ = up;
  yaw_ = yaw;
  pitch_ = pitch;
  updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float yaw, float pitch)
    : front_(Math::Vec3F(0.0f, 0.0f, -1.0f)),
      speed_(SPEED),
      sensitivity_(SENSITIVITY),
      zoom_(ZOOM) {
  position_ = Math::Vec3F(posX, posY, posZ);
  world_up_ = Math::Vec3F(upX, upY, upZ);
  yaw_ = yaw;
  pitch_ = pitch;
  updateCameraVectors();
}

DirectX::XMMATRIX Camera::GetViewMatrix() {
  DirectX::XMVECTOR pos = {position_.X, position_.Y, position_.Z};
  DirectX::XMVECTOR front = {position_.X + front_.X, position_.Y + front_.Y,
                             position_.Z + front_.Z};
  DirectX::XMVECTOR up = {up_.X, up_.Y, up_.Z};
  return DirectX::XMMatrixLookAtLH(pos, front, up);
}

void Camera::ProcessKeyboard(Direction direction, float deltaTime) {
  float velocity = speed_ * deltaTime;
  Math::Vec3F front = {front_.X, 0, front_.Z};
  if (direction == FORWARD) position_ = position_ + front * velocity;
  if (direction == BACKWARD) position_ = position_ - front * velocity;
  if (direction == RIGHT) position_ = position_ - right_ * velocity;
  if (direction == LEFT) position_ = position_ + right_ * velocity;
  if (direction == UP) position_ = position_ + up_ * velocity;
  if (direction == DOWN) position_ = position_ - up_ * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset,
                                  bool constrainPitch) {
  xoffset *= sensitivity_;
  yoffset *= sensitivity_;

  yaw_ += xoffset;
  pitch_ += yoffset;

  // make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrainPitch) {
    if (pitch_ > 89.0f) pitch_ = 89.0f;
    if (pitch_ < -89.0f) pitch_ = -89.0f;
  }

  // update Front, Right and Up Vectors using the updated Euler angles
  updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset) {
  zoom_ -= (float) yoffset;
  if (zoom_ < 1.0f) zoom_ = 1.0f;
  if (zoom_ > 45.0f) zoom_ = 45.0f;
}

void Camera::updateCameraVectors() {
  // calculate the new Front vector
  Math::Vec3F front;
  front.X = cos(DirectX::XMConvertToRadians(yaw_)) *
      cos(DirectX::XMConvertToRadians(pitch_));
  front.Y = sin(DirectX::XMConvertToRadians(pitch_));
  front.Z = sin(DirectX::XMConvertToRadians(yaw_)) *
      cos(DirectX::XMConvertToRadians(pitch_));
  front_ = Math::Vec3F::Normalized(front);
  // also re-calculate the Right and Up vector
  right_ = Math::Vec3F::Normalized(Math::Vec3F::CrossProduct(front_, world_up_));
  up_ = Math::Vec3F::Normalized(Math::Vec3F::CrossProduct(right_, front_));
}
