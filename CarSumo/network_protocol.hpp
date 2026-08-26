//Hannah Kellett D00260463

#pragma once
#include <SFML/System/Vector2.hpp>
const unsigned short SERVER_PORT = 50000; //Greater than 49151, in dynamic port range
namespace Server
{
	enum class PacketType
	{
		kBroadcastMessage, //Takes a std::string and sends it to all clients, they show on their screens for a number of seconds
		kInitialState, //takes two float values, the world height, and the initial scrolling in the world, then sf::Int32 with the number of car, then for each car its identifier, position, health and missiles
		kPlayerEvent, //This takes two sf::Int32 variables, the car identifier and the action identifier from action.hpp, this is used to tell that a particular plane has triggered some action
		kPlayerRealtimeChange, //Same as playerevent for real time actions
		kPlayerConnect, //The same as SpawnSelf but indicates that an car from a different client is connecting
		kPlayerDisconnect, //Takes sf::Int32 car identifier that is disconnecting
		kSpawnSelf, //This takes an sf::Int32 for the car identifier and two float values for the initial position. 
		kUpdateCarInfo, //Only takes car identifier and new car type. Used to ensure cars are the correct type on all clients since I don't know how else to ensure this
		kUpdateClientState, //This takes one float with the current scrolling of the world in the server, and then a sf::Int32 for the number of car. For each car, it packs one sf::Int32 value with the identifier, two floats for position, health, and ammo. Think about enemies. If we don't send anything they will be locally tracked
		kMissionSuccess // This has no arguments. It just informs the client that the game is over and the client can show the appropriate state
	};
}

namespace Client
{
	enum class PacketType
	{
		kPlayerInformation, //This is sent when the client connects to the server. It sends the car type
		kPlayerEvent, // Two sf::Int32, car identifer and event. It is used to request the server to trigger an event on the car
		kPlayerRealtimeChange, // The same kPlayerEvent, additionally takes a boolean for real time action
		kStateUpdate, //sf::Int32 with number of local car, for each car send sf::Int32 identifier, two floats for position, health and ammo 
		kGameEvent, //This is for explosions
		kQuit
	};
}

namespace GameActions
{
	enum Type
	{
		kEnemyExplode
	};

	struct Action
	{
		Action() = default;
		Action(Type type, sf::Vector2f position) :type(type), position(position)
		{

		}

		Type type;
		sf::Vector2f position;
	};
}

