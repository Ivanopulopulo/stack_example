#include "stack.hpp"
#include <string>
#include <chrono>

void producer(stack<int> &Stack)
{
	for(;;)
	{
        try
        {
		        Stack.push(std::rand() % 100);
        }
        catch(std::bad_alloc)
        {
                std::this_thread::sleep_for(std::chrono::seconds(std::rand() % (3) + 1));
        }
        std::this_thread::sleep_for(std::chrono::seconds(std::rand() % (3) + 1));
	}
}

void consumer(stack<int> &Stack)
{
	for(;;)
	{
		try
		{
			Stack.pop();
		}
		catch(std::logic_error)
		{
			std::this_thread::sleep_for(std::chrono::seconds(std::rand() % (3) + 2));
		}
		std::this_thread::sleep_for(std::chrono::seconds(std::rand() % (3) + 2));
	}	
}

int main()
{
	stack<int> Stack;
	
	std::thread prod(producer, std::ref(Stack));
	std::thread cons(consumer, std::ref(Stack));
	
	prod.join();
	cons.join();

	
	return 0;
}
