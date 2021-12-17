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
			std::thread thread = std::thread([&]()
				{
					while (true)
					{
						Volt::ErrorCode error = clientNode.FlushNode();
						if (error)
							break;
					}
				});

			std::this_thread::sleep_for(std::chrono::seconds(5));

			if (!clientNode.GetPeerList().IsEmpty())
			{
				const auto& list = clientNode.GetPeerList();
				std::cout << "Added node to peer list!\n";

				Volt::ECKeyPair keyPair("VPK_022102EEFF84CBD0D70BA47E778E451D7A38F2E6AA2E885692DCEB731377F6F18F",
					"0B42DACF5D7B8B825FBB4543298601359FB27A8D85147F3235F0DAA140553DD6");

				Volt::Node::OperationReturn futureReturn;
				Volt::RequestAddressBalance(clientNode, keyPair, futureReturn);
				
				std::cout << "Returned Balance: " << std::get<double>(futureReturn.get()) << std::endl;
			}

			thread.join();
		}
		else
			std::cout << "An error occurred while adding node!\n";
	}

	///////////////////////////////////////////////////////////////
	
	std::system("pause");
	return 0;
}