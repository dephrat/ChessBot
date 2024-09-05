#include <memory>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>
#include "chessstrategy.h"
#include "level2.h"


using namespace std;

Level2::Level2(ChessModule *cm) : ChessStrategy(cm) {}

int Level2::findBestMove(default_random_engine rng, bool colour, std::vector<std::pair<std::string, std::pair<std::pair<int, int>, std::pair<int, int> > > >&all_legal_moves, std::vector<std::vector<std::shared_ptr<Piece> > >&theboard) {
    shuffle( all_legal_moves.begin(), all_legal_moves.end(), rng );
for (int i = 0; i < all_legal_moves.size(); ++i) {
    int fromRow = all_legal_moves.at(i).second.first.first;
    int fromCol = all_legal_moves.at(i).second.first.second;
    int toRow = all_legal_moves.at(i).second.second.first;
    int toCol = all_legal_moves.at(i).second.second.second;
    string move_type = all_legal_moves.at(i).first;
    
    bool capturing = false;
    bool enpassanting = false;
    shared_ptr<Piece> temp;
    if (theboard.at(toRow).at(toCol).get() != nullptr) { // There's a piece where we're going
        return i; // because it's level 2
    } else if (move_type == "pawncapture_black" || move_type == "pawncapture_white") { // En_passant
        return i; // because it's level 2
    }
    /* Move the piece */
    theboard.at(toRow).at(toCol) = move(theboard.at(fromRow).at(fromCol)); 
    /* -------------- */

    pair<int, int> enpassant_square = cm->getEnPassantSquare();
        int temp1 = enpassant_square.first;
        int temp2 = enpassant_square.second;
        if (move_type == "pawn2_black") {
            enpassant_square.first = toRow + 1;
            enpassant_square.second = toCol;
        } else if (move_type == "pawn2_white") {
            enpassant_square.first = toRow - 1;
            enpassant_square.second = toCol;
        } else {
            enpassant_square.first = -1;
            enpassant_square.second = -1;
            if (move_type == "castleright") {
                theboard.at(fromRow).at(fromCol + 1) = move(theboard.at(fromRow).at(fromCol + 3));
            } else if (move_type == "castleleft") {
                theboard.at(fromRow).at(fromCol - 1) = move(theboard.at(fromRow).at(fromCol - 4));
            }
        }

        // check for checkmate and stuff
        bool in_check = cm->inCheck(!colour);

   //undo the move
        cm->setEnPassantSquare(temp1, temp2);
        theboard.at(fromRow).at(fromCol) = move(theboard.at(toRow).at(toCol));
        if (capturing) {
            theboard.at(toRow).at(toCol) = move(temp);
        } else if (enpassanting) {
            theboard.at(fromRow).at(toCol) = move(temp);
        } else if (move_type == "castleright") {
            theboard.at(fromRow).at(fromCol + 3) = move(theboard.at(fromRow).at(fromCol + 1));
        } else if (move_type == "castleleft") {
            theboard.at(fromRow).at(fromCol - 4) = move(theboard.at(fromRow).at(fromCol - 1));
        }
        if (in_check) return i;
    }
        return 0; 
}
