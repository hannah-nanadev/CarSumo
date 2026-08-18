#include "selectcar_state.hpp"
#include "Utility.hpp"

SelectCarState::SelectCarState(StateStack& stack, Context context)
	: State(stack, context)
	, m_gui_container()
	, m_background_sprite(context.textures->Get(TextureID::kTitleScreen))
{
	//Build car selection buttons and labels
	for (std::size_t x = 0; x < static_cast<int>(CarType::kCarCount); ++x)
	{
		AddButtonLabel(x, x * 200 + 100, 300, CarTypeNames[x], context);
	}
	UpdateLabels();

	auto back_button = std::make_shared<gui::Button>(context);
	back_button->setPosition(sf::Vector2f(80.f, 620.f));
	back_button->SetText("Back");
	back_button->SetCallback(std::bind(&SelectCarState::RequestStackPop, this));
	m_gui_container.Pack(back_button);
}

void SelectCarState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.draw(m_background_sprite);
	window.draw(m_gui_container);
}

bool SelectCarState::Update(sf::Time dt)
{
	return true;
}

bool SelectCarState::HandleEvent(const sf::Event& event)
{
	m_gui_container.HandleEvent(event);
	return false;
}

void SelectCarState::UpdateLabels()
{
	for (std::size_t i = 0; i < static_cast<int>(CarType::kCarCount); ++i)
	{
		m_car_labels[i]->SetText(CarTypeNames[i]);
	}
}

void SelectCarState::AddButtonLabel(std::size_t index, std::size_t x, std::size_t y, const std::string& text, Context context)
{
	auto button = std::make_shared<gui::Button>(context);
	button->setPosition(sf::Vector2f(x, y));
	button->SetText(text);
	button->SetCallback([this, index]()
		{
			UpdateLabels();
		});
	m_gui_container.Pack(button);
	m_car_buttons[index] = button;
	auto label = std::make_shared<gui::Label>("", *context.fonts);
	label->setPosition(sf::Vector2f(x, y + 50));
	m_gui_container.Pack(label);
	m_car_labels[index] = label;
}