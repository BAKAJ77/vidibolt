#ifndef VIDIBOLT_THREAD_SAFE_VECTOR_H
#define VIDIBOLT_THREAD_SAFE_VECTOR_H

#include <util/volt_api.h>

#include <memory>
#include <vector>
#include <mutex>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// A vector class that wraps the std::vector operations with locks for increased thread safety.
	template <typename Ty> class Vector
	{
	private:
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename U> class Implementation
		{
		private:
			std::vector<U> vector;
			mutable std::mutex mutex;
		public:
			Implementation() = default;
			Implementation(const Implementation<U>& impl) :
				vector(impl.vector)
			{}

			Implementation(const std::vector<U>& vector) :
				vector(vector)
			{}

			~Implementation() = default;

			void operator=(const Implementation<U>& impl)
			{
				this->vector = impl.vector;
			}

			void PushBackElement(const U& data)
			{
				std::scoped_lock lock(this->mutex);
				this->vector.push_back(data);
			}

			void PopBackElement()
			{
				std::scoped_lock lock(this->mutex);
				this->vector.pop_back();
			}

			void ClearElements()
			{
				std::scoped_lock lock(this->mutex);
				this->vector.clear();
			}

			void InsertElementAtIndex(const U& data, size_t index)
			{
				std::scoped_lock lock(this->mutex);
				auto elementLoc = this->vector.begin();
				std::advance(elementLoc, index);

				this->vector.insert(elementLoc, data);
			}

			void EraseElementAtIndex(size_t index)
			{
				std::scoped_lock lock(this->mutex);
				auto elementLoc = this->vector.begin();
				std::advance(elementLoc, index);

				this->vector.erase(elementLoc);
			}

			const U& GetFrontElement() const
			{
				std::scoped_lock lock(this->mutex);
				return this->vector.front();
			}

			const U& GetBackElement() const
			{
				std::scoped_lock lock(this->mutex);
				return this->vector.back();
			}

			bool IsEmpty() const
			{
				std::scoped_lock lock(this->mutex);
				return this->vector.empty();
			}

			size_t GetSize() const
			{
				std::scoped_lock lock(this->mutex);
				return this->vector.size();
			}

			U& operator[](size_t index)
			{
				std::scoped_lock lock(this->mutex);
				return this->vector[index];
			}

			const U& operator[](size_t index)
			{
				std::scoped_lock lock(this->mutex);
				return this->vector[index];
			}

			const std::vector<U>& GetVectorObject() const
			{
				std::scoped_lock lock(this->mutex);
				return this->vector;
			}
		};

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		std::unique_ptr<Implementation<Ty>> impl;
	public:
		VOLT_EXPORT Vector();
		VOLT_EXPORT Vector(const Vector<Ty>& other);
		VOLT_EXPORT Vector(const std::vector<Ty>& vector);

		VOLT_EXPORT ~Vector() = default;

		VOLT_EXPORT void operator=(const Vector<Ty>& other);
		
		// Appends the element to the back of the vector
		VOLT_EXPORT void PushBackElement(const Ty& data);

		// Removes the element at the back of the vector.
		VOLT_EXPORT void PopBackElement();

		// Clears the vector of all elements.
		VOLT_EXPORT void ClearElements();

		// Inserts the element into index specified in the vector.
		VOLT_EXPORT void InsertElementAtIndex(const Ty& data, size_t index);

		// Erases the element at the index specified in the vector.
		VOLT_EXPORT void EraseElementAtIndex(size_t index);

		// Returns the first element in the vector.
		VOLT_EXPORT const Ty& GetFrontElement() const;

		// Returns the last element in the vector. 
		VOLT_EXPORT const Ty& GetBackElement() const;

		// Returns TRUE if vector is empty, else FALSE is returned.
		VOLT_EXPORT bool IsEmpty() const;

		// Returns the size of the vector.
		VOLT_EXPORT size_t GetSize() const;

		// Operator overload that returns the element in the vector at the index specified.
		VOLT_EXPORT Ty& operator[](size_t index);

		// Operator overload that returns the element in the vector at the index specified.
		VOLT_EXPORT const Ty& operator[](size_t index) const;

		// Returns the underlying vector object that is wrapped by the class.
		VOLT_EXPORT const std::vector<Ty>& GetVectorObject() const;
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#include <util/ts_vector.inl>

#endif
