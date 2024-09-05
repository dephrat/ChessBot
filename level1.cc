#include <memory>
#include <string>
#include <algorithm>
#include <chrono>
#include <random>
#include "chessstrategy.h"
#include "level1.h"

using namespace std;

Level1::Level1(ChessModule *cm) : ChessStrategy(cm) {}

int Level1::findBestMove(default_random_engine rng, bool colour, std::vector<std::pair<std::string, std::pair<std::pair<int, int>, std::pair<int, int> > > >&all_legal_moves, std::vector<std::vector<std::shared_ptr<Piece> > >&theboard) {
        shuffle( all_legal_moves.begin(), all_legal_moves.end(), rng );
        //return random int from 0 to legal_moves.size() - 1;
        return 0; //temporary
      
}
