#ifndef TENGINEVECTOR_H
#define TENGINEVECTOR_H

namespace EAE_Engine
{
	namespace Container
	{
		template<typename T>
		class TEVectorElement
		{
		public:
			TEVectorElement();
			~TEVectorElement();

			T _element;
			//TEVectorElement<E>* _pNext;
			size_t _index;
			bool _bUsed;
		};


		template<typename T>
		TEVectorElement<T>::TEVectorElement() : _bUsed(used), _index(0) //, _pNext(nullptr)
		{
		}

		template<typename T>
		TEVectorElement<T>::~TEVectorElement()
		{
		}


		template<typename T>
		class TEngineVector
		{
		public:
			TEngineVector();
			~TEngineVector();

			TEVectorElement<T>* operator[] (size_t index);
			void Reserve(size_t i_count);    //will realloc memory and will lose data
			inline size_t Capacity() { return _capacity; }
			bool Push_back(T element); //push back

			TEVectorElement<T>* _pElements;

		private:
			//TEngineVector<T>* _pHead;
			//TEngineVector<T>* _pEnd;
			size_t _capacity;
		};


		template<typename T>
		TEngineVector<T>::TEngineVector() : _pElements(nullptr), _capacity(0) //, _pHead(nullptr), _pEnd(nullptr)
		{
		}

		template<typename T>
		TEngineVector<T>::~TEngineVector()
		{
			if (_pElements)
			{
				Engine::Memory::align_free(_pElements);
				_pElements = nullptr;
			}
		}


		template<typename T>
		TEVectorElement<T>* TEngineVector<T>::operator[] (size_t index)
		{
			if (index >= _capacity)
			{
				return nullptr;
			}

			TEVectorElement<T>* pResult = _pElements + index;
			return pResult;
		}

		template<typename T>
		void TEngineVector<T>::Reserve(size_t i_count)
		{
			//if the vector is empty
			if (_capacity == 0)
			{
				_capacity = i_count;
				_pElements = Engine::Memory::align_malloc(sizeof(TEVectorElement<T>)*_capacity, Engine::Memory::NewAlignment::NEW_ALIGN_4);
				//assert _pElements!=nullptr

				return;
			}

			//if need to expand the vector or reserve it.
			if (_capacity < i_count)
			{
				_capacity = i_count;
				_pElements = Engine::Memory::align_realloc(_pElements, sizeof(TEVectorElement<T>)*_capacity, Engine::Memory::NewAlignment::NEW_ALIGN_4);
			}

		}

		template<typename T>
		bool TEngineVector<T>::Push_back(T element)
		{
			TEVectorElement<T>* pIter = _pElements;
			size_t index = 0;
			for (pIter = _pElements; pIter < _pElements + _capacity; ++pIter, ++index)
			{
				if (pIter->_bUsed == false)
				{
					pIter->_index = index;
					pIter->_bUsed = true;
					break;
				}
			}

			if (pIter->_bUsed && index<_capacity)
			{
				pIter->_element = element;
				return true;
			}
			return false;
		}



	}
}




#endif//TENGINEVECTOR_H