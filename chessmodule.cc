#include <random>
#include "chessmodule.h"
using namespace std;

// at some point, after organizing the methods/functions in chessmodule.h in some logical order, 
// organize them here based on the order they appear in chessmodule.h

int abs(int x) {
    return x < 0 ? x * -1 : x;
}

// returns pair<col, row>
pair<int, int> posToCoord(string pos) {
    if (pos.length() != 2 || pos[0] < 'a' || pos[0] > 'h' || pos[1] < '1' || pos[1] > '8') throw ChessException(); //Invalid input
    pair<int, int> retPair = make_pair(pos[0] - 'a', pos[1] - '1');
    return retPair;
}

void ChessModule::switchWhoseMove() { whose_move = !whose_move; };

bool ChessModule::inCheck(bool who_is_attacked) const {
    vector<pair<char, pair<int, int> > > king_pos = findPiece(who_is_attacked, false, 'K'); // Where is the king?
    if (king_pos.size() == 0) throw ChessException(); // This player has no king!
    return isSquareAttacked(who_is_attacked, king_pos.at(0).second.first, king_pos.at(0).second.second);
}

//should be strongly exception-safe, assumes the move is legal.
// note that we don't need to check if moving the castling rook places the king in check, since it's assumed castling is legal here
// and there's no way for the rook to block the king from check or place the king into check by castling
// similarly, pawn promotion doesn't affect "placing the king in check" any differently from normal pawn moves/captures.
void ChessModule::tryMove(bool make_move, bool who_is_moving, int fromRow, int fromCol, int toRow, int toCol, string move_type) {
    bool capturing = false;
    bool enpassanting = false;
    shared_ptr<Piece> temp;
    if (theboard.at(toRow).at(toCol).get() != nullptr) { // There's a piece where we're going
        capturing = true;
        temp = move(theboard.at(toRow).at(toCol));
    } else if (move_type == "pawncapture_black" || move_type == "pawncapture_white") { // En_passant
        enpassanting = true; // Legal pawn capture with no piece at "to" means it must be en-passant
        temp = move(theboard.at(fromRow).at(toCol));
    }
    /* Move the piece */
    theboard.at(toRow).at(toCol) = move(theboard.at(fromRow).at(fromCol)); 
    /* -------------- */

    // these next lines might be simplifiable once we implement undo_move()

    bool in_check = false;
    try {
        in_check = inCheck(who_is_moving); // Are we in check now?
    } catch (ChessException ce) { // This player has no king! Technically this should never happen here.
        // First undo the move, then re-throw the exception
        theboard.at(fromRow).at(fromCol) = move(theboard.at(toRow).at(toCol));
        if (capturing) {
            theboard.at(toRow).at(toCol) = move(temp);
        } else if (enpassanting) {
            theboard.at(fromRow).at(toCol) = move(temp);
        }
        throw ce; // This player has no king! 
    }

    if (in_check) { // If we're now in check, undo the move and throw an exception
        theboard.at(fromRow).at(fromCol) = move(theboard.at(toRow).at(toCol));
        if (capturing) {
            theboard.at(toRow).at(toCol) = move(temp);
        } else if (enpassanting) {
            theboard.at(fromRow).at(toCol) = move(temp);
        }
        throw ChessException(); // Illegal move, places own king in check
    }

    if (make_move == false) { // if we don't want to actually make the move, undo the move
        theboard.at(fromRow).at(fromCol) = move(theboard.at(toRow).at(toCol));
        if (capturing) {
            theboard.at(toRow).at(toCol) = move(temp);
        } else if (enpassanting) {
            theboard.at(fromRow).at(toCol) = move(temp);
        }
    }
}

