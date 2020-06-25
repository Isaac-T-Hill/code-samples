
#include "semaphore.h"

void Semaphore::wait()
{
	std::unique_lock<std::mutex> lk(cv_m_);
	//std::cerr << "Count is " << count << "\n"; // Debug Output

	if(count_ > -1)
		--count_;

	cv_.wait(lk, [this] {return count_ >= 0; });
}

void Semaphore::signal()
{
	std::unique_lock<std::mutex> lk(cv_m_);

	++count_;
	cv_.notify_one();
}