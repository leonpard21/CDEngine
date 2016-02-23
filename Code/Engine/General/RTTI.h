#ifndef RTTI_H
#define RTTI_H
#include <cstdint>
#include "HashString/HashedString.h"

	// for any unkonwn types
	// for each type we will make an id for them
	typedef EAE_Engine::HashCode typeid_t;
	const typeid_t g_UnkownType = (EAE_Engine::HashCode)(UINT_MAX);

	/*
     * The reason that we don't use the static int to represent the Type is,
     * that we cannot hold everything together when the type is located in DLL.
     * because each dll has their own static int.
     *****************************    Not support DLL        ***********************
	 // Unique ID generator
	 // Could also use the __COUNTER__ preprocessor macro, but it may be MS specific
     typeid_t generateUniqueIDForType()
     {
         static int currentTypeID = 0;
         return ++currentTypeID;
     }
	 */

    // Unique ID generator
    // Could also use the __COUNTER__ preprocessor macro, but it may be MS specific
	inline typeid_t generateUniqueIDForType(const char* pType)
	{
		EAE_Engine::HashedString hashStr(pType);
		return EAE_Engine::HashedString::Hash(pType);
	}
	
	// Use some template trickery to create a unique ID per class or type
	template<typename T>
	inline typeid_t getTypeID()
	{
		// when you get this error,
		// you have to declare first the type with this macro
		//static const typeid_t typeID = generateUniqueID();
		return g_UnkownType;
	}

	// a handy marco, returns for every name a unique typedid_t
	//I think this is OK for multithread, because we generate the typeid by their name.
	//so the same type name will generate the same typedid_t 
#define RTTI_DECLARE_META_TYPE(T)                                 \
	template<>                                                    \
	inline typeid_t getTypeID<T>()                                \
	{                                                             \
		static const typeid_t val = generateUniqueIDForType(#T);  \
		return val;                                               \
	}                                                   

	// use it like this:
	RTTI_DECLARE_META_TYPE(int)
	RTTI_DECLARE_META_TYPE(unsigned int)
	RTTI_DECLARE_META_TYPE(bool)
	RTTI_DECLARE_META_TYPE(void)
	RTTI_DECLARE_META_TYPE(float)
	RTTI_DECLARE_META_TYPE(double)
	RTTI_DECLARE_META_TYPE(char)
	RTTI_DECLARE_META_TYPE(unsigned char)
	RTTI_DECLARE_META_TYPE(long)
	RTTI_DECLARE_META_TYPE(long long)
	RTTI_DECLARE_META_TYPE(short)
	RTTI_DECLARE_META_TYPE(unsigned short)
	RTTI_DECLARE_META_TYPE(std::string)
	//RTTI_DECLARE_META_TYPE(std::vector<int>)
	//RTTI_DECLARE_META_TYPE(std::vector<float>)
	//RTTI_DECLARE_META_TYPE(std::vector<double>)

	// Use even more template trickery to get the classes unique ID from an instance of it.
	template<typename T>
	typeid_t getTypeIDFromInstance(const T*)
	{
		return getTypeID<T>();
	}

	// Use the function to test a pointer's type has been defined or not.
	template<typename T>
	bool isKnownType(const T* p)
	{
		typeid_t id = getTypeIDFromInstance<T>(p);
		if (id == g_UnkownType)
		{
			return false;
		}
		return true;
	}



	// A handy macro for adding type ID info to a class
#define ADD_TYPEID() \
public: \
	virtual typeid_t GetTypeID() const { return getTypeIDFromInstance( this ); } \
private:


	// A template for casting from base to derived, if it's possible
	template<typename To, typename From>
	To * attempt_cast(From * i_pFrom)
	{
		assert(i_pFrom);
		if (!isKnownType<From>(i_pFrom))
		{
			return nullptr;
		}
		if (!isKnownType<To>(nullptr))
		{
			return nullptr;
		}

		if (i_pFrom->_getTypeID() == getTypeID<To>())
			return static_cast<To*>(i_pFrom);
		else
			return nullptr;
	}

	template<typename To, typename From>
	const To* attempt_cast(const From * i_pFrom)
	{
		assert(i_pFrom);
		if (!isKnownType<From>(i_pFrom))
		{
			return nullptr;
		}
		if (!isKnownType<To>(nullptr))
		{
			return nullptr;
		}

		if (i_pFrom->_getTypeID() == getTypeID<To>())
			return static_cast<const To *>(i_pFrom);
		else
			return nullptr;
	}



	/*
	// sample!!
	class Base
	{
	ADD_TYPEID();
	};
	RTTI_DECLARE_META_TYPE(Base)

	class Derived1 : public Base
	{
	ADD_TYPEID();
	};
	RTTI_DECLARE_META_TYPE(Derived1)

	class Derived2 : public Base
	{
	ADD_TYPEID();
	};
	RTTI_DECLARE_META_TYPE(Derived2)


	//Test the sample code in UnitTest
	{
		// create some instances
		Derived1 D1;
		Derived2 D2;

		// cast the to base class pointers so we lose hierarchy info
		Base * pD1AsBase = &D1;
		Base * pD2AsBase = &D2;

		// attempt to cast pD1AsBase to the two derived types
		Derived1 * pD1CastToD1 = attempt_cast<Derived1>(pD1AsBase);
		Derived2 * pD1CastToD2 = attempt_cast<Derived2>(pD1AsBase);

		// print the results
		printf("pD1CastToD1 returned %p\n", pD1CastToD1);
		printf("pD1CastToD2 returned %p\n", pD1CastToD2);
	}
	*/

//}

#endif//RTTI_H