#ifndef NODE_H
#define NODE_H

#include <vector>
#include <limits>
using namespace std;
struct Node
{
    int id;
    std::vector<double> coordinates;

    Node *parent;
    Node *firstChild;
    Node *nextSibling;

    int depth;
    int descendantCount;

    // Constructor baraye data ha (bayad test beshe)
    Node(int id, const std::vector<double> &coords)
        : id(id), coordinates(coords), parent(nullptr), firstChild(nullptr),
          nextSibling(nullptr), depth(0), descendantCount(0) {}

    Node()
        : id(-1), parent(nullptr), firstChild(nullptr), nextSibling(nullptr),
          depth(0), descendantCount(0) {}
};

#endif