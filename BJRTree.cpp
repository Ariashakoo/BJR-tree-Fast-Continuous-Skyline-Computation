#include "BJRTree.h"
#include <stdexcept>
#include <algorithm>
#include <limits>
using namespace std;
BJRTree::BJRTree(int lazy_depth) : lazyDepthThreshold(lazy_depth)
{
    root = make_unique<Node>();
}

BJRTree::~BJRTree() = default;

unordered_map<int, unique_ptr<Node>> &BJRTree::getNodeMap()
{
    return nodeMap;
}

bool BJRTree::dominates(const Node *a, const Node *b)
{
    // Seyed :)
}

void BJRTree::addChild(Node *p, Node *c)
{
    c->parent = p;
    c->depth = p->depth + 1;
    c->nextSibling = p->firstChild;
    p->firstChild = c;
    updateAncestorDescendants(p, 1 + c->descendantCount);
}

void BJRTree::removeChild(Node *p, Node *c)
{
    // Seyed :)
}

void BJRTree::updateAncestorDescendants(Node *startNode, int delta)
{
    Node *current = startNode;
    while (current)
    {
        if (current->id != -1)
        {
            current->descendantCount += delta;
        }
        current = current->parent;
    }
}

void BJRTree::inject(Node *newNode)
{
    if (!newNode)
        return;
    recursiveInject(root.get(), newNode);
}

void BJRTree::eject(Node *v)
{
    // Seyed :)
}

// Injection logic with balancing and lazy evaluation, from Algorithm 3[cite: 328, 1258].
void BJRTree::recursiveInject(Node *r, Node *v)
{
    Node *target = nullptr;

    if (r->depth < lazyDepthThreshold)
    {
        int minDescendants = std::numeric_limits<int>::max();
        Node *currentChild = r->firstChild;
        while (currentChild)
        {
            if (dominates(currentChild, v))
            {
                if (currentChild->descendantCount < minDescendants)
                {
                    target = currentChild;
                    minDescendants = currentChild->descendantCount;
                }
            }
            currentChild = currentChild->nextSibling;
        }
    }

    if (target)
    {
        recursiveInject(target, v);
        return;
    }

    std::vector<Node *> existingChildren;
    Node *currentChild = r->firstChild;
    while (currentChild)
    {
        existingChildren.push_back(currentChild);
        currentChild = currentChild->nextSibling;
    }

    addChild(r, v);

    for (Node *c : existingChildren)
    {
        if (c->parent == r && dominates(v, c))
        {
            removeChild(r, c);
            addChild(v, c);
        }
    }
}

std::vector<Node *> BJRTree::getSkyline() const
{
    std::vector<Node *> skyline_nodes;
    Node *current = root->firstChild;
    while (current)
    {
        skyline_nodes.push_back(current);
        current = current->nextSibling;
    }
    return skyline_nodes;
}