//looked it up, there's really no faster way than checking each piece
vector<pair<string, pair<pair<int, int>, pair<int, int> > > > ChessModule::allLegalMoves(bool which_player, bool find_all) {    
    vector<pair<string, pair<pair<int, int>, pair<int, int> > > > allMoves(0, make_pair("", make_pair(make_pair(-1, -1), make_pair(-1, -1))));
    vector<pair<char, pair<int, int>>> all_pieces = findPiece(which_player, true, ' ');
    for (auto piece : all_pieces) {
        int fromRow = piece.second.first;
        int fromCol = piece.second.second;
        if (piece.first == 'N') { // 8 possibilities
            for (int i = -2; i < 3; ++i) {
                if (i == 0) continue;
                if (!isNormalMoveRestricted(which_player, fromRow, fromCol, fromRow + i, fromCol + 3 - abs(i), "knight")) {
                    try {
                        tryMove(false, which_player, fromRow, fromCol, fromRow + i, fromCol + 3 - abs(i), "knight");
                        allMoves.push_back(make_pair("knight", make_pair(make_pair(fromRow, fromCol), make_pair(fromRow + i, fromCol + 3 - abs(i)))));
                        if(find_all == false) break;
                    } catch (ChessException) {// just means we can't add the move to the list
                    }  
                }
                if (!isNormalMoveRestricted(which_player, fromRow, fromCol, fromRow + i, fromCol + abs(i) - 3, "knight")) {
                    try {
                        tryMove(false, which_player, fromRow, fromCol, fromRow + i, fromCol + abs(i) - 3, "knight");
                        allMoves.push_back(make_pair("knight", make_pair(make_pair(fromRow, fromCol), make_pair(fromRow + i, fromCol + abs(i) - 3))));
                        if(find_all == false) break;
                    } catch (ChessException) {// just means we can't add the move to the list
                    }
                }
            }
        } else if (piece.first == 'B') {
            for (int r = 0; r < 8; ++r) {
                if (r == fromRow) continue;
                if (fromCol - (r - fromRow) >= 0 && fromCol - (r - fromRow) < 8 &&
                !isNormalMoveRestricted(which_player, fromRow, fromCol, r, fromCol - (r - fromRow), "bishop")) {   
                    try {
                        tryMove(false, which_player, fromRow, fromCol, r, fromCol - (r - fromRow), "bishop");
                        allMoves.push_back(make_pair("bishop", make_pair(make_pair(fromRow, fromCol), make_pair(r, fromCol - (r - fromRow)))));
                        if(find_all == false) break;
                    } catch (ChessException) {// just means we can't add the move to the list
                    }                   
                }
                if (fromCol + (r - fromRow) < 8 && fromCol + (r - fromRow) >= 0 &&
                !isNormalMoveRestricted(which_player, fromRow, fromCol, r, fromCol + (r - fromRow), "bishop")) {
                    try {
                        tryMove(false, which_player, fromRow, fromCol, r, fromCol + (r - fromRow), "bishop");
                        allMoves.push_back(make_pair("bishop", make_pair(make_pair(fromRow, fromCol), make_pair(r, fromCol + (r - fromRow)))));
                        if(find_all == false) break;
                    } catch (ChessException) {// just means we can't add the move to the list
                    }  
                }
            }
        } else if (piece.first == 'R') {
            for (int r = 0; r < 8; ++r) {
                // check vertical move, i.e. same column, different row
                if (r != fromRow && !isNormalMoveRestricted(which_player, fromRow, fromCol, r, fromCol, "rook")) {
                    try {
                        tryMove(false, which_player, fromRow, fromCol, r, fromCol, "rook");
                        allMoves.push_back(make_pair("rook", make_pair(make_pair(fromRow, fromCol), make_pair(r, fromCol))));
                        if(find_all == false) break;
                    } catch (ChessException) {// just means we can't add the move to the list
                    }
                }
                // check horizontal move, i.e. same row, different column
                if (r != fromCol && !isNormalMoveRestricted(which_player, fromRow, fromCol, fromRow, r, "rook")) {
                    try {
                        tryMove(false, which_player, fromRow, fromCol, fromRow, r, "rook");
                        allMoves.push_back(make_pair("rook", make_pair(make_pair(fromRow, fromCol), make_pair(fromRow, r))));
                        if(find_all == false) break;
                    } catch (ChessException) {// just means we can't add the move to the list
                    }
                }
            }
        } else if (piece.first == 'Q') {
            for (int r = 0; r < 8; ++r) {
                // first check bishop moves
                if (r != fromRow && fromCol - (r - fromRow) >= 0 && fromCol - (r - fromRow) < 8 &&
                !isNormalMoveRestricted(which_player, fromRow, fromCol, r, fromCol - (r - fromRow), "queen")) {   
                    try {
                        tryMove(false, which_player, fromRow, fromCol, r, fromCol - (r - fromRow), "queen");
                        allMoves.push_back(make_pair("queen", make_pair(make_pair(fromRow, fromCol), make_pair(r, fromCol - (r - fromRow)))));
                        if(find_all == false) break;
                    } catch (ChessException) {// just means we can't add the move to the list
                    }                   
                }
                if (r != fromRow && fromCol + (r - fromRow) < 8 && fromCol + (r - fromRow) >= 0 &&
                !isNormalMoveRestricted(which_player, fromRow, fromCol, r, fromCol + (r - fromRow), "queen")) {
                    try {
                        tryMove(false, which_player, fromRow, fromCol, r, fromCol + (r - fromRow), "queen");
                        allMoves.push_back(make_pair("queen", make_pair(make_pair(fromRow, fromCol), make_pair(r, fromCol + (r - fromRow)))));
                        if(find_all == false) break;
                    } catch (ChessException) {// just means we can't add the move to the list
                    }  
                }
                // second check rook moves
                if (r != fromRow && !isNormalMoveRestricted(which_player, fromRow, fromCol, r, fromCol, "queen")) {
                    try {
                        tryMove(false, which_player, fromRow, fromCol, r, fromCol, "queen");
                        allMoves.push_back(make_pair("queen", make_pair(make_pair(fromRow, fromCol), make_pair(r, fromCol))));
                        if(find_all == false) break;
                    } catch (ChessException) {// just means we can't add the move to the list
                    }
                }
                if (r != fromCol && !isNormalMoveRestricted(which_player, fromRow, fromCol, fromRow, r, "queen")) {
                    try {
                        tryMove(false, which_player, fromRow, fromCol, fromRow, r, "queen");
                        allMoves.push_back(make_pair("queen", make_pair(make_pair(fromRow, fromCol), make_pair(fromRow, r))));
                        if(find_all == false) break;
                    } catch (ChessException) {// just means we can't add the move to the list
                    }
                }
            }
        } else if (piece.first == 'K') {
            for (int i = -1; i < 2; ++i) {
                for (int j = -1; j < 2; ++j) {
                    if ((j != 0 || i != 0) && !isNormalMoveRestricted(which_player, fromRow, fromCol, fromRow + i, fromCol + j, "king")) {
                        try {
                            tryMove(false, which_player, fromRow, fromCol, fromRow + i, fromCol + j, "king");
                            allMoves.push_back(make_pair("king", make_pair(make_pair(fromRow, fromCol), make_pair(fromRow + i, fromCol + j))));
                            if(find_all == false) break;
                        } catch (ChessException) {// just means we can't add the move to the list
                        }
                    }
                } 
            }
            if (!isCastlingMoveRestricted(which_player, fromRow, fromCol, fromRow, fromCol + 2, "castleright")) {
                try {
                    tryMove(false, which_player, fromRow, fromCol, fromRow, fromCol + 2, "castleright");
                    allMoves.push_back(make_pair("castleright", make_pair(make_pair(fromRow, fromCol), make_pair(fromRow, fromCol + 2))));
                    if(find_all == false) break; // this should never break, since it would've broken earlier
                } catch (ChessException) {// just means we can't add the move to the list
                }        
            }
            if (!isCastlingMoveRestricted(which_player, fromRow, fromCol, fromRow, fromCol - 2, "castleleft")) {
                try {
                    tryMove(false, which_player, fromRow, fromCol, fromRow, fromCol - 2, "castleleft");
                    allMoves.push_back(make_pair("castleleft", make_pair(make_pair(fromRow, fromCol), make_pair(fromRow, fromCol - 2))));
                    if(find_all == false) break; // this should never break, since it would've broken earlier
                } catch (ChessException) {// just means we can't add the move to the list
                }        
            }  
        } else if (piece.first == 'P') {
            // check pawn_, pawn2_, and pawncapture_ (right or left)
            if (which_player) { // black pieces
                if (!isNormalMoveRestricted(which_player, fromRow, fromCol, fromRow - 1, fromCol, "pawn_black")) {
                    try {
                        tryMove(false, which_player, fromRow, fromCol, fromRow - 1, fromCol, "pawn_black");
                        allMoves.push_back(make_pair("pawn_black", make_pair(make_pair(fromRow, fromCol), make_pair(fromRow - 1, fromCol))));
                        if(find_all == false) break;
                    } catch (ChessException) {// just means we can't add the move to the list
                    }  
                }
                if (!isNormalMoveRestricted(which_player, fromRow, fromCol, fromRow - 2, fromCol, "pawn2_black")) {
                    try {
                        tryMove(false, which_player, fromRow, fromCol, fromRow - 2, fromCol, "pawn2_black");
                        allMoves.push_back(make_pair("pawn2_black", make_pair(make_pair(fromRow, fromCol), make_pair(fromRow - 2, fromCol))));
                        if(find_all == false) break;  // this should never break, since it would've broken earlier
                    } catch (ChessException) {// just means we can't add the move to the list
                    }  
                }
                if (!isNormalMoveRestricted(which_player, fromRow, fromCol, fromRow - 1, fromCol - 1, "pawncapture_black")) { //capture left
                    try {
                        tryMove(false, which_player, fromRow, fromCol, fromRow - 1, fromCol - 1, "pawncapture_black");
                        allMoves.push_back(make_pair("pawncapture_black", make_pair(make_pair(fromRow, fromCol), make_pair(fromRow - 1, fromCol - 1))));
                        if(find_all == false) break; 
                    } catch (ChessException) {// just means we can't add the move to the list
                    }  
                }
                if (!isNormalMoveRestricted(which_player, fromRow, fromCol, fromRow - 1, fromCol + 1, "pawncapture_black")) { //capture right
                    try {
                        tryMove(false, which_player, fromRow, fromCol, fromRow - 1, fromCol + 1, "pawncapture_black");
                        allMoves.push_back(make_pair("pawncapture_black", make_pair(make_pair(fromRow, fromCol), make_pair(fromRow - 1, fromCol + 1))));
                        if(find_all == false) break;
                    } catch (ChessException) {// just means we can't add the move to the list
                    }  
                }
            } else { // white pieces
                if (!isNormalMoveRestricted(which_player, fromRow, fromCol, fromRow + 1, fromCol, "pawn_white")) {
                    try {
                        tryMove(false, which_player, fromRow, fromCol, fromRow + 1, fromCol, "pawn_white");
                        allMoves.push_back(make_pair("pawn_white", make_pair(make_pair(fromRow, fromCol), make_pair(fromRow + 1, fromCol))));
                        if(find_all == false) break;
                    } catch (ChessException) {// just means we can't add the move to the list
                    }  
                }
                if (!isNormalMoveRestricted(which_player, fromRow, fromCol, fromRow + 2, fromCol, "pawn2_white")) {
                    try {
                        tryMove(false, which_player, fromRow, fromCol, fromRow + 2, fromCol, "pawn2_white");
                        allMoves.push_back(make_pair("pawn2_white", make_pair(make_pair(fromRow, fromCol), make_pair(fromRow + 2, fromCol))));
                        if(find_all == false) break; // this should never break, since it would've broken earlier
                    } catch (ChessException) {// just means we can't add the move to the list
                    }  
                }
                if (!isNormalMoveRestricted(which_player, fromRow, fromCol, fromRow + 1, fromCol - 1, "pawncapture_white")) { //capture left
                    try {
                        tryMove(false, which_player, fromRow, fromCol, fromRow + 1, fromCol - 1, "pawncapture_white");
                        allMoves.push_back(make_pair("pawncapture_white", make_pair(make_pair(fromRow, fromCol), make_pair(fromRow + 1, fromCol - 1))));
                        if(find_all == false) break;
                    } catch (ChessException) {// just means we can't add the move to the list
                    }  
                }
                if (!isNormalMoveRestricted(which_player, fromRow, fromCol, fromRow + 1, fromCol + 1, "pawncapture_white")) { //capture right
                    try {
                        tryMove(false, which_player, fromRow, fromCol, fromRow + 1, fromCol + 1, "pawncapture_white");
                        allMoves.push_back(make_pair("pawncapture_white", make_pair(make_pair(fromRow, fromCol), make_pair(fromRow + 1, fromCol + 1))));
                        if(find_all == false) break;
                    } catch (ChessException) {// just means we can't add the move to the list
                    }  
                }
            }
        }
    }
    return allMoves;
}

