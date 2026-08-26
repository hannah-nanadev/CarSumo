//Hannah Kellett D00260463

#pragma once
#include "component.hpp"
#include "resource_identifiers.hpp"
#include "button_type.hpp"
#include "state.hpp"
#include "sound_player.hpp"
#include "car_type.hpp"
#include <functional>

namespace gui
{
	class Button : public Component
	{
	public:
		typedef std::shared_ptr<Button> Ptr;
		typedef std::function<void()> Callback;

	public:
		Button(State::Context context);
		void SetCallback(Callback callback);
		void SetText(const std::string& text);
		void SetToggle(bool flag);

		virtual bool IsSelectable() const override;
		virtual void Select() override;
		virtual void Deselect() override;
		virtual void Activate() override;
		virtual void Deactivate() override;
		virtual void HandleEvent(const sf::Event& event) override;

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
		void ChangeTexture(ButtonType button_type);

	private:
		Callback m_callback;
		sf::Sprite m_sprite;
		sf::Text m_text;
		bool m_is_toggle;

		SoundPlayer& m_sounds;
	};

	class CarPanel : public Component
	{
	public:
		CarPanel(State::Context context, CarType car);
		State::Context GetContext() const;
		void SetCarTexture();
		CarType GetCar() const;

		virtual bool IsSelectable() const override;
		virtual void HandleEvent(const sf::Event& event) override;

	private:
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	private:
		State::Context m_context;
		sf::Sprite m_background_sprite;
		sf::Sprite m_car_sprite;
		CarType m_car_type;
	};
}

