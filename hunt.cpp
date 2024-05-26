//
//  hunt.cpp
//  p1-treasure
// IDENTIFIER  = 40FB54C86566B9DDEAB902CC80E8CE85C1C62AAD
//  Created by Aileen Gonzalez on 5/4/23.
//

#include "hunt.hpp"
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include "xcode_redirect.hpp"

using namespace std;

// Print help for the user when requested.
// argv[0] is the name of the currently executing program
void printHelp() {
    cout << "help button >P<";
}  // printHelp()

bool checkValidTerrain(char mapChar) {
    if (mapChar == '.' || // Water
        mapChar == 'o' || // Land
        mapChar == '#' || // Impassable (land or water)
        mapChar == '@' || // Starting location (always water)
        mapChar == '$' ) { // Treasure location (always land)
        return true;
    } // if
    else {
        return false;
    } // else
} // checkValidTerrain

void TreasureHunt::getMode(int argc, char * argv[]) {
    // These are used with getopt_long()
    // Don't display getopt error messages about options
    opterr = false; // Let us handle all error output for command line options
    int choice;
    int index = 0;
    int path_counter = 0;
    int north_count = 0, east_count = 0, south_count = 0, west_count = 0;
    captain_scheme = "STACK"; // if unspecified, container default is stack
    mate_scheme = "QUEUE"; // if unspecified, container default is queue
    hunt_order = "NESW"; // if unspecified, the default order is: North->East->South->West
    print_verbose = false;
    display_path = false;
    display_stats = false;
    // use getopt to find command line options
    option long_options[] = {{"help", no_argument, nullptr, 'h'},
        {"captain", required_argument, nullptr, 'c'},
        {"first-mate", required_argument, nullptr, 'f'},
        {"hunt-order", required_argument, nullptr, 'o'},
        {"verbose", no_argument, nullptr, 'v'},
        {"stats", no_argument, nullptr, 's'},
        {"show-path", required_argument, nullptr, 'p'},
        { nullptr, 0, nullptr, '\0' }};
    /*
     Options with required_argument (print) need a colon after the
     char, options with no_argument do not (help).
     */
    while ((choice = getopt_long(argc, argv, "hc:vf:so:p:", long_options, &index)) != -1) {
        switch (choice) {
            case 'h':
                printHelp();
                exit(0);

            case 'c':
                captain_scheme = optarg;
                if (captain_scheme != "QUEUE" && captain_scheme != "STACK") {
                    cerr << "Invalid argument to --captain" << endl;  // autograder displays
                    exit(1);
                }
                break;
                
            case 'f':
                mate_scheme = optarg;
                if (mate_scheme != "QUEUE" && mate_scheme != "STACK") {
                    cerr << "Invalid argument to --first-mate" << endl;  // autograder displays
                    exit(1);
                }
                break;
                
                // If Hunt Order option is specified, the argument provided must be four characters long
//                If Hunt Order option is specified, the argument provided must contain one and only one of each of “NESW” (in any order)
                
            case 'o':
                hunt_order = optarg;
                // the order of discovery of adjacent tiles around the current location, a four character string using exactly one of each of the four characters ‘N’, ‘E’, ‘S’, and ‘W’ (if unspecified, the default order is: North->East->South->West)
                // enum?

               if (hunt_order.size() != 4) {
                    cerr << "Invalid argument to --hunt-order" << endl;  // autograder displays
                    exit(1);
                }
               
                for (size_t i = 0; i < hunt_order.size(); i++) {
                    if (hunt_order[i] != 'N' && hunt_order[i] != 'E' && hunt_order[i] != 'S' && hunt_order[i] != 'W' ) {
                        cerr << "Invalid argument to --hunt-order" << endl;  // autograder displays
                        exit(1);
                    } // if
                    switch (hunt_order[i]) {
                        case 'N':
                            north_count++;
                            break;
                            
                        case 'E':
                            east_count++;
                            break;
                            
                        case 'S':
                            south_count++;
                            break;
                            
                        case 'W':
                            west_count++;
                            break;
                            
                        default:
                            break;
                    } // switch
                    if (north_count > 1 || south_count > 1 || east_count > 1 || west_count > 1) {
                        cerr << "Invalid argument to --hunt-order" << endl;  // autograder displays
                        exit(1);
                    } // if
                } // for
                break;
                
            case 'v':
                print_verbose = true;
                break;
                
            case 's':
                display_stats = true;
                break;
                
                // If Show Path option is specified, the argument provided must be ‘M’ or ‘L’
                // The Show Path option can only be specified once
            case 'p':
                if (path_counter != 0) {
                    cerr << "Specify --show-path only once" << endl;  // autograder displays
                    exit(1);
                }
                path_mode = optarg;
                if (path_mode != "M" && path_mode != "L") { // If Show Path option is specified, the argument provided must be ‘M’ or ‘L’
                    cerr << "Invalid argument to --show-path" << endl;  // autograder displays
                    exit(1);
                }
                display_path = true;
                path_counter++;
                break;
                
                // All short or long options not in the spec should result in program termination with a non-zero exit code
            default:
                cerr << "Unknown option" << endl;
                exit(1);
        } // switch
    } // while
} // get_options