bool ChessModule::isCheckmate(bool who_is_attacked) {
    bool in_check = false;
    try {
        in_check = inCheck(who_is_attacked);
    } catch (ChessException ce) { // This player has no king!
        throw ce;
    }
    if (in_check) {
        vector<pair<string, pair<pair<int, int>, pair<int, int> > > > all_legal_moves = allLegalMoves(who_is_attacked, true);
        return all_legal_moves.size();
    } else return false;
}
bool ChessModule::isStalemate(bool who_is_attacked) {
    bool in_check = false;
    try {
        in_check = inCheck(who_is_attacked);
    } catch (ChessException ce) { // This player has no king!
        throw ce;
    }
    if (in_check) return false;
    else {
        vector<pair<string, pair<pair<int, int>, pair<int, int> > > > all_legal_moves = allLegalMoves(who_is_attacked, true);
        return all_legal_moves.size();
    } 
}

ChessModule::ChessModule() : whose_move(false), winner("ongoing") {
    theboard = vector<vector<shared_ptr<Piece> > >(8, vector<shared_ptr<Piece> >(8, nullptr));
}

// compiler yells if I use {}
ChessModule::ChessModule(bool whose_move) : whose_move(whose_move), winner("ongoing") { 
    theboard = vector<vector<shared_ptr<Piece> > >(8, vector<shared_ptr<Piece> >(8, nullptr));
};

ChessModule::ChessModule(unsigned seed, bool whose_move) : whose_move(whose_move), seed(seed), rng(seed), winner("ongoing") {
    theboard = vector<vector<shared_ptr<Piece> > >(8, vector<shared_ptr<Piece> >(8, nullptr));
}

ChessModule::~ChessModule() {}

default_random_engine ChessModule::getRNG() { return rng; };

vector<vector<shared_ptr<Piece> > > &ChessModule::getBoard() { return theboard; }

void ChessModule::setSeed(unsigned _seed) { seed = _seed; }

void ChessModule::poprecordmove(){
    this->last_move.pop_back();
    this->last_move.pop_back();
}
void ChessModule::setrecordmove(std::string from,std::string to){
    this->last_move.emplace_back(from);
    this->last_move.emplace_back(to);
}

bool ChessModule::getSetupConditionsBool(){
    return this->setup_mode_conditions_passed;
}

std::vector<std::string> ChessModule::getrecordmove(){
    return this->last_move;
}

int ChessModule::getwhitescore(){
    return this->white_score;
}

int ChessModule::getblackscore(){
    return this->black_score;
}

void ChessModule::addscore(bool colour){
    if(colour){
        this->black_score++;
        return;
    }
    this->white_score++;
    return;
}

