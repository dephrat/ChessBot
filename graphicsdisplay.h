/*#ifndef GRAPHICSDISPLAY_H
#define GRAPHICSDISPLAY_H
#include <iostream>
#include <vector>
#include "observer.h"
#include "window.h"
#include <string>

class GraphicsDisplay : public Observer  {
    //Xwindow w;
    // n should be 8
    int n;
    const int size;

public:
    GraphicsDisplay(int n, int size);

    void notify (std::vector<std::vector<std::shared_ptr<Piece> > > &theboard) override;

};
#endif
*/