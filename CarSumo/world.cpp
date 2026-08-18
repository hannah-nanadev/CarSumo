#include "world.hpp"
#include "sprite_node.hpp"
#include <iostream>
#include "state.hpp"
#include <SFML/System/Angle.hpp>
#include "particle_node.hpp"
#include "particletype.hpp"
#include "sound_node.hpp"

World::World(sf::RenderTarget& output_target, FontHolder& font, SoundPlayer& sounds, bool networked)
	: m_target(output_target)
	, m_camera(output_target.getDefaultView())
	, m_textures()
	, m_fonts(font)
	, m_sounds(sounds)
	, m_scene_graph(ReceiverCategories::kNone)
	, m_scene_layers()
	, m_world_bounds(sf::Vector2f(0.f, 0.f), sf::Vector2f(m_camera.getSize().x, m_camera.getSize().y))
	, m_spawn_position(m_camera.getSize().x / 2.f, m_world_bounds.size.y - m_camera.getSize().y/2.f)
	, m_player_car()
	, m_networked_world(networked)
	, m_network_node(nullptr)
	, m_finish_sprite(nullptr)
{
	m_scene_texture.resize({ m_target.getSize().x, m_target.getSize().y });
	LoadTextures();
	BuildScene();
	m_camera.setCenter(m_spawn_position);
}

void World::Update(sf::Time dt)
{
	//Process commands from the scenegraph
	while (!m_command_queue.IsEmpty())
	{
		m_scene_graph.OnCommand(m_command_queue.Pop(), dt);
	}

	AdaptPlayerVelocity();
	HandleCollisions();

	auto first_to_remove = std::remove_if(m_player_car.begin(), m_player_car.end(), std::mem_fn(&Car::IsMarkedForRemoval));
	m_player_car.erase(first_to_remove, m_player_car.end());
	m_scene_graph.RemoveWrecks();

	m_scene_graph.Update(dt, m_command_queue);
	AdaptPlayerPosition();
	//UpdateSounds();
}



void World::Draw()
{
	m_target.setView(m_camera);
	m_target.draw(m_scene_graph);
}

Car* World::GetCar(int identifier) const
{
	for (Car* a : m_player_car)
	{
		if (a->GetIdentifier() == identifier)
		{
			return a;
		}
	}
	return nullptr;
}

void World::RemoveCar(uint8_t identifier)
{
	Car* car = GetCar(identifier);
	if (car)
	{
		car->Destroy();
		m_player_car.erase(std::find(m_player_car.begin(), m_player_car.end(), car));
	}
}

Car* World::AddCar(uint8_t identifier, CarType type)
{
	std::unique_ptr<Car> player(new Car(type, m_textures, m_fonts));
	player->setPosition(m_camera.getCenter());
	std::cout << "World::AddCar " << +identifier << std::endl;
	player->SetIdentifier(identifier);

	m_player_car.emplace_back(player.get());
	m_scene_layers[static_cast<int>(SceneLayers::kUpperAir)]->AttachChild(std::move(player));
	return m_player_car.back();
}

bool World::PollGameAction(GameActions::Action& out)
{
	return m_network_node->PollGameAction(out);
}

CommandQueue& World::GetCommandQueue()
{
	return m_command_queue;
}

bool World::HasAlivePlayer() const
{
	return !m_player_car.empty();
}

void World::LoadTextures()
{
	m_textures.Load(TextureID::kCars, "Media/Textures/carscombined.png");
	m_textures.Load(TextureID::kExplosion, "Media/Textures/Explosion.png");
	m_textures.Load(TextureID::kArena, "Media/Textures/arena.png");
	m_textures.Load(TextureID::kParticle, "Media/Textures/Particle.png");
}

