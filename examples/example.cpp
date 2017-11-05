#include "stack.hpp"
#include <string>
#include <chrono>
#include <thread>

void producer (stack<int> &st)
{
   for (;;)
    {
      st.push(rand()%100);
      std::this_thread::sleep_for(std::chrono::seconds(std::rand() % (3) + 1));
    }
}

void consumer (stack<int> &st)
{
   for (;;)
    {
	   try
	   { 
       		st.pop();
	   }
	   catch(...)
	   {
		 std::this_thread::sleep_for(std::chrono::seconds(std::rand() % (3) +2 ));   
	   }
      	   std::this_thread::sleep_for(std::chrono::seconds(std::rand() % (3) +2 ));
    }
}

int main()
{
	stack<int> st1;
	
	std::thread prod(producer, std::ref(st1));
	std::thread cons(consumer, std::ref(st1));
	
	prod.join();
	cons.join();
  return 0;
}
