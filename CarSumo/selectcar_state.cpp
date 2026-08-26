//Hannah Kellett D00260463

#include "selectcar_state.hpp"
#include "Utility.hpp"

SelectCarState::SelectCarState(StateStack& stack, Context context, int playercount, StateID nextState)
	: State(stack, context)
	, m_gui_container()
	, m_background_sprite(context.textures->Get(TextureID::kTitleScreen))
	, m_next_state(nextState)
	, m_playercount(playercount)
{
	//Build car selection buttons, panels and labels for player 1
	for (std::size_t x = 0; x < static_cast<int>(CarType::kCarCount); ++x)
	{
		AddButtonLabel(x, x * 200 + 100, 300, CarTypeNames[x], static_cast<CarType>(x), 0, context);
	}

	if (playercount >= 2)
	{
		//Same for player 2
		for (std::size_t x = 0; x < static_cast<int>(CarType::kCarCount); ++x)
		{
			AddButtonLabel(x, x * 200 + 100, 550, CarTypeNames[x], static_cast<CarType>(x), 1, context);
		}
		UpdateLabels();
	}

	auto back_button = std::make_shared<gui::Button>(context);
	back_button->setPosition(sf::Vector2f(80.f, 620.f));
	back_button->SetText("Back");
	back_button->SetCallback(std::bind(&SelectCarState::RequestStackPop, this));
	m_gui_container.Pack(back_button);

	auto start_button = std::make_shared<gui::Button>(context);
	start_button->setPosition(sf::Vector2f(800.f, 620.f));
	start_button->SetText("Start");
	start_button->SetCallback([this]()
		{
			RequestStackClear();
			RequestStackPush(m_next_state);
		}
	);
	m_gui_container.Pack(start_button);
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
	//Logic for selecting cars for player 1 and player 2

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

void SelectCarState::AddButtonLabel(std::size_t index, std::size_t x, std::size_t y, const std::string& text, CarType car, int player, Context context)
{
	auto button = std::make_shared<gui::Button>(context);
	auto car_panel = std::make_shared<gui::CarPanel>(context, car);
	car_panel->setPosition(sf::Vector2f(x, y - 200));
	button->setPosition(sf::Vector2f(x, y));
	button->SetText(text);
	button->SetCallback([this, car, player]()
		{
			std::cout << "Player " << player + 1 << " selected car: " << CarTypeNames[static_cast<int>(car)] << std::endl;
			if(player == 0)
			{
				*GetContext().p1car = car;
			}
			else
			{
				*GetContext().p2car = car;
			}
		});
	m_gui_container.Pack(button);
	m_gui_container.Pack(car_panel);
	m_car_buttons[index] = button;
	auto label = std::make_shared<gui::Label>("", *context.fonts);
	label->setPosition(sf::Vector2f(x, y + 50));
	m_gui_container.Pack(label);
	m_car_labels[index] = label;
}