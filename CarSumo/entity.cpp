#include "entity.hpp"
#include "command_queue.hpp"

Entity::Entity(int hitpoints) :m_hitpoints(hitpoints)
{
}

void Entity::SetVelocity(sf::Vector2f velocity)
{
	m_velocity = velocity;
}

void Entity::SetVelocity(float vx, float vy)
{
	m_velocity.x = vx;
	m_velocity.y = vy;
}

sf::Vector2f Entity::GetVelocity() const
{
	return m_velocity;
}

void Entity::SetTurnSpeed(sf::Angle speed)
{
	m_rotation_speed = speed;
}

sf::Angle Entity::GetTurnSpeed() const
{
	return m_rotation_speed;
}

sf::Vector2f Entity::GetDirection() const
{
	sf::Angle angle = getRotation();
	sf::Vector2f direction(std::cos(angle.asRadians()), std::sin(angle.asRadians()));
	float magnitude = std::sqrt(direction.x * direction.x + direction.y * direction.y);
	return direction / magnitude; // Normalize the direction vector
}

void Entity::Accelerate(sf::Vector2f velocity)
{
	m_velocity += velocity;
}

void Entity::Turn(sf::Angle amount)
{
	m_rotation_speed += amount;
}

void Entity::Accelerate(float vx, float vy)
{
	m_velocity.x += vx;
	m_velocity.y += vy;
}

int Entity::GetHitPoints() const
{
	return m_hitpoints;
}

void Entity::SetHitpoints(int points)
{
	//assert(points > 0);
	m_hitpoints = points;
}

void Entity::Repair(int points)
{
	assert(points >= 0);
	m_hitpoints += points;
}

void Entity::Damage(int points)
{
	assert(points > 0);
	m_hitpoints -= points;
}

void Entity::Destroy()
{
	m_hitpoints = 0;
}

bool Entity::IsDestroyed() const
{
	return m_hitpoints <= 0;
}

void Entity::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	m_velocity = m_velocity * (1.f - kMoveFriction * dt.asSeconds());
	m_rotation_speed = m_rotation_speed * (1.f - kTurnFriction * dt.asSeconds());

	move(m_velocity * dt.asSeconds());
	rotate(m_rotation_speed * dt.asSeconds());
}

void Entity::Remove()
{
	Destroy();
}