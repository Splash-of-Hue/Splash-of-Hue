#pragma once

/// <summary>
/// The distance between each point on the trail 
/// </summary>
const float PARTY_TRAIL_DISTANCE_STEP = 70;
/// <summary>
/// The number of indices a member can be away before snapping back
/// </summary>
const int PARTY_TRAIL_MAX_INDEX_DISTANCE = 5;
/// <summary>
/// The distance from the trail marker, before it's considered reached
/// </summary>
const int PARTY_TRAIL_DELTA = 5;

enum class PARTY_CHANGE_REQUEST_TYPE
{
	JOIN,
	LEAVE,
};
