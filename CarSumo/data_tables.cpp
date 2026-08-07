#include "data_tables.hpp"
#include "car_type.hpp"
#include "car.hpp"
#include "constants.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "particletype.hpp"

std::vector<CarData> InitializeCarData()
{
	std::vector<CarData> data(static_cast<int>(CarType::kCarCount));

	data[static_cast<int>(CarType::kBasic)].m_hitpoints = 20;
	data[static_cast<int>(CarType::kBasic)].m_speed = 50.f;
	data[static_cast<int>(CarType::kBasic)].m_texture = TextureID::kCars;
	data[static_cast<int>(CarType::kBasic)].m_texture_rect = sf::IntRect({0, 0}, {60, 100});

	data[static_cast<int>(CarType::kJeep)].m_hitpoints = 30;
	data[static_cast<int>(CarType::kJeep)].m_speed = 50.f;
	data[static_cast<int>(CarType::kJeep)].m_texture = TextureID::kCars;
	data[static_cast<int>(CarType::kJeep)].m_texture_rect = sf::IntRect({ 60, 0 }, { 60, 100 });

	data[static_cast<int>(CarType::kTruck)].m_hitpoints = 50;
	data[static_cast<int>(CarType::kTruck)].m_speed = 35.f;
	data[static_cast<int>(CarType::kTruck)].m_texture = TextureID::kCars;
	data[static_cast<int>(CarType::kTruck)].m_texture_rect = sf::IntRect({ 120, 0 }, { 60, 100});

	data[static_cast<int>(CarType::kMonster)].m_hitpoints = 40;
	data[static_cast<int>(CarType::kMonster)].m_speed = 40.f;
	data[static_cast<int>(CarType::kMonster)].m_texture = TextureID::kCars;
	data[static_cast<int>(CarType::kMonster)].m_texture_rect = sf::IntRect({ 180, 0 }, { 60, 100});

	return data;
}

std::vector<ParticleData> InitializeParticleData()
{
	std::vector<ParticleData> data(static_cast<int>(ParticleType::kParticleCount));

	data[static_cast<int>(ParticleType::kPropellant)].m_color = sf::Color(255, 255, 50);
	data[static_cast<int>(ParticleType::kPropellant)].m_lifetime = sf::seconds(0.5f);

	data[static_cast<int>(ParticleType::kSmoke)].m_color = sf::Color(50, 50, 50);
	data[static_cast<int>(ParticleType::kSmoke)].m_lifetime = sf::seconds(2.5f);
	return data;
}
