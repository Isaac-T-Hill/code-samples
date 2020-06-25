/******************************************************************************/
/*!
\file   semaphore.h
\author Isaac Hill
\par    email: Isaac.Hill@digipen.edu
\par    DigiPen login: Isaac.Hill
\par    Course: CS355
\par    Assignment #2
\date   2/11/2020
\brief
This is the Header file for a semaphore implementation using mutexes
*/
/******************************************************************************/

#pragma once

#include <condition_variable>
#include <mutex>
#include <iostream>

class Semaphore
{
public:
	Semaphore(int init_count) : count_(init_count) {}

	void wait();
	void signal();

private:
	int count_;
	std::mutex cv_m_;
	std::condition_variable cv_;
};