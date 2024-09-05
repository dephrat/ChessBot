#ifndef _LEVEL4_H_
#define _LEVEL4_H_
#include <random>
#include "chessstrategy.h"
#include "chessmodule.h"

class Level4 : public ChessStrategy {
    public:
    Level4(ChessModule *cm);
    int findBestMove(std::default_random_engine, bool, std::vector<std::pair<std::string, std::pair<std::pair<int, int>, std::pair<int, int> > > >&, std::vector<std::vector<std::shared_ptr<Piece> > >&) override;
};

#endif
