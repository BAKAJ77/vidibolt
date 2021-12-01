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
	msg.header.id = Volt::MessageRepType::TEST_TYPE;

	std::string str = GetTimeString();
	msg << str;

	int num;
	std::cin >> num;

	///////////////////////////////////////////////////////////////

	if (num == 1) // Server side
	{
		Volt::Node serverNode(60000);
		std::cout << "Waiting for connection...\n";

		while (true)
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
		}
	}
	else // Client side
	{
		Volt::Node clientNode(60000);
		clientNode.GetClient().Connect("100.70.63.205");
		clientNode.GetClient().PushOutboundMessage(msg);
		clientNode.GetClient().UpdateState();
		clientNode.GetClient().Disconnect();
	}

	///////////////////////////////////////////////////////////////
	
	std::system("pause");
	return 0;
}