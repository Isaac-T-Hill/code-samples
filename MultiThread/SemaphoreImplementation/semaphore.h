#pragma once

#include <condition_variable>
#include <mutex>
#include <iostream>

class Semaphore
{
public:
	Semaphore(int init_count) : count(init_count) {}

	void wait();
	void signal();

private:
	int count_;
	std::mutex cv_m_;
	std::condition_variable cv_;
};