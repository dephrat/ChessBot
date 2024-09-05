#include <iostream>
#include "chessexception.h"
using namespace std;

void ChessException::print_exception(){
    if (this->continue_your_game_phrase){
        cout << "Countinuing your game..." << endl;
        this->continue_your_game_phrase = false; // After print, toggle this back to original phase.
    }
    else if (this->illegal_move) {
        cout << "Illegal move!" << endl;
        this->illegal_move = false; // After print, toggle this back to original phase.
    }
    else if (this->network){
        cout << "Unknown network issues" << endl;
        this->network = false; // After print, toggle this back to original phase.
    }
    else if (this->game_is_not_on){
        cout << "Currently, the game is not on" << endl;
        this->game_is_not_on = false;
    }
    else if(this->setup){
        cout << "Setup mode error, please make sure all conditions are satisfied." << endl;
        this->setup = false;
    }
    

    // else if ()
}
