#include <iostream>
#include <vector>
#include <sstream>

#include "display.h"

using namespace std;

/*
8  _ _ _ _
7 _ _ _ _ 
6  _ _ _ _
5 _ _ _ _
4  _ _ _ _
3 _ _ _ _
2  _ _ _ _
1 _ _ _ _


(7,0) (7,1) (7,2) (7,3) (7,4) (7,5) (7,6) (7,7)
(6,0) (6,1) (6,2) (6,3) (6.4) (6,5) (6,6) (6,7)
(5,0) (5,1) (5,2) (5,3) (5,4) (5,5) (5,6) (5,7)
(4,0) (4,1) (4,2) (4,3) (4,4) (4,5) (4,6) (4,7)
(3,0) (3,1) (3,2) (3,3) (3.4) (3,5) (3,6) (3,7)
(2,0) (2,1) (2,2) (2,3) (2,4) (2,5) (2,6) (2,7)
(1,0) (1,1) (1,2) (1,3) (1,4) (1,5) (1,6) (1,7)
(0,0) (0,1) (0,2) (0,3) (0.4) (0,5) (0,6) (0,7)
*/

TextDisplay::TextDisplay(int n) : gridSize{n} {
    theDisplay = vector<vector<char> >(8, vector<char>(8, ' '));
    auto temp = vector<vector<shared_ptr<Piece> > >(8, vector<shared_ptr<Piece> >(8, nullptr));
    setBoard(temp);
}

void TextDisplay::notify(std::vector<std::vector<std::shared_ptr<Piece> > > &theboard){
    setBoard(theboard);
    cout << *this << endl;
}

void TextDisplay::setBoard(std::vector<std::vector<std::shared_ptr<Piece> > > &theboard) {
    for (int i = 0; i < gridSize; i++){
        for (int j = 0; j < gridSize; j++){
            // check if the square if empty or not ' '
            if (theboard.at(i).at(j) == nullptr){
                if ((i + j) % 2 == 0) {
                    theDisplay.at(i).at(j) = '-';
                } else {
                    theDisplay.at(i).at(j) = ' ';
                }
            } else {
                if (theboard.at(i).at(j).get()->getColour() == false){ // White pieces
                    theDisplay.at(i).at(j) = theboard.at(i).at(j).get()->getType();
                } else { // Black pieces
                    theDisplay.at(i).at(j) = theboard.at(i).at(j).get()->getType() + ('a' - 'A');
                }
            }
        }
    }
}

ostream &operator<<(std::ostream &out, const TextDisplay &d){
    int size = d.gridSize;
    for(int i = size - 1; i >= 0; i--){
        out << i + 1 << " ";
        for(int j = 0; j < size; j++){
            out << d.theDisplay[i][j];
        } out << endl;
    }
    out << endl << "  ";
    for (int i = 0; i < size; ++i) {
        out << char('a' + i);
    }
    out << endl;
    return out;
}
