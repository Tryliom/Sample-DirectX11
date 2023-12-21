#pragma once

#include "math/Vec2.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Windowsx.h>

namespace input {
enum class MouseButton {
  LEFT = 0,
  RIGHT = 1,
  MIDDLE = 2,
};

/**
 * @brief Called when an input event occurs.
 * @param msg The message.
 * @param wParam The wParam.
 * @param lParam The lParam.
 */
void OnInput(UINT msg, WPARAM wParam, LPARAM lParam);

/**
 * @brief Updates the input state.
 */
void Update();

/**
 * @brief Returns true if the key was pressed this frame.
 * @param key The key to check. Example: KB_KEY_A
 * @return
 */
bool IsKeyPressed(int key);
/**
 * @brief Returns true if the key was released this frame.
 * @param key The key to check. Example: KB_KEY_A
 * @return
 */
bool IsKeyReleased(int key);
/**
 * @brief Returns true if the key is being held down.
 * @param key The key to check. Example: KB_KEY_A
 * @return
 */
bool IsKeyHeld(int key);

/**
 * @brief Returns true if the mouse button was pressed this frame.
 * @param button The mouse button to check. Example: MouseButton::LEFT
 * @return
 */
bool IsMouseButtonPressed(MouseButton button);
/**
 * @brief Returns true if the mouse button was released this frame.
 * @param button The mouse button to check. Example: MouseButton::LEFT
 * @return
 */
bool IsMouseButtonReleased(MouseButton button);
/**
 * @brief Returns true if the mouse button is being held down.
 * @param button The mouse button to check. Example: MouseButton::LEFT
 * @return
 */
bool IsMouseButtonHeld(MouseButton button);

/**
 * @brief Returns the mouse position.
 * @return
 */
Math::Vec2F GetMousePosition();
/**
 * @brief Returns the mouse delta.
 * @return
 */
Math::Vec2F GetMouseDelta();

/**
 * @brief Returns the mouse wheel delta.
 * @return
 */
float GetMouseWheelDelta();
}