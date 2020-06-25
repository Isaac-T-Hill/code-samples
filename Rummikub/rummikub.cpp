/******************************************************************************/
/*!
\file   rummikub.cpp
\author Isaac Hill
\par    email: Isaac.Hill@digipen.edu
\par    DigiPen login: Isaac.Hill
\par    Course: CS280
\par    Assignment #3
\date   2/6/2018
\brief
This is the Implementation file for a rummikub solver that figures out if there 
is a solution involving all tiles from a given set

Operations include:
-Constructor
-Get groups
-Get runs
-add tile to hand
-solve
-add tile to group
-add tile to run
-create group
-create run
-Check solution
*/
/******************************************************************************/

#include "rummikub.h"

#include <map>

/******************************************************************************/
/*!
Creates an empty hand for the solver
*/
/******************************************************************************/
RummiKub::RummiKub()
{
  num_tiles_ = 0;
}

/******************************************************************************/
/*!
Returns the groups from the solution

\return
vector of vectors containing groups
*/
/******************************************************************************/
std::vector< std::vector< Tile > > RummiKub::GetGroups() const
{
  return groups_final_;
}

/******************************************************************************/
/*!
Returns the runs from the solution

\return
vector of vectors containing runs
*/
/******************************************************************************/
std::vector< std::vector< Tile > > RummiKub::GetRuns() const
{
  return runs_final_;
}

/******************************************************************************/
/*!
Adds a tile to the hand

\param tile
The tile being added

\param add
The amount to add for number of tiles
*/
/******************************************************************************/
void RummiKub::Add(Tile const& tile, unsigned add)
{
  hand_.push_back(tile);
  num_tiles_ += add;
}

/******************************************************************************/
/*!
Recursively solves the rummikub problem

\return
boolean stating whether the problem is solved
*/
/******************************************************************************/
bool RummiKub::Solve()
{
  bool result = false;
  std::pair<int, std::pair<int, int>> placement;

  // base case
  // out of tiles - check if runs and groups are legal
  if (hand_.empty())
  {
    if (CheckSolved())
    {
      //move only non empty groups to groups final
      for (auto& group : groups_)
      {
        if (!group.empty())
        {
          groups_final_.push_back(group);
        }
      }

      //move only non empty runs to runs final
      for (auto& run : runs_)
      {
        if (!run.empty())
        {
          runs_final_.push_back(run);
        }
      }
      
      return true;
    }
    else
    {
      //Return false if not solved
      return false;
    }
  }
  
  //Loop through all 4 possible actions
  for (int i = 0; i < 4; i++) 
  {
    //Add to group
    if (i == 0)
    {
      placement = add2group(hand_.back(), placement.second.second);
    }
    //Add to run
    else if (i == 1)
    {
      placement = add2run(hand_.back(), placement.second.second);
    }
    //make new group
    else if (i == 2)
    {
      placement = addgroup(hand_.back());
    }
    //make new run
    else
    {
      placement = addrun(hand_.back());
    }

    if (placement.first != -1)
    {
      result = Solve(); // recursive call
      //if solved return true;
      if (result)
      {
        return true;
      }

      //undo the action
      else
      {
        //If group
        if (placement.first == 0)
        {
          //Add tile back to hand
          Add(groups_[placement.second.first].back(), 0);
          //pop from group
          groups_[placement.second.first].pop_back();
        }
        else // if run
        {
          //add tile back to hand
          Add(runs_[placement.second.first].back(), 0);
          //pop from run
          runs_[placement.second.first].pop_back();
        }
      }

      //If not all groups/runs were tested
      if (placement.second.second != -1)
      {
        //re-run loop
        i -= 1;
      }
      else
      {
        //start at first run/group for next loop
        placement.second.second = 0;
      }
    }
    else
    {
      //start at first run/group for next loop
      placement.second.second = 0;
    }
    
  }

  // could not find place for current tile - backtrack
  return false; 

}

/******************************************************************************/
/*!
Adds a tile to a valid group

\param tile
The tile being added

\param skip
group to skip to

\return
information regarding placement of tile
*/
/******************************************************************************/
std::pair<int, std::pair<int, int>> RummiKub::add2group(Tile tile, int skip)
{
  //Initialize information result
  std::pair<int, std::pair<int, int>> result;
  result.first = -1;
  result.second.second = -1;

  int num = 0;
  bool valid = true; 

  //Go through groups
  int i = 0;
  for (auto& group : groups_)
  {
    valid = true;

    //if group is the right number and not empty
    if(!group.empty())
      if (tile.denomination == group[0].denomination)
      {
        //If we have made it to the untried group
        if (num == skip)
        {
          //Go through tiles in the group
          for (auto& element : group)
          {
            //if there is already a tile of that color
            if (tile.color == element.color)
            {
              //Not a valid placement
              valid = false;
              break;
            }
          }

          //If group is still a valid placement
          if (valid)
          {
            //add tile to group
            group.push_back(tile);
            //Remove tile from hand
            hand_.pop_back();

            //update result pair with information
            result.first = 0;                   //groups
            result.second.first = i;            //group number
            result.second.second = skip + 1;    //element number

            break;
          }
        }
        else
        {
          num += 1;
        }
      }
    i++;
  }

  //return information on what happened to tile
  return result;
}