void TreasureHunt::read_data() {
//    bool startLocationFound = false;
//    bool treasureLocationFound = false;
    // check for
    // Map does not have treasure
//    Map does not have a start location
//    Invalid coordinates in list mode input
    char character;
    string junk;
    
    while (cin >> character) { // could be comment or could be starting the coordinates
        if (character == '#') {
            getline(cin, junk); // we dont care abt comments so
        } // if character
        else {
            break;
        }
    } // while read in comment
    
    mode_input = character;
    cin >> map_size;
    getline(cin, junk); // get the \n after the map size
            
    // need to resize/reserve/assign map here
    map.assign(map_size, vector<Square>(map_size, {false, '.', 'x'}));

    if (mode_input == 'L') {
        char symbol;
        uint32_t row, col;
        while (cin >> row) {
            cin >> col >> symbol;
            
            if (!checkValidTerrain(symbol)) {
                cerr << "Invalid terrain type" << endl;
                exit(1);
            } // check if valid terrain
            
            map[row][col].terrain_type = symbol;
            
            if (map[row][col].terrain_type == '@') {
                start.row = row;
                start.col = col;
//                startLocationFound = true;
            } // if starting location
            if (map[row][col].terrain_type == '$') {
                treasure.row = row;
                treasure.col = col;
//                treasureLocationFound = true;
            } // if treasure location
        }
    } // if list mode
    
    if (mode_input == 'M') {
        string map_line;
        for (uint32_t row = 0; row < map_size; ++row) {
            getline(cin, map_line);
            for (uint32_t col = 0; col < map_size; ++col) {
                
                if (!checkValidTerrain(map_line[col])) {
                    cerr << "Invalid terrain type" << endl;
                    exit(1);
                } // check if valid terrain
                
                map[row][col].terrain_type = map_line[col];
                
                if (map_line[col] == '@') {
                    start.row = row;
                    start.col = col;
//                    startLocationFound = true;
                } // if starting location
                if (map_line[col] == '$') {
                    treasure.row = row;
                    treasure.col = col;
//                    treasureLocationFound = true;
                } // if treasure location
                
            } // for row
        } // for col
    } // if map mode
} // read_data()

