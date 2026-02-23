#include "camera_components.hpp"
#include "camera_system.hpp"
#include "common.hpp"
#include "ecs/registry.hpp"

void CameraSystem::step(float elapsed_ms)
{
	if (registry.cameras.size() == 0)
	{
		Entity camera_entity;
		registry.cameras.emplace(camera_entity);
		registry.positions.emplace(camera_entity);
		registry.velocities.emplace(camera_entity, vec2(0));
	}

	if (registry.changeCameraTargetRequests.size() == 0)
	{
		return;
	}

	// Take only the most recent camera change request
	const Entity& most_recent_request_entity = registry.changeCameraTargetRequests.entities.back();
	const ChangeCameraTargetRequest& change_request = registry.changeCameraTargetRequests.get(most_recent_request_entity);
	assert(registry.cameras.size() == 1);
	const Entity& camera_entity = registry.cameras.entities[0];
	const vec2& camera_position = registry.positions.get(camera_entity);
	vec2& camera_velocity = registry.velocities.get(camera_entity);

	if (camera_position == change_request.target)
	{
		// We have arrived, clear all requests and finish
		registry.changeCameraTargetRequests.clear();
		camera_velocity = vec2(0);
		return;
	}

	const vec2& request_position_difference = change_request.target - camera_position;
	const vec2& camera_move_direction = normalize(request_position_difference);
	// Only go part of the way if we have less than the speed to get there
	const vec2& move_velocity = fminf(change_request.speed, length(request_position_difference)) * camera_move_direction;
	camera_velocity = move_velocity;

	// If multiple camera changes are requested, we only care about the most recent one
	// Remove them going backwards
	if (registry.changeCameraTargetRequests.size() > 1)
	{
		while (registry.changeCameraTargetRequests.entities.size() > 1)
		{
			const unsigned long long& next_remove_index = registry.changeCameraTargetRequests.size() - 2;
			const Entity& second_last_request_entity = registry.changeCameraTargetRequests.entities[next_remove_index];

			registry.changeCameraTargetRequests.remove(second_last_request_entity);
		}
	}
}
