#ifndef BJR_TREE_H
#define BJR_TREE_H

#include "Node.h"
#include <vector>
#include <unordered_map>
#include <memory>
using namespace std;
class BJRTree
{
public:
    BJRTree(int lazy_depth = 3);

    ~BJRTree();

    void inject(Node *newNode);

    void eject(Node *nodeToEject);

    vector<Node *> getSkyline() const;

    static bool dominates(const Node *a, const Node *b);

    unordered_map<int, std::unique_ptr<Node>> &getNodeMap();

private:
    unique_ptr<Node> root;

    unordered_map<int, std::unique_ptr<Node>> nodeMap;
    int lazyDepthThreshold;

    void recursiveInject(Node *r, Node *v);

    void addChild(Node *parent, Node *child);
    void removeChild(Node *parent, Node *child);

    void updateAncestorDescendants(Node *startNode, int delta);
};

#endif