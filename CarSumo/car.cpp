#include "car.hpp"
#include "texture_id.hpp"
#include "data_tables.hpp"
#include "utility.hpp"
#include "constants.hpp"
#include <iostream>
#include "sound_node.hpp"
#include "network_node.hpp"


namespace
{
	const std::vector<CarData> Table = InitializeCarData();
}

TextureID ToTextureID(CarType type)
{
	switch (type)
	{
	case CarType::kBasic:
		return TextureID::kBasic;
		break;
	case CarType::kJeep:
		return TextureID::kJeep;
		break;
	case CarType::kTruck:
		return TextureID::kTruck;
		break;
	case CarType::kMonster:
		return TextureID::kMonster;
		break;
	}
	return TextureID::kBasic;
}

Car::Car(CarType type, const TextureHolder& textures, const FontHolder& fonts) 
	: Entity(Table[static_cast<int>(type)].m_hitpoints) 
	, m_type(type) 
	, m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture), Table[static_cast<int>(type)].m_texture_rect)
	, m_health_display(nullptr) 
	, m_directions_index(0)
	, m_is_marked_for_removal(false)
	, m_show_explosion(true)
	, m_explosion(textures.Get(TextureID::kExplosion))
	, m_explosion_began(false)
	, m_identifier(0)
{
	m_explosion.SetFrameSize(sf::Vector2i(256, 256));
	m_explosion.SetNumFrames(16);
	m_explosion.SetDuration(sf::seconds(1));
	Utility::CentreOrigin(m_sprite);
	Utility::CentreOrigin(m_explosion);

	std::string* health = new std::string("");
	std::unique_ptr<TextNode> health_display(new TextNode(fonts, *health));
	m_health_display = health_display.get();
	AttachChild(std::move(health_display));
	UpdateTexts();
}

uint8_t	Car::GetIdentifier()
{
	return m_identifier;
}

void Car::SetIdentifier(uint8_t identifier)
{
	m_identifier = identifier;
}

unsigned int Car::GetCategory() const
{
	return static_cast<unsigned int>(ReceiverCategories::kPlayerCar);
}


void Car::UpdateTexts()
{
	if (IsDestroyed())
	{
		m_health_display->SetString("");
	}
	else
	{
		m_health_display->SetString(std::to_string(GetHitPoints()) + "HP");
	}
	m_health_display->setPosition(sf::Vector2f(0.f, 50.f));
	m_health_display->setRotation(-getRotation());
}

float Car::GetMaxSpeed() const
{
	return Table[static_cast<int>(m_type)].m_speed;
}

sf::FloatRect Car::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

bool Car::IsMarkedForRemoval() const
{
	return IsDestroyed() && (m_explosion.IsFinished() || !m_show_explosion);
}

void Car::PlayLocalSound(CommandQueue& commands, SoundEffect effect)
{
	sf::Vector2f world_position = GetWorldPosition();

	Command command;
	command.category = static_cast<int>(ReceiverCategories::kSoundEffect);
	command.action = DerivedAction<SoundNode>(
		[effect, world_position](SoundNode& node, sf::Time)
		{
			node.PlaySound(effect, world_position);
		});
	commands.Push(command);
}

void Car::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (IsDestroyed() && m_show_explosion)
	{
		target.draw(m_explosion, states);
	}
	else
	{
		target.draw(m_sprite, states);
	}
}

void Car::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (IsDestroyed())
	{
		m_explosion.Update(dt);
		//Play explosion sound only once
		if (!m_explosion_began)
		{
			SoundEffect soundEffect = (Utility::RandomInt(2) == 0) ? SoundEffect::kExplosion1 : SoundEffect::kExplosion2;
			PlayLocalSound(commands, soundEffect);
			m_explosion_began = true;
		}
		return;
	}
	Entity::UpdateCurrent(dt, commands);
	UpdateTexts();
	std::cout << "Car " << +m_identifier << " Position: " << GetWorldPosition().x << ", " << GetWorldPosition().y << std::endl;
}

void Car::Remove()
{
	Entity::Remove();
	m_show_explosion = false;
}
