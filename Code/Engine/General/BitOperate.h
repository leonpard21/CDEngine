#ifndef BIT_OPERATE_H
#define BIT_OPERATE_H

/* a=target variable, b=bit number to act upon 0-n */
#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))

/* x=target variable, y=mask */
#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_FLIP(x,y) ((x) ^= (y))
#define BITMASK_CHECK(x,y) ((x) & (y))


namespace EAE_Engine 
{

	//set one bit in the address
	template <typename T = uint8_t>
	inline void SetBit(T& pAddress, size_t bit)
	{
		pAddress |= (static_cast<T>(0x1) << bit);
	}

	//clear one bit in the address
	template <typename T = uint8_t>
	inline void ClearBit(T& pAddress, size_t bit)
	{
		pAddress &= ~(static_cast<T>(0x1) << bit);
	}


	//check one bit in the address
	template <typename T = uint8_t>
	inline bool CheckBit(T& pAddress, size_t bit)
	{
		auto result = (pAddress & (static_cast<T>(0x1) << bit));
		bool temp = result == 0 ? false : true;
		return temp;// reinterpret_cast<bool&>(result);
	}

}

#endif//BIT_OPERATE_H