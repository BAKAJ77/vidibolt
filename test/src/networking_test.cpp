#include <iostream>
#include <ctime>
#include <node/node.h>
#include <core/chain.h>

int main(int argc, char** argv)
{
	Volt::Chain chain;

	///////////////////////////////////////////////////////////////

	int num = 0;
	std::cin >> num;

	if (num == 1) // Server side
	{
		Volt::Node serverNode(Volt::NodeType::FULL, nullptr, &chain);
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
	}
	else // Client side
	{
		Volt::Node clientNode(Volt::NodeType::FULL, nullptr, &chain);
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
	}

	///////////////////////////////////////////////////////////////
	
	std::system("pause");
	return 0;
}