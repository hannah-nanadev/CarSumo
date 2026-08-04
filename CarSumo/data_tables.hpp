#pragma once
#include "texture_id.hpp"
#include <SFML/System/Time.hpp>
#include <functional>
#include "car.hpp"

struct Direction
{
	Direction(float angle, float distance)
		: m_angle(angle), m_distance(distance) {
	}
	float m_angle;
	float m_distance;
};

struct CarData
{
	int m_hitpoints;
	float m_speed;
	TextureID m_texture;
	sf::IntRect m_texture_rect;
	sf::Time m_fire_interval;
	std::vector<Direction> m_directions;
};

struct ProjectileData
{
	int m_damage;
	float m_speed;
	TextureID m_texture;
	sf::IntRect m_texture_rect;
};

struct PickupData
{
	std::function<void(Car&)> m_action;
	TextureID m_texture;
	sf::IntRect m_texture_rect;
};

struct ParticleData
{
	sf::Color m_color;
	sf::Time m_lifetime;
};

std::vector<CarData> InitializeCarData();
std::vector<ParticleData> InitializeParticleData();





