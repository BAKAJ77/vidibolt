#include <util/ts_deque.h>

namespace Volt
{
	template <typename T> Deque<T>::Deque() :
		impl(std::make_unique<Implementation<T>>())
	{}

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

	template <typename T> const T& Deque<T>::GetFrontElement()
	{
		return this->impl->GetFrontElement();
	}

	template <typename T> const T& Deque<T>::GetBackElement()
	{
		return this->impl->GetBackElement();
	}

	template <typename T> bool Deque<T>::IsEmpty()
	{
		return this->impl->IsEmpty();
	}

	template <typename T> size_t Deque<T>::GetSize()
	{
		return this->impl->GetSize();
	}

	template <typename T> T& Deque<T>::operator[](size_t index)
	{
		return (*this->impl)[index];
	}
}