#include <iostream>
#include <string>
#include <memory>
#include <mutex>


template <typename T>
class stack {

public:
	stack();
	stack(const stack&);
	std::size_t count() const ;
	void push(const T&);
	std::shared_ptr<T> pop();
	bool empty() const;
	stack& operator=(const stack&);
	~stack();
	

private:
	
	T* array_;
	std::size_t array_size_;
	std::size_t count_;
    mutable std::mutex mutex_;

	void swap(stack&);
	T* copy_arr(const T*, std::size_t, std::size_t);
};

template<typename T>
T* stack<T>::copy_arr(const T* src_ar, std::size_t src_count, std::size_t src_array_size) {

	T* dest_ar = nullptr;
	if (src_array_size > 0) {
		dest_ar = new T[src_array_size];
		try {
			std::copy(src_ar, src_ar + src_count, dest_ar);
		}
		catch (...) {
			delete[] dest_ar;
			throw;
		}
	}

	return dest_ar;
}

template<typename T>
void stack<T>::swap(stack<T>& other) {

	std::swap(array_, other.array_);
	std::swap(array_size_, other.array_size_);
	std::swap(count_, other.count_);
}

template<typename T>
stack<T>::stack()
	: 
	array_size_{0}, 
	count_{0}, 
    array_{nullptr} {
		
}

template<typename T>
stack<T>::stack(const stack<T>& other) {
	
	std::lock_guard<std::mutex> lockother(other.mutex_);
	array_size_ = other.array_size_;
	count_ = other.count_;
	array_ = copy_arr(other.array_, count_, array_size_);
}

template<typename T>
stack<T>& stack<T>::operator=(const stack<T>& other) {
	
	std::lock_guard<std::mutex> lock(mutex_);
	if (this != &other) {
		stack<T> tmp(other);
		tmp.swap(*this);
	}
	return *this;
}

template<typename T>
stack<T>::~stack() {
	
	delete[] array_;
}

template<typename T>
std::size_t stack<T>::count() const {
	
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
bool stack<T>::empty() const {
	
	mutex_.lock();
	bool em = (count_ == 0);
	mutex_.unlock();
	return em;
}
