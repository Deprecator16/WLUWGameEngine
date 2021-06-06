#include "WMenu.h"

WMenu::WMenu()
{
}

WMenu::WMenu(vector<Button>& buttons, vector<Slider>& sliders) :
	buttons(&buttons),
	sliders(&sliders)
{
	//Assign every menu button and slider a unique menu ID

	for (Button b : buttons)
	{
		b.menuID = this->menuIDCOunter++; //Assign id then increment id counter
	}

	for (Slider s : sliders)
	{
		s.menuID = this->menuIDCOunter++; //Assign id then increment id counter
	}
}

WMenu::~WMenu()
{
	delete buttons;
	delete sliders;

	buttons = nullptr;
	sliders = nullptr;
}

/**
* Add button to menu
* 
* @param button Button to add to menu
*/
void WMenu::addButton(Button button)
{
	button.menuID = menuIDCOunter++; //Assign unique menu ID
	buttons->push_back(button);
}

/**
* Add slider to menu
* 
* @param slider Slider to add to menu
*/
void WMenu::addSlider(Slider slider)
{
	slider.menuID = menuIDCOunter++; //Assign unique menu ID
	sliders->push_back(slider);
}

/**
* Remove button by menuID
* 
* @param menuID ID of button to remove
* @return Removed button
*/
Button* WMenu::removeButton(int menuID)
{
	//Search for button with menuID, then remove and return it
	Button* b;
	for (int i = 0; i < buttons->size(); i++)
	{
		if (buttons->at(i).menuID == menuID)
		{
			b = &buttons->at(i);
			buttons->erase(buttons->begin() + i);
			return b;
		}
	}
}

/**
* Remove slider by menuID
*
* @param menuID ID of slider to remove
* @return Removed slider
*/
Slider* WMenu::removeSlider(int menuID)
{
	//Search for slider with menuID, then remove and return it
	Slider* s;
	for (int i = 0; i < sliders->size(); i++)
	{
		if (sliders->at(i).menuID == menuID)
		{
			s = &sliders->at(i);
			sliders->erase(sliders->begin() + i);
			return s;
		}
	}
}
