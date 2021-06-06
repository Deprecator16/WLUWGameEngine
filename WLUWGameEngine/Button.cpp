#include "Button.h"

Button::Button(Vector2 &pos, Vector2 &size, WTexture& normal, WTexture& hovered, WTexture& pressed, int menuID) :
	pos(&pos), 
	size(&size), 
	normal(&normal), 
	hovered(&hovered), 
	pressed(&pressed),
	menuID(menuID)
{
	state = ButtonStates::NORMAL;
}

Button::~Button()
{
	//Delete pointers and set to nullptr
	delete pos;
	delete size;
	delete normal;
	delete hovered;
	delete pressed;

	pos = nullptr;
	size = nullptr;
	normal = nullptr;
	hovered = nullptr;
	pressed = nullptr;

}

/**
* Check if mouse is inside button
* 
* @param x mouse x coord relative to screen
* @param y mouse y coord relative to screen
* @return True if mouse is inside button
*/
bool Button::mouseIsInside(int x, int y)
{
	bool inside = true; //Return value

	//Button coords
	int bX = pos->x;
	int bY = pos->y;

	//Iff any of the below are true, the mouse is not inside the button
	
	//Mouse is left of the button
	if (x < bX)
		inside = false;
	//Mouse is right of the button
	else if (x > bX + size->x)
		inside = false;
	//Mouse above the button
	else if (y < bY)
		inside = false;
	//Mouse below the button
	else if (y > bY + size->y)
		inside = false;

	return inside;
}

/**
* Update button states if mouse event occurred
* 
* @param e SDL_Event to check if mouse event occurred
*/
void Button::updateButton(SDL_Event* e)
{
	if (isEnabled)
	{
		//Get mouse position
		int x = 0;
		int y = 0;
		SDL_GetMouseState(&x, &y);

		//Update button states and check if button was clicked or pressed
		if (mouseIsInside(x, y) && e->type)
		{
			//Set button texture based on current state
			switch (e->type)
			{
			case SDL_MOUSEBUTTONDOWN:
				setState(ButtonStates::PRESSED);
				isPressed = true;
				break;

			case SDL_MOUSEBUTTONUP:
				setState(ButtonStates::NORMAL);

				//If the button was pressed and released on the button, consider this a button click
				if (isPressed)
				{
					isPressed = false;
					wasClicked = true;
				}
				break;

			default:
				setState(ButtonStates::HOVERED);
				break;
			}
		}
		//Set to normal state if mouse was depressed off of button or moved off without clicking
		else if (state == ButtonStates::HOVERED || e->type == SDL_MOUSEBUTTONUP)
			setState(ButtonStates::NORMAL);
	}
}

/**
* Set button state
* 
* @param state Set state of button
*/
void Button::setState(ButtonStates state)
{
	this->state = state;
}

void Button::onClick(onClickFunc clickFunc)
{
	clickFunc();
}

/**
* Get the current button texture
* 
* @return Currently displayed button texture
*/
WTexture* Button::getTexture()
{
	WTexture* tex = nullptr;
	switch (state)
	{
	case ButtonStates::HOVERED:
		tex = hovered;
		break;

	case ButtonStates::PRESSED:
		tex = pressed;
		break;

	case ButtonStates::NORMAL:
		tex = normal;
		break;
	default:
		tex = normal;
	}
	return tex;
}
