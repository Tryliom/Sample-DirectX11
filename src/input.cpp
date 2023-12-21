#include "input.h"

namespace input {
bool _keysPressed[512];
bool _keysReleased[512];
bool _keysHeld[512];

bool _mouseButtonsPressed[5];
bool _mouseButtonsReleased[5];
bool _mouseButtonsHeld[5];

Math::Vec2F _mousePosition;
Math::Vec2F _mouseDelta;

float _mouseWheelDelta;

void OnInput(UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_KEYDOWN: _keysPressed[wParam] = true;
      _keysHeld[wParam] = true;
      break;
    case WM_KEYUP: _keysReleased[wParam] = true;
      _keysHeld[wParam] = false;
      break;
    case WM_LBUTTONDOWN: _mouseButtonsPressed[static_cast<int>(MouseButton::LEFT)] = true;
        _mouseButtonsHeld[static_cast<int>(MouseButton::LEFT)] = true;
      break;
    case WM_LBUTTONUP: _mouseButtonsReleased[static_cast<int>(MouseButton::LEFT)] = true;
        _mouseButtonsHeld[static_cast<int>(MouseButton::LEFT)] = false;
        break;
    case WM_RBUTTONDOWN: _mouseButtonsPressed[static_cast<int>(MouseButton::RIGHT)] = true;
        _mouseButtonsHeld[static_cast<int>(MouseButton::RIGHT)] = true;
      break;
    case WM_RBUTTONUP: _mouseButtonsReleased[static_cast<int>(MouseButton::RIGHT)] = true;
        _mouseButtonsHeld[static_cast<int>(MouseButton::RIGHT)] = false;
        break;
    case WM_MBUTTONDOWN: _mouseButtonsPressed[static_cast<int>(MouseButton::MIDDLE)] = true;
        _mouseButtonsHeld[static_cast<int>(MouseButton::MIDDLE)] = true;
      break;
    case WM_MBUTTONUP: _mouseButtonsReleased[static_cast<int>(MouseButton::MIDDLE)] = true;
        _mouseButtonsHeld[static_cast<int>(MouseButton::MIDDLE)] = false;
        break;
    case WM_MOUSEWHEEL: _mouseWheelDelta = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam));
      break;
    case WM_MOUSEMOVE:
      const auto old_mouse_position = _mousePosition;
      _mousePosition.X = static_cast<float>(GET_X_LPARAM(lParam));
      _mousePosition.Y = static_cast<float>(GET_Y_LPARAM(lParam));
      _mouseDelta = _mousePosition - old_mouse_position;
      break;
  }
}

void Update() {
  for (int i = 0; i < 512; i++) {
    _keysPressed[i] = false;
    _keysReleased[i] = false;
  }

  for (int i = 0; i < 5; i++) {
    _mouseButtonsPressed[i] = false;
    _mouseButtonsReleased[i] = false;
  }

  _mouseWheelDelta = 0;
  _mouseDelta = Math::Vec2F(0, 0);
}

bool IsKeyPressed(int key) {
  return _keysPressed[key];
}

bool IsKeyReleased(int key) {
  return _keysReleased[key];
}

bool IsKeyHeld(int key) {
  return _keysHeld[key];
}

bool IsMouseButtonPressed(MouseButton button) {
  return _mouseButtonsPressed[static_cast<int>(button)];
}

bool IsMouseButtonReleased(MouseButton button) {
  return _mouseButtonsReleased[static_cast<int>(button)];
}

bool IsMouseButtonHeld(MouseButton button) {
  return _mouseButtonsHeld[static_cast<int>(button)];
}

Math::Vec2F GetMousePosition() {
  return _mousePosition;
}

Math::Vec2F GetMouseDelta() {
  return _mouseDelta;
}

float GetMouseWheelDelta() {
  return _mouseWheelDelta;
}
}