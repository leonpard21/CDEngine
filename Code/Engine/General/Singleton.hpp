#ifndef EAE_ENGINE_GENERAL_SINGLETON_H
#define EAE_ENGINE_GENERAL_SINGLETON_H

namespace EAE_Engine 
{
	// this class comes from https://gist.github.com/daniebker/2299755
	template<typename T>
	class Singleton
	{
	public:
		static T* GetInstance();
		static void Destroy();

	private:

		Singleton(Singleton const&) {};
		Singleton& operator=(Singleton const&) {};

	protected:
		static T* _instance;

		Singleton() { _instance = static_cast <T*> (this); };
		~Singleton() {  };
	};

	template<typename T>
	typename T* Singleton<T>::_instance = 0;

	template<typename T>
	T* Singleton<T>::GetInstance()
	{
		if (!_instance)
		{
			Singleton<T>::_instance = new T();
		}

		return _instance;
	}

	template<typename T>
	void Singleton<T>::Destroy()
	{
		if (_instance)
		{
			delete Singleton<T>::_instance;
			Singleton<T>::_instance = 0;
		}
	}
}



#endif//EAE_ENGINE_GENERAL_SINGLETON_H