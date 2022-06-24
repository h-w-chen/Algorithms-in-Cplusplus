// By your BOI : Yahia B
// feel free to take whatever you would like
#ifndef SkipList_H
#define SkipList_H

#include <iostream>
#include <cstdlib>
#include <limits>
#include <random>
#include <ctime>
#include <vector>

using namespace std;

// left/right bounds
template<typename T>
T Min();

template<typename T>
T Max();

template<>
int Min<int>() { return -INFINITY; }

template<>
int Max<int>() { return +INFINITY; }

template<>
std::string Min<std::string>() { return "---"; }

template<>
std::string Max<std::string>() { return "+++"; };


// type for internal node
template<class T>
class Node {
public:
    virtual ~Node() = default;

public:
    T key;
    Node<T> *next;
    Node<T> *prev;
    Node<T> *up;
    Node<T> *down;

    Node(const T &key) {
        this->key = key;
        this->next = nullptr;
        this->prev = nullptr;
        this->up = nullptr;
        this->down = nullptr;
    };

    virtual void printData() {
        // not need to print the left/right bounds
        if (this->key == Min<T>() || this->key == Max<T>())
            return;
        cout << this->key << " ";

        // TO SEE UP,DOwN,LEFT,RIGHT NODE DATA FOR EVERY NODE, UNCOMMENT BELOW
        /* if(down != nullptr)cout << ": down = " << down->key << "  / ";
         if(up != nullptr)cout << ": up = " << up->key << "  / ";
         if(prev != nullptr)cout << ": prev = " << prev->key << "  / ";
         if(next != nullptr)cout << ": next = " << next->key << "  / ";
         */
    };
};

// type for leaf nodes
template<class T, typename TVal>
struct LeafNode : Node<T> {
public:
    TVal value;

    LeafNode(const T &key, const TVal &value) : Node<T>(key) {
        this->value = value;
    };

    virtual void printData() override {
        // not need to print the left/right bounds
        if (this->key == Min<T>() || this->key == Max<T>())
            return;
        cout << this->key << "(" << this->value << ")" << " ";
    }
};

// each level is an ordered Doubly Linked List
template<class T, typename TLess, typename TVal>
class SkipList {
private:
    class list{
    private:
        Node<T> *header_;
        Node<T> *tail_;
    public:
        list(Node<T> * header, Node<T> *tail): header_(header), tail_(tail) {
            this->header_->next = this->tail_;
            this->tail_->prev = this->header_;
        }
        Node<T> *getTail() const { return this->tail_;}
        Node<T> *getHeader() const { return this->header_;}
    };

    // Head of each Linked List in Skiplist
    vector<list> Heads;

    TLess lesser;

    // To insert to a specific Level
    // Returns the pointer to the up node
    Node<T> *insert_to_level(T N, TVal val, int level, Node<T> *down);

    // Deletes a Node from its linked list
    void Delete_Node(Node<T> *N);

public:
    SkipList();

    void insert(T key, TVal value);

    // Print all of Linked List Data
    void printData();

    // Search for node, return Node if found
    LeafNode<T, TVal> *Search(T key);

    // Deletes node from skip list
    void Delete(T N);
};

// Constructor
template<class T, typename TLess, typename TVal>
SkipList<T, TLess, TVal>::SkipList() {
    // Set seed for random number generator to ensure randomness
    srand(static_cast<unsigned int>(time(NULL)));

    Node<T> *Head1 = new LeafNode<T, TVal>(Min<T>(), TVal());

    Node<T> *Tail1 = new LeafNode<T, TVal>(Max<T>(), TVal());

    Head1->next = Tail1;

    Tail1->prev = Head1;

    Heads.push_back(list{Head1, Tail1});
}

template<class T, typename TLess, typename TVal>
LeafNode<T, TVal> *SkipList<T, TLess, TVal>::Search(T key) {
    Node<T> *topleft;
    topleft = Heads[Heads.size() - 1].getHeader();
    Node<T> *pt; // = new Node<T>(-INFINITY);
    pt = topleft;
    while (pt != nullptr) {
        if (pt->key == key) {
            break;
        } else if (key > pt->key && !this->lesser.isLessThan(key, pt->next->key)) {
            pt = pt->next;
        } else if (key > pt->key && this->lesser.isLessThan(key, pt->next->key)) {
            pt = pt->down;
        }
    }

    while ( pt != nullptr && pt->down != nullptr)
        pt = pt->down;

    return static_cast<LeafNode<T, TVal> *>(pt);
}

// Insert to Level Function
// Inserts a node to a level given the data to be inserted, The level to be
// inserted to , the node that will be the down of the node added
template<class T, typename TLess, typename TVal>
Node<T> *SkipList<T, TLess, TVal>::insert_to_level(T data, TVal value, int level, Node<T> *Down) {
    int i = level;
    Node<T> *N;
    if (level == 0) {
        N = new LeafNode<T, TVal>(data, value);
//      N = new Node<T>(data);
    } else {
        N = new Node<T>(data);
    }
    N->down = Down;
    Node<T> *pt; // = new Node<T>(0);
    pt = Heads[i].getHeader();

    while (pt->next != nullptr && this->lesser.isLessThan(pt->next->key, data)) {
        pt = pt->next;
    }
    N->prev = pt;
    N->next = pt->next;
    pt->next = N;
    N->next->prev = N;
    return N;
}

// Insert to skip list function
template<class T, typename TLess, typename TVal>
void SkipList<T, TLess, TVal>::insert(T key, TVal value) {
    // insert to leaf node level first
    Node<T> *Down = insert_to_level(key, value, 0, nullptr);  // Insert key to bottom level

    // Now Decide Whether to Create Top Layer and Insert;
    int Coin_Toss = rand() % 2;
    int i = 0;
    while (Coin_Toss == 0) {
        i++;
        // check to grow the tower one more level
        if (this->Heads.size() - 1 < i) {
            Node<T> *NewHead = new Node<T>(Min<T>());
            Node<T> *NewTail = new Node<T>(Max<T>());

            // left/right pivots
            Heads[i - 1].getHeader()->up = NewHead;
            NewHead->down = Heads[i - 1].getHeader();
            Heads[i - 1].getTail()->up = NewTail;
            NewTail->down = Heads[i - 1].getTail();

            Heads.push_back(list(NewHead, NewTail));
        }
        Node<T> *N = insert_to_level(key, value, i, Down);
        Down->up = N;
        Down = N;

        Coin_Toss = rand() % 2;
    }
    return;
}

template<class T, typename TLess, typename TVal>
void SkipList<T, TLess, TVal>::Delete(T N) {
    Node<T> *pt = Search(N);
    while (pt != nullptr) {
        Node<T> *temp = pt->down;
        Delete_Node(pt);
        pt = temp;
    }
}

template<class T, typename TLess, typename TVal>
void SkipList<T, TLess, TVal>::Delete_Node(Node<T> *N) {
    if (N->down != nullptr) N->down->up = nullptr;
    if (N->up != nullptr) N->up->down = nullptr;
    Node<T> *Next = N->next;
    Node<T> *Prev = N->prev;
    Prev->next = Next;
    delete (N);
    Next->prev = Prev;
}

// Print Skip List Data By Level
template<class T, typename TLess, typename TVal>
void SkipList<T, TLess, TVal>::printData() {
    for (int i = 0; i != Heads.size(); i++) {
        cout << " LEVEL : " << i << endl;
        Node<T> *pt; //= new Node<T>(Min<T>());
        pt = Heads[i].getHeader();
        while (pt != nullptr) {
            pt->printData();
            pt = pt->next;
        }
        cout << endl;
    }
}

#endif
