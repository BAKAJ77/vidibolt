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
	template<typename T>
	class Deque
	{
	private:
		std::deque<T> deque;
		std::mutex mutex;
	public:
		Deque() = default;
		Deque(const Deque<T>&) = delete;

		virtual ~Deque() { this->ClearElements(); }

		/*
			Pushes element to the back of the queue.
		*/
		void PushBackElement(const T& data)
		{
			std::scoped_lock lock(mutex);
			this->deque.push_back(data);
		}

		/*
			Pushes element to the front of the queue.
		*/
		void PushFrontElement(const T& data)
		{
			std::scoped_lock lock(mutex);
			this->deque.push_front(data);
		}

		/*
			Clears out the queue of all elements it contains.
		*/
		void ClearElements()
		{
			std::scoped_lock lock(mutex);
			this->deque.clear();
		}

		/*
			Pop the element from the front of the queue.
		*/
		void PopFrontElement()
		{
			std::scoped_lock lock(mutex);
			this->deque.pop_front();
		}

		/*
			Pop the element from the back of the queue.
		*/
		void PopBackElement()
		{
			std::scoped_lock lock(mutex);
			this->deque.pop_back();
		}

		/*
			Returns the element at the front of the queue.
		*/
		const T& GetFrontElement()
		{
			std::scoped_lock lock(mutex);
			return this->deque.front();
		}

		/*
			Returns the element at the back of the queue.
		*/
		const T& GetBackElement()
		{
			std::scoped_lock lock(mutex);
			return this->deque.back();
		}

		/*
			Returns TRUE if the queue is empty, else if it's not then FALSE is returned.
		*/
		bool IsEmpty()
		{
			std::scoped_lock lock(mutex);
			return this->deque.empty();
		}

		/*
			Returns the size of the queue.
		*/
		size_t GetSize()
		{
			std::scoped_lock lock(mutex);
			return this->deque.size();
		}

		/*
			Operator overload that returns the element at the index specified.
		*/
		T& operator[](size_t index)
		{
			std::scoped_lock lock(mutex);
			return this->deque[index];
		}
	};
}

#endif