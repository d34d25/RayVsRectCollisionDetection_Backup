#pragma once
#include <iostream>
#include <limits>
#include "raylib.h"

struct Entity
{
	Vector2 position;
	Vector2 velocity;
	
	int width, height;

	bool collision;
};

extern bool can_jump;

void DrawEntity(struct Entity* entity, Color color);

bool RayvsRect(Vector2* position, Vector2* direction, Vector2* targetPosition, float targetWidth, float targetHeight, Vector2& contact_point, float& normal_x, float& normal_y, float& time);

bool DynamicRectVsRect(Entity* entity, Rectangle* target, Vector2& contact_point, float& normal_x, float& normal_y, float& time, float elapsed_time);

void ResolveDynamicRectVsRect(struct Entity* entity, Rectangle* obstacle);

void ResolveFrameCollisions(Entity* entity);

void UpdateEntity(struct Entity* entity, float elapsedTime);