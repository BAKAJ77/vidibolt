#ifndef VIDIBOLT_THREAD_SAFE_DEQUE_H
#define VIDIBOLT_THREAD_SAFE_DEQUE_H

#include <util/volt_api.h>

#include <iterator>
#include <cstddef>
#include <memory>
#include <deque>
#include <mutex>

namespace Volt
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// A double ended queue class that wraps the std::deque operations with locks for increased thread safety.
	template<typename Ty> class Deque
	{
	private:
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		template<typename Ty> class Implementation
		{
		private:
			std::deque<Ty> deque;
			mutable std::mutex mutex;
		public:
			Implementation() = default;
			Implementation(const Implementation<Ty>& other) :
				deque(other.deque)
			{}

			Implementation(const std::deque<Ty>& deque) :
				deque(deque)
			{}

			~Implementation() = default;

			void operator=(const Implementation<Ty>& other)
			{
				this->deque = other.deque;
			}

			void PushBackElement(const Ty& data)
			{
				std::scoped_lock lock(this->mutex);
				this->deque.push_back(data);
			}

			void PushFrontElement(const Ty& data)
			{
				std::scoped_lock lock(this->mutex);
				this->deque.push_front(data);
			}

			void ClearElements()
			{
				std::scoped_lock lock(this->mutex);
				this->deque.clear();
			}

			void PopFrontElement()
			{
				std::scoped_lock lock(this->mutex);
				this->deque.pop_front();
			}

			void PopBackElement()
			{
				std::scoped_lock lock(this->mutex);
				this->deque.pop_back();
			}

			void PopElementAtIndex(size_t index)
			{
				std::scoped_lock lock(this->mutex);
				auto elementLoc = this->deque.begin();
				std::advance(elementLoc, index);

				this->deque.erase(elementLoc);
			}

			const Ty& GetFrontElement() const
			{
				std::scoped_lock lock(this->mutex);
				return this->deque.front();
			}

			const Ty& GetBackElement() const
			{
				std::scoped_lock lock(this->mutex);
				return this->deque.back();
			}

			bool IsEmpty() const
			{
				std::scoped_lock lock(this->mutex);
				return this->deque.empty();
			}

			size_t GetSize() const
			{
				std::scoped_lock lock(this->mutex);
				return this->deque.size();
			}

			Ty& operator[](size_t index)
			{
				std::scoped_lock lock(this->mutex);
				return this->deque[index];
			}

			const Ty& operator[](size_t index) const
			{
				std::scoped_lock lock(this->mutex);
				return this->deque[index];
			}

			const std::deque<Ty>& GetDequeObject() const
			{
				std::scoped_lock lock(this->mutex);
				return this->deque;
			}
		};

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		std::unique_ptr<Implementation<Ty>> impl;
	public:
		VOLT_EXPORT Deque();
		VOLT_EXPORT Deque(const Deque<Ty>& other);
		VOLT_EXPORT Deque(const std::deque<Ty>& deque);

		VOLT_EXPORT ~Deque() = default;

		VOLT_EXPORT void operator=(const Deque<Ty>& other);

		// Pushes element to the back of the queue.
		VOLT_EXPORT void PushBackElement(const Ty& data);

		// Pushes element to the front of the queue.
		VOLT_EXPORT void PushFrontElement(const Ty& data);

		// Clears out the queue of all elements it contains.
		VOLT_EXPORT void ClearElements();

		// Pop the element from the front of the queue.
		VOLT_EXPORT void PopFrontElement();

		// Pop the element from the back of the queue.
		VOLT_EXPORT void PopBackElement();

		// Pops the element at the index specified in the queue.
		VOLT_EXPORT void PopElementAtIndex(size_t index);

		// Returns the element at the front of the queue.
		VOLT_EXPORT const Ty& GetFrontElement() const;

		// Returns the element at the back of the queue.
		VOLT_EXPORT const Ty& GetBackElement() const;

		// Returns TRUE if the queue is empty, else if it's not then FALSE is returned.
		VOLT_EXPORT bool IsEmpty() const;

		// Returns the size of the queue.
		VOLT_EXPORT size_t GetSize() const;

		// Operator overload that returns the element at the index specified.
		VOLT_EXPORT Ty& operator[](size_t index);

		// Operator overload that returns the element at the index specified.
		VOLT_EXPORT const Ty& operator[](size_t index) const;

		// Returns the underlying deque object that is wrapped by the class.
		VOLT_EXPORT const std::deque<Ty>& GetDequeObject() const;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#include <util/ts_deque.inl>

#endif