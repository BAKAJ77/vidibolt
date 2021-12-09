#include <util/ts_deque.h>

namespace Volt
{
	template <typename T> Deque<T>::Deque() :
		impl(std::make_unique<Implementation<T>>())
	{}

	template <typename T> Deque<T>::Deque(const Deque<T>& other) :
		impl(std::make_unique<Implementation<T>>(*other.impl))
	{}

	template<typename T>
	Deque<T>::Deque(const std::deque<T>& deque) :
		impl(std::make_unique<Implementation<T>>(deque))
	{}

	template <typename T> void Deque<T>::operator=(const Deque<T>& other)
	{
		this->impl = std::make_unique<Implementation<T>>(*other.impl);
	}

	template <typename T> void Deque<T>::PushBackElement(const T& data)
	{
		this->impl->PushBackElement(data);
	}

	template <typename T> void Deque<T>::PushFrontElement(const T& data)
	{
		this->impl->PushFrontElement(data);
	}

	template <typename T> void Deque<T>::ClearElements()
	{
		this->impl->ClearElements();
	}

	template <typename T> void Deque<T>::PopFrontElement()
	{
		this->impl->PopFrontElement();
	}

	template <typename T> void Deque<T>::PopBackElement()
	{
		this->impl->PopBackElement();
	}

	template <typename T> void Deque<T>::PopElementAtIndex(size_t index)
	{
		this->impl->PopElementAtIndex(index);
	}

	template <typename T> const T& Deque<T>::GetFrontElement() const
	{
		return this->impl->GetFrontElement();
	}

	template <typename T> const T& Deque<T>::GetBackElement() const
	{
		return this->impl->GetBackElement();
	}

	template <typename T> bool Deque<T>::IsEmpty() const
	{
		return this->impl->IsEmpty();
	}

	template <typename T> size_t Deque<T>::GetSize() const
	{
		return this->impl->GetSize();
	}

	template<typename T>
	const std::deque<T>& Deque<T>::GetDequeObject() const
	{
		return this->impl->GetDequeObject();
	}

	template <typename T> T& Deque<T>::operator[](size_t index)
	{
		return (*this->impl)[index];
	}

	template <typename T> const T& Deque<T>::operator[](size_t index) const
	{
		return (*this->impl)[index];
	}
}