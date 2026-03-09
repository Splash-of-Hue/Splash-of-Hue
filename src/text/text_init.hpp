#pragma once
#include "common.hpp"
#include <string>

/// <summary>
/// Creates an entity for rendered text in screen space.
/// </summary>
/// <param name="text_string">The actual string to render</param>
/// <param name="position">The position to render the text</param>
/// <param name="scale">The size of the text. NOTE: This is not font size, but direct scaling of the text.</param>
/// <param name="color">The color of the text.</param>
/// <param name="font_id">The ID of the font to use.</param>
/// <returns>The entity for the text.</returns>
Entity create_text(std::string text_string, vec2 position, vec2 scale, vec4 color, FONT_ID font_id = DEFAULT_FONT);
