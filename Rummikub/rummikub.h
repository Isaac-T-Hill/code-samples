/******************************************************************************/
/*!
\file   rummikub.h
\author Isaac Hill
\par    email: Isaac.Hill@digipen.edu
\par    DigiPen login: Isaac.Hill
\par    Course: CS280
\par    Assignment #3
\date   2/6/2018
\brief
This is the Header file for a rummikub solver that figures out if there
is a solution involving all tiles from a given set
*/
/******************************************************************************/

#ifndef RUMMIKUB_H_
#define RUMMIKUB_H_

#include <fstream>
#include <vector>
#include <iostream>

enum Color 
{ 
	kRed,
	kGreen,
	kBlue,
	kYellow
};

struct Tile 
{
    int     denomination;
    Color   color;
};

// Overloaded output for debugging
std::ostream& operator<<(std::ostream& os, Tile const& t)
{
  os << "{ " << t.denomination << ",";
  switch (t.color) {
  case kRed:      os << "R"; break;
  case kGreen:    os << "G"; break;
  case kBlue:     os << "B"; break;
  case kYellow:   os << "Y"; break;
  }
  os << " }";
  return os;
}

class RummiKub {
  public:
    RummiKub(); // empty hand
    void Add( Tile const& , unsigned add = 1); // add a tile to hand
    
    bool Solve(); // solve
    
    // get solution - groups_
    std::vector< std::vector< Tile > > GetGroups() const;
    
    // get solution - runs
    std::vector< std::vector< Tile > > GetRuns() const;
  
  private:
    // Helper Functions
    std::pair<int, std::pair<int, int>> add2group(Tile, int);
    std::pair<int, std::pair<int, int>> add2run(Tile, int);
    std::pair<int, std::pair<int, int>> addgroup(Tile);
    std::pair<int, std::pair<int, int>> addrun(Tile);
    bool CheckSolved();
  
    // Member variables
    std::vector<std::vector<Tile>> groups_final_;
    std::vector<std::vector<Tile>> runs_final_;
    std::vector<std::vector<Tile>> groups_;
    std::vector<std::vector<Tile>> runs_;
    std::vector<Tile> hand_;
    unsigned num_tiles_;
};

#include "rummikub.cpp"

#endif // RUMMIKUB_H_
