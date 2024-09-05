#ifndef _LEVEL1_H_
#define _LEVEL1_H_
#include <random>
#include "chessstrategy.h"

class Level1 : public ChessStrategy {
    public:
    Level1(ChessModule *cm);
    int findBestMove(std::default_random_engine, bool, std::vector<std::pair<std::string, std::pair<std::pair<int, int>, std::pair<int, int> > > >&, std::vector<std::vector<std::shared_ptr<Piece> > >&) override;
};

#endif
