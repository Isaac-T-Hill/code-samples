/******************************************************************************/
/*!
\file   water.cpp
\author Isaac Hill
\par    email: Isaac.Hill@digipen.edu
\par    DigiPen login: Isaac.Hill
\par    Course: CS280
\par    Assignment #6
\date   2/6/2018
\brief
This is the Implementation file for a 2D water retention solver

Operations include:
-Column::Update
-Constructor
-Destructor
-edgeUpdate
-World::Update
-checkNeighbor
-getHeld
-waterret
*/
/******************************************************************************/

#include "water.h"

/******************************************************************************/
/*!
Updates the column

\param col
column to update

\return
if the column was updated
*/
/******************************************************************************/
bool Column::Update(Column* col)
{
  //If update is needed
  if(col->cutoff_ > cutoff_ && col->edge_ == false)
  {
    col->cutoff_ = std::max(cutoff_, col->height_);
    col->waterlevel_ = col->cutoff_ - col->height_;

    return true;
  }

  return false;
}

/******************************************************************************/
/*!
Creates a map from the text of given file

\param filename
File that contains map to build
*/
/******************************************************************************/
World::World(char const* filename)
{
  //Open file for reading
  std::ifstream infile(filename);
  if (!infile.is_open())
  {
    throw "Errors in input file: cannot open";
  }

  //Initialize data members
  infile >> height_;
  infile >> width_;

  //init grid with columns
  for(long i = 0; i < width_; i++)
  {
    grid_.push_back(std::vector<Column*>());
  }
  
  for (long i = 0; i < height_ * width_; i++)
  {
    long colHeight;
    infile >> colHeight;

    bool edge = false;

	// If position is on edge
    if(i%width_ == 0 || i%width_ == width_-1 ||i / width_ == 0 || i/width_ == height_-1)
    {
      edge = true;
    }

    grid_[i % width_].push_back(new Column(colHeight, edge));
  }
}

/******************************************************************************/
/*!
Frees Memory allocated during construction of World
*/
/******************************************************************************/
World::~World()
{
  for(unsigned i = 0; i < grid_.size(); ++i)
  {
	for(unsigned j = 0; j < grid_[i].size(); ++j)
	{
	  // Free all column that were allocated during constructor
	  delete grid_[i][j];
	}
  }
}

/******************************************************************************/
/*!
Updates the edge spaces based neighboring spaces
*/
/******************************************************************************/
void World::edgeUpdate()
{
  for(long i = 0; i < width_; i++)
  {
    for(long j = 0; j < height_; j++)
    {
      Column* col = grid_[i][j];

      if(col->isEdge())
      {
        Column* currCol;
        //Check left neighbor
        if(i-1 > 0)
        {
          currCol = grid_[i - 1][j];
          if (!currCol->isEdge())
          {
            bool result = col->Update(currCol);
            if (result)
            {
              updateQueue_.push(std::pair<long, long>(i-1, j));
            }
          }
        }
        //Check right neighbor
        if(i+1 < width_)
        {
          currCol = grid_[i + 1][j];
          if (!currCol->isEdge())
          {
            bool result = col->Update(currCol);
            if (result)
            {
              updateQueue_.push(std::pair<long, long>(i+1, j));
            }
          }
        }
        //Check up neighbor
        if(j-1 > 0)
        {
          currCol = grid_[i][j - 1];
          if (!currCol->isEdge())
          {
            bool result = col->Update(currCol);
            if (result)
            {
              updateQueue_.push(std::pair<long, long>(i, j-1));
            }
          }
        }
        //Check down neighbor
        if(j+1 < height_)
        {
          currCol = grid_[i][j + 1];
          if (!currCol->isEdge())
          {
            bool result = col->Update(currCol);
            if (result)
            {
              updateQueue_.push(std::pair<long, long>(i, j+1));
            }
          }
        }

        
      }
    }
  }
}

/******************************************************************************/
/*!
Updates all columns in the update queue
*/
/******************************************************************************/
void World::Update()
{
  while(!updateQueue_.empty())
  {
    std::pair<long, long> coord = updateQueue_.front();
    updateQueue_.pop();

    Column* col = grid_[coord.first][coord.second];

    //std::cout << "Column: " << coord.first << " " << coord.second << " -> ";

    //Check left neighbor
    //std::cout << "check: " << coord.first - 1 << " " << coord.second << std::endl;
    checkNeighbor(col, std::pair<long, long>(coord.first - 1, coord.second));

    //Check right neighbor
    //std::cout << "check: " << coord.first + 1 << " " << coord.second << std::endl;
    checkNeighbor(col, std::pair<long, long>(coord.first + 1, coord.second));
    
    //Check up neighbor
    //std::cout << "check: " << coord.first << " " << coord.second - 1 << std::endl;
    checkNeighbor(col, std::pair<long, long>(coord.first, coord.second - 1));

    //Check down neighbor
    //std::cout << "check: " << coord.first << " " << coord.second + 1 << std::endl;
    checkNeighbor(col, std::pair<long, long>(coord.first, coord.second + 1));
  }
}

/******************************************************************************/
/*!
Checks to see if neighbor needs to be updated
*/
/******************************************************************************/
void World::checkNeighbor(Column* col, std::pair<long, long> coord)
{
  if ((coord.first >= 0 && coord.first < width_) && (coord.second >= 0 && coord.second < height_))
  {
    //get neighbor
    Column* currCol = grid_[coord.first][coord.second];

    //try update
    bool result = col->Update(currCol);

    //push coordinates of neighbor to queue if updated
    if (result)
    {
      updateQueue_.push(coord);
    }
  }
}

/******************************************************************************/
/*!
Gets the total amount of water held after no more updates are needed

\return
The amount of water retained
*/
/******************************************************************************/
long World::getHeld()
{
  long result = 0;

  for(long i = 0; i < width_*height_; i++)
  {
    result += grid_[i%width_][i / width_]->getHeld();

    //std::cout << result << std::endl;
  }

  return result;
}

/******************************************************************************/
/*!
Initiates each step of the solver

\return
The amount of water retained
*/
/******************************************************************************/
long waterret(char const* filename)
{
  World world = World(filename);
  world.edgeUpdate();
  world.Update();

  //  PRlong WORLD
  long height = world.getHeight();
  long width = world.getWidth();

  for(long i = 0; i < height * width; i++)
  {
    //std::cout << i%width << " " << i / width << " " << world.grid_[i%width][i / width]->getHeld() << std::endl;
  }

  return world.getHeld();
}
