#ifndef _CHESS_STRATEGY_H_
#define _CHESS_STRATEGY_H_
#include <utility>
#include <string>
#include <vector>
#include <memory>
#include <random>
#include "piece.h"

class ChessModule;

class ChessStrategy {
    protected:
    ChessModule *cm;
    public: 
    ChessStrategy(ChessModule *cm);
    virtual ~ChessStrategy() = 0;
    // returns an index to the move it likes
    // if we want to make this private/protected, consider making ChessModule a friend class  
    virtual int findBestMove(std::default_random_engine, bool, std::vector<std::pair<std::string, std::pair<std::pair<int, int>, std::pair<int, int> > > > &, 
    std::vector<std::vector<std::shared_ptr<Piece> > > &) = 0;
};

#endif
