//
//  hunt.hpp
//  p1-treasure
// IDENTIFIER  = 40FB54C86566B9DDEAB902CC80E8CE85C1C62AAD
//  Created by Aileen Gonzalez on 5/4/23.
//

#ifndef hunt_hpp
#define hunt_hpp

#include <stdio.h>

#endif /* hunt_hpp */

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include "xcode_redirect.hpp"

using namespace std;

class TreasureHunt {
private:
    struct Square {
        bool discovered;
        char terrain_type;
        char direction;
    };
    struct Location {
        uint32_t row;
        uint32_t col;
    };
    string captain_scheme;
    string mate_scheme;
    string hunt_order;
    bool print_verbose = 0;
    bool display_stats = 0;
    bool display_path = 0;
    string path_mode;
    
    vector<vector<Square>> map;
    // sail and search container
    deque<Location> sail_hunt;
    deque<Location> land_hunt;
    Location start;
    Location treasure;

    Location land_location;
    Location sail_location;
    Location current;

    bool treasure_found = 0;
    
    int water_locations_investigated = 0;
    int land_locations_investigated = 0;
    int times_ashore = 0;
    int path_length = 0;
    
    char mode_input;
    uint32_t map_size;
public:
    void getMode(int argc, char * argv[]);
    void read_data();
    void sail_search_hunt();
    void land_search_hunt();
};