/******************************************************************************/
/*!
Adds a tile to a valid run

\param tile
The tile being added

\param skip
run to skip to

\return
information regarding placement of tile
*/
/******************************************************************************/
std::pair<int, std::pair<int, int>> RummiKub::add2run(Tile tile, int skip)
{
  //Initialize information result
  std::pair<int, std::pair<int, int>> result;
  result.first = -1;
  result.second.second = -1;

  int num = 0;
  bool valid = true;

  //Go through runs
  int i = 0;
  for (auto& run : runs_)
  {
    valid = true;

    //If run is the right color and not empty
    if(!run.empty())
    if (tile.color == run[0].color)
    {
      //If we have made it to the untried run
      if (num == skip)
      {
        //go through tiles in run
        for (auto& element : run)
        {
          //if there is a tile of the same denomination
          if (tile.denomination == element.denomination)
          {
            //run is no longer valid
            valid = false;
            break;
          }
        }

        //if run is still valid
        if (valid)
        {
          //Add tile to run
          runs_[i].push_back(tile);
          //Remove tile from hand
          hand_.pop_back();

          //update the result pair
          result.first = 1;
          result.second.first = i;
          result.second.second = skip + 1;

          break;
        }
      }
      else
      {
        num += 1;
      }
    }
    i++;
  }

  return result;
}

/******************************************************************************/
/*!
Creates a new group and ands the tile to it

\param tile
The tile being added

\return
information regarding placement of tile
*/
/******************************************************************************/
std::pair<int, std::pair<int, int>> RummiKub::addgroup(Tile tile)
{
  //Initialize information result
  std::pair<int, std::pair<int, int>> result;
  result.first = 0;
  result.second.first = static_cast<int>(groups_.size());
  result.second.second = -1;

  bool empty = false;

  //Check for empty groups
  for (unsigned i = 0; i < groups_.size(); i++)
  {
    if (groups_[i].empty())
    {
      result.second.first = static_cast<int>(i);
      empty = true;
    }
  }

  //if there is an empty group use it
  if (empty == true)
  {
    groups_[result.second.first].push_back(tile);
  }
  else
  {
    //Make new vector at end
    std::vector<Tile> add;
    add.push_back(tile);
    groups_.push_back(add);
  }

  //Remove tile from hand
  hand_.pop_back();

  return result;
}

/******************************************************************************/
/*!
Creates a new run and ands the tile to it

\param tile
The tile being added

\return
information regarding placement of tile
*/
/******************************************************************************/
std::pair<int, std::pair<int, int>> RummiKub::addrun(Tile tile)
{
  //Initialize information result
  std::pair<int, std::pair<int, int>> result;
  result.first = 1;
  result.second.first = static_cast<int>(runs_.size());
  result.second.second = -1;

  bool empty = false;

  //Check for empty runs
  for (unsigned i = 0; i < runs_.size(); i++)
  {
    if (runs_[i].empty())
    {
      result.second.first = static_cast<int>(i);
      empty = true;
    }
  }

  //If there is an empty run use it
  if (empty == true)
  {
    runs_[result.second.first].push_back(tile);
  }
  else
  {
    //Make new vector at end
    std::vector<Tile> add;
    add.push_back(tile);
    runs_.push_back(add);
  }

  //Remove tile from hand
  hand_.pop_back();

  return result;
}

/******************************************************************************/
/*!
Checks to see if the solution consists of valid runs and groups

\return
boolean as to if the solution is correct or not
*/
/******************************************************************************/
bool RummiKub::CheckSolved()
{
  //Check groups
  for (auto const& group : groups_) 
  {
    //Added a long
    unsigned long long size = group.size();

    //If the group is not empty
    if (size > 0) 
    { 
      //If the group is an incorrect size
      if (size < 3 || size > 4) 
      {
        return false;
      }
    }
  }

  //check runs are legal
  for (auto const& run : runs_)
  {
    //If run is not empty
    if (!run.empty())
    {
      //keep track of denominations
      int counts[13] = { 0,0,0,0,0,0,0,0,0,0,0,0,0 };

      //store how many of each denomination
      size_t end = run.size();
      for (unsigned long i = 0; i < end; ++i)
      {
        ++counts[run[i].denomination];
      }
      
      int run_length = 0;
      for (int denomination : counts) 
      {
        //If denomination exists
        if (denomination) 
        {
          ++run_length;
        }
        //Run ended
        else 
        {
          //If there was a run
          if (run_length > 0) 
          { 
            //if run was too small
            if (run_length < 3) 
            {
              return false;
            }
            //reset run length
            run_length = 0;
          }
        }
      }

      //check the length of the last run
      if (counts[12] && run_length < 3) 
      {
        return false;
      }
    }
  }

  return true;
}