#include "game_server.hpp"
#include "network_protocol.hpp"
#include "car_type.hpp"
#include "utility.hpp"
#include <SFML/Network/Packet.hpp>
#include <SFML/System/Sleep.hpp>
#include <iostream>

GameServer::GameServer(sf::Vector2f battlefield_size)
    : m_thread(&GameServer::ExecutionThread, this)
    , m_listening_state(false)
    , m_client_timeout(sf::seconds(1.f))
    , m_max_connected_players(20)
    , m_connected_players(0)
    , m_world_height(5000.f)
    , m_battlefield_rect(sf::Vector2f(0.f, m_world_height - battlefield_size.y),sf::Vector2f(battlefield_size.x, battlefield_size.y))
    , m_car_count(0)
    , m_peers(1)
    , m_car_identifier_counter(1)
    , m_waiting_thread_end(false)
{
    m_listener_socket.setBlocking(false);
    m_peers[0].reset(new RemotePeer);
}

GameServer::~GameServer()
{
    m_waiting_thread_end = true;
    m_thread.join();
}

void GameServer::NotifyPlayerSpawn(uint8_t car_identifier)
{
    sf::Packet packet;
    //First thing in every is what type of packet it is
    packet << static_cast<uint8_t>(Server::PacketType::kPlayerConnect);
    packet << car_identifier << m_car_info[car_identifier].m_position.x << m_car_info[car_identifier].m_position.y;
    SendToAll(packet);
}

void GameServer::NotifyPlayerRealtimeChange(uint8_t car_identifier, uint8_t action, bool action_enabled)
{
    sf::Packet packet;
    //First thing in every is what type of packet it is
    packet << static_cast<uint8_t>(Server::PacketType::kPlayerRealtimeChange);
    packet << car_identifier;
    packet << action;
    packet << action_enabled;
    SendToAll(packet);

}

void GameServer::NotifyPlayerEvent(uint8_t car_identifier, uint8_t action)
{
    sf::Packet packet;
    std::cout << "Server: Notify Player Event" << +car_identifier << +action << std::endl;
    //First thing in every is what type of packet it is
    packet << static_cast<uint8_t>(Server::PacketType::kPlayerEvent);
    packet << car_identifier;
    packet << action;
    SendToAll(packet);
}

void GameServer::SetListening(bool enable)
{
    //Check if the server is already listening
    if (enable)
    {
        if (!m_listening_state)
        {
            m_listening_state = (m_listener_socket.listen(SERVER_PORT) == sf::TcpListener::Status::Done);
        }
    }
    else
    {
        m_listener_socket.close();
        m_listening_state = false;
    }
}

void GameServer::ExecutionThread()
{
    //Initialisation
    SetListening(true);

    sf::Time frame_rate = sf::seconds(1.f / 60.f);
    sf::Time frame_time = sf::Time::Zero;
    sf::Time tick_rate = sf::seconds(1.f / 20.f);
    sf::Time tick_time = sf::Time::Zero;
    sf::Clock frame_clock, tick_clock;

    while (!m_waiting_thread_end)
    {
        //This is the game loop
        HandleIncomingConnections();
        HandleIncomingPackets();

        frame_time += frame_clock.getElapsedTime();
        frame_clock.restart();
        tick_time += tick_clock.getElapsedTime();
        tick_clock.restart();

        //Fixed time step
        while (frame_time >= frame_rate)
        {
            frame_time -= frame_rate;
        }

        while (tick_time >= tick_rate)
        {
            Tick();
            tick_time -= tick_rate;
        }
        
        //sleep to allow me to run the client on this machine as well
        //maybe rethink this if performance is poor
        sf::sleep(sf::milliseconds(50));
    }
}

void GameServer::Tick()
{
    UpdateClientState();

    //Check if the game is over = all cars position.y < offset
    bool all_car_done = true;
    for (const auto& current : m_car_info)
    {
        //As long as one player has not crossed the finish line the game is live
        if (current.second.m_position.y > 0.f)
        {
            all_car_done = false;
            break;
        }
    }
    if (all_car_done)
    {
        sf::Packet mission_success_packet;
        mission_success_packet << static_cast<uint8_t>(Server::PacketType::kMissionSuccess);
        SendToAll(mission_success_packet);
    }

    //Remove car that have been destroyed
    for (auto itr = m_car_info.begin(); itr != m_car_info.end();)
    {
        if (itr->second.m_hitpoints <= 0)
        {
            m_car_info.erase(itr++);
        }
        else
        {
            ++itr;
        }
    }
}

