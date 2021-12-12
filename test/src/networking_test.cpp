#include <iostream>
#include <ctime>
#include <node/node.h>

std::string GetTimeString()
{
	std::time_t time = std::time(nullptr);
	return std::ctime(&time);
}

int main(int argc, char** argv)
{
	///////////////////////////////////////////////////////////////
	
	Volt::Message msg;
	msg.header.id = Volt::MessageRepType::TRANSACTION_BROADCAST;
	msg.header.networkID = VOLT_MAINNET_NETWORK_ID;

	std::string str = GetTimeString();
	msg << str;

	int num;
	std::cin >> num;

	///////////////////////////////////////////////////////////////

	if (num == 1) // Server side
	{
		Volt::Node serverNode(Volt::NodeType::REGULAR);
		serverNode.InitNode(0);

		std::cout << "Node 0 has started!\n";

		while (true)
		{
			Volt::ErrorCode error = serverNode.FlushNode();
			if (error)
			{
				std::cout << "An error occurred!\n";
				break;
			}
		}

		/*while (true)
		{
			serverNode.GetServer().UpdateState();

			if (!serverNode.GetServer().GetInboundMessages().IsEmpty())
			{
				Volt::RecievedMessage msgIn = serverNode.GetServer().GetInboundMessages().GetFrontElement();
				serverNode.GetServer().GetInboundMessages().PopFrontElement();

				std::string strIn;
				msgIn >> strIn;

				std::cout << strIn;
			}
		}*/
	}
	else // Client side
	{
		Volt::Node clientNode(Volt::NodeType::REGULAR);
		clientNode.InitNode(1);

		std::cout << "Node 1 has started!\n";

		Volt::ErrorCode error = Volt::AddPeerNode(clientNode, "192.168.1.31");

		if (!error)
		{
			while (true)
			{
				Volt::ErrorCode error = clientNode.FlushNode();
				if (error)
				{
					std::cout << "An error occurred!\n";
					break;
				}

				if (!clientNode.GetPeerList().IsEmpty())
				{
					const auto& list = clientNode.GetPeerList();
					std::cin.get();
				}
			}
		}
		else
			std::cout << "An error occurred!\n";

		/*clientNode.GetClient().Connect("192.168.1.31");
		clientNode.GetClient().PushOutboundMessage(msg);
		clientNode.GetClient().UpdateState();
		clientNode.GetClient().Disconnect();*/
	}

	///////////////////////////////////////////////////////////////
	
	std::system("pause");
	return 0;
}