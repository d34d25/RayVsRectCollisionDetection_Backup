#include <iostream>
#include "entity.h"

bool can_jump;

void DrawEntity(struct Entity *entity, Color color)
{
	DrawRectangle(entity->position.x, entity->position.y, entity->width, entity->height, color);
}


bool RayvsRect(Vector2* position, Vector2* direction, Vector2* targetPosition, float targetWidth, float targetHeight, Vector2& contact_point, float& normal_x, float& normal_y, float& t_hit_near)
{
    //position is the ray origin

    normal_x = 0.0f;
    normal_y = 0.0f;
    contact_point = { 0.0f, 0.0f };

    Vector2 t_near{}, t_far{};

    Vector2 invdir;

    invdir.x = 1.0f / direction->x;
    invdir.y = 1.0f / direction->y;

    // Calculate intersections with rectangle bounding axes
    t_near.x = (targetPosition->x - position->x) * invdir.x;
    t_near.y = (targetPosition->y - position->y) * invdir.y;

    t_far.x = (targetPosition->x + targetWidth - position->x) * invdir.x;
    t_far.y = (targetPosition->y + targetHeight - position->y) * invdir.y;

    if (std::isnan(t_far.y) || std::isnan(t_far.x)) return false;
    if (std::isnan(t_near.y) || std::isnan(t_near.x)) return false;

    // Sort distances
    if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
    if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);

    // Early rejection		
    if (t_near.x > t_far.y || t_near.y > t_far.x) return false;

    // Closest 'time' will be the first contact
    t_hit_near = std::max(t_near.x, t_near.y);
    // Furthest 'time' is contact on opposite side of target
    float t_hit_far = std::min(t_far.x, t_far.y);

    // Reject if ray direction is pointing away from object
    if (t_hit_far < 0)
        return false;

    // Contact point of collision from parametric line equation
    contact_point.x = position->x + t_hit_near * direction->x;
    contact_point.y = position->y + t_hit_near * direction->y;

    if (t_near.x > t_near.y)
    {
        if (invdir.x < 0)
        {
            normal_x = 1.0f;
            normal_y = 0.0f;
        }
        else
        {
            normal_x = -1.0f;
            normal_y = 0.0f;
        }
    }
    else if (t_near.x < t_near.y)
    {
        if (invdir.y < 0)
        {
            normal_x = 0.0f;
            normal_y = 1.0f;
        }
        else
        {
            normal_x = 0.0f;
            normal_y = -1.0f;
        }
    }

    return true;
}

bool DynamicRectVsRect(Entity* entity, Rectangle* target, Vector2& contact_point, float& normal_x, float& normal_y, float& time, float elapsed_time)
{
    if (entity->velocity.x == 0 && entity->velocity.y == 0)
        return false;

    Vector2 entity_center;
    entity_center.x = entity->position.x + entity->width / 2;
    entity_center.y = entity->position.y + entity->height / 2;

    Vector2 entity_velocity;
    entity_velocity.x = entity->velocity.x * elapsed_time;
    entity_velocity.y = entity->velocity.y * elapsed_time;

    Vector2 expanded_target_pos = { target->x - entity->width / 2 , target->y - entity->height / 2 };
    Vector2 expanded_target_size = { target->width + entity->width , target->height + entity->height - 1 };

    if (RayvsRect(&entity_center, &entity_velocity, &expanded_target_pos, expanded_target_size.x, expanded_target_size.y, contact_point, normal_x, normal_y, time))
    {
        if (time >= 0.0f && time <= 1.0f)
        {
            return true;
        }
    }

    return false;
}

void ResolveDynamicRectVsRect(Entity* entity, Rectangle* obstacle)
{
    Vector2 contactPoint{};
    float normalX, normalY, time;

    float elapsedTime = GetFrameTime();

    // First, check for collision and resolve it
    if (DynamicRectVsRect(entity, obstacle, contactPoint, normalX, normalY, time, elapsedTime))
    {
        entity->velocity.x += normalX * abs(entity->velocity.x) * (1 - time);
        entity->velocity.y += normalY * abs(entity->velocity.y) * (1 - time);

        if (normalY < 0)
        {
            can_jump = true;
        }
    }
}

void ResolveFrameCollisions(Entity* entity)
{
    if (entity->position.x < 0)
    {
        entity->velocity.x = 0;
        entity->position.x = 0;
    }

    if (entity->position.x > GetScreenWidth())
    {
        entity->velocity.x = 0;
        entity->position.x = GetScreenWidth() - entity->width;
    }

    if (entity->position.y < 0)
    {
        entity->velocity.y = 0;
        entity->position.y = 0;
    }

    if (entity->position.y > GetScreenHeight())
    {
        entity->velocity.y = 0;
        entity->position.y = GetScreenHeight() - entity->height;
    }
}


void UpdateEntity(Entity* entity, float elapsedTime)
{
    //it's important to multiply the velocity by the elapsed time
    //otherwise the collisions won't be correctly detected

    entity->position.x += entity->velocity.x * elapsedTime; 
    entity->position.y += entity->velocity.y * elapsedTime;

    std::cout << entity->velocity.x << "\n";
    std::cout << entity->velocity.y << "\n";
}


