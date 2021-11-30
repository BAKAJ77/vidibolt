#ifndef VIDIBOLT_THREAD_SAFE_DEQUE_H
#define VIDIBOLT_THREAD_SAFE_DEQUE_H

#include <util/volt_api.h>

#include <memory>
#include <vector>
#include <deque>
#include <mutex>

namespace Volt
{
	/*
		A double ended queue class that wraps the std::deque operations with locks for increased thread safety.
	*/
	template<typename T> class Deque
	{
	private:
		template<typename U> class Implementation
		{
		private:
			std::deque<U> deque;
			mutable std::mutex mutex;
		public:
			Implementation() = default;
			Implementation(const Implementation<U>& other) :
				deque(other.deque)
			{}

			~Implementation() = default;

			void operator=(const Implementation<U>& other)
			{
				this->deque = other.deque;
			}

			void PushBackElement(const U& data)
			{
				std::scoped_lock lock(this->mutex);
				this->deque.push_back(data);
			}

			void PushFrontElement(const U& data)
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

			const U& GetFrontElement() const
			{
				std::scoped_lock lock(this->mutex);
				return this->deque.front();
			}

			const U& GetBackElement() const
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

			U& operator[](size_t index)
			{
				std::scoped_lock lock(this->mutex);
				return this->deque[index];
			}

			const U& operator[](size_t index) const
			{
				std::scoped_lock lock(this->mutex);
				return this->deque[index];
			}

			const std::deque<U>& GetDequeObject() const
			{
				std::scoped_lock lock(this->mutex);
				return this->deque;
			}
		};
		
		std::unique_ptr<Implementation<T>> impl;
	public:
		VOLT_EXPORT Deque();
		VOLT_EXPORT Deque(const Deque<T>& other);

		VOLT_EXPORT ~Deque() = default;

		VOLT_EXPORT void operator=(const Deque<T>& other);

		/*
			Pushes element to the back of the queue.
		*/
		VOLT_EXPORT void PushBackElement(const T& data);

		/*
			Pushes element to the front of the queue.
		*/
		VOLT_EXPORT void PushFrontElement(const T& data);

		/*
			Clears out the queue of all elements it contains.
		*/
		VOLT_EXPORT void ClearElements();

		/*
			Pop the element from the front of the queue.
		*/
		VOLT_EXPORT void PopFrontElement();

		/*
			Pop the element from the back of the queue.
		*/
		VOLT_EXPORT void PopBackElement();

		/*
			Returns the element at the front of the queue.
		*/
		VOLT_EXPORT const T& GetFrontElement() const;

		/*
			Returns the element at the back of the queue.
		*/
		VOLT_EXPORT const T& GetBackElement() const;

		/*
			Returns TRUE if the queue is empty, else if it's not then FALSE is returned.
		*/
		VOLT_EXPORT bool IsEmpty() const;

		/*
			Returns the size of the queue.
		*/
		VOLT_EXPORT size_t GetSize() const;

		/*
			Operator overload that returns the element at the index specified.
		*/
		VOLT_EXPORT T& operator[](size_t index);

		/*
			Operator overload that returns the element at the index specified.
		*/
		VOLT_EXPORT const T& operator[](size_t index) const;

		/*
			Operator overload for casting a deque to a vector of the same type.
		*/
		VOLT_EXPORT operator std::vector<T>() const;
	};
}

#include <util/ts_deque.inl>

#endif