void ChessModule::addhalf(){
    this->black_score = this->black_score + 0.5;
    this->white_score = this->white_score + 0.5;
    return;
}

void ChessModule::print_score(){
    cout << "Final Score:" << endl;
    cout << "White: " << this->white_score << endl;
    cout << "Black: " << this->black_score << endl;
}

void ChessModule::notifyObservers() {
    for (auto ob : observers) ob.get()->notify(getBoard());
}

void ChessModule::setWhiteComp(shared_ptr<ChessStrategy> compw) {white_computer = compw;}
void ChessModule::setBlackComp(shared_ptr<ChessStrategy> compb) {black_computer = compb;}

void ChessModule::clear_board(){
    theboard = vector<vector<shared_ptr<Piece>>>(8, vector<shared_ptr<Piece>>(8, nullptr));
}

void ChessModule::setShowGraphicsDisplay(bool show) { graphics_display_on = show;}

void ChessModule::init(string white_player, string black_player) { 
    clear_board();
    seed = std::chrono::system_clock::now().time_since_epoch().count();
    last_move.clear();
    winner = "ongoing";
    black_score = 0;
    white_score = 0;
    //White pieces:
    theboard.at(0).at(0) = make_shared<Piece>(5, 'R', false);
    theboard.at(0).at(7) = make_shared<Piece>(5, 'R', false);
    theboard.at(0).at(1) = make_shared<Piece>(3, 'N', false);
    theboard.at(0).at(6) = make_shared<Piece>(3, 'N', false);
    theboard.at(0).at(2) = make_shared<Piece>(3, 'B', false);
    theboard.at(0).at(5) = make_shared<Piece>(3, 'B', false);
    theboard.at(0).at(3) = make_shared<Piece>(9, 'Q', false);
    theboard.at(0).at(4) = make_shared<Piece>(1000, 'K', false);

    for (int i = 0; i < 8; ++i) {
        theboard.at(1).at(i) = make_shared<Piece>(1, 'P', false);
    }

    //Black pieces:
    //Note that these are uppercase intentionally, they get converted when being displayed.
    theboard.at(7).at(0) = make_shared<Piece>(5, 'R', true);
    theboard.at(7).at(7) = make_shared<Piece>(5, 'R', true);
    theboard.at(7).at(1) = make_shared<Piece>(3, 'N', true);
    theboard.at(7).at(6) = make_shared<Piece>(3, 'N', true);
    theboard.at(7).at(2) = make_shared<Piece>(3, 'B', true);
    theboard.at(7).at(5) = make_shared<Piece>(3, 'B', true);
    theboard.at(7).at(3) = make_shared<Piece>(9, 'Q', true);
    theboard.at(7).at(4) = make_shared<Piece>(1000, 'K', true);

    for (int i = 0; i < 8; ++i) {
        theboard.at(6).at(i) = make_shared<Piece>(1, 'P', true);
    }

    white_king_moved = false;
    white_king_rook_moved = false;
    white_queen_rook_moved = false;
    black_king_moved = false;
    black_king_rook_moved = false;
    black_queen_rook_moved = false;
    whose_move = false;
    shared_ptr<TextDisplay> temp = make_shared<TextDisplay>(8);
    attach(temp);
    if (graphics_display_on) {
        shared_ptr<GraphicsDisplay> tempp = make_shared<GraphicsDisplay>(8, 500);
        attach(tempp);
    }
    if (white_player != "human" && white_player != "multiplayer") {
        if (white_player == "computer1") {
            shared_ptr<Level1> compw = make_shared<Level1>(this);
            setWhiteComp(compw);
        } else if (white_player == "computer2") {
            shared_ptr<Level2> compw = make_shared<Level2>(this);
            setWhiteComp(compw);
        } else if (white_player == "computer3") {
            shared_ptr<Level3> compw = make_shared<Level3>(this);
            setWhiteComp(compw);
        } else if (white_player == "computer4") {
            shared_ptr<Level4> compw = make_shared<Level4>(this);
            setWhiteComp(compw);
        } else throw ChessException(); // incorrect input!
    }
    if (black_player != "human" && black_player != "multiplayer") {
        if (black_player == "computer1") {
            shared_ptr<Level1> compw = make_shared<Level1>(this);
            setBlackComp(compw);
        } else if (black_player == "computer2") {
            shared_ptr<Level2> compw = make_shared<Level2>(this);
            setBlackComp(compw);
        } else if (black_player == "computer3") {
            shared_ptr<Level3> compw = make_shared<Level3>(this);
            setBlackComp(compw);
        } else if (black_player == "computer4") {
            shared_ptr<Level4> compw = make_shared<Level4>(this);
            setBlackComp(compw);
        } else throw ChessException(); // incorrect input!
    }
    setup_mode_on = true;
    notifyObservers();
}

void ChessModule::clear_last_move(){
    this->last_move.clear();
}

