#pragma once
#include <SFML/Graphics.hpp>
#include "resource_identifiers.hpp"
#include "scene_node.hpp"
#include "scene_layers.hpp"
#include "car.hpp"
#include "command_queue.hpp"
#include "bloom_effect.hpp"
#include "sound_player.hpp"
#include "sprite_node.hpp"

#include <array>
#include "network_node.hpp"

class World
{
public:
	explicit World(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, bool networked = false);
	void Update(sf::Time dt);
	void Draw();

	sf::FloatRect GetViewBounds() const;
	CommandQueue& GetCommandQueue();

	Car* AddCar(uint8_t identifier, CarType type);
	void RemoveCar(uint8_t identifier);

	bool HasAlivePlayer() const;

	void SetWorldScrollCompensation(float compensation);
	Car* GetCar(int identifier) const;
	bool PollGameAction(GameActions::Action& out);

private:
	void LoadTextures();
	void BuildScene();

	void HandleCollisions();

	void UpdateSounds();

private:
	struct SpawnPoint
	{
		SpawnPoint(CarType type, float x, float y) :m_type(type), m_x(x), m_y(y)
		{

		}
		CarType m_type;
		float m_x;
		float m_y;
	};

private:
	sf::RenderTarget& m_target;
	sf::RenderTexture m_scene_texture;
	sf::View m_camera;
	TextureHolder m_textures;
	FontHolder& m_fonts;
	SoundPlayer& m_sounds;
	SceneNode m_scene_graph;
	std::array<SceneNode*, static_cast<int>(SceneLayers::kLayerCount)> m_scene_layers;
	sf::FloatRect m_world_bounds;
	sf::Vector2f m_spawn_position;

	std::vector<Car*> m_player_car;

	CommandQueue m_command_queue;

	BloomEffect m_bloom_effect;
	bool m_networked_world;
	NetworkNode* m_network_node;
	SpriteNode* m_finish_sprite;
};

