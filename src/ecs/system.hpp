#pragma once

struct SystemInterface
{
	virtual void step(float elapsed_ms) = 0;
};
