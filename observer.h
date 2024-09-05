#ifndef _OBSERVER_H_
#define _OBSERVER_H_
#include "piece.h"
#include <memory>
#include <vector>

class Observer {
public:
    virtual void notify(std::vector<std::vector<std::shared_ptr<Piece> > > &theboard) = 0; // pass the Subject that called
    // the notify method
    virtual ~Observer() = default;
};

#endif
