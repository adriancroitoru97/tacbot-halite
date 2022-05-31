#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <set>
#include <fstream>
#include <queue>
#include <algorithm>

#include "hlt.hpp"
#include "networking.hpp"

hlt::GameMap presentMap;
unsigned char myID;

/**
 * @brief Computes the direction of the tile with the least strength from
 * current location l
 * 
 * @param l current location
 * @return int - represents a direction
 */

int dirToMinStr(hlt::Location l) {
    std::vector<std::pair<int, int>> result;

    for (int i = 0; i < 4; i++) {
        hlt::Site currSite = presentMap.getSite(l, CARDINALS[i]);
        if (currSite.owner != myID) {
            result.push_back(std::make_pair(currSite.strength, CARDINALS[i]));
        }
    }

    if (result.size() == 0) {
        return -1;
    }
    
    std::sort(result.begin(), result.end(), [=](std::pair<int, int> p1, std::pair<int, int> p2) {
        return p1.first < p2.first;
    });

    return result[0].second;
}

/**
 * @brief Computes the direction of the nearest enemy tile (not neutral)
 * from current location l
 * 
 * @param l current location
 * @return int - represents a direction
 */

int fastestDirToEdge(hlt::Location l) {
    hlt::Move moves[] = {{l , NORTH}, {l, EAST}, {l, SOUTH}, {l, WEST}};

    int dist[] = {0,0,0,0}; // vector with distances to the edge for each direction

    int margins[] = {presentMap.height / 2, presentMap.width / 2};

    for (int i = 0; i < 4; i++) {
        while (presentMap.getSite(moves[i].loc).owner == myID && dist[i] < margins[i / 2]) {
            hlt::Location newL;
            newL = presentMap.getLocation(moves[i].loc, moves[i].dir);
            moves[i].loc = newL;
            dist[i]++;
        }
    }

    int enemyDist[] = {0,0,0,0}; // vector with distances to the enemy from the edge
    int aggression = presentMap.width; // if enemy is between 0 and aggression spaces, it will be attacked

    for (int i = 0; i < 4; i++) {
        if (dist[i] != margins[i / 2]) {
            if (presentMap.getSite(moves[i].loc).owner == 0) {
                while (enemyDist[i] < aggression && presentMap.getSite(moves[i].loc).owner == 0) {
                    hlt::Location newL = presentMap.getLocation(moves[i].loc, moves[i].dir);
                    moves[i].loc = newL;
                    enemyDist[i]++;
                }
            }
        } else {
            enemyDist[i] = aggression;
        }
    }

    // calculate the minimum distance
    int minDist = dist[0] + enemyDist[0];
    int minIdx = 0;
    for (int i = 1; i < 4; i++) {
        if (minDist > dist[i] + enemyDist[i]) {
            minDist = dist[i] + enemyDist[i];
            minIdx = i;
        }
    }
    return moves[minIdx].dir;
}
/**
 * @brief Check whether we can attack the least powerful tile;
 * If we can, make the move, if we can't, stand still
 * 
 * @param a coordinate on Oy
 * @param b coordinate on Ox
 * @param minDir direction to minimum strength tile
 * @param moves move list
 */
void minDirStrategy(unsigned short a, unsigned short b,
                    int minDir, std::set<hlt::Move> &moves)
{
    hlt::Site currSite = presentMap.getSite({b, a});
    int currStr = currSite.strength;

    hlt::Location attackLoc = presentMap.getLocation({b, a}, minDir);
    hlt::Site attackSite = presentMap.getSite(attackLoc);

    if (attackSite.strength < currStr) {
        moves.insert({{b, a}, (unsigned char) minDir});
    } else {
        moves.insert({{b, a}, STILL});
    }
}

/**
 * @brief If strength of current tile is large enough, move in the direction
 * of the enemy, if not, stand still
 * 
 * @param a coordinate on Oy
 * @param b coordinate on Ox
 * @param fastDir direction to nearest enemy
 * @param moves move list
 */
void fastDirStrategy(unsigned short a, unsigned short b,
                     int fastDir, std::set<hlt::Move> &moves)
{
    hlt::Site currSite = presentMap.getSite({b, a});
    int currStr = currSite.strength;
    int currProd = currSite.production;

    if (currStr > currProd * 5) {
        moves.insert({{b, a}, (unsigned char) fastDir});
    } else {
        moves.insert({{b, a}, STILL});
    }
}

int main() {
    srand(time(NULL));

    int northWest[2] = {NORTH, WEST};

    std::cout.sync_with_stdio(0);
    getInit(myID, presentMap);
    sendInit("TACBot");

    while(true) {
        std::set<hlt::Move> moves;

        getFrame(presentMap);

        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                hlt::Site currSite = presentMap.getSite({b, a});
                hlt::Location currLoc = presentMap.getLocation({b, a}, (unsigned char) STILL);
                if (currSite.owner == myID) { // check if its ours
                    int minDir = dirToMinStr({b, a});
                    int fastDir = fastestDirToEdge({b, a});
    
                    if (currSite.strength == 0) {
                        moves.insert({{b, a}, STILL});
                    } else if (minDir != -1) {
                        minDirStrategy(a, b, minDir, moves);
                    } else if (fastDir != -1) {
                        fastDirStrategy(a, b, fastDir, moves);
                    } else { // if we can't use any strategy, we randomly move north or west
                        moves.insert({{b, a}, (unsigned char) northWest[rand() % 2]});
                    }
                }
            }
        }

        sendFrame(moves);
    }

    return 0;
}
