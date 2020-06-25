/******************************************************************************/
/*!
\file   gol.cpp
\author Isaac Hill
\par    email: Isaac.Hill@digipen.edu
\par    DigiPen login: Isaac.Hill
\par    Course: CS355
\par    Assignment #1
\date   1/21/2020
\brief
This is the Implementation file for a game of life simulation
*/
/******************************************************************************/

#include "gol.h"

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <chrono>
#include <thread>
#include <stdlib.h>

enum State
{
	kDead,
	kAlive
};

struct Arguments
{
	int x;
	int y;
	int num_threads;
	int iterations;
	std::vector<std::vector<State>> *p_board;
};

//Forward declarartion of helper functions
std::vector<std::vector<State>> CreateBoard(std::vector< std::tuple<int, int> > initial_population, int max_x, int max_y);
State CalculateNewState(std::vector<std::vector<State>> *board, int x_pos, int y_pos);
std::vector< std::tuple<int, int> > GetResult(std::vector<std::vector<State>> board);

//Global thread variables
int count = 0;
pthread_mutex_t print_mutex;
sem_t count_mutex;
sem_t barrier;
sem_t barrier2;

/******************************************************************************/
/*!
Main logic for thread simulation

\param p
Arguments passed to thread
*/
/******************************************************************************/
void* Simulate(void * p)
{
	Arguments arg = *reinterpret_cast<Arguments*>(p);

	for (int i = 0; i < arg.iterations; ++i)
	{
		//Adjust counter inside mutex
		//last thread signals barrier and resets barrier2
		sem_wait(&count_mutex);
		count = count + 1;
		if (count == arg.num_threads) 
		{
			sem_wait(&barrier2);
			sem_post(&barrier);
		}
		sem_post(&count_mutex);

		//First barrier
		sem_wait(&barrier);
		sem_post(&barrier);

		// critical point
		// Look at neighbors and find out fate
		State new_state = CalculateNewState(arg.p_board, arg.x, arg.y);

		//Adjust counter inside mutex
		//last thread signals barrier2 and resets barrier
		sem_wait(&count_mutex);
		count = count - 1;
		if (count == 0) 
		{
			sem_wait(&barrier);
			sem_post(&barrier2);
		}
		sem_post(&count_mutex);

		//Second barrier
		sem_wait(&barrier2);
		sem_post(&barrier2);

		//Critical point
		//Write new state to board
		(*arg.p_board)[arg.x][arg.y] = new_state;
	}

	return NULL;
}

/******************************************************************************/
/*!
Main program logic to create and run threads

\param initial_population
Coordinates of initial live spaces

\param num_iter
Number of iterations to run

\param max_x
max width of board

\param max_y
max height of board

\return
coordinates of live cells at end of simulation
*/
/******************************************************************************/
std::vector< std::tuple<int, int> > // return vector of coordinates of the alive cells of the final population
run(std::vector< std::tuple<int, int> > initial_population, int num_iter, int max_x, int max_y)
{
	//Create board from initial population
	std::vector<std::vector<State>> Board = CreateBoard(initial_population, max_x, max_y);

	int const size = max_x * max_y;

	// storage for threads
	pthread_t* threads_id = new pthread_t[size];

	//Initialize synchronization tools
	pthread_mutex_init(&print_mutex, 0);
	sem_init(&count_mutex, 0, 1);
	sem_init(&barrier, 0, 0);
	sem_init(&barrier2, 0, 1);


	// storage for arguments
	Arguments* args = new Arguments[size];

	// pre-allocate all thread arguments
	int x = 1;
	int y = 1;

	//Initialize arguments for all threads
	for (int i = 0; i < size; ++i) 
	{
		args[i].x = x++;
		args[i].y = y;
		args[i].num_threads = size;
		args[i].iterations = num_iter;
		args[i].p_board = &Board;

		if (x > max_x)
		{
			x = 1;
			++y;
		}
	}

	//Create threads
	for (int i = 0; i < size; ++i) {
		pthread_create(&threads_id[i], 0, Simulate, &args[i]);
	}

	//wait for all threads to finish
	for (int i = 0; i < size; ++i) 
	{
		pthread_join(threads_id[i], 0);
	}

	delete[] args;
	delete[] threads_id;

	return GetResult(Board);
}

/******************************************************************************/
/*!
Creates the intitial board for the simulation

\param initial_population
Coordinates of initial live spaces

\param max_x
max width of board

\param max_y
max height of board

\return
Created board to use
*/
/******************************************************************************/
std::vector<std::vector<State>> CreateBoard(std::vector< std::tuple<int, int> > initial_population, int max_x, int max_y)
{
	//Create board and initialize to kDead
	// +2 to x and y gives entire board a a 1 space border
	std::vector<std::vector<State>> result(max_x + 2, std::vector<State>(max_y + 2, State::kDead));

	int x, y;
	//Set all initial spaces to kAlive
	for (int i = 0; static_cast<unsigned int>(i) < initial_population.size(); ++i)
	{
		std::tie(x, y) = initial_population[i];

		//Increment x and y to account for border
		++x;
		++y;

		result[x][y] = State::kAlive;
	}

	return result;
}

/******************************************************************************/
/*!
Calculates new state based on surrounding states

\param board
The board in it's current state

\param x_pos
x coordinate of space being checked

\param y-pos
y coordinate of space being checked

\return
The new state for the current location
*/
/******************************************************************************/
State CalculateNewState(std::vector<std::vector<State>> *board, int x_pos, int y_pos)
{
	State old_state = (*board)[x_pos][y_pos];
	State new_state = old_state;
	int count = 0;

	//Loops to look at neighboring spaces
	for (int i = x_pos - 1; i <= x_pos + 1; ++i)
	{
		for (int j = y_pos - 1; j <= y_pos + 1; ++j)
		{
			//Skip the space occupied by self
			if (i != x_pos || j != y_pos)
			{
				//Increase count if neighboring space is alive
				if ((*board)[i][j] == State::kAlive)
				{
					++count;
				}
					
			}
		}
	}

	//Determine new state from number of alive neighboring spaces
	if (old_state == State::kDead)
	{
		if (count == 3) 
		{
			new_state = State::kAlive;
		}
	}
	else 
	{
		if (count < 2 || count > 3) 
		{
			new_state = State::kDead; 
		}
	}

	return new_state;
}

/******************************************************************************/
/*!
Gets coordinates of all live spaces on board

\param board
The board in the final state

\return
All coordinates of live spaces on board
*/
/******************************************************************************/
std::vector< std::tuple<int, int> > GetResult(std::vector<std::vector<State>> board)
{
	std::vector<std::tuple<int, int>> result;

	for (int i = 0; static_cast<unsigned int>(i) < board.size(); ++i)
	{
		for (int j = 0; static_cast<unsigned int>(j) < board[i].size(); ++j)
		{
			if (board[i][j] == State::kAlive)
			{
				// -1 to set position back to 0 index due to border
				result.push_back(std::make_tuple(i - 1, j - 1));
			}
		}
	}

	return result;
}