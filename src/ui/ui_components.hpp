#pragma once
#include "common.hpp"
#include "ui_constants.hpp"

struct Menu
{
	bool active;
	virtual void select() = 0;
	virtual void cancel() = 0;
	virtual void move(DIRECTION direction) = 0;
	virtual void enable() = 0;
	virtual void disable() = 0;
};

struct MenuToggleRequest
{
	MENU_TYPE type;
};

struct MenuAction
{
	MENU_ACTION type;
};
