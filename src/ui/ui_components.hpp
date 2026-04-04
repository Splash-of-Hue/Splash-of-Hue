#pragma once
#include "common.hpp"
#include "ui_constants.hpp"

struct Menu
{
	bool active = false;
	virtual void select() = 0;
	virtual void cancel() = 0;
	virtual void move(DIRECTION direction) = 0;
	virtual void enable() = 0;
	virtual void disable() = 0;
};

struct UiToggleRequest
{
	UI_TYPE type;
};

struct MenuAction
{
	MENU_ACTION type;
};

struct Screen
{
	bool active = false;
	virtual void enable() = 0;
	virtual void disable() = 0;
};
