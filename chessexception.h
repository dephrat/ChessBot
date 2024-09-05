#ifndef CHESSEXCEPTION_H
#define CHESSEXCEPTION_H

class ChessException{
    // Note: (Current setup)
    // Each throw should only produce one output.
    //
    public:
    // add your attribute ...
    bool continue_your_game_phrase = false;
    bool illegal_move = false;
    bool network = false;
    bool game_is_not_on = false;
    bool setup = false;


    //
    // printing function
    void print_exception();
};

#endif
