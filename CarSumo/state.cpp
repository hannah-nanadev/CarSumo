#include "state.hpp"
#include "statestack.hpp"

State::State(StateStack& stack, Context context) : m_stack(&stack), m_context(context)
{
}

State::~State()
{
}

State::Context::Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, MusicPlayer& music, SoundPlayer& sound, KeyBinding& keys1, KeyBinding& keys2, CarType& p1car, CarType& p2car): window(&window), textures(&textures), fonts(&fonts), music(&music), sound(&sound), keys1(&keys1), keys2(&keys2), p1car(&p1car), p2car(&p2car)
{
}

void State::RequestStackPush(StateID state_id)
{
	std::cout << "RequestStackPush: " << StateIDNames[static_cast<int>(state_id)] << std::endl;
	m_stack->PushState(state_id);
}

void State::RequestStackPop()
{
	std::cout << "RequestStackPop" << std::endl;
	m_stack->PopState();
}

void State::RequestStackClear()
{
	std::cout << "RequestStackClear" << std::endl;
	m_stack->ClearStack();
}

void State::SetCar(CarType car, int player_number)
{
	switch (player_number)
	{
	case 0:
		*m_context.p1car = car;
		break;
	case 1:
		*m_context.p2car = car;
		break;
	default:
		throw std::invalid_argument("Invalid player number");
	}
}

State::Context State::GetContext() const
{
	return m_context;
}

void State::OnActivate()
{

}

void State::OnDestroy()
{

}