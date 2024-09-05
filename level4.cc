#include <memory>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>
#include "chessstrategy.h"
#include "level4.h"


using namespace std;

double absForDouble(double x) {return (x < 0)? -x : x;}

Level4::Level4(ChessModule *cm) : ChessStrategy(cm) {}

// Level 4 can checkmate you, avoid bad trades, and has a bias towards the middle
int Level4::findBestMove(default_random_engine rng, bool colour, std::vector<std::pair<std::string, std::pair<std::pair<int, int>, std::pair<int, int> > > >&all_legal_moves, std::vector<std::vector<std::shared_ptr<Piece> > >&theboard) {
    shuffle( all_legal_moves.begin(), all_legal_moves.end(), rng );
    vector<pair<int, int> > candidates_w_scores(all_legal_moves.size());
    int score;
    bool checkmate = false;
    bool ill_get_captured = false;
for (int i = 0; i < all_legal_moves.size(); ++i) {
    ill_get_captured = false;
    score = 0;
    int fromRow = all_legal_moves.at(i).second.first.first;
    int fromCol = all_legal_moves.at(i).second.first.second;
    int toRow = all_legal_moves.at(i).second.second.first;
    int toCol = all_legal_moves.at(i).second.second.second;
    string move_type = all_legal_moves.at(i).first;
    
    bool capturing = false;
    bool enpassanting = false;
    shared_ptr<Piece> temp;
    if (theboard.at(toRow).at(toCol).get() != nullptr) { // There's a piece where we're going
        score += theboard.at(toRow).at(toCol).get()->getPoints(); // because it's level 4
        capturing = true;
        temp = move(theboard.at(toRow).at(toCol));
    } else if (move_type == "pawncapture_black" || move_type == "pawncapture_white") { // En_passant
        score += theboard.at(toRow).at(toCol).get()->getPoints(); // because it's level 4
        enpassanting = true; // Legal pawn capture with no piece at "to" means it must be en-passant
        temp = move(theboard.at(fromRow).at(toCol));
    }
    if (abs(toRow - 3.5) < abs(fromRow - 3.5)) ++score;
    if (abs(toCol - 3.5) < abs(fromCol - 3.5)) ++score;
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
        if (cm->inCheck(!colour)) ++score; // check is not bad
        // now loop through all their possible moves
        // for each move, check if that move can attack the newly-moved piece
        vector<pair<string, pair<pair<int, int>, pair<int, int> > > > their_moves = cm->allLegalMoves(!colour, true);
        //if (cm->isCheckmate(!colour)) checkmate = true; // if it's checkmate, do the checkmate 
        if (their_moves.size() == 0) checkmate = true;

        for (int j = 0; j < their_moves.size(); ++j) {
            if (!ill_get_captured && their_moves.at(j).second.second.first == toRow && their_moves.at(j).second.second.second == toCol) {
                ill_get_captured = true;
            }
        }
        if (ill_get_captured) score -= theboard.at(toRow).at(toCol).get()->getPoints() + 1;

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

        candidates_w_scores.push_back(make_pair(score, i));
        if (checkmate) return i;
    }
    sort(candidates_w_scores.rbegin(), candidates_w_scores.rend());
    return candidates_w_scores.at(0).second; // returns move index with highest score
}
