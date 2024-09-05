#include "chessstrategy.h"
#include "chessmodule.h"

ChessStrategy::~ChessStrategy() {}

ChessStrategy::ChessStrategy(ChessModule *cm) : cm(cm) {}
