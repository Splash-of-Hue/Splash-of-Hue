#pragma once
#include "common.hpp"

// font character structure
struct Character
{
	unsigned int TextureID;  // ID handle of the glyph texture
	ivec2   Size;       // Size of glyph
	ivec2   Bearing;    // Offset from baseline to left/top of glyph
	unsigned int Advance;    // Offset to advance to next glyph
	char character;
};

enum class FONT_ID
{
	ALOJA_EXTENDED = 0,
	FONT_COUNT
};
const int font_count = (int)FONT_ID::FONT_COUNT;

const FONT_ID DEFAULT_FONT = FONT_ID::ALOJA_EXTENDED;
