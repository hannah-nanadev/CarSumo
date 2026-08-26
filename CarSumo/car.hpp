#pragma once
#include "entity.hpp"
#include "car_type.hpp"
#include "resource_identifiers.hpp"
#include "text_node.hpp"
#include "command_queue.hpp"
#include "animation.hpp"

class Car : public Entity
{
public:
	Car(CarType type, const TextureHolder& textures, const FontHolder& fonts);
	unsigned int GetCategory() const override;
	uint8_t GetIdentifier();
	void SetIdentifier(uint8_t identifier);

	void UpdateTexts();

	float GetMaxSpeed() const;
	float GetTurnSpeed() const;
	float GetMaxTurnSpeed() const;

	void Knockback();

	CarType GetCarType() const;
	void SetCarType(CarType type);

	sf::FloatRect GetBoundingRect() const override;
	bool IsMarkedForRemoval() const override;
	void Remove() override;
	void PlayLocalSound(CommandQueue& commands, SoundEffect effect);

private:
	virtual void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

private:
	CarType m_type;
	sf::Sprite m_sprite;
	Animation m_explosion;

	TextNode* m_health_display;

	int m_directions_index;

	bool m_is_marked_for_removal;
	bool m_show_explosion;
	bool m_explosion_began;

	uint8_t m_identifier;

};

