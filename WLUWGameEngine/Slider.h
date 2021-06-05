#pragma once

#include "Header.h"
#include "Vector2.h"
#include "WTexture.h"

class Slider
{
public:
	enum class SliderStates
	{
		HELD,
		UNHELD,
	};

	WTexture* bar; //Slider bar texture
	WTexture* knob; //Slider knob texture
	Vector2* knobDims; //Knob dimensions
	Vector2* barDims; //Bar dimensions
	Vector2* pos; //Top left coords of bar
	float value = 1.0f; //Value of slider from 0 to 1, inclusive
	bool isEnabled = true; //True if the slider should be useable
	SliderStates state = SliderStates::UNHELD; //Current state of slider

	Slider(WTexture &bar, WTexture &knob, Vector2 &barDims, Vector2 &knobDims, Vector2 &pos, float value);
	~Slider();

	bool mouseIsInsideKnob(int x, int y);
	bool mouseIsInsideBar(int x, int y);
	bool mouseIsInsideSlider(int x, int y);
	void setState(SliderStates state);
	Vector2 getKnobPos();
	void updateSlider(SDL_Event* e);
};

