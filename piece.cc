#include "piece.h"

Piece::Piece(int points, char type, bool colour) : 
    points(points), type(type), colour(colour) {}

int Piece::getPoints() {
    return this->points;
}

char Piece::getType(){
    return this->type;
}

bool Piece::getColour(){
    return this->colour;
}
