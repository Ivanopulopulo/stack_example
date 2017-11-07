#include <iostream>
#include <algorithm>
#include <mutex>
#include <new>
#include <memory>
#include <thread>

template <typename T>
class stack
{
public:
	stack() noexcept;
	~stack() noexcept;
	stack(stack<T> const&)/*strong*/;
	stack& operator=(stack<T> const&)/*strong*/;
	size_t count()const noexcept;
	void push(T const&)/*strong*/;
	std::shared_ptr<T> pop();/*strong*/;
	void print() const /*strong*/;
	bool empty()const noexcept;
private:
	void swap(stack<T>&)noexcept;
	T * array_;
	mutable std::mutex mutex_;
	size_t array_size_;
	size_t count_;
};

template <typename T> 
stack<T>::stack() noexcept : array_{ nullptr }, array_size_{ 0 }, count_{ 0 } {}
template <typename T>
stack<T>::~stack()noexcept
{
	delete[] array_;
}
template <typename T>
stack<T>::stack(stack<T> const& other)
{
	std::lock_guard<std::mutex> lock(other.mutex_);
	T new_array = new T [other.array_size_];
	array_size_ = other.array_size_;
	count_ = other.count_;	
	array_ = new_array;
	try
	{
		std::copy(other.array_, other.array_ + count_, array_);	
	}
	catch( ... )
	{
		std::cerr << "ERROR!" << std::endl;
		delete[] array_;
	}				
}
template <typename T>
stack<T>& stack<T>::operator=(stack<T> const & other)
{
	if (&other != this)
		stack(other).swap(*this);
	return *this;
}
template <typename T>
size_t stack<T>::count()const noexcept
{
	std::lock_guard<std::mutex> lock(mutex_);
	return count_;
}
template <typename T>
void stack<T>::push(T const & value)
{
	std::lock_guard<std::mutex> lock(mutex_);

	if (count_ == array_size_)
	{
		auto array_size = array_size_ == 0 ? 1 : array_size_ * 2;
		T* new_array = new T[array_size];
		try
		{
			std::copy(array_, array_ + count_, new_array);
		}
		catch (...)
		{
			delete[] new_array;
			throw "ERROR!\n";
		}

		array_size_ = array_size;

		if (array_)
			delete[] array_;
		array_ = new_array;
	}

	array_[count_] = value;
	++count_;

}
template <typename T>
std::shared_ptr<T> stack<T>::pop() {
	
	std::lock_guard<std::mutex> lock(mutex_);
	if (count_ > 0) {
		auto res = std::make_shared<T>(array_[count_ - 1]);;
		--count_;
		return res;
	} else {
		throw "Empty!";
	}
}
template <typename T>
void stack<T>::print() const 
{
	mutex_.lock();
	if(!empty())
	{
	        for (unsigned int i = 0; i < count_; ++i)
		        std::cout << array_[i] << " ";
	        std::cout << std::endl;
	}
	mutex_.unlock();
}
template <typename T>
void stack<T>::swap(stack<T>& other)noexcept
{
	std::lock(mutex_, other.mutex_);
	std::swap(array_, other.array_);
	std::swap(array_size_, other.array_size_);
	std::swap(count_, other.count_);
	mutex_.unlock();
	other.mutex_.unlock();
}
template <typename T>
bool stack<T>::empty()const noexcept
{
	std::lock_guard<std::mutex> lock(mutex_);
	return (count_ == 0);
}
