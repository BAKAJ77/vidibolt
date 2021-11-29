#ifndef VIDIBOLT_THREAD_SAFE_UNORDERED_MAP_H
#define VIDIBOLT_THREAD_SAFE_UNORDERED_MAP_H

#include <util/volt_api.h>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace Volt
{
	/*
		A unordered map class that wraps the std::unordered_map operations with locks for increased thread safety.
	*/
	template <typename Key, typename Ty> class UnorderedMap
	{
	private:
		template<typename K, typename T> class Implementation
		{
		private:
			std::unordered_map<K, T> map;
			std::mutex mutex;
		public:
			Implementation() = default;
			Implementation(const Implementation<K, T>&) = delete;

			~Implementation() = default;

			void Reserve(size_t count)
			{
				std::scoped_lock lock(this->mutex);
				this->map.reserve(count);
			}

			void Insert(const K& key, const T& data)
			{
				std::scoped_lock lock(this->mutex);
				this->map.insert(std::make_pair(key, data));
			}

			void Emplace(const K& key, const T& data)
			{
				std::scoped_lock lock(this->mutex);
				this->map.emplace(std::make_pair(key, data));
			}

			void Erase(const K& key)
			{
				std::scoped_lock lock(this->mutex);
				this->map.erase(key);
			}

			void ClearElements()
			{
				std::scoped_lock lock(this->mutex);
				this->map.clear();
			}

			T& GetElement(const K& key)
			{
				std::scoped_lock lock(this->mutex);
				return this->map.at(key);
			}

			T& GetElementAtIndex(size_t indexPos)
			{
				std::scoped_lock lock(this->mutex);
				auto it = this->map.begin();
				std::advance(it, indexPos);

				return it->second;
			}

			bool ElementExists(const K& key)
			{
				std::scoped_lock lock(this->mutex);
				return this->map.find(key) != this->map.end() ? true : false;
			}

			size_t GetSize()
			{
				std::scoped_lock lock(this->mutex);
				return this->map.size();
			}

			bool IsEmpty()
			{
				std::scoped_lock lock(this->mutex);
				return this->map.empty();
			}

			T& operator[](const K& key)
			{
				std::scoped_lock lock(this->mutex);
				return this->map[key];
			}
		};

		std::shared_ptr<Implementation<Key, Ty>> impl;
	public:
		VOLT_EXPORT UnorderedMap();
		VOLT_EXPORT UnorderedMap(const UnorderedMap<Key, Ty>&) = delete;

		VOLT_EXPORT ~UnorderedMap() = default;

		/*
			Reserves space for at least the specified number of elements and regenerates the hash table.
		*/
		VOLT_EXPORT void Reserve(size_t count);

		/*
			Inserts new specified element into the container.
		*/
		VOLT_EXPORT void Insert(const Key& key, const Ty& data);

		/*
			Emplaces the specified element into the container.
		*/
		VOLT_EXPORT void Emplace(const Key& key, const Ty& data);

		/*
			Removes element, matching the specified key, from the container.
		*/
		VOLT_EXPORT void Erase(const Key& key);

		/*
			Clears the container of all existing elements.
		*/
		VOLT_EXPORT void ClearElements();

		/*
			Returns the value of the element with the matching key specified.
		*/
		VOLT_EXPORT Ty& GetElement(const Key& key);

		/*
			Returns the value of the element at the index position specified.
		*/
		VOLT_EXPORT Ty& GetElementAtIndex(size_t indexPos);

		/*
			Returns TRUE if an element with the matchig key specified is found, else FALSE is returned.
		*/
		VOLT_EXPORT bool ElementExists(const Key& key);

		/*
			Returns the amount of elements in the container.
		*/
		VOLT_EXPORT size_t GetSize();

		/*
			Returns TRUE if container is empty, else FALSE is returned.
		*/
		VOLT_EXPORT bool IsEmpty();

		/*
			Operator overload that returns the element that has a key matching the one specified.
		*/
		VOLT_EXPORT Ty& operator[](const Key& key);
	};
}

#include <util/ts_unordered_map.inl>

#endif
