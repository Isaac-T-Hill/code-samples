/******************************************************************************/
/*!
\file   water.h
\author Isaac Hill
\par    email: Isaac.Hill@digipen.edu
\par    DigiPen login: Isaac.Hill
\par    Course: CS280
\par    Assignment #6
\date   2/6/2018
\brief
This is the Header file for a 2D water retention problem solver
*/
/******************************************************************************/

#ifndef WATER_H_
#define WATER_H_

#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm>
#include <string>
#include <vector>
#include <queue>

// Function called by driver
long int waterret( char const* filename );

class Column
{
public:
  Column(long height, bool edge) : height_(height), cutoff_(height), waterlevel_(0), edge_(edge)
  {
    if(!edge)
    {
      cutoff_ = std::numeric_limits<long>::max();
      waterlevel_ = std::numeric_limits<long>::max();
    }
  };

  ~Column() {};

  long getHeight() { return height_; };
  long isEdge() { return edge_; };
  long getHeld() { return waterlevel_; };
  bool Update(Column* col);

private:
  long height_;
  long cutoff_;
  long waterlevel_;
  bool edge_;
};

class World
{
public:
  World(char const* filename);
  ~World();

  long getHeight() const { return height_; };
  long getWidth()  const { return width_; };
  void edgeUpdate();
  void Update();
  void checkNeighbor(Column* col, std::pair<long,long> coord);
  long getHeld();

  std::vector<std::vector<Column*>> grid_;

private:
  long height_;
  long width_;

  std::queue<std::pair<long, long>> updateQueue_;
};

#endif // WATER_H_
