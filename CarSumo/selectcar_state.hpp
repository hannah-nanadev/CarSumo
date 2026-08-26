//Hannah Kellett D00260463

#pragma once
#include "statestack.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include "container.hpp"
#include "button.hpp"
#include "label.hpp"
#include "car_type.hpp"

class SelectCarState : public State
{
public:
	SelectCarState(StateStack& stack, Context context, int playercount, StateID nextState);
	virtual void Draw() override;
	virtual bool Update(sf::Time dt) override;
	virtual bool HandleEvent(const sf::Event& event) override;

private:
	void UpdateLabels();
	void AddButtonLabel(std::size_t index, std::size_t x, std::size_t y, const std::string& text, CarType car, int player, Context context);

private:
	sf::Sprite m_background_sprite;
	gui::Container m_gui_container;
	std::array<gui::Button::Ptr, static_cast<int>(CarType::kCarCount)> m_car_buttons;
	std::array<gui::Label::Ptr, static_cast<int>(CarType::kCarCount)> m_car_labels;
	StateID m_next_state;
	int m_playercount;
};