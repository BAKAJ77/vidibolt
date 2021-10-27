#ifndef BLOCK_H
#define BLOCK_H

#include <util/volt_api.h>
#include <core/transaction.h>
#include <vector>
#include <string>

namespace VOLT
{
	/*
		A class that handles block data and its validation.
	*/
	class VOLT_API Block
	{
		friend class Blockchain;
	private:
		std::vector<Transaction> txs;
		std::string previousHash, blockHash, timestamp;
		uint32_t index, nonse;
	private:
		Block(const Block* previousBlock, const std::vector<Transaction>& txs);

		/*
			Calculates the hash of the block based on its contents.
		*/
		std::string CalculateHash() const;
	public:
		Block();
		Block(const Block& other);
		~Block();
		
		/*
			Returns a boolean with the value of TRUE if the block is valid, 
			however if its not then FALSE is returned.
		*/
		bool IsValid() const;

		/*
			Returns the hash of the previous block.
		*/
		const std::string& GetPreviousHash() const;

		/*
			Returns the hash of this block.
		*/
		const std::string& GetBlockHash() const;

		/*
			Returns the timestamp of this block.
		*/
		const std::string& GetTimestamp() const;

		/*
			Returns the height of the block in the block chain.
		*/
		const uint32_t& GetBlockHeight() const;

		/*
			Returns the nonse value of the block.
		*/
		const uint32_t& GetNonse() const;	

		/*
			Returns the transactions contained within the block.
		*/
		const std::vector<Transaction>& GetTransactions() const;
	};
}

#endif