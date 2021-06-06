#include "Slider.h"

Slider::Slider(WTexture& bar, WTexture& knob, Vector2& barDims, Vector2& knobDims, Vector2& pos, float value, int menuID) :
	bar(&bar),
	knob(&knob),
	barDims(&barDims),
	knobDims(&knobDims),
	pos(&pos),
	value(value),
	menuID(menuID)
{

}

Slider::~Slider()
{
	//Delete pointers and set to nullptr
	delete bar;
	delete knob;
	delete knobDims;
	delete barDims;
	delete pos;

	bar = nullptr;
	knob = nullptr;
	knobDims = nullptr;
	barDims = nullptr;
	pos = nullptr;
}

/**
* Check if mouse is inside knob. Note that this does not check if mouse is inside the bar itself
* 
* @param x mouse x coord relative to window
* @param y mouse y coord relative to window
* @return True if mouse is inside knob
*/
bool Slider::mouseIsInsideKnob(int x, int y)
{
	bool inside = true; //Return value

	//Get top left coords of knob
	//Assuming the knob slides along the bar from its centre, 
	//allowing for the knob to "hang" off either end of the bar 
	//when put on the ends of the bar, the top left will be at:
	// x = bar x coord + percentage of current value * length of bar - 1/2 * knob length
	//   = pos.x + value * barDims.x - 1/2 * knobDims.x
	// y = bar y coord + 1/2 * bar height - 1/2 * knob height
	//   = pos.y + 1/2 * barDims.y - 1/2 * knobDims.y
	int kX = pos->x + value * barDims->x - 1 / 2 * knobDims->x;
	int kY = pos->y + 1 / 2 * barDims->y - 1 / 2 * knobDims->y;

	//Mouse is left of the button
	if (x < kX)
	{
		inside = false;
	}
	//Mouse is right of the button
	else if (x > kX + knobDims->x)
	{
		inside = false;
	}
	//Mouse above the button
	else if (y < kY)
	{
		inside = false;
	}
	//Mouse below the button
	else if (y > kY + knobDims->y)
	{
		inside = false;
	}
	return inside;
}

/**
* Check if mouse is inside bar. Note that this does not check if mouse is inside the knob
*
* @param x mouse x coord relative to window
* @param y mouse y coord relative to window
* @return True if mouse is inside bar
*/
bool Slider::mouseIsInsideBar(int x, int y)
{
	bool inside = true; //Return value

	int bX = pos->x;
	int bY = pos->y;

	//Mouse is left of the button
	if (x < bX)
	{
		inside = false;
	}
	//Mouse is right of the button
	else if (x > bX + barDims->x)
	{
		inside = false;
	}
	//Mouse above the button
	else if (y < bY)
	{
		inside = false;
	}
	//Mouse below the button
	else if (y > bY + barDims->y)
	{
		inside = false;
	}
	return inside;
}

/**
* Check if mouse is inside slider. This includes the bar and the slider
* 
* @param x mouse x coord relative to window
* @param y mouse y coord relative to window
* @return True if mouse is inside slider
*/
bool Slider::mouseIsInsideSlider(int x, int y)
{
	return mouseIsInsideBar(x, y) || mouseIsInsideKnob(x, y);
}

/**
* Set state of slider
* 
* @param state State to set slider to
*/
void Slider::setState(SliderStates state)
{
	this->state = state;
}

/**
* Get top left coord of knob
* 
* @return Vector2 of knob's coords
*/
Vector2 Slider::getKnobPos()
{
	float newPosX = pos->x + value * barDims->x - (0.5f) * knobDims->x;
	float newPosY = pos->y + (0.5f) * barDims->y - (0.5f) * knobDims->y;

	return Vector2{ newPosX, newPosY };
}

/**
* Update slider states if mouse event occurred
*
* @param e SDL_Event to check if mouse event occurred
*/
void Slider::updateSlider(SDL_Event* e)
{
	if (isEnabled)
	{
		//Get mouse position
		int x = 0;
		int y = 0;
		SDL_GetMouseState(&x, &y);

		//If mouse was inside slider and pressed
		if (mouseIsInsideSlider(x, y) && e->type == SDL_MOUSEBUTTONDOWN)
			setState(SliderStates::HELD);
		else if (e->type == SDL_MOUSEBUTTONUP)
			setState(SliderStates::UNHELD);

		//Update value if slider was dragged or a click ocurred on the slider
		if (state == SliderStates::HELD)
		{
			//Make value maximum if mouse is at the right end or further to the right
			if (x >= pos->x + barDims->x)
				value = 1.0f;
			//Make value maximum if mouse is at the left end or further to the left
			else if (x <= pos->x)
				value = 0.0f;
			//Mouse is between both ends of the slider, so calculate value. value = (mouseX - barX) / bar width
			else
				value = (x - pos->x) / barDims->x;
		}
	}
}
