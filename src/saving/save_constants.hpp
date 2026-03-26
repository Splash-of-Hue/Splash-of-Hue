#pragma once

enum class SAVE_TYPE
{
	INVALID = 0,
	// If the thing has a position we should remember
	POSITION,
	// If the thing was removed or not during gameplay and should stay removed
	REMOVED,
};