void ChessModule::re_init(string white_player, string black_player) { 
    ++seed;
    rng = default_random_engine(seed);
    winner = "ongoing";
    theboard = vector<vector<shared_ptr<Piece>>>(8, vector<shared_ptr<Piece>>(8, nullptr));

    //White pieces:
    theboard.at(0).at(0) = make_shared<Piece>(5, 'R', false);
    theboard.at(0).at(7) = make_shared<Piece>(5, 'R', false);
    theboard.at(0).at(1) = make_shared<Piece>(3, 'N', false);
    theboard.at(0).at(6) = make_shared<Piece>(3, 'N', false);
    theboard.at(0).at(2) = make_shared<Piece>(3, 'B', false);
    theboard.at(0).at(5) = make_shared<Piece>(3, 'B', false);
    theboard.at(0).at(3) = make_shared<Piece>(9, 'Q', false);
    theboard.at(0).at(4) = make_shared<Piece>(1000, 'K', false);

    for (int i = 0; i < 8; ++i) {
        theboard.at(1).at(i) = make_shared<Piece>(1, 'P', false);
    }

    //Black pieces:
    //Note that these are uppercase intentionally, they get converted when being displayed.
    theboard.at(7).at(0) = make_shared<Piece>(5, 'R', true);
    theboard.at(7).at(7) = make_shared<Piece>(5, 'R', true);
    theboard.at(7).at(1) = make_shared<Piece>(3, 'N', true);
    theboard.at(7).at(6) = make_shared<Piece>(3, 'N', true);
    theboard.at(7).at(2) = make_shared<Piece>(3, 'B', true);
    theboard.at(7).at(5) = make_shared<Piece>(3, 'B', true);
    theboard.at(7).at(3) = make_shared<Piece>(9, 'Q', true);
    theboard.at(7).at(4) = make_shared<Piece>(1000, 'K', true);

    for (int i = 0; i < 8; ++i) {
        theboard.at(6).at(i) = make_shared<Piece>(1, 'P', true);
    }

    white_king_moved = false;
    white_king_rook_moved = false;
    white_queen_rook_moved = false;
    black_king_moved = false;
    black_king_rook_moved = false;
    black_queen_rook_moved = false;
    whose_move = false;

    if (white_player != "human" && white_player != "multiplayer") {
        if (white_player == "computer1") {
            shared_ptr<Level1> compw = make_shared<Level1>(this);
            setWhiteComp(compw);
        } else if (white_player == "computer2") {
            shared_ptr<Level2> compw = make_shared<Level2>(this);
            setWhiteComp(compw);
        } else if (white_player == "computer3") {
            shared_ptr<Level3> compw = make_shared<Level3>(this);
            setWhiteComp(compw);
        } else if (white_player == "computer4") {
            shared_ptr<Level4> compw = make_shared<Level4>(this);
            setWhiteComp(compw);
        } else throw ChessException(); // incorrect input!
    }
    if (black_player != "human" && black_player != "multiplayer") {
        if (black_player == "computer1") {
            shared_ptr<Level1> compw = make_shared<Level1>(this);
            setBlackComp(compw);
        } else if (black_player == "computer2") {
            shared_ptr<Level2> compw = make_shared<Level2>(this);
            setBlackComp(compw);
        } else if (black_player == "computer3") {
            shared_ptr<Level3> compw = make_shared<Level3>(this);
            setBlackComp(compw);
        } else if (black_player == "computer4") {
            shared_ptr<Level4> compw = make_shared<Level4>(this);
            setBlackComp(compw);
        } else throw ChessException(); // incorrect input!  
    }
    
    notifyObservers();
}

void ChessModule::setup(char symbol, char piece, string pos, string colour) { 
    pair<int, int> Pos = make_pair(-1, -1);
    if (colour == ""){
        try {
            Pos = posToCoord(pos);
        } catch (ChessException ce) {
            throw ce; //Invalid input
        }
    }
    if(symbol == '+'){
        if(theboard.at(Pos.second).at(Pos.first) != nullptr){ // piece is already existed on that square
            // Remove piece
            theboard.at(Pos.second).at(Pos.first) = nullptr;
            //
        }
        // Add piece to pos
        if (piece < 'a'){
            theboard.at(Pos.second).at(Pos.first) = make_shared<Piece>(1,piece, false); // White
        }else{
            theboard.at(Pos.second).at(Pos.first) = make_shared<Piece>(1,piece-32, true); // Black
        }
        //
    }else if(symbol == '-'){
        if(theboard.at(Pos.second).at(Pos.first) != nullptr){ // no piece on this square 
            // Remove piece
            theboard.at(Pos.second).at(Pos.first) = nullptr;
            //
        }
    }else if(symbol == '='){
        if(colour == "black"){
            this->whose_move = true;
        }else if(colour == "white"){
            this->whose_move = false;
        }
    }
    // Verify that the board contains exactly one white king and one black king.
    vector<pair<char, pair<int, int>>> black_kings = findPiece(true, true, 'K');
    vector<pair<char, pair<int, int>>> white_kings = findPiece(false, true, 'K');
    if(black_kings.size() > 1 || white_kings.size() > 1) { // <-- There are NOT exactly one white and one black king.
        std::cout << "There are not exactly one white and one black king. Please modified the board again." << std::endl;
        this->setup_mode_conditions_passed = false;
    }   
    else if(black_kings.size() == 1 && white_kings.size() == 1) { // <-- There are exactly one white and one black king.
        if(!inCheck(true) && !inCheck(false)) { // <-- True if neither king is in check
            this->setup_mode_conditions_passed = true;
        }else{
            std::cout << "Friendly reminder: King is in check." << std::endl;
            this->setup_mode_conditions_passed = false;
        }
    }else{
        std::cout << "Both sides must have at least one King." << std::endl;
        this->setup_mode_conditions_passed = false;
    }
    if(!setup_mode_on){
        white_king_moved = false;
        white_king_rook_moved = false;
        white_queen_rook_moved = false;
        black_king_moved = false;
        black_king_rook_moved = false;
        black_queen_rook_moved = false;
        whose_move = false;
        shared_ptr<TextDisplay> temp = make_shared<TextDisplay>(8);
        attach(temp);
        if (graphics_display_on) {
            shared_ptr<GraphicsDisplay> tempp = make_shared<GraphicsDisplay>(8, 500);
            attach(tempp);
        }
        setup_mode_on = true;
    }
    notifyObservers();
}

std::pair<int, int> &ChessModule::getEnPassantSquare() { return enpassant_square; }

void ChessModule::setEnPassantSquare(int row, int col) {
    enpassant_square.first = row;
    enpassant_square.second = col;
}

bool ChessModule::getWhoseMove() { return whose_move; }

string ChessModule::getWinner() { return winner; }

void ChessModule::resign(bool who_resigned) {
    if (!who_resigned) winner = "black_resign";
    else winner = "white_resign";
    notifyObservers();
}

