#pragma once

class MapSystem
{
public:
	// This checks if there are any ChangeMapLevelRequests and if
	// there are it processes them.
	void step(float elapsed_ms);
};
