#ifndef _SUBJECT_H_
#define _SUBJECT_H_
#include <vector>
#include <memory>
#include "observer.h"

//class Observer;

class Subject {
    protected:
    std::vector<std::shared_ptr<Observer> > observers;

    public:
    // attach new observer to vector
    void attach(std::shared_ptr<Observer> o);
    void detach(std::shared_ptr<Observer> o);

    // notify all observers if subject has changed
    virtual void notifyObservers() = 0;
    virtual ~Subject();
};

#endif