// Determines whether the square at row, col is under attack by the other player
// pass who_is_attacked
bool ChessModule::isSquareAttacked(bool who_is_attacked, int row, int col) const {
    string move_type;
    for (int r = 0; r < theboard.capacity(); ++r) {
        for (int c = 0; c < theboard.at(0).capacity(); ++c) {
            if (theboard.at(r).at(c) != nullptr && theboard.at(r).at(c).get()->getColour() != who_is_attacked) {
                move_type = ChessModule::canPieceMakeMove(!who_is_attacked, theboard.at(r).at(c).get()->getType(), row - r, col - c);
                if (move_type != "illegal" && move_type != "pawn_black" && move_type != "pawn_white" && 
                move_type != "pawn2_black" && move_type != "pawn2_white" && 
                move_type != "castleright" && move_type != "castleleft" &&
                !isNormalMoveRestricted(!who_is_attacked, r, c, row, col, move_type)) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Returns the type of move (assuming the move is allowed)
// DOESN'T CHECK WHETHER PIECE CAN DO THIS WRT TO THE BOARD
string ChessModule::canPieceMakeMove(bool which_player, char piece_type, int up_move, int right_move) const {
    if (up_move == 0 && right_move == 0) return "illegal";
    // King
    if (piece_type == 'K') {
        if (abs(up_move) > 1 || abs(right_move) > 2) return "illegal";
        else if (abs(right_move) == 2) {
            if (up_move) return "illegal";
            else return (right_move > 0 ? "castleright" : "castleleft");
        } else return "king";
    // Queen
    } else if (piece_type == 'Q') {
        if (right_move == 0 || up_move == 0 || 
        abs(right_move) == abs(up_move)) {
            return "queen";
        } else return "illegal";
    // Rook
    } else if (piece_type == 'R') {
        if (right_move == 0 || up_move == 0) {
            return "rook";
        } else return "illegal";
    // Knight
    } else if (piece_type == 'N') {
        if ((abs(right_move) == 1 && abs(up_move) == 2) ||
            (abs(right_move) == 2 && abs(up_move) == 1)) {
            return "knight";
        } else return "illegal";
    } else if (piece_type == 'B') {
    // Bishop
        if (abs(right_move) == abs(up_move)) { //defines diagonal move
            return "bishop";            
        } else return "illegal";
    // Pawn
    } else if (piece_type == 'P') {
        if ((up_move >= 0 && which_player) || (up_move <= 0 && !which_player)) return "illegal";
        if (abs(up_move) == 1) {
            if (right_move == 0) return string("pawn_") + (which_player ? "black" : "white");
            else if (abs(right_move) > 1) return "illegal";
            else return string("pawncapture_") + (which_player ? "black" : "white");
        } else if (abs(up_move) == 2) {
            if (right_move == 0) return string("pawn2_") + (which_player ? "black" : "white");
            else return "illegal";
        } else return "illegal";
    } else {
        throw ChessException(); // Internal error, piece_type unaccounted for
    }

}

//Tests whether a move is legal wrt to the board, given the piece is able to make the move
// DOESN'T TEST FOR CHECK
// Don't call this function with "castleright" or "castleleft"
bool ChessModule::isNormalMoveRestricted(bool whose_move, int fromRow, int fromCol, int toRow, int toCol, string move_type) const {
    
    if (toRow > 7 || toRow < 0 || toCol > 7 || toCol < 0) return true;
    
    // true means move is restricted
    int up_move = toRow - fromRow;
    int right_move = toCol - fromCol;

    if (theboard.at(toRow).at(toCol) != nullptr 
    && theboard.at(toRow).at(toCol).get()->getColour() == theboard.at(fromRow).at(fromCol).get()->getColour()) return true; // Can't capture same colour
    else if (move_type == "knight" || move_type == "king") return false;
    else if (move_type == "bishop") { 
        if (bishopBlocked(up_move, right_move, fromRow, fromCol)) return true;     
    } else if (move_type == "rook") {
        if (rookBlocked(up_move, right_move, fromRow, fromCol)) return true;
    } else if (move_type == "queen") {
        if (up_move && right_move) {
            if (bishopBlocked(up_move, right_move, fromRow, fromCol)) return true;
        } else {
            if (rookBlocked(up_move, right_move, fromRow, fromCol)) return true;
        }
    } else if ((move_type == "pawn_black" && theboard.at(fromRow - 1).at(fromCol).get() != nullptr) ||
        (move_type == "pawn_white" && theboard.at(fromRow + 1).at(fromCol).get() != nullptr)) {
        return true;
    } else if (move_type == "pawn2_black" || move_type == "pawn2_white") {
        if  ((fromRow != 1 && fromRow != 6) || rookBlocked(up_move + (up_move / 2), right_move, fromRow, fromCol)) {
            return true;      
        }
    } else if (move_type == "pawncapture_black" || move_type == "pawncapture_white") {
        if (theboard.at(toRow).at(toCol).get() == nullptr && (enpassant_square.first != toRow || enpassant_square.second != toCol)) {
            return true;
        }
    }
    return false;
}

// Assumes the "moved" variables correspond to starting locations
// Only call this function with "castleright" or "castleleft"
// This function could be a part of isNormalMoveRestricted, just out here for flexibility (might not be necessary)
bool ChessModule::isCastlingMoveRestricted(bool whose_move, int fromRow, int fromCol, int toRow, int toCol, string move_type) const {
    
    if (toRow > 7 || toRow < 0 || toCol > 7 || toCol < 0) return true;

    // We're assuming here that if either of the king and rook needed to castle are not in the correct row, then the corresponding
    // bool variable(s) will be false and the castle will fail. Thus, we don't check what row the castle is in.
    if (move_type == "castleright") {
        if ((this->whose_move && (black_king_moved || black_king_rook_moved)) || // black moved
        (!(this->whose_move) && (white_king_moved || white_king_rook_moved)) || // white moved
        rookBlocked(0, -3, fromRow, fromCol + 3) || isSquareAttacked(this->whose_move, fromRow, fromCol) || //pieces between or spaces 
        isSquareAttacked(this->whose_move, fromRow, fromCol + 1) || isSquareAttacked(this->whose_move, fromRow, fromCol + 2)) { // under attack
            return true;
        }
    } else if (move_type == "castleleft") {

        if ((this->whose_move && (black_king_moved || black_queen_rook_moved)) || // black moved
        (!(this->whose_move) && (white_king_moved || white_queen_rook_moved)) || //white moved
        rookBlocked(0, 4, fromRow, fromCol - 4) || isSquareAttacked(this->whose_move, fromRow, fromCol) ||
        isSquareAttacked(this->whose_move, fromRow, fromCol - 1) || isSquareAttacked(this->whose_move, fromRow, fromCol - 2)) {
            return true;
        }
    }
    return false;    
}

bool ChessModule::bishopBlocked(int up_move, int right_move, int fromRow, int fromCol) const {
    int count = abs(up_move) - 1;
    int up_increment = up_move / abs(up_move);
    int right_increment = right_move / abs(right_move);
    while (count > 0) {
        up_move -= up_increment;
        right_move -= right_increment;
        if (theboard.at(fromRow + up_move).at(fromCol + right_move).get() != nullptr) {
            return true;
        }
        --count;
    }
    return false;
}

bool ChessModule::rookBlocked(int up_move, int right_move, int fromRow, int fromCol) const {
    int count;
    int increment;
    if (up_move) { // Vertical move
        count = abs(up_move) - 1;
        increment = up_move / abs(up_move);
        while (count > 0) {
            up_move -= increment;
            if (theboard.at(fromRow + up_move).at(fromCol).get() != nullptr) {
                return true;
            }
            --count;
        }
    } else { // Horizontal move
        count = abs(right_move) - 1;
        increment = right_move / abs(right_move);
        while (count > 0) {
            right_move -= increment;
            if (theboard.at(fromRow).at(fromCol + right_move).get() != nullptr) {
                return true;
            }
            --count;
        }
    }
    return false;
}

// If you want all the pieces of that colour, pass ' ' to piece_type
// If ' ' is passed to piece_type but find_all is false, it'll just find you the first piece of the given colour starting from
// a1 and moving to the right and up. Odd behaviour, but still supported.
vector<pair<char, pair<int, int> > > ChessModule::findPiece(bool colour, bool find_all, char piece_type) const {
    vector<pair<char, pair<int, int> > > retVec(0, make_pair(' ', make_pair(-1, -1)));
    for (int row = 0; row < theboard.capacity(); ++row) {
        for (int col = 0; col < theboard.at(0).capacity(); ++col) {
            // the two statements could be easily merged, and I might later on, but for now this is more easily understood.
            if (piece_type == ' ') { // this means we want all the pieces of the given colour
                if (theboard.at(row).at(col).get() != nullptr &&
                theboard.at(row).at(col).get()->getColour() == colour) {
                    retVec.push_back(make_pair(theboard.at(row).at(col).get()->getType(), make_pair(row, col)));
                    if (!find_all) break;
                }
            } else {
                if (theboard.at(row).at(col).get() != nullptr && 
                theboard.at(row).at(col).get()->getColour() == colour && theboard.at(row).at(col).get()->getType() == piece_type) {
                    retVec.push_back(make_pair(piece_type, make_pair(row, col)));
                    if (!find_all) break;
                }
            }
            
        }
    }
    return retVec;
}

void ChessModule::promotePawn(bool who_is_promoting, int row, int col, char _input) {
    // to be implemented, requires user input, which requires display
    // consider promoting to queen as default for testing purposes
    string input;
    if (_input == 'Q') theboard.at(row).at(col) = make_shared<Piece>(9, 'Q', who_is_promoting);
    else if (_input == 'R') theboard.at(row).at(col) = make_shared<Piece>(5, 'R', who_is_promoting);
    else if (_input == 'N') theboard.at(row).at(col) = make_shared<Piece>(3, 'N', who_is_promoting);
    else if (_input == 'B') theboard.at(row).at(col) = make_shared<Piece>(3, 'B', who_is_promoting);
    else if (_input == ' ') {
    while (true) {
        if (who_is_promoting) {
            cout << "Promote pawn: q, r, n, or b?" << endl;
            cin >> input;
            if (input == "q") {
                theboard.at(row).at(col) = make_shared<Piece>(9, 'Q', who_is_promoting);
                break;
            } else if (input == "r") {
                theboard.at(row).at(col) = make_shared<Piece>(5, 'R', who_is_promoting);
                break;
            } else if (input == "n") {
                theboard.at(row).at(col) = make_shared<Piece>(3, 'N', who_is_promoting);
                break;
            } else if (input == "b") {
                theboard.at(row).at(col) = make_shared<Piece>(3, 'B', who_is_promoting);
                break;
            } else {
                cout << "Invalid promotion. " << endl;
            }
        } else {
            cout << "Promote pawn: Q, R, N, or B?" << endl;
            cin >> input;
            if (input == "Q") {
                theboard.at(row).at(col) = make_shared<Piece>(9, 'Q', who_is_promoting);
                break;
            } else if (input == "R") {
                theboard.at(row).at(col) = make_shared<Piece>(5, 'R', who_is_promoting);
                break;
            } else if (input == "N") {
                theboard.at(row).at(col) = make_shared<Piece>(3, 'N', who_is_promoting);
                break;
            } else if (input == "B") {
                theboard.at(row).at(col) = make_shared<Piece>(3, 'B', who_is_promoting);
                break;
            } else {
                cout << "Invalid promotion. " << endl;
            }
        }
    }
    }
}

void ChessModule::checkEndGame(bool player) {
    bool in_check = false;
    try {
        in_check = inCheck(player);
    } catch(ChessException ce) { // This player has no king!
        throw ce;
    }
    vector<pair<string, pair<pair<int, int>, pair<int, int> > > > all_legal_moves = allLegalMoves(player, true);
    if (all_legal_moves.size() == 0) {
        if (in_check) winner = string((player) ? "white" : "black") + "_checkmate"; // black's move -> white wins, white's move -> black wins
        else winner = "stalemate";
    } else if (in_check) {
        if (player) cout << "Black is in check." << endl;
        else cout << "White is in check." << endl;
    }
}

// This function currently assumes theboard.at(0).at(0) is the bottom left
// Requires/assumes that current player is not in checkmate or stalemate
void ChessModule::movePiece(string from_pos, string to_pos) {
    // This function is split into three parts.
    // Part 1: Check that the input is valid
        // Format, within board limits, piece at from_pos, piece is correct colour
    // Part 2: Check that the specified piece is able to move to 'to_pos'
    // Part 3: Check if the move would place own king in check
    // Part 4: Move the piece
    // Part 5: Check for check, checkmate, or stalemate (for the other player)
    
    /* Part 1: */

    if (from_pos == to_pos || from_pos.length() != 2 || to_pos.length() != 2 || 
        from_pos[0] < 'a' || from_pos[0] > 'h' || to_pos[0] < 'a' || to_pos[0] > 'h' || 
        from_pos[1] < '1' || from_pos[1] > '8' || to_pos[1] < '1' || to_pos[1] > '8') {
        throw ChessException(); //Invalid input
    }
    pair<int, int> fromPos = make_pair(-1, -1);
    pair<int, int> toPos = make_pair(-1, -1);
    try {
        fromPos = posToCoord(from_pos);
        toPos = posToCoord(to_pos);
        if (fromPos.first == toPos.first && fromPos.second == toPos.second) throw ChessException(); //Invalid input
    } catch (ChessException ce) { //Invalid input
        throw ce;
    }
    int fromCol = fromPos.first;
    int fromRow = fromPos.second;
    int toCol = toPos.first;
    int toRow = toPos.second;

    if (theboard.at(fromRow).at(fromCol) == nullptr) throw ChessException(); //Invalid selection / illegal move
    if (theboard.at(fromRow).at(fromCol).get()->getColour() != whose_move) throw ChessException(); //Invalid selection / illegal move
    /* Part 2 */

    // K-King, Q-Queen, R-Rook, N-Knight, B-Bishop, P-Pawn
    char piece_type = theboard.at(fromRow).at(fromCol).get()->getType();
    string move_type = canPieceMakeMove(whose_move, piece_type, toRow - fromRow, toCol - fromCol);
    if (move_type == "illegal") {
        throw ChessException(); // illegal move
    } else if (move_type == "castleright" || move_type == "castleleft") {
        if (isCastlingMoveRestricted(whose_move, fromRow, fromCol, toRow, toCol, move_type)) {
            throw ChessException(); //illegal move
        }
    } else {
        if (isNormalMoveRestricted(whose_move, fromRow, fromCol, toRow, toCol, move_type)) {
            throw ChessException(); //illegal move
        }
    }

    /* Part 3/4 */

    // if the move would place own king in check, undo the moves and throw an exception.
    // otherwise, undo the move and throw an exception
    try {
        tryMove(true, whose_move, fromRow, fromCol, toRow, toCol, move_type); // strongly exception-safe, catching for understanding
    } catch (ChessException ce) {
        // either "This player has no king!" or "Illegal move, places own king in check"
        throw ce;
    }
      
    
// Removing enpassant-ed pawns is done in tryMove()
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
                if (whose_move) {
                    if (!black_king_moved) black_king_moved = true;
                    if (!black_king_rook_moved) black_king_rook_moved = true;
                } else {
                    if (!white_king_moved) white_king_moved = true;
                    if (!white_king_rook_moved) white_king_rook_moved = true;
                }
            } else if (move_type == "castleleft") {
                theboard.at(fromRow).at(fromCol - 1) = move(theboard.at(fromRow).at(fromCol - 4));
                if (whose_move) {
                    if (!black_king_moved) black_king_moved = true;
                    if (!black_queen_rook_moved) black_queen_rook_moved = true;
                } else {
                    if (!white_king_moved) white_king_moved = true;
                    if (!white_queen_rook_moved) white_queen_rook_moved = true;
                }
            } else if (move_type == "king") {
                if (whose_move && !black_king_moved) black_king_moved = true;
                else if (!whose_move && !white_king_moved) white_king_moved = true;
            } else if (move_type == "rook") {
                if (whose_move) {
                    if (fromCol == 0 && !black_queen_rook_moved) black_queen_rook_moved = true;
                    else if (fromCol == 7 && !black_king_rook_moved) black_king_rook_moved = true;
                } else {
                    if (fromCol == 0 && !white_queen_rook_moved) white_queen_rook_moved = true;
                    else if (fromCol == 7 && !white_king_rook_moved) white_king_rook_moved = true;
                }
            } else if ((move_type == "pawn_black" || move_type == "pawn_white" || move_type == "pawncapture_black" ||
            move_type == "pawncapture_white")) {
                if ((whose_move && toRow == 0) || (!whose_move && toRow == 7)) {
                    promotePawn(whose_move, toRow, toCol, ' ');
                }
            }
        }
    //}
    /* Part 5 */
    
    switchWhoseMove();

    
    try{
        checkEndGame(whose_move);
    } catch (ChessException ce) { // This player has no king!
        throw ce;
    }

    notifyObservers();
}

void ChessModule::compMove(bool player) {
    if ((player && black_computer == nullptr) || (!player && white_computer == nullptr)) throw ChessException(); // No computer player!
    vector<pair<string, pair<pair<int, int>, pair<int, int> > > > all_legal_moves = allLegalMoves(player, true);
    int which_move = 0;
    if (all_legal_moves.size() == 0) throw ChessException(); // No legal moves! Checkmate should've been called already
    
    if (player) {
        which_move = black_computer.get()->findBestMove(getRNG(), player, all_legal_moves, theboard);
    } else {
        which_move = white_computer.get()->findBestMove(getRNG(), player, all_legal_moves, theboard);
    }
    pair<string, pair<pair<int, int>, pair<int, int> > > comp_move = all_legal_moves.at(which_move);
    int fromRow = comp_move.second.first.first;
    int fromCol = comp_move.second.first.second;
    int toRow = comp_move.second.second.first;
    int toCol = comp_move.second.second.second;
    string move_type = comp_move.first;
    try {
        tryMove(true, player, comp_move.second.first.first, comp_move.second.first.second, comp_move.second.second.first, 
        comp_move.second.second.second, comp_move.first);
        
        
        
        
    } catch (ChessException ce) { // Either no king, or illegal move (places own king in check)
        // Should never happen, since all_legal_moves is a list of legal moves
        throw ce;
    }
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
                if (whose_move) {
                    if (!black_king_moved) black_king_moved = true;
                    if (!black_king_rook_moved) black_king_rook_moved = true;
                } else {
                    if (!white_king_moved) white_king_moved = true;
                    if (!white_king_rook_moved) white_king_rook_moved = true;
                }
            } else if (move_type == "castleleft") {
                theboard.at(fromRow).at(fromCol - 1) = move(theboard.at(fromRow).at(fromCol - 4));
                if (whose_move) {
                    if (!black_king_moved) black_king_moved = true;
                    if (!black_queen_rook_moved) black_queen_rook_moved = true;
                } else {
                    if (!white_king_moved) white_king_moved = true;
                    if (!white_queen_rook_moved) white_queen_rook_moved = true;
                }
            } else if (move_type == "king") {
                if (whose_move && !black_king_moved) black_king_moved = true;
                else if (!whose_move && !white_king_moved) white_king_moved = true;
            } else if (move_type == "rook") {
                if (whose_move) {
                    if (fromCol == 0 && !black_queen_rook_moved) black_queen_rook_moved = true;
                    else if (fromCol == 7 && !black_king_rook_moved) black_king_rook_moved = true;
                } else {
                    if (fromCol == 0 && !white_queen_rook_moved) white_queen_rook_moved = true;
                    else if (fromCol == 7 && !white_king_rook_moved) white_king_rook_moved = true;
                }
            } else if ((move_type == "pawn_black" || move_type == "pawn_white" || move_type == "pawncapture_black" ||
            move_type == "pawncapture_white")) {
                if ((whose_move && toRow == 0) || (!whose_move && toRow == 7)) {
                    promotePawn(whose_move, toRow, toCol, 'Q');
                }
            }
        }
    switchWhoseMove();

    cout << "move " << char('a' + comp_move.second.first.second) << char('1' + comp_move.second.first.first) << " " 
    << char('a' + comp_move.second.second.second) << char('1' + comp_move.second.second.first) << endl;
    try {
        checkEndGame(whose_move);
    } catch (ChessException ce) { // This player has no king!
        throw ce;
    }
    notifyObservers();
}
