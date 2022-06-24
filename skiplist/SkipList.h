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

template <class T>
struct Node {
 public:
  // Node Data
  T key;
  Node<T>* next;
  Node<T>* prev;
  Node<T>* up;
  Node<T>* down;

  // Node Constructor
  Node(const T& key) {
    this->key = key;
    this->next = nullptr;
    this->prev = nullptr;
    this->up = nullptr;
    this->down = nullptr;
  };

  // IF KEY IS LESS THAN INFINITY AND GREATER THAN NEGATIVE INFINITY --> Print
  void printData() {
//      if (key < 2147483645 && key > -2147483645) cout << key << " ";
      cout << key << " ";

    // TO SEE UP,DOwN,LEFT,RIGHT NODE DATA FOR EVERY NODE, UNCOMMENT BELOW
    /* if(down != nullptr)cout << ": down = " << down->key << "  / ";
     if(up != nullptr)cout << ": up = " << up->key << "  / ";
     if(prev != nullptr)cout << ": prev = " << prev->key << "  / ";
     if(next != nullptr)cout << ": next = " << next->key << "  / ";
     */
  };
};

template <typename T> T Min();
template <typename T> T Max();

template <> int Min<int>( ){ return -INFINITY; }
template <> int Max<int>( ){ return +INFINITY; }
template <> std::string Min<std::string>( ){ return "---"; }
template <> std::string Max<std::string>() { return "+++"; };

template <class T, typename TLess>
class SkipList {
 public:
  // Constructor for Doubly Linked List
  SkipList();

  // Insert Node with key N to maintain Ordered Linked List
  void insert(T N);

  // To insert to a specific Level
  Node<T>* insert_to_level(
      T N, int level, Node<T>* down);  // Returns the pointer to the up node

  // Print all of Linked List Data
  void printData();

  // Search for node, return Node if found
  Node<T>* Search(T key);

  // Deletes node from skip list
  void Delete(T N);

  // Deletes a Node from its linked list
  void Delete_Node(Node<T>* N);

  vector<Node<T>*> Heads;  // Head of each Linked List in Skiplist

  int Levels;  // Number of Levels in SkipList, Accessed via Heads

  TLess lesser;
};

// Constructor
template <class T, typename TLess>
SkipList<T, TLess>::SkipList() {
  // Set seed for random number generator to ensure randomness
  srand(static_cast<unsigned int>(time(NULL)));

  Node<T>* Head1 = new Node<T>(Min<T>());

  Node<T>* Tail1 = new Node<T>(Max<T>());

  Head1->next = Tail1;

  Tail1->prev = Head1;

  Heads.push_back(Head1);
}

template <class T, typename TLess>
Node<T>* SkipList<T, TLess>::Search(T key) {
  Node<T>* topleft;
  topleft = Heads[Heads.size() - 1];
  Node<T>* pt = new Node<T>(-INFINITY);
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
  return pt;
}

// Insert to Level Function
// Inserts a node to a level given the data to be inserted, The level to be
// inserted to , the node that will be the down of the node added
template <class T, typename TLess>
Node<T>* SkipList<T, TLess>::insert_to_level(T data, int level, Node<T>* Down) {
  int i = level;
  Node<T>* N = new Node<T>(data);
  N->down = Down;
  Node<T>* pt; // = new Node<T>(0);
  pt = Heads[i];

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
template <class T, typename TLess>
void SkipList<T, TLess>::insert(T data) {
  int i = 0;
  Node<T>* Down =
      insert_to_level(data, i, nullptr);  // Insert data to bottom level
  // Now Decide Whether to Create Top Layer and Insert;
  int Coin_Toss = rand() % 2;
  while (Coin_Toss == 0) {
    i++;
    if (Levels < i) {
      Levels += 1;
      Node<T>* NewHead = new Node<T>(Min<T>());
      Node<T>* NewTail = new Node<T>(Max<T>());
      NewHead->next = NewTail;
      NewTail->prev = NewHead;
      Heads[i - 1]->up = NewHead;
      NewHead->down = Heads[i - 1];
      Heads.push_back(NewHead);
    }
    Node<T>* N = insert_to_level(data, i, Down);
    Down->up = N;
    Down = N;
    Coin_Toss = rand() % 2;
  }
  return;
}

template <class T, typename TLess>
void SkipList<T, TLess>::Delete(T N) {
  Node<T>* pt = Search(N);
  while (pt != nullptr) {
    Node<T>* temp = pt->down;
    Delete_Node(pt);
    pt = temp;
  }
}

template <class T, typename TLess>
void SkipList<T, TLess>::Delete_Node(Node<T>* N) {
  if (N->down != nullptr) N->down->up = nullptr;
  if (N->up != nullptr) N->up->down = nullptr;
  Node<T>* Next = N->next;
  Node<T>* Prev = N->prev;
  Prev->next = Next;
  delete (N);
  Next->prev = Prev;
}

// Print Skip List Data By Level
template <class T, typename TLess>
void SkipList<T, TLess>::printData() {
  for (int i = 0; i != Heads.size(); i++) {
    cout << " LEVEL : " << i << endl;
    Node<T>* pt; //= new Node<T>(Min<T>());
    pt = Heads[i];
    while (pt != nullptr) {
      pt->printData();
      pt = pt->next;
    }
    cout << endl;
  }
}

#endif
