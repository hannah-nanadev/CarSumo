#pragma once
#include "scene_node.hpp"
#include "constants.hpp"

class Entity : public SceneNode
{
public:
	Entity(int hitpoints);
	void SetVelocity(sf::Vector2f velocity);
	void SetVelocity(float vx, float vy);
	sf::Vector2f GetVelocity() const;
	void SetTurnSpeed(sf::Angle speed);
	sf::Angle GetTurnSpeed() const;
	sf::Vector2f GetDirection() const;
	void Accelerate(sf::Vector2f velocity);
	void Accelerate(float vx, float vy);
	void Turn(sf::Angle amount);

	int GetHitPoints() const;
	void SetHitpoints(int points);
	void Repair(int points);
	void Damage(int points);
	void Destroy();
	virtual bool IsDestroyed() const override;
	virtual void Remove();

protected:
	virtual void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

private:
	sf::Vector2f m_velocity;
	sf::Angle m_rotation_speed;
	int m_hitpoints;
};

