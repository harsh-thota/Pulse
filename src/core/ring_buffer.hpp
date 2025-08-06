#pragma once
#include <cstddef>

template<typename T, size_t N>
class RingBuffer
{
public:
	void push(const T& value)
	{
		buffer_[head_] = value;
		head_ = (head_ + 1) % N;
		if (size_ < N) ++size_;
	}

	T get(size_t index) const
	{
		if (index >= size_) return T{};
		size_t actualIndex = (head_ + N - size_ + index) % N;
		return buffer_[actualIndex];
	}

	size_t size() const { return size_; }
	size_t capacity() const { return N; }
	bool empty() const { return size_ == 0; }
	bool full() const { return size_ == N; }

	T latest() const
	{
		if (empty()) return T{};
		return buffer_[(head_ + N - 1) % N];
	}

	void clear()
	{
		head_ = 0;
		size_ = 0;
	}

private:
	T buffer_[N];
	size_t head_ = 0;
	size_t size_ = 0;
};