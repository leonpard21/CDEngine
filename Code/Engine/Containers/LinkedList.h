#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "Engine/General/MemoryOp.h"

namespace EAE_Engine
{
	namespace Container
	{
		template<typename T>
		class LinkedElement
		{
		public:
			LinkedElement() : _pNext(nullptr){}
			LinkedElement(T current) :_current(current), _pNext(nullptr){}
			virtual ~LinkedElement()
			{
				_pNext = nullptr;
			}
			inline void SetNext(LinkedElement* pNext){ _pNext = pNext; }
			inline bool operator == (const LinkedElement& right);
			inline bool operator == (const T constRight);
			inline T& GetValue(){ return _current; }

		public:
			T _current;
			LinkedElement* _pNext;
		};

		template<typename T>
		bool LinkedElement<T>::operator == (const LinkedElement<T>& right)
		{
			if (_current == right._current && _pNext == right._pNext)
			{
				return true;
			}
			return false;
		}


		template<typename T>
		bool LinkedElement<T>::operator == (const T constRight)
		{
			if (_current == constRight)
			{
				return true;
			}
			else
				return false;
		}


		template<typename T>
		class LinkedList
		{
		public:
			LinkedList() :_pHead(nullptr), _pTail(nullptr), _number(0){}
			virtual ~LinkedList()
			{
				Clear();
			}
			void Push(T node)
			{
				LinkedElement<T>* pElement = new LinkedElement<T>(node);
				if (pElement == nullptr)
				{
					return;
				}
				pElement->SetNext(nullptr);
				if (_pHead == nullptr)
				{
					_pHead = pElement;
					pElement->_pNext = nullptr;
				}
				else
				{
					_pTail->SetNext(pElement);
				}
				_pTail = pElement;
				
				_number++;

			}

			LinkedElement<T>* PopHead()
			{
				LinkedElement<T>* pResult = _pHead;
				if (_pTail == _pHead)
				{
					_pTail = nullptr;
				}
				_pHead = _pHead->_pNext;
				if (_number>0)
				{
					_number--;
				}
				return pResult;
			}

			LinkedElement<T>* Pop();
			void Clear();
			LinkedElement<T>* Remove(T& t);

			//for every element in the list, run the function F
			//F should be the function to operate T*
			template<typename F, typename ...Args>
			void RunIterate(F func, Args&&... args);

			bool Contains(T& t);

		public:
			unsigned int _number;
			LinkedElement<T>* _pHead;  // head of the linked list
			LinkedElement<T>* _pTail;  // tail of the linked list

		};


		template<typename T>
		LinkedElement<T>* LinkedList<T>::Pop()
		{
			//find the second last element
			LinkedElement<T>* pSecondLast = _pHead;
			LinkedElement<T>* result = _pTail;
			if (pSecondLast == nullptr || pSecondLast->_pNext == nullptr)
			{
				_pHead = nullptr;
				_pTail = nullptr;
				_number = 0;
				return result;
			}
			for (; pSecondLast->_pNext != _pTail; pSecondLast = pSecondLast->_pNext){}
			_pTail = pSecondLast;
			pSecondLast->SetNext(nullptr);
			if (_number>0)
			{
				_number--;
			}
			return result;
		}

		template<typename T>
		void LinkedList<T>::Clear()
		{
			LinkedElement<T>* pCurrent = _pHead;
			while (pCurrent != nullptr)
			{
				LinkedElement<T>* pTemp = pCurrent;
				_number--;
				pCurrent = pCurrent->_pNext;
				SAFE_DELETE(pTemp);
			}
			_pHead = nullptr;
			_pTail = nullptr;
		}

		template<typename T>
		LinkedElement<T>* LinkedList<T>::Remove(T& t)
		{
			LinkedElement<T>* pCurrent = _pHead;
			if (*pCurrent == t)
			{
				_number--;
				//if only one element(the head) in list, shoud clear the head and tail as null 
				if (pCurrent->_pNext == nullptr)
				{
					_pHead = nullptr;
					_pTail = nullptr;
					return pCurrent;
				}
				else
				{
					_pHead = pCurrent->_pNext;
				}
				return pCurrent;
			}
			for (;pCurrent;)
			{
				LinkedElement<T>* pTemp = pCurrent;
				if (pTemp->_pNext && (*(pTemp->_pNext) == t))
				{
					pCurrent->SetNext(pTemp->_pNext->_pNext);
					_number--;
					return pTemp;
				}
				pCurrent = pCurrent->_pNext;
			}
			return nullptr;
		}
		
		template<typename T>
		bool LinkedList<T>::Contains(T& t)
		{
			LinkedElement<T>* pCurrent = _pHead;
			while (pCurrent != nullptr)
			{
				if (pCurrent->GetValue() == t)
				{
					return true;
				}
				pCurrent = pCurrent->_pNext;
			}
			return false;
		}

		//for every element in the list, run the function F
		//F should be the function to operate T*
		template<typename T>
		template<typename F, typename ...Args>
		void LinkedList<T>::RunIterate(F func, Args&&... args)
		{
			LinkedElement<T>* pCurrent = _pHead;
			for (; pCurrent != nullptr;)
			{
				LinkedElement<T>* pTemp = pCurrent;
				pCurrent = pCurrent->_pNext;
				func(pTemp->GetValue(), std::forward<Args>(args)...);
			}
		}

	}
}



#endif //LINKEDLIST_H
