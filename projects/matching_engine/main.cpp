#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <forward_list>

#ifdef _WIN32

#include <Windows.h>

#elif defined(__linux)

#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif

#include <sys/time.h>

#else

#error "Unknown system"

#endif

class Order
{
	private:
		float price;
		int volume;
		float timestamp;

	public:
		Order(int _volume)
		{
			volume = _volume;

#ifdef _WIN32
			
			FILETIME ft;
			GetSystemTimeAsFileTime(&ft);
			unsigned long long tt = ft.dwHighDateTime;
			tt <<= 32;
			tt |= ft.dwLowDateTime;
			tt /= 10;
			tt -= 11644473600000000ULL;
			timestamp = ((float) tt) / 1000000.0f;
			
#elif defined(__linux)
			
			struct timeval now;
			gettimeofday(&now, NULL);
			timestamp = (float) now.tv_sec + ((float) tv_usec / 1000000.0f);
			
#endif
		}

		~Order()
		{

		}
};

#define ORDER_SIDE_BUY		0
#define ORDER_SIDE_SELL		1

class TickOrders
{
	private:
		int side;
		float price;
		std::forward_list<Order> orders;

	public:
		TickOrders(float _price)
		{
			price = _price;
		}
};

#define EXCHINST_SUCCESS				0

class ExchangeInst
{
	private:
		char name[64];
		float tickSize;
		//float lowCollar; FUTURE!
		//float highCollar; FUTURE!
		std::forward_list<TickOrders> buyOrders;
		std::forward_list<TickOrders> sellOrders;

	public:
		ExchangeInst(const char* const _name, const float _tickSize)
		{
			strncpy(name, _name, 64);
			tickSize = _tickSize;
		}

		~ExchangeInst()
		{
			
		}

		int enterOrder()
};

int main(int argc, char** argv)
{
	

	return EXIT_SUCCESS;
}
