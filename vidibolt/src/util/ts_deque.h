#ifndef VIDIBOLT_THREAD_SAFE_DEQUE_H
#define VIDIBOLT_THREAD_SAFE_DEQUE_H

#include <util/volt_api.h>
#include <deque>
#include <mutex>

namespace Volt
{
	/*
		A double ended queue class that wraps the std::deque operations with locks for increased thread safety.
	*/
	template<typename T> class VOLT_EXPORT Deque
	{
	private:
		template<typename U> class Implementation
		{
		private:
			std::deque<U> deque;
			std::mutex mutex;
		public:
			Implementation() = default;
			Implementation(const Implementation<U>&) = delete;

			~Implementation() = default;

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

			const U& GetFrontElement()
			{
				std::scoped_lock lock(this->mutex);
				return this->deque.front();
			}

			const U& GetBackElement()
			{
				std::scoped_lock lock(this->mutex);
				return this->deque.back();
			}

			bool IsEmpty()
			{
				std::scoped_lock lock(this->mutex);
				return this->deque.empty();
			}

			size_t GetSize()
			{
				std::scoped_lock lock(this->mutex);
				return this->deque.size();
			}

			U& operator[](size_t index)
			{
				std::scoped_lock lock(this->mutex);
				return this->deque[index];
			}
		};
		
		Implementation<T>* impl;
	public:
		Deque();
		Deque(const Deque<T>&) = delete;

		~Deque();

		/*
			Pushes element to the back of the queue.
		*/
		void PushBackElement(const T& data);

		/*
			Pushes element to the front of the queue.
		*/
		void PushFrontElement(const T& data);

		/*
			Clears out the queue of all elements it contains.
		*/
		void ClearElements();

		/*
			Pop the element from the front of the queue.
		*/
		void PopFrontElement();

		/*
			Pop the element from the back of the queue.
		*/
		void PopBackElement();

		/*
			Returns the element at the front of the queue.
		*/
		const T& GetFrontElement();

		/*
			Returns the element at the back of the queue.
		*/
		const T& GetBackElement();

		/*
			Returns TRUE if the queue is empty, else if it's not then FALSE is returned.
		*/
		bool IsEmpty();

		/*
			Returns the size of the queue.
		*/
		size_t GetSize();

		/*
			Operator overload that returns the element at the index specified.
		*/
		T& operator[](size_t index);
	};
}

#include <util/ts_deque.inl>

#endif