//---------------------------------------------------------------
//
// NetworkController.cpp
//

#pragma once

#include "client/NetworkController.h"

#include "common/Log.h"
#include "common/NetworkHelper.h"
#include "common/NetworkTypes.h"

#include <windows.h>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/Network/IpAddress.hpp>

namespace Client {

namespace {
	const int s_port = 50001;
	const std::string s_serverAddress = "127.0.0.1";
}

//===============================================================================

NetworkController::NetworkController()
	: m_networkHelper(new Common::NetworkHelper)
	, m_connection(new Common::Connection)
{

}

NetworkController::~NetworkController()
{

}

void NetworkController::ConnectToServer(const std::string& address, int port)
{
	std::string ip = address.empty() ? s_serverAddress : address;
	int portNum = port == 0 ? s_port : port;

	const Common::Connection& connection = *m_connection.get();
	if (connection.socket->getRemoteAddress() != sf::IpAddress::None)
	{
		LOG_DEBUG("Connection attempt succeeded."
			" remote-ip=" + connection.socket->getRemoteAddress().toString() +
			" remote-port=" + std::to_string(connection.socket->getRemotePort()) +
			" local-port=" + std::to_string(connection.socket->getLocalPort()));
		return;
	}

	bool done = connection.socket->connect(ip, portNum) != sf::Socket::Done;
	int ec = WSAGetLastError();
	if (ec)
	{
		if (ec == WSAEWOULDBLOCK)
		{
			LOG_DEBUG("Connecting to server...");
		}
		else if (ec == WSAENETUNREACH)
		{
			LOG_DEBUG("Error: Network unreachable error=" + std::to_string(ec));
		}
		else if (ec == WSAETIMEDOUT)
		{
			LOG_DEBUG("Error: Network connection attempt timed out. error=" + std::to_string(ec));
		}
		else if (ec == WSAECONNREFUSED)
		{
			LOG_DEBUG("Error: Network connection attempt refused. error=" + std::to_string(ec));
		}
		else
		{
			LOG_DEBUG("Error: There was an error attempting to connect to server. error="
				+ std::to_string(ec));
		}
	}
}

bool NetworkController::IsConnected() const
{
	return m_connection->IsConnected();
}

//===============================================================================

} // namespace Client