void World::BuildScene()
{
	//Initialise the different layers
	for (int i = 0; i < static_cast<int>(SceneLayers::kLayerCount); i++)
	{
		ReceiverCategories category = (i == static_cast<int>(SceneLayers::kUpperAir)) ? ReceiverCategories::kScene : ReceiverCategories::kNone;
		SceneNode::Ptr layer(new SceneNode(category));
		m_scene_layers[i] = layer.get();
		m_scene_graph.AttachChild(std::move(layer));
	}

	//Prepare the background
	sf::Texture& texture = m_textures.Get(TextureID::kArena);
	sf::IntRect texture_rect(m_world_bounds);
	texture.setRepeated(true);

	float view_height = m_camera.getSize().y;
	texture_rect.size.y += static_cast<int>(view_height);

	//Add the background sprite to the world
	std::unique_ptr<SpriteNode> background_sprite(new SpriteNode(texture, texture_rect));
	background_sprite->setPosition(sf::Vector2f(m_world_bounds.position.x, m_world_bounds.position.y - view_height));
	m_scene_layers[static_cast<int>(SceneLayers::kBackground)]->AttachChild(std::move(background_sprite));

	/*
	//Add the particle nodes to the scene
	std::unique_ptr<ParticleNode> smokeNode(new ParticleNode(ParticleType::kSmoke, m_textures));
	m_scene_layers[static_cast<int>(SceneLayers::kLowerAir)]->AttachChild(std::move(smokeNode));

	std::unique_ptr<ParticleNode> propellantNode(new ParticleNode(ParticleType::kPropellant, m_textures));
	m_scene_layers[static_cast<int>(SceneLayers::kLowerAir)]->AttachChild(std::move(propellantNode));
	*/

	//Add sound effect node
	std::unique_ptr<SoundNode> soundNode(new SoundNode(m_sounds));
	m_scene_graph.AttachChild(std::move(soundNode));

	if (m_networked_world)
	{
		std::unique_ptr<NetworkNode> network_node(new NetworkNode());
		m_network_node = network_node.get();
		m_scene_graph.AttachChild(std::move(network_node));
	}
}

void World::AdaptPlayerVelocity()
{
	for (Car* car : m_player_car)
	{
		sf::Vector2f velocity = car->GetVelocity();

		//If they are moving diagonally divide by sqrt 2
		if (velocity.x != 0.f && velocity.y != 0.f)
		{
			car->SetVelocity(velocity / std::sqrt(2.f));
		}

	}
}

void World::AdaptPlayerPosition()
{
	//keep player on the screen
	sf::FloatRect view_bounds = GetViewBounds();
	const float border_distance = 10.f;

	for (Car* car : m_player_car)
	{
		sf::Vector2f position = car->getPosition();
		position.x = std::min(position.x, view_bounds.position.x + view_bounds.size.x - border_distance);
		position.x = std::max(position.x, view_bounds.position.x + border_distance);
		position.y = std::min(position.y, view_bounds.position.y + view_bounds.size.y - border_distance);
		position.y = std::max(position.y, view_bounds.position.y + border_distance);
		car->setPosition(position);
	}

}

sf::FloatRect World::GetViewBounds() const
{
	return sf::FloatRect(m_camera.getCenter() - m_camera.getSize() / 2.f, m_camera.getSize());
}

bool MatchesCategories(SceneNode::Pair& colliders, ReceiverCategories type1, ReceiverCategories type2)
{
	unsigned int category1 = colliders.first->GetCategory();
	unsigned int category2 = colliders.second->GetCategory();

	if ((static_cast<int>(type1) & category1) && (static_cast<int>(type2) & category2))
	{
		return true;
	}
	else if ((static_cast<int>(type1) & category2) && (static_cast<int>(type2) & category1))
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}

}

void World::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_scene_graph.CheckSceneCollision(m_scene_graph, collision_pairs);

	for (SceneNode::Pair pair : collision_pairs)
	{
		if (MatchesCategories(pair, ReceiverCategories::kPlayerCar, ReceiverCategories::kPlayerCar))
		{
			auto& player1 = static_cast<Car&>(*pair.first);
			auto& player2 = static_cast<Car&>(*pair.second);
			//Collision response
			player1.Damage(player2.GetHitPoints());
		}
	}
}

void World::UpdateSounds()
{
	sf::Vector2f listener_position;

	// 0 players (multiplayer mode, until server is connected) -> view center
	if (m_player_car.empty())
	{
		listener_position = m_camera.getCenter();
	}

	// 1 or more players -> mean position between all cars
	else
	{
		for (Car* car : m_player_car)
		{
			listener_position += car->GetWorldPosition();
		}

		listener_position /= static_cast<float>(m_player_car.size());
	}

	m_sounds.SetListenerPosition(listener_position);

	m_sounds.RemoveStoppedSounds();
}


