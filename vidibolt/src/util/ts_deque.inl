#include <util/ts_deque.h>

namespace Volt
{
	template <typename Ty> Deque<Ty>::Deque() :
		impl(std::make_unique<Implementation<Ty>>())
	{}

	template <typename Ty> Deque<Ty>::Deque(const Deque<Ty>& other) :
		impl(std::make_unique<Implementation<Ty>>(*other.impl))
	{}

	template<typename Ty>
	Deque<Ty>::Deque(const std::deque<Ty>& deque) :
		impl(std::make_unique<Implementation<Ty>>(deque))
	{}

	template <typename Ty> void Deque<Ty>::operator=(const Deque<Ty>& other)
	{
		this->impl = std::make_unique<Implementation<Ty>>(*other.impl);
	}

	template <typename Ty> void Deque<Ty>::PushBackElement(const Ty& data)
	{
		this->impl->PushBackElement(data);
	}

	template <typename Ty> void Deque<Ty>::PushFrontElement(const Ty& data)
	{
		this->impl->PushFrontElement(data);
	}

	template <typename Ty> void Deque<Ty>::ClearElements()
	{
		this->impl->ClearElements();
	}

	template <typename Ty> void Deque<Ty>::PopFrontElement()
	{
		this->impl->PopFrontElement();
	}

	template <typename Ty> void Deque<Ty>::PopBackElement()
	{
		this->impl->PopBackElement();
	}

	template <typename Ty> void Deque<Ty>::PopElementAtIndex(size_t index)
	{
		this->impl->PopElementAtIndex(index);
	}

	template <typename Ty> const Ty& Deque<Ty>::GetFrontElement() const
	{
		return this->impl->GetFrontElement();
	}

	template <typename Ty> const Ty& Deque<Ty>::GetBackElement() const
	{
		return this->impl->GetBackElement();
	}

	template <typename Ty> bool Deque<Ty>::IsEmpty() const
	{
		return this->impl->IsEmpty();
	}

	template <typename Ty> size_t Deque<Ty>::GetSize() const
	{
		return this->impl->GetSize();
	}

	template<typename Ty>
	const std::deque<Ty>& Deque<Ty>::GetDequeObject() const
	{
		return this->impl->GetDequeObject();
	}

	template <typename Ty> Ty& Deque<Ty>::operator[](size_t index)
	{
		return (*this->impl)[index];
	}

	template <typename Ty> const Ty& Deque<Ty>::operator[](size_t index) const
	{
		return (*this->impl)[index];
	}
}