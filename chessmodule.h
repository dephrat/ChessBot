#ifndef CHESSMODULE_H
#define CHESSMODULE_H
#include "piece.h"
#include "subject.h"
#include "chessexception.h"
#include "display.h"
#include "graphicsdisplay.h"
#include "chessstrategy.h"
#include "level1.h"
#include "level2.h"
#include "level3.h"
#include "level4.h"
#include <vector>
#include <memory>
#include <utility>
#include <string>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <random>

class ChessModule : public Subject { // Logic module
    std::vector<std::vector<std::shared_ptr<Piece> > > theboard;
    std::shared_ptr<ChessStrategy> white_computer;
    std::shared_ptr<ChessStrategy> black_computer;
    std::vector<std::string> last_move;
    //int computer_level;
    bool whose_move; // false = white, true = black
    bool white_king_moved = false;
    bool white_king_rook_moved = false;
    bool white_queen_rook_moved = false;
    bool black_king_moved = false;
    bool black_king_rook_moved = false;
    bool black_queen_rook_moved = false;
    bool setup_mode_conditions_passed = false;
    bool setup_mode_on = false;
    bool graphics_display_on = true;
    double black_score=0;
    double white_score=0;
    unsigned seed;
    std::default_random_engine rng;
    std::pair <int, int> enpassant_square = std::make_pair<int, int>(-1, -1); // default initialize this to (-1, -1)
    std::string winner = std::string("unstarted"); // one of unstarted, ongoing, stalemate, 
    // black_checkmate, white_checkmate, black_resign, white_resign 
    // ^^^to clarify, black_resign means black won by resignation, similar for white_resign.

    // Add private member/methods if you want
    void nextBotMove(); // next move by computer.
    void promotePawn(bool, int, int, char); // A pawn reaches the other end, gets to promote to (rook, knight, bishop, queen)
    
    // allLegalMoves returns a vec<pair<piece_type, pair<pair<fromRow, fromCol>, pair<toRow, toCol>>>>
    // so a vec of pairs of the piece's type, and another pair which contains the positions for the move
    std::vector<std::pair<std::string, std::pair<std::pair<int, int>, std::pair<int, int> > > > allLegalMoves(bool, bool);
    bool bishopBlocked(int, int, int, int) const;
    bool rookBlocked(int, int, int, int) const;
    bool isNormalMoveRestricted(bool, int, int, int, int, std::string) const;
    bool isCastlingMoveRestricted(bool, int, int, int, int, std::string) const;
    bool isSquareAttacked(bool, int, int) const;
    std::string canPieceMakeMove(bool, char, int, int) const;
    std::vector<std::pair<char, std::pair<int, int> > > findPiece(bool, bool, char) const;
    
    public:
    ChessModule();
    ChessModule(bool whose_move);
    ChessModule(unsigned seed, bool whose_move);
    ~ChessModule();

    bool isCheckmate(bool);
    bool isStalemate(bool);
    bool inCheck(bool) const;

    void notifyObservers() override;

    void tryMove(bool, bool, int, int, int, int, std::string);

    void setObserverGraphic(Observer* ob);
    void resign(bool who_resigned); // false = white called resign, true = black called resign
    // init(white_player, black_player) both parameters can be either "human" or "computer[1-4]"
    void init(std::string white_player, std::string black_player); // initialize the game. 
    void re_init(std::string white_player, std::string black_player); // Reinitialize the game. 
    void movePiece(std::string from_pos, std::string to_pos);
    void compMove(bool);
    void checkEndGame(bool);

    void setup(char symbol, char piece, std::string pos, std::string colour); // setup mode.
    void print_score();
    void clear_board();

    // set methods:
    void addscore(bool colour); // false = white, true = black;
    void addhalf(); 

    // get methods:
    int getwhitescore();
    int getblackscore();
    std::vector<std::string> getrecordmove();
    bool getWhoseMove();
    bool getSetupConditionsBool();
    std::default_random_engine getRNG();
    std::string getWinner(); // gets winner as described above
    std::vector<std::vector<std::shared_ptr<Piece> > > &getBoard();
    std::pair<int, int> &getEnPassantSquare();
    void setEnPassantSquare(int, int);
    void setrecordmove(std::string,std::string);
    void switchWhoseMove();
    void setWhiteComp(std::shared_ptr<ChessStrategy>);
    void setBlackComp(std::shared_ptr<ChessStrategy>);
    void setShowGraphicsDisplay(bool);
    void setSeed(unsigned);
    void poprecordmove();
    void clear_last_move();

    friend std::ostream &operator<<(std::ostream &out, const ChessModule &cm);
    // add public functions:
    friend class Level1;
    friend class Level2;
    friend class Level3;
    friend class Level4;

};

#endif


