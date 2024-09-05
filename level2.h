#ifndef _LEVEL2_H_
#define _LEVEL2_H_
#include <random>
#include "chessstrategy.h"
#include "chessmodule.h"

class Level2 : public ChessStrategy {
    public:
    Level2(ChessModule *cm);
    int findBestMove(std::default_random_engine, bool, std::vector<std::pair<std::string, std::pair<std::pair<int, int>, std::pair<int, int> > > >&, std::vector<std::vector<std::shared_ptr<Piece> > >&) override;
};

#endif