void TreasureHunt::land_search_hunt() {
    times_ashore++;
    if (print_verbose) {
        cout << "Went ashore at: " << land_hunt.back().row << "," << land_hunt.back().col << endl;
    }
    while (!land_hunt.empty()) {
        if (mate_scheme == "STACK") { // stack mode
            land_location = land_hunt.back();
            land_hunt.pop_back();
        } // if first mate stack mode
        else { // queue mode
            land_location = land_hunt.front();
            land_hunt.pop_front();
        } // if first mate in queue mode
        
        for (size_t i = 0; i < hunt_order.size(); i++) {
            if (map[land_location.row][land_location.col].terrain_type == '$') {
                treasure_found = true;
                while (!land_hunt.empty()) {
                    land_hunt.pop_back();
                } // while landhunt search container isnt empty
                break;
            }
            switch (hunt_order[i]) {
                case 'N':
                    if (land_location.row != 0) { // look north + check valid location
                        Location north;
                        north.row = land_location.row - 1;
                        north.col = land_location.col;
                        if ((map[land_location.row - 1][land_location.col].discovered == false) &&
                            (map[land_location.row - 1][land_location.col].terrain_type == 'o')){
                            
                            map[north.row][north.col].direction = 'n';
                            map[north.row][north.col].discovered = true;
                            land_hunt.push_back(north);
                        } // if not discovered and is water (not impassable)
                        
                        else if (map[land_location.row - 1][land_location.col].terrain_type == '$') {
                            treasure_found = true;
                            map[north.row][north.col].direction = 'n';
                            map[north.row][north.col].discovered = true;
                            land_hunt.push_back(north);
                        } // if treasure
                    } // if can look north
                    break;
                    
                case 'E':
                    if (land_location.col < map_size - 1) { // look east + check valid location
                        Location east;
                        east.row = land_location.row;
                        east.col = land_location.col + 1;
                        
                        if ((map[land_location.row][land_location.col + 1].discovered == false) &&
                            (map[land_location.row][land_location.col + 1].terrain_type == 'o')) {
                            
                            map[east.row][east.col].direction = 'e';
                            map[east.row][east.col].discovered = true;
                            land_hunt.push_back(east);

                        } // not discovered + water
                        
                        else if (map[land_location.row][land_location.col + 1].terrain_type == '$') {
                            treasure_found = true;
                            map[east.row][east.col].direction = 'e';
                            map[east.row][east.col].discovered = true;
                            land_hunt.push_back(east);
                        } // if treasure
                        
                    } // if can look east
                    break;
                    
                case 'S':
                    if (land_location.row < map_size - 1) { // looking south
                        Location south;
                        south.row = land_location.row + 1;
                        south.col = land_location.col;
                    
                        if ((map[land_location.row + 1][land_location.col].discovered == false) &&
                            (map[land_location.row + 1][land_location.col].terrain_type == 'o')) {
                            
                            map[south.row][south.col].direction = 's';
                            map[south.row][south.col].discovered = true;
                            land_hunt.push_back(south);
                        } // not discovered + walkable
                        
                        else if (map[land_location.row + 1][land_location.col].terrain_type == '$') {
                            treasure_found = true;
                            map[south.row][south.col].direction = 's';
                            map[south.row][south.col].discovered = true;
                            land_hunt.push_back(south);
                        } // if treasure
                        
                    } // if can look south
                    break;
                    
                case 'W':
                    if (land_location.col != 0) { // look west
                        Location west;
                        west.row = land_location.row;
                        west.col = land_location.col - 1;
                        
                        if ((map[land_location.row][land_location.col - 1].discovered == false) &&
                            (map[land_location.row][land_location.col - 1].terrain_type == 'o')) {
                            
                            map[west.row][west.col].direction = 'w';
                            map[west.row][west.col].discovered = true;
                            land_hunt.push_back(west);
                        } // not discovered + walkable
                        
                        else if (map[land_location.row][land_location.col - 1].terrain_type == '$') {
                            treasure_found = true;
                            map[west.row][west.col].direction = 'w';
                            map[west.row][west.col].discovered = true;
                            land_hunt.push_back(west);
                        } // if treasure
                        
                    } // if can look west
                    break;
                    
                default:
                    break;
            } // switch nesw
            if (treasure_found) {
                land_locations_investigated++;
                while (!land_hunt.empty()) {
                    land_hunt.pop_back();
                } // while landhunt search container isnt empty
                break;
            } // if treasure found
        } // for nesw
        land_locations_investigated++;
    } // while !land_hunt.empty()
    if (treasure_found) {
        if (print_verbose) {
            cout << "Searching island... party found treasure at " << treasure.row << "," << treasure.col << "." << endl;
        } // print verbose
    }
    else {
        if (print_verbose) {
            cout << "Searching island... party returned with no treasure." << endl;
        } // print verbose
    } // if treasure not found
} // land_search_hunt()

