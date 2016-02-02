#ifndef RING_BUFFER_H
#define RING_BUFFER_H
#include "UserOutput/Source/Assert.h"

namespace EAE_Engine
{
	namespace Container
	{
		/*
		 * This buffer has one feature:
		 * The End and the Start of the buffer just link with each other.
		 */
		template<class T>
		class RingBuffer
		{
		public:
			RingBuffer();
			RingBuffer(const size_t num);
			~RingBuffer();
			T& operator[](const size_t i_index);//returns elements in the reverse order that they enter the buffer.
			const T& operator[](const size_t i_index)const;//returns elements in the reverse order that they enter the buffer.
			inline size_t GetCount(){ return _count; }
			inline size_t GetSize(){ return _sizeOfBuffer; }
			void Push(T element);

		private:
			T* _pBuffer;        //the buffer of (T)s.
			size_t _sizeOfBuffer;//how many T we can hold in the buffer.
			size_t _currentIndex;//current Index of the buffer
			size_t _count;       //how many T we already have in the buffer.
		};

		template<class T>
		RingBuffer<T>::RingBuffer() :
			_sizeOfBuffer(0), _pBuffer(nullptr), _currentIndex(0), _count(0)
		{
		}

		template<class T>
		RingBuffer<T>::~RingBuffer()
		{
			if (_pBuffer)
			{
				delete[] _pBuffer;
				_pBuffer = nullptr;
			}
			_currentIndex = 0;
			_sizeOfBuffer = 0;
		}

		template<class T>
		RingBuffer<T>::RingBuffer(const size_t size) :
			_sizeOfBuffer(size), _currentIndex(0), _count(0)
		{
			this->_pBuffer = new T[_sizeOfBuffer];
		}

		//returns elements in the reverse order that they enter the buffer
		template<class T>
		T& RingBuffer<T>::operator[](const size_t i_index)
		{
			MessagedAssert(i_index < _sizeOfBuffer, "the ring buffer index is illegal");
			if (i_index > _sizeOfBuffer)
			{
				return _pBuffer[_sizeOfBuffer - 1];//return the last element
			}
			//because _currentIndex is pointing to the next slot of the buffer, so we need to -1
			//we also need to add the _sizeOfBuffer so that the index will be positive
			size_t realIndex = (_currentIndex + _sizeOfBuffer - 1) - i_index;//calculate the last element
			realIndex %= _sizeOfBuffer;
			return *(_pBuffer + realIndex);
		}
		
		template<class T>
		const T& RingBuffer<T>::operator[](const size_t i_index)const//returns elements in the reverse order that they enter the buffer.
		{
			MessagedAssert(_pBuffer != nullptr, "the buffer cannot be null.");
			MessagedAssert(i_index < _sizeOfBuffer, "the ring buffer index is illegal");
			if (i_index > _sizeOfBuffer)
			{
				return _pBuffer[_sizeOfBuffer - 1];//return the last element
			}
			//because _currentIndex is pointing to the next slot of the buffer, so we need to -1
			//we also need to add the _sizeOfBuffer so that the index will be positive
			size_t realIndex = (_currentIndex + _sizeOfBuffer - 1) - i_index;//calculate the last element
			realIndex %= _sizeOfBuffer;
			return *(_pBuffer + realIndex);
		}
		
		template<class T>
		void RingBuffer<T>::Push(T element)
		{
			if (!_pBuffer)
			{
				return;
			}
			//Push the element in the Buffer.
			//T* pElement = (_pBuffer +_currentIndex++);
			//*pElement = element;
			 _pBuffer[_currentIndex++] = element;
			_currentIndex %= _sizeOfBuffer;
			//change the _count
			if (++_count >= _sizeOfBuffer)
			{
				_count = _sizeOfBuffer;
			}
		}

		
	}
}

#endif //RING_BUFFER_H

