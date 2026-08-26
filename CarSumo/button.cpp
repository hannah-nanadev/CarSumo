//Hannah Kellett D00260463

#include "button.hpp"
#include "fontID.hpp"
#include "utility.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "texture_id.hpp"
#include "data_tables.hpp"

gui::Button::Button(State::Context context)
    : m_sprite(context.textures->Get(TextureID::kButtons))
    , m_text(context.fonts->Get(FontID::kMain), "", 16)
    , m_is_toggle(false)
    , m_sounds(*context.sound)
{
    ChangeTexture(ButtonType::kNormal);
    sf::FloatRect bounds = m_sprite.getLocalBounds();
    m_text.setPosition(sf::Vector2f(bounds.size.x / 2, bounds.size.y / 2));
}

void gui::Button::SetCallback(Callback callback)
{
    m_callback = std::move(callback);
}

void gui::Button::SetToggle(bool flag)
{
    m_is_toggle = flag;
}

void gui::Button::SetText(const std::string& text)
{
    m_text.setString(text);
    Utility::CentreOrigin(m_text);
}

bool gui::Button::IsSelectable() const
{
    return true;
}

void gui::Button::Select()
{
    Component::Select();
    ChangeTexture(ButtonType::kSelected);
}

void gui::Button::Deselect()
{
    Component::Deselect();
    ChangeTexture(ButtonType::kNormal);
}

void gui::Button::Activate()
{
    Component::Activate();
    if (m_is_toggle)
    {
        ChangeTexture(ButtonType::kPressed);
    }
    if (m_callback)
    {
        m_callback();
    }
    if (!m_is_toggle)
    {
        Deactivate();
    }
    m_sounds.Play(SoundEffect::kButton);
}

void gui::Button::Deactivate()
{
    Component::Deactivate();
    if (m_is_toggle)
    {
        if (IsSelected())
        {
            ChangeTexture(ButtonType::kSelected);
        }
        else
        {
            ChangeTexture(ButtonType::kNormal);
        }
    }
}

void gui::Button::HandleEvent(const sf::Event& event)
{
}

void gui::Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_sprite, states);
    target.draw(m_text, states);
}

void gui::Button::ChangeTexture(ButtonType buttonType)
{
    sf::IntRect textureRect({ 0, 50 * static_cast<int>(buttonType) }, { 200, 50 });
    m_sprite.setTextureRect(textureRect);
}

gui::CarPanel::CarPanel(State::Context context, CarType car)
    : m_context(context)
    , m_background_sprite(context.textures->Get(TextureID::kCarSelectBG))
    , m_car_sprite(context.textures->Get(TextureID::kCars))
    , m_car_type(car)
{
	SetCarTexture();
}

State::Context gui::CarPanel::GetContext() const
{
    return m_context;
}

void gui::CarPanel::SetCarTexture()
{
    m_car_sprite.setTextureRect(GetCarTextureRect(m_car_type));
    
    sf::FloatRect bg_bounds = m_background_sprite.getLocalBounds();
    sf::FloatRect car_bounds = m_car_sprite.getLocalBounds();

    float car_x = (bg_bounds.size.x - car_bounds.size.x) / 2.f;
    float car_y = (bg_bounds.size.y - car_bounds.size.y) / 2.f;

	sf::Vector2f car_position(car_x, car_y);

    m_car_sprite.setPosition(car_position);
}

CarType gui::CarPanel::GetCar() const
{
    return m_car_type;
}

bool gui::CarPanel::IsSelectable() const
{
    return false;
}

void gui::CarPanel::HandleEvent(const sf::Event& event)
{
    //CarPanel does not recieve events, this method only exists to appease the compiler
}

void gui::CarPanel::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    target.draw(m_background_sprite, states);
    target.draw(m_car_sprite, states);
}