void TreasureHunt::sail_search_hunt() {
    map[start.row][start.col].discovered = true;
//    map[start.row][start.col].direction = '@';
    sail_hunt.push_back(start);
    sail_location = start;
    
    if (print_verbose) {
        cout << "Treasure hunt started at: " << start.row << "," << start.col << endl;
    }
    
    while (!sail_hunt.empty()) {
        if (captain_scheme == "STACK") {
            sail_location = sail_hunt.back();
            sail_hunt.pop_back();
        } // if captain stack mode
        else { // meaning queue mode
            sail_location = sail_hunt.front();
            sail_hunt.pop_front();
        } // if captain in queue mode
        // The treasure hunt will be one large hunt (over water) with zero or more smaller hunts (over land).
        for (size_t i = 0; i < hunt_order.size(); i++) {
            switch (hunt_order[i]) {
                case 'N':
                    if (sail_location.row != 0) { // look north + check valid location
                        Location north;
                        north.row = sail_location.row - 1;
                        north.col = sail_location.col;
                        if ((map[north.row][north.col].discovered == false) && ((map[north.row][north.col].terrain_type == 'o') || (map[north.row][north.col].terrain_type == '$'))) {
                            map[north.row][north.col].direction = 'n';
                            map[north.row][north.col].discovered = true;
                            land_hunt.push_back(north);
                            land_search_hunt();
    
                        } // if we find land ,, dont keep water search!, go to land search immediately
                        else if ((map[sail_location.row - 1][sail_location.col].discovered == false) &&
                            (map[sail_location.row - 1][sail_location.col].terrain_type == '.')){
                            
                            map[north.row][north.col].direction = 'n';
                            map[north.row][north.col].discovered = true;
                            sail_hunt.push_back(north);

                        } // if not discovered and is water (not impassable)
                    } // if can look north
                    break;
                    
                case 'E':
                    if (sail_location.col < map_size - 1) { // look east + check valid location
                        Location east;
                        east.row = sail_location.row;
                        east.col = sail_location.col + 1;
                        
                        if ((map[east.row][east.col].discovered == false) && ((map[east.row][east.col].terrain_type == 'o') || (map[east.row][east.col].terrain_type == '$'))) {
                            map[east.row][east.col].direction = 'e';
                            map[east.row][east.col].discovered = true;
                            land_hunt.push_back(east);
                            land_search_hunt();
    
                        } // if we find land ,, dont keep water search!, go to land search immediately
                        
                        else if ((map[sail_location.row][sail_location.col + 1].discovered == false) &&
                            (map[sail_location.row][sail_location.col + 1].terrain_type == '.')) {
                            
                            map[east.row][east.col].direction = 'e';
                            map[east.row][east.col].discovered = true;
                            sail_hunt.push_back(east);

                        } // not discovered + water
                    } // if can look east
                    break;
                    
                case 'S':
                    if (sail_location.row < map_size - 1) { // looking south
                        Location south;
                        south.row = sail_location.row + 1;
                        south.col = sail_location.col;
                        
                        if ((map[south.row][south.col].discovered == false) && ((map[south.row][south.col].terrain_type == 'o') || (map[south.row][south.col].terrain_type == '$'))) {
                            map[south.row][south.col].direction = 's';
                            map[south.row][south.col].discovered = true;
                            land_hunt.push_back(south);
                            land_search_hunt();
    
                        } // if we find land ,, dont keep water search!, go to land search immediately
                        else if ((map[sail_location.row + 1][sail_location.col].discovered == false) &&
                            (map[sail_location.row + 1][sail_location.col].terrain_type == '.')) {
                            
                            map[south.row][south.col].direction = 's';
                            map[south.row][south.col].discovered = true;
                            sail_hunt.push_back(south);

                        } // not discovered + walkable
                    } // if can look south
                    break;
                    
                case 'W':
                    if (sail_location.col != 0) { // look west
                        Location west;
                        west.row = sail_location.row;
                        west.col = sail_location.col - 1;
                        
                        if ((map[west.row][west.col].discovered == false) && ((map[west.row][west.col].terrain_type == 'o') || (map[west.row][west.col].terrain_type == '$'))) {
                            map[west.row][west.col].direction = 'w';
                            map[west.row][west.col].discovered = true;
                            land_hunt.push_back(west);
                            land_search_hunt();
    
                        } // if we find land ,, dont keep water search!, go to land search immediately
                        
                        else if ((map[sail_location.row][sail_location.col - 1].discovered == false) &&
                            (map[sail_location.row][sail_location.col - 1].terrain_type == '.')) {
                            
                            map[west.row][west.col].direction = 'w';
                            map[west.row][west.col].discovered = true;
                            sail_hunt.push_back(west);
                            
                        } // not discovered + walkable
                    } // if can look west
                    break;
                    
                default:
                    break;
            } // switch nesw
            if (treasure_found) {
                while (!sail_hunt.empty()) {
                    sail_hunt.pop_back();
                }
                break;
            } // if treasure_found
        } // for nesw
        water_locations_investigated++;
    } // while sail container not empty
    
    if (print_verbose) {
        if (!treasure_found) {
            cout << "Treasure hunt failed" << endl;
        }
    }
    
    // backtracing for path length
    if (treasure_found) {
        current = treasure;
        char prev_direction = map[treasure.row][treasure.col].direction;
        char prev_terrain = map[treasure.row][treasure.col].terrain_type;
        land_hunt.push_back(treasure);
        while (prev_terrain != '@') {
            if (map[current.row][current.col].direction == 'n' ){
                current.row = current.row + 1;
                prev_terrain = map[current.row][current.col].terrain_type;
                map[current.row][current.col].terrain_type = '|';
            }
            else if (map[current.row][current.col].direction == 'e') {
                current.col = current.col - 1;
                prev_terrain = map[current.row][current.col].terrain_type;
                map[current.row][current.col].terrain_type = '-';
            }
            else if (map[current.row][current.col].direction == 's') {
                current.row = current.row - 1;
                prev_terrain = map[current.row][current.col].terrain_type;
                map[current.row][current.col].terrain_type = '|';
            }
            else if (map[current.row][current.col].direction == 'w') {
                current.col = current.col + 1;
                prev_terrain = map[current.row][current.col].terrain_type;
                map[current.row][current.col].terrain_type = '-';
            }
            
            if ((prev_direction == 'w' || prev_direction == 'e') && ((map[current.row][current.col].direction == 'n') || (map[current.row][current.col].direction == 's'))) {
                map[current.row][current.col].terrain_type = '+';
            }
            else if ((prev_direction == 's' || prev_direction == 'n') && ((map[current.row][current.col].direction == 'e') || (map[current.row][current.col].direction == 'w'))) {
                map[current.row][current.col].terrain_type = '+';
            }
            
            prev_direction = map[current.row][current.col].direction;
            if (prev_terrain == '.') {
                sail_hunt.push_back(current);
                path_length++;
            }
            else if (prev_terrain == 'o') {
                land_hunt.push_back(current);
                path_length++;
            }
        }
        map[start.row][start.col].terrain_type = '@';
        map[treasure.row][treasure.col].terrain_type = 'X';
        sail_hunt.push_back(start);
        path_length++;
    }
    
    if (display_stats) {
        cout << "--- STATS ---" << endl;
        cout << "Starting location: " << start.row << "," << start.col << endl;
        cout << "Water locations investigated: " << water_locations_investigated << endl;
        cout << "Land locations investigated: " << land_locations_investigated << endl;
        cout << "Went ashore: " << times_ashore << endl;
        if (treasure_found) {
            cout << "Path length: " << path_length << endl;
            cout << "Treasure location: " << treasure.row << "," << treasure.col << endl;
        }
        cout << "--- STATS ---" << endl;
    }
    
    if (treasure_found) {
        if (display_path) {
            if (path_mode == "M") { // map mode
                    // mode mode : go from start to hangar and modify whats in the map
                for (uint32_t row = 0; row < map_size; ++row) {
                    for (uint32_t col = 0; col < map_size; ++col) {
                        cout << map[row][col].terrain_type;
                    } // for col
                    cout << endl;
                } // for row
            } // if map mode
            else { // list mode
                    cout << "Sail:" << endl;
                    while (!sail_hunt.empty()) {
                        cout << sail_hunt.back().row << "," << sail_hunt.back().col << endl;
                        sail_hunt.pop_back();
                    } //while !sail_hunt empty
                    cout << "Search:" << endl;
                    while (!land_hunt.empty()) {
                        cout << land_hunt.back().row << "," << land_hunt.back().col << endl;
                        land_hunt.pop_back();
                    } // while !land_hunt empty
            } // else if list mode
        } // display path
        cout << "Treasure found at " << treasure.row << "," << treasure.col << " with path length " << path_length << "." << endl;
    }
    else {
        cout << "No treasure found after investigating " << land_locations_investigated + water_locations_investigated << " locations." << endl;
    }
} // sail_search_hunt()

int main(int argc, char *argv[]) {
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);

    TreasureHunt map;
    
    map.getMode(argc, argv);
    map.read_data();
    map.sail_search_hunt();
    
    return 0;
}
