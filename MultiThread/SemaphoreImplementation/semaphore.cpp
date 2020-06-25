/******************************************************************************/
/*!
\file   semaphore.cpp
\author Isaac Hill
\par    email: Isaac.Hill@digipen.edu
\par    DigiPen login: Isaac.Hill
\par    Course: CS355
\par    Assignment #2
\date   2/11/2020
\brief
This is the Implementation file for a semaphore using mutexes
*/
/******************************************************************************/

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