sf::Time GameServer::Now() const
{
    return m_clock.getElapsedTime();
}

void GameServer::HandleIncomingPackets()
{
    bool detected_timeout = false;

    for (PeerPtr& peer : m_peers)
    {
        if (peer->m_ready)
        {
            sf::Packet packet;
            while (peer->m_socket.receive(packet) == sf::Socket::Status::Done)
            {
                //Interpret the packet and react to it
                HandleIncomingPackets(packet, *peer, detected_timeout);

                peer->m_last_packet_time = Now();
                packet.clear();
            }

            if (Now() > peer->m_last_packet_time + m_client_timeout)
            {
                peer->m_timed_out = true;
                detected_timeout = true;
            }
        }
    }

    if (detected_timeout)
    {
        HandleDisconnections();
    }
}

void GameServer::HandleIncomingPackets(sf::Packet& packet, RemotePeer& receiving_peer, bool& detected_timeout)
{
    uint8_t packet_type;
    packet >> packet_type;

    switch (static_cast<Client::PacketType>(packet_type))
    {
    case Client::PacketType::kQuit:
    {
        receiving_peer.m_timed_out = true;
        detected_timeout = true;
    }
    break;

    case Client::PacketType::kPlayerEvent:
    {
        uint8_t car_identifier;
        uint8_t action;
        packet >> car_identifier >> action;
        NotifyPlayerEvent(car_identifier, action);
    }
    break;

    case Client::PacketType::kPlayerRealtimeChange:
    {
        uint8_t car_identifier;
        uint8_t action;
        bool action_enabled;
        packet >> car_identifier >> action >> action_enabled;
        NotifyPlayerRealtimeChange(car_identifier, action, action_enabled);
    }
    break;

    case Client::PacketType::kRequestCoopPartner:
    {
        receiving_peer.m_car_identifiers.emplace_back(m_car_identifier_counter);
        m_car_info[m_car_identifier_counter].m_position = sf::Vector2f(m_battlefield_rect.size.x / 2, m_battlefield_rect.position.y + m_battlefield_rect.size.y / 2);
        m_car_info[m_car_identifier_counter].m_hitpoints = 100;

        sf::Packet request_packet;
        request_packet << static_cast<uint8_t>(Server::PacketType::kAcceptCoopPartner);
        request_packet << m_car_identifier_counter;
        request_packet << m_car_info[m_car_identifier_counter].m_position.x;
        request_packet << m_car_info[m_car_identifier_counter].m_position.y;

        receiving_peer.m_socket.send(request_packet);
        m_car_count++;

        // Tell everyone else about the new car
        sf::Packet notify_packet;
        notify_packet << static_cast<uint8_t>(Server::PacketType::kPlayerConnect);
        notify_packet << m_car_identifier_counter;
        notify_packet << m_car_info[m_car_identifier_counter].m_position.x;
        notify_packet << m_car_info[m_car_identifier_counter].m_position.y;

        for (PeerPtr& peer : m_peers)
        {
            if (peer.get() != &receiving_peer && peer->m_ready)
            {
                peer->m_socket.send(notify_packet);
            }
        }
        m_car_identifier_counter++;
    }
    break;

    case Client::PacketType::kStateUpdate:
    {
        uint8_t num_car;
        packet >> num_car;

        for (uint8_t i = 0; i < num_car; ++i)
        {
            uint8_t car_identifier;
            uint8_t car_hitpoints;
            sf::Vector2f car_position;
            packet >> car_identifier >> car_position.x >> car_position.y >> car_hitpoints;
            m_car_info[car_identifier].m_position = car_position;
            m_car_info[car_identifier].m_hitpoints = car_hitpoints;
        }
    }
    break;
    case Client::PacketType::kGameEvent:
    {
        uint8_t action;
        float x;
        float y;

        packet >> action;
        packet >> x;
        packet >> y;
    }
    }
}

