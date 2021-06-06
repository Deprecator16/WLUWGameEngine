#pragma once

#include <vector>
#include "Header.h"
#include "Button.h"
#include "Slider.h"

/**
* Base Menu class
*/
class WMenu
{
public:
	vector<Button>* buttons = nullptr; //Contains all buttons in this menu
	vector<Slider>* sliders = nullptr; //Contains all sliders in this menu
	int menuIDCounter = 0; //Menu ID to assign individual components

	WMenu();
	WMenu(vector<Button> &buttons, vector<Slider> &sliders);
	~WMenu();

	void addButton(Button button);
	void addSlider(Slider slider);
	Button* removeButton(int menuID);
	Slider* removeSlider(int menuID);
};

