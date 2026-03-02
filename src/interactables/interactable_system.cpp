#include "interactable_system.hpp"
#include <ecs/registry.hpp>
#include <iostream>

void InteractableSystem::step(float elapsed_ms)
{
    if (registry.players.entities.empty()) return;
    Entity player = registry.players.entities[0];
    std::vector<Entity> orbs_to_remove(registry.orbCollected.entities.begin(), registry.orbCollected.entities.end());

    for (Entity orb : orbs_to_remove)
    {
        if (!registry.colourOrbs.has(orb)) {
            continue;
        }

        Player& playerComp = registry.players.get(player);
        ColourOrb& orbComp = registry.colourOrbs.get(orb);

        if (orbComp.colour == COLOUR_STATE::BLUE)
        {
            playerComp.blue_unlocked = true;
            std::cout << "Player unlocked BLUE" << std::endl;
        }
        else if (orbComp.colour == COLOUR_STATE::RED)
        {
            playerComp.red_unlocked = true;
            std::cout << "Player unlocked RED" << std::endl;
        }

        registry.remove_all_components_of(orb);
    }
}
