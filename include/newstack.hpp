#include <iostream>
#include <string>
#include <memory>
#include <mutex>

template <typename T>
class stack
{
public:
	stack() noexcept;
	~stack() noexcept;
	stack(stack<T> const&)/*strong*/;
	stack& operator=(stack<T> const&)/*strong*/;
	size_t count()const noexcept;
  void push(const T&);
	std::shared_ptr<T> pop();
	bool empty() const;
private:
	void swap(stack&);
	T* cop_arr(const T*, std::size_t, std::size_t);
	T * array_;
	mutable std::mutex mutex_;
	std::condition_variable cond_;
	size_t array_size_;
	size_t count_;
};
template<typename T>
T* stack<T>::cop_arr(const T* src_ar, std::size_t src_count, std::size_t src_array_size) {

	T* dest_ar = nullptr;
	if (src_array_size > 0) {
		destr_ar = new T[src_array_size];
		try {
			std::copy(src_ar, src_ar + src_count, destr_ar);
		}
		catch (...) {
			delete[] destr_ar;
			throw;
		}
	}

	return destr_ar;
}

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
template<typename T>
void stack<T>::push(const T& val) {
	
	std::lock_guard<std::mutex> lock(mutex_);
	if (array_size_ == 0) {
		array_ = new T[1];
		array_[0] = val;
		count_ = array_size_ = 1;
	} else {
		if (count_ == array_size_) {
			T* tmp = nullptr;
			std::size_t newsize = array_size_ * 2;
			tmp = copy_arr(array_, count_, newsize);
			array_size_ = newsize;
			delete[] array_;
			array_ = tmp;
		}
		array_[count_] = val;
		++count_;
	}
}
template<typename T>
std::shared_ptr<T> stack<T>::pop() {
	
	std::lock_guard<std::mutex> lock(mutex_);
	if (count_ > 0) {
		auto res = std::make_shared<T>(array_[count_ - 1]);
		--count_;
		return res;
	}
	return nullptr;
}

template<typename T>
void stack<T>::swap(stack<T>& other) {

	std::swap(array_, other.array_);
	std::swap(array_size_, other.array_size_);
	std::swap(count_, other.count_);
}

template <typename T>
bool stack<T>::empty() const {
	
	mutex_.lock();
	bool em = (count_ == 0);
	mutex_.unlock();
	return em;
}
