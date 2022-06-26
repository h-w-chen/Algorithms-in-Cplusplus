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
#include <cassert>

using namespace std;

namespace hwlib {
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
        class list {
        private:
            Node<T> *header_;
            Node<T> *tail_;
        public:
            list(Node<T> *header, Node<T> *tail) : header_(header), tail_(tail) {
                this->header_->next = this->tail_;
                this->tail_->prev = this->header_;
            }

            Node<T> *getTail() const { return this->tail_; }

            Node<T> *getHeader() const { return this->header_; }
        };

        // Head of each Linked List in Skiplist
        vector<list> Heads;

        TLess lesser;

        // Deletes a Node from its linked list
        void Delete_Node(Node<T> *N);

    public:
        SkipList();

        bool insert(T key, TVal value);

        // Print all of Linked List Data
        void printData();

        // Search for node, return Node if found
        LeafNode<T, TVal> *Search(T key);

        // Deletes node from skip list
        void Delete(T N);

        ~SkipList();

        // iterator for skiplist (actual at leaf level)
        class Iterator {
        protected:
            LeafNode<T, TVal> *node;
        public:
            Iterator(LeafNode<T, TVal> *node) : node(node) {}

            LeafNode<T, TVal> *operator->() const { return this->node; }

            Iterator &operator++() {
                this->node = static_cast<LeafNode<T, TVal> *>(this->node->next);
                return *this;
            }

            Iterator &operator--() {
                this->node = static_cast<LeafNode<T, TVal> *>(this->node->prev);
                return *this;
            }

            bool operator==(Iterator b) const { return this->node == b.node; }

            bool operator!=(Iterator b) const { return this->node != b.node; }
        };

        Iterator lower_bound(T key); //{return SkipList::Iterator(nullptr);}
        Iterator upper_bound(T key);
    };

// Constructor
    template<class T, typename TLess, typename TVal>
    SkipList<T, TLess, TVal>::SkipList() {
        // Set seed for random number generator to ensure randomness
        srand(static_cast<unsigned int>(time(NULL)));

        Node<T> *Head1 = new LeafNode<T, TVal>(Min<T>(), TVal());
        Node<T> *Tail1 = new LeafNode<T, TVal>(Max<T>(), TVal());
        Heads.push_back(list{Head1, Tail1});
    }

    template<class T, typename TLess, typename TVal>
    LeafNode<T, TVal> *SkipList<T, TLess, TVal>::Search(T key) {
        Node<T> *pt = Heads[Heads.size() - 1].getHeader();
        while (pt != nullptr) {
            if (pt->key == key) {
                break;
            } else if (key > pt->key && !this->lesser.isLessThan(key, pt->next->key)) {
                pt = pt->next;
            } else if (key > pt->key && this->lesser.isLessThan(key, pt->next->key)) {
                pt = pt->down;
            }
        }

        if (pt == nullptr) return nullptr;

        while (pt->down != nullptr)
            pt = pt->down;
        return static_cast<LeafNode<T, TVal> *>(pt);
    }

    template<class T>
    void hookup_leaf_node(Node<T> *node, Node<T> *right) {
        node->next = right;
        node->prev = right->prev;
        right->prev->next = node;
        right->prev = node;
    }

    template<class T>
    void hookup_internal_node(Node<T> *node, Node<T> *right, Node<T> *down) {
        node->next = right;
        node->prev = right->prev;
        right->prev->next = node;
        right->prev = node;
        down->up = node;
        node->down = down;
    }

// Insert to skip list function
    template<class T, typename TLess, typename TVal>
    bool SkipList<T, TLess, TVal>::insert(T key, TVal value) {
        // rewritten based on https://www.geeksforgeeks.org/skip-list-set-2-insertion/
        /*    start from the highest level of skip list
            move the current pointer forward while key
            of node of pointer is not less than the key;
            move one level down and continue search
        */
        vector<Node<T> *> anchors = {};

        // at the highest level, locate the first node not less than key
        Node<T> *pt = this->Heads.back().getHeader();
        while (this->lesser.isLessThan(pt->key, key)) {
            pt = pt->next;
        }
        anchors.push_back(pt);

        // go through all subsequent levels
        int level = this->Heads.size() - 2;
        while (level >= 0) {
            assert(pt->key == pt->down->key);
            assert(!this->lesser.isLessThan(pt->key, key));
            pt = pt->down;
            while (!this->lesser.isLessThan(pt->prev->key, key)) {
                pt = pt->prev;
            }
            anchors.push_back(pt);
            level--;
        }

        // now we should have reached the first node not less than key, at the leaf level
        if (pt->key == key) {
            return false;
        }

        assert(this->lesser.isLessThan(pt->prev->key, key));
        assert(this->lesser.isLessThan(key, pt->key));
        Node<T> *leaf = new LeafNode<T, TVal>(key, value);
        hookup_leaf_node(leaf, pt);
        Node<T> *down = leaf;
        anchors.pop_back();

        // check to grow the skip tower from bottom up
        int Coin_Toss = rand() % 2;
        int i = 0;
        while (Coin_Toss == 0) {
            i++;
            if (anchors.size() == 0) {
                // need to create one more level
                Node<T> *NewHead = new Node<T>(Min<T>());
                Node<T> *NewTail = new Node<T>(Max<T>());

                // left/right pivots
                Heads[i - 1].getHeader()->up = NewHead;
                NewHead->down = Heads[i - 1].getHeader();
                Heads[i - 1].getTail()->up = NewTail;
                NewTail->down = Heads[i - 1].getTail();

                Heads.push_back(list(NewHead, NewTail));
                pt = NewTail;
            } else {
                pt = anchors.back();
                anchors.pop_back();
            }

            Node<T> *N = new Node<T>(key);
            hookup_internal_node(N, pt, down);
            down = N;

            Coin_Toss = rand() % 2;
        }
        return true;
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

    template<class T, typename TLess, typename TVal>
    SkipList<T, TLess, TVal>::~SkipList() {
        for (list &levelist: this->Heads) {
            Node<T> *p = levelist.getHeader();
            while (p) {
                auto next = p->next;
                delete p;
                p = next;
            }
        }
    }

    template<class T, typename TLess, typename TVal>
    typename SkipList<T, TLess, TVal>::Iterator SkipList<T, TLess, TVal>::lower_bound(T key) {
        // at the highest level, locate the first node not less than key
        Node<T> *pt = this->Heads.back().getHeader();
        while (this->lesser.isLessThan(pt->key, key)) {
            pt = pt->next;
        }

        // go through all subsequent levels
        int level = this->Heads.size() - 2;
        while (level >= 0) {
            assert(pt->key == pt->down->key);
            assert(!this->lesser.isLessThan(pt->key, key));
            pt = pt->down;
            while (!this->lesser.isLessThan(pt->prev->key, key)) {
                pt = pt->prev;
            }
            level--;
        }

        // now we should have reached the first node not less than key, at the leaf level
        assert(this->lesser.isLessThan(pt->prev->key, key));
        assert(!this->lesser.isLessThan(pt->key, key));
        return SkipList::Iterator(static_cast<LeafNode<T, TVal> *>(pt));
    }

    template<class T, typename TLess, typename TVal>
    typename SkipList<T, TLess, TVal>::Iterator SkipList<T, TLess, TVal>::upper_bound(T key) {
        auto p = this->lower_bound(key);
        // todo: change upper_bound to bigger than key
        // while (!this->lesser.isLessThan(key, p->key))
        //        ++p;
        return p;
    }
}
#endif
