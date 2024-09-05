#ifndef DISPLAY_H
#define DISPLAY_H
#include "observer.h"
#include <vector>
#include <iostream>

class TextDisplay : public Observer{
    std::vector<std::vector<char>> theDisplay;
    const int gridSize;

    public:
    TextDisplay(int n);
    void notify(std::vector<std::vector<std::shared_ptr<Piece> > > &theboard) override;
    void setBoard(std::vector<std::vector<std::shared_ptr<Piece> > > &theboard);
    friend std::ostream &operator<<(std::ostream &out, const TextDisplay& d);
    

};

#endif
