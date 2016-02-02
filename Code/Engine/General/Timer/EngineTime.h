#pragma once
#include <iostream>
#include <chrono>
#include <time.h>
namespace EAE_Engine
{
	namespace Time
	{
		template<typename TimeT = std::chrono::milliseconds>
		class Timer
		{
		public:
			//calculate the time to run the function
			template<typename F, typename ...Args>
			static typename TimeT::rep execution(F func, Args&&... args)
			{
				auto start = std::chrono::system_clock::now();

				// Now call the function with all the parameters you need.
				func(std::forward<Args>(args)...);

				auto duration = std::chrono::duration_cast< TimeT>
					(std::chrono::system_clock::now() - start);

				return duration.count();
			}

		public:
			Timer(){}
			~Timer(){}

			inline void Start()
			{
				_start = std::chrono::system_clock::now();
			}

			inline typename TimeT::rep GetPassedTimeNow()
			{
				_now = std::chrono::system_clock::now();
				auto duration = std::chrono::duration_cast< TimeT>(_now - _start);
				return duration.count();
			}

		private:
			std::chrono::system_clock::time_point _start;
			std::chrono::system_clock::time_point _now;

		};



	}
}
