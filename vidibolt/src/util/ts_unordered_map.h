#ifndef VIDIBOLT_THREAD_SAFE_UNORDERED_MAP_H
#define VIDIBOLT_THREAD_SAFE_UNORDERED_MAP_H

#include <util/volt_api.h>
#include <unordered_map>
#include <mutex>

namespace Volt
{
	template <typename Key, typename Ty> class VOLT_EXPORT UnorderedMap
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

		Implementation<Key, Ty>* impl;
	public:
		UnorderedMap();
		UnorderedMap(const UnorderedMap<Key, Ty>&) = delete;

		~UnorderedMap();

		void Reserve(size_t count);

		void Insert(const Key& key, const Ty& data);
		void Emplace(const Key& key, const Ty& data);
		void Erase(const Key& key);
		void ClearElements();

		Ty& GetElement(const Key& key);
		Ty& GetElementAtIndex(size_t indexPos);
		bool ElementExists(const Key& key);

		size_t GetSize();
		bool IsEmpty();

		Ty& operator[](const Key& key);
	};
}

#include <util/ts_unordered_map.inl>

#endif