void GameServer::HandleIncomingConnections()
{
    if (!m_listening_state)
    {
        return;
    }

    if (m_listener_socket.accept(m_peers[m_connected_players]->m_socket) == sf::TcpListener::Status::Done)
    {
        //Order the new client to spawn its player 1
        m_car_info[m_car_identifier_counter].m_position = sf::Vector2f(m_battlefield_rect.size.x / 2, m_battlefield_rect.position.y + m_battlefield_rect.size.y / 2);
        m_car_info[m_car_identifier_counter].m_hitpoints = 100;

        sf::Packet packet;
        packet << static_cast<uint8_t>(Server::PacketType::kSpawnSelf);
        packet << m_car_identifier_counter;
        packet << m_car_info[m_car_identifier_counter].m_position.x;
        packet << m_car_info[m_car_identifier_counter].m_position.y;

        m_peers[m_connected_players]->m_car_identifiers.emplace_back(m_car_identifier_counter);

        BroadcastMessage("New player");
        InformWorldState(m_peers[m_connected_players]->m_socket);
        NotifyPlayerSpawn(m_car_identifier_counter++);

        m_peers[m_connected_players]->m_socket.send(packet);
        m_peers[m_connected_players]->m_ready = true;
        m_peers[m_connected_players]->m_last_packet_time = Now();

        m_car_count++;
        m_connected_players++;

        if (m_connected_players >= m_max_connected_players)
        {
            SetListening(false);
        }
        else
        {
            m_peers.emplace_back(PeerPtr(new RemotePeer()));
        }
    }
}

void GameServer::HandleDisconnections()
{
    for (auto itr = m_peers.begin(); itr != m_peers.end();)
    {
        if ((*itr)->m_timed_out)
        {
            //Inform everyone of a disconnection, erase
            for (uint8_t identifer : (*itr)->m_car_identifiers)
            {
                SendToAll((sf::Packet() << static_cast<uint8_t>(Server::PacketType::kPlayerDisconnect) << identifer));
                m_car_info.erase(identifer);
            }

            m_connected_players--;
            m_car_count -= (*itr)->m_car_identifiers.size();

            itr = m_peers.erase(itr);

            //If the number of peers has dropped below max_connections
            if (m_connected_players < m_max_connected_players)
            {
                m_peers.emplace_back(PeerPtr(new RemotePeer()));
                SetListening(true);
            }

            BroadcastMessage("A player has disconnected");

        }
        else
        {
            ++itr;
        }
    }
}

void GameServer::InformWorldState(sf::TcpSocket& socket)
{
    sf::Packet packet;
    packet << static_cast<uint8_t>(Server::PacketType::kInitialState);
    packet << m_world_height << m_battlefield_rect.position.y + m_battlefield_rect.size.y;
    packet << static_cast<uint8_t>(m_car_count);

    for (std::size_t i = 0; i < m_connected_players; ++i)
    {
        if (m_peers[i]->m_ready)
        {
            for (uint8_t identifier : m_peers[i]->m_car_identifiers)
            {
                packet << identifier << m_car_info[identifier].m_position.x << m_car_info[identifier].m_position.y << m_car_info[identifier].m_hitpoints;
            }
        }
    }

    socket.send(packet);
}

void GameServer::BroadcastMessage(const std::string& message)
{
    sf::Packet packet;
    packet << static_cast<uint8_t>(Server::PacketType::kBroadcastMessage);
    packet << message;
    for (std::size_t i = 0; i < m_connected_players; ++i)
    {
        if (m_peers[i]->m_ready)
        {
            m_peers[i]->m_socket.send(packet);
        }
    }
}

void GameServer::SendToAll(sf::Packet& packet)
{
    for (std::size_t i = 0; i < m_connected_players; ++i)
    {
        if (m_peers[i]->m_ready)
        {
            m_peers[i]->m_socket.send(packet);
        }
    }
}

void GameServer::UpdateClientState()
{
    sf::Packet update_client_state_packet;
    update_client_state_packet << static_cast<uint8_t>(Server::PacketType::kUpdateClientState);
    update_client_state_packet << static_cast<float>(m_battlefield_rect.position.y + m_battlefield_rect.size.y);
    update_client_state_packet << static_cast<uint8_t>(m_car_count);

    for (const auto& car : m_car_info)
    {
        update_client_state_packet << car.first << car.second.m_position.x << car.second.m_position.y << car.second.m_hitpoints;
    }

    SendToAll(update_client_state_packet);
}

//It is essential to set the sockets to non-blocking - m_socket.setBlocking(false)
//otherwise the server will hang waiting to read input from a connection
GameServer::RemotePeer::RemotePeer()
    : m_ready(false)
    , m_timed_out(false)
{
    m_socket.setBlocking(false);
}
