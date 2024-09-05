#include "subject.h"
using namespace std;

Subject::~Subject() {}

void Subject::attach(shared_ptr<Observer> o) {
    observers.emplace_back(o);
    //observers.push_back(o);
}

void Subject::detach(shared_ptr<Observer> o){
    for ( auto it = observers.begin(); it != observers.end(); ++it ) {
        if ( *it == o ) {
            observers.erase(it);
        break;
    }
  }
}
