#include "common.hpp"
#include "ecs/registry.hpp"
#include "renderer/render_constants.hpp"
#include "text_constants.hpp"
#include "text_init.hpp"
#include <string>

Entity create_text(std::string text_string, vec2 position, vec2 scale, vec4 color, FONT_ID font_id)
{
	Entity entity;

	registry.texts.insert(entity, { text_string, font_id });
	registry.positions.insert(entity, { position });
	registry.scales.insert(entity, { scale });
	registry.colors.insert(entity, { color });
	registry.renderRequests.insert(entity, {
		EFFECT_ASSET_ID::FONT,
		GEOMETRY_BUFFER_ID::SCREEN_TEXT,
		RENDER_LAYER::UI
    });

	return entity;
}
