#include <util/ts_vector.h>

namespace Volt
{
	template<typename Ty> Vector<Ty>::Vector() :
		impl(std::make_unique<Implementation<Ty>>())
	{}

	template<typename Ty> Vector<Ty>::Vector(const Vector<Ty>& other) :
		impl(std::make_unique<Implementation<Ty>>(*other.impl))
	{}

	template<typename Ty> Vector<Ty>::Vector(const std::vector<Ty>& vector) :
		impl(std::make_unique<Implementation<Ty>>(vector))
	{}

	template<typename Ty> void Vector<Ty>::operator=(const Vector<Ty>& other)
	{
		this->impl = std::make_unique<Implementation<Ty>>(*other.impl);
	}

	template<typename Ty> void Vector<Ty>::PushBackElement(const Ty& data)
	{
		this->impl->PushBackElement(data);
	}

	template<typename Ty> void Vector<Ty>::EmplaceBackElement(const Ty& data)
	{
		this->impl->EmplaceBackElement(data);
	}

	template<typename Ty> void Vector<Ty>::PopBackElement()
	{
		this->impl->PopBackElement();
	}

	template<typename Ty> void Vector<Ty>::ClearElements()
	{
		this->impl->ClearElements();
	}

	template<typename Ty> void Vector<Ty>::InsertElementAtIndex(const Ty& data, size_t index)
	{
		this->impl->InsertElementAtIndex(data, index);
	}

	template<typename Ty> void Vector<Ty>::EraseElementAtIndex(size_t index)
	{
		this->impl->EraseElementAtIndex(index);
	}

	template<typename Ty> const Ty& Vector<Ty>::GetFrontElement() const
	{
		return this->impl->GetFrontElement();
	}

	template<typename Ty> const Ty& Vector<Ty>::GetBackElement() const
	{
		return this->impl->GetBackElement();
	}

	template<typename Ty> bool Vector<Ty>::IsEmpty() const
	{
		return this->impl->IsEmpty();
	}

	template<typename Ty> size_t Vector<Ty>::GetSize() const
	{
		return this->impl->GetSize();
	}

	template<typename Ty> Ty& Vector<Ty>::operator[](size_t index)
	{
		return (*this->impl)[index];
	}

	template<typename Ty> const Ty& Vector<Ty>::operator[](size_t index) const
	{
		return (*this->impl)[index];
	}

	template<typename Ty> const std::vector<Ty>& Vector<Ty>::GetVectorObject() const
	{
		return this->impl->GetVectorObject();
	}
}