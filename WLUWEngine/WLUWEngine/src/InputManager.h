#pragma once

#include <map>
#include "SDL.h"

namespace WLUW
{
	class InputManager
	{
	public:
		// Update functions
		void handleEvent(SDL_Event &e)
		{
			// Keyboard
			//If a key was pressed
			if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			{
				SDL_Keycode keyCode = e.key.keysym.sym;
				keyStates[keyCode].held = true;
				keyStates[keyCode].down = true;
			}
			//If a key was released
			if (e.type == SDL_KEYUP && e.key.repeat == 0)
			{
				SDL_Keycode keyCode = e.key.keysym.sym;
				keyStates[keyCode].held = false;
				keyStates[keyCode].up = true;
			}

			// Mouse
			//If a button was pressed
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				Uint8 buttonCode = e.button.button;
				buttonStates[buttonCode].held = true;
				buttonStates[buttonCode].down = true;
			}
			//If a button was released
			if (e.type == SDL_MOUSEBUTTONUP)
			{
				Uint8 buttonCode = e.button.button;
				buttonStates[buttonCode].held = false;
				buttonStates[buttonCode].up = true;
			}
		}
		void resetStates()
		{
			// Keyboard
			for (auto& k : keyStates)
			{
				k.second.down = false;
				k.second.up = false;
			}
			// Mouse
			for (auto& b : buttonStates)
			{
				b.second.down = false;
				b.second.up = false;
			}
		}

		// Getters
		// Keyboard
		bool getKey(SDL_Keycode key) { return keyStates[key].held; }
		bool getKeyDown(SDL_Keycode key) { return keyStates[key].down; }
		bool getKeyUp(SDL_Keycode key) { return keyStates[key].up; }
		// Mouse
		bool getButton(Uint8 button) { return buttonStates[button].held; }
		bool getButtonDown(Uint8 button) { return buttonStates[button].down; }
		bool getButtonUp(Uint8 button) { return buttonStates[button].up; }

	private:
		class KeyState
		{
		public:
			bool held;
			bool down;
			bool up;
		};

		std::map<SDL_Keycode, KeyState> keyStates;
		std::map<Uint8, KeyState> buttonStates;
	};

}

