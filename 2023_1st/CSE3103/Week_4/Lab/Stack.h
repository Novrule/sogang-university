#ifndef __STACK__
#define __STACK__

#include <iostream>
#include "LinkedList.h"

using namespace std;

template <class T> class Stack : public LinkedList<T> {
public:
	virtual bool Delete(T& element);
};

template <class T> bool Stack<T>::Delete(T& element) {
    if (LinkedList<T>::current_size == 0) {
        return false;
    }

    Node<T>* temp = this->first;
    element = temp->data;
    this->first = this->first->link;
    delete temp;
    this->current_size--;

    return true;
}

#endif
