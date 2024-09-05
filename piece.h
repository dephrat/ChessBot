#ifndef PIECE_H
#define PIECE_H

class Piece {
    int points;
    /*const*/ char type; // 'K' King, 'Q' Queen, 'B' Bishop, 'R' Rook, 'N' Knight, 'P' Pawn,
    /*const*/ bool colour; // (true 'black') or (false 'white')


    public:
    // constructor
    Piece(int points, char type, bool colour);
    
    // get methods
    int getPoints();
    char getType();
    bool getColour();
    
};

#endif
