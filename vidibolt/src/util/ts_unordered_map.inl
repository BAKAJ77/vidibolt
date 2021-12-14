#include <util/ts_unordered_map.h>

namespace Volt
{
	template<typename Key, typename Ty> UnorderedMap<Key, Ty>::UnorderedMap() :
		impl(std::make_unique<Implementation<Key, Ty>>())
	{}

	template<typename Key, typename Ty> UnorderedMap<Key, Ty>::UnorderedMap(const UnorderedMap<Key, Ty>& other) :
		impl(std::make_unique<Implementation<Key, Ty>>(*other.impl))
	{}

	template<typename Key, typename Ty>
	UnorderedMap<Key, Ty>::UnorderedMap(const std::unordered_map<Key, Ty>& map) :
		impl(std::make_unique<Implementation<Key, Ty>>(map))
	{}

	template<typename Key, typename Ty> void UnorderedMap<Key, Ty>::operator=(const UnorderedMap<Key, Ty>& other)
	{
		this->impl = std::make_unique<Implementation<Key, Ty>>(*other.impl);
	}

	template<typename Key, typename Ty> void UnorderedMap<Key, Ty>::Reserve(size_t count)
	{
		this->impl->Reserve(count);
	}

	template<typename Key, typename Ty> void UnorderedMap<Key, Ty>::Insert(const Key& key, const Ty& data)
	{
		this->impl->Insert(key, data);
	}

	template<typename Key, typename Ty> void UnorderedMap<Key, Ty>::Emplace(const Key& key, const Ty& data)
	{
		this->impl->Emplace(key, data);
	}

	template<typename Key, typename Ty> void UnorderedMap<Key, Ty>::Erase(const Key& key)
	{
		this->impl->Erase(key);
	}

	template<typename Key, typename Ty> void UnorderedMap<Key, Ty>::ClearElements()
	{
		this->impl->ClearElements();
	}

	template<typename Key, typename Ty> Ty& UnorderedMap<Key, Ty>::GetElement(const Key& key)
	{
		return this->impl->GetElement(key);
	}

	template<typename Key, typename Ty> const Ty& UnorderedMap<Key, Ty>::GetElement(const Key& key) const
	{
		return this->impl->GetElement(key);
	}

	template<typename Key, typename Ty> Ty& UnorderedMap<Key, Ty>::GetElementAtIndex(size_t indexPos)
	{
		return this->impl->GetElementAtIndex(indexPos);
	}

	template<typename Key, typename Ty> const Ty& UnorderedMap<Key, Ty>::GetElementAtIndex(size_t indexPos) const
	{
		return this->impl->GetElementAtIndex(indexPos);
	}

	template<typename Key, typename Ty> bool UnorderedMap<Key, Ty>::ElementExists(const Key& key) const
	{
		return this->impl->ElementExists(key);
	}

	template<typename Key, typename Ty> size_t UnorderedMap<Key, Ty>::GetSize() const
	{
		return this->impl->GetSize();
	}

	template<typename Key, typename Ty> bool UnorderedMap<Key, Ty>::IsEmpty() const
	{
		return this->impl->IsEmpty();
	}

	template<typename Key, typename Ty> Ty& UnorderedMap<Key, Ty>::operator[](const Key& key)
	{
		return (*this->impl)[key];
	}

	template<typename Key, typename Ty> const Ty& UnorderedMap<Key, Ty>::operator[](const Key& key) const
	{
		return (*this->impl)[key];
	}

	template<typename Key, typename Ty>
	const std::unordered_map<Key, Ty>& UnorderedMap<Key, Ty>::GetUnorderedMapObject() const
	{
		return this->impl->GetUnorderedMapObject();
	}
}