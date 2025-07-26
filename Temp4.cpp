#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <limits>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <iterator>
#include <list>

using namespace std;

//----------------------------------------
// Custom hash for pair<int, int>
//----------------------------------------
struct PairHash {
    size_t operator()(const pair<int, int>& p) const {
        return hash<int>()(p.first) ^ (hash<int>()(p.second) << 1);
    }
};

//----------------------------------------
// Node structure
//----------------------------------------
struct Node {
    int id;
    vector<double> coordinates;

    Node* parent;
    Node* firstChild;
    Node* nextSibling;

    int depth;
    int descendantCount;

    Node(int id, const vector<double>& coords)
        : id(id), coordinates(coords), parent(nullptr), firstChild(nullptr),
          nextSibling(nullptr), depth(0), descendantCount(0) {}

    Node() : id(-1), parent(nullptr), firstChild(nullptr), nextSibling(nullptr), depth(0), descendantCount(0) {}
};

//----------------------------------------
// NDCache class (LRU Cache)
//----------------------------------------
class NDCache {
public:
    NDCache(size_t capacity) : capacity(capacity) {}

    bool getDominance(const Node* a, const Node* b) {
        auto key = make_pair(a->id, b->id);
        auto it = cache.find(key);
        if (it != cache.end()) {
            cacheList.splice(cacheList.begin(), cacheList, it->second);
            return cacheData[key];
        }
        return false;
    }

    void putDominance(const Node* a, const Node* b, bool relation) {
        auto key = make_pair(a->id, b->id);
        if (cache.find(key) == cache.end()) {
            if (cache.size() >= capacity) {
                auto last = cacheList.back();
                cache.erase(last);
                cacheData.erase(last);
                cacheList.pop_back();
            }
            cacheList.push_front(key);
        } else {
            cacheList.splice(cacheList.begin(), cacheList, cache[key]);
        }
        cache[key] = cacheList.begin();
        cacheData[key] = relation;
    }

private:
    size_t capacity;
    list<pair<int, int>> cacheList;
    unordered_map<pair<int, int>, list<pair<int, int>>::iterator, PairHash> cache;
    unordered_map<pair<int, int>, bool, PairHash> cacheData;
};

//----------------------------------------
// BJRTree class
//----------------------------------------
class BJRTree {
public:
    BJRTree(int lazy_depth = 3, int cache_size = 1000);
    ~BJRTree() = default;

    void inject(Node* newNode);
    void eject(Node* nodeToEject);
    vector<Node*> getSkyline() const;
    bool dominates(const Node* a, const Node* b);

private:
    unique_ptr<Node> root;
    NDCache cache;
    int lazyDepthThreshold;

    void recursiveInject(Node* r, Node* v);
    void addChild(Node* parent, Node* child);
    void removeChild(Node* parent, Node* child);
    void updateAncestorDescendants(Node* startNode, int delta);
};

BJRTree::BJRTree(int lazy_depth, int cache_size) : lazyDepthThreshold(lazy_depth), cache(cache_size) {
    root = make_unique<Node>();
}

bool BJRTree::dominates(const Node* a, const Node* b) {
    bool strictly_better = false;

    if (cache.getDominance(a, b)) return true;
    if (cache.getDominance(b, a)) return false;

    for (size_t i = 0; i < a->coordinates.size(); ++i) {
        if (a->coordinates[i] > b->coordinates[i]) return false;
        if (a->coordinates[i] < b->coordinates[i]) strictly_better = true;
    }

    cache.putDominance(a, b, strictly_better);
    cache.putDominance(b, a, !strictly_better);
    return strictly_better;
}

void BJRTree::addChild(Node* p, Node* c) {
    c->parent = p;
    c->depth = p->depth + 1;
    c->nextSibling = p->firstChild;
    p->firstChild = c;
    updateAncestorDescendants(p, 1 + c->descendantCount);
}

void BJRTree::removeChild(Node* p, Node* c) {
    Node** indirect = &p->firstChild;
    while (*indirect) {
        if (*indirect == c) {
            *indirect = c->nextSibling;
            updateAncestorDescendants(p, -(1 + c->descendantCount));
            return;
        }
        indirect = &((*indirect)->nextSibling);
    }
}

void BJRTree::updateAncestorDescendants(Node* startNode, int delta) {
    Node* current = startNode;
    while (current) {
        if (current->id != -1)
            current->descendantCount += delta;
        current = current->parent;
    }
}

void BJRTree::inject(Node* newNode) {
    if (!newNode) return;
    recursiveInject(root.get(), newNode);
}

void BJRTree::recursiveInject(Node* r, Node* v) {
    Node* target = nullptr;
    if (r->depth < lazyDepthThreshold) {
        int minDescendants = numeric_limits<int>::max();
        Node* currentChild = r->firstChild;
        while (currentChild) {
            if (dominates(currentChild, v) && currentChild->descendantCount < minDescendants) {
                target = currentChild;
                minDescendants = currentChild->descendantCount;
            }
            currentChild = currentChild->nextSibling;
        }
    }

    if (target) {
        recursiveInject(target, v);
        return;
    }

    vector<Node*> existingChildren;
    Node* currentChild = r->firstChild;
    while (currentChild) {
        existingChildren.push_back(currentChild);
        currentChild = currentChild->nextSibling;
    }

    addChild(r, v);

    for (Node* c : existingChildren) {
        if (c->parent == r && dominates(v, c)) {
            removeChild(r, c);
            addChild(v, c);
        }
    }
}

void BJRTree::eject(Node* v) {
    if (!v || !v->parent) return;
    Node* p = v->parent;
    vector<Node*> toReinsert;
    Node* child = v->firstChild;
    while (child) {
        Node* next = child->nextSibling;
        child->parent = nullptr;
        child->nextSibling = nullptr;
        toReinsert.push_back(child);
        child = next;
    }
    removeChild(p, v);
    for (Node* rein : toReinsert) {
        inject(rein);
    }
    delete v;
}

vector<Node*> BJRTree::getSkyline() const {
    vector<Node*> skyline_nodes;
    Node* current = root->firstChild;
    while (current) {
        skyline_nodes.push_back(current);
        current = current->nextSibling;
    }
    return skyline_nodes;
}

//----------------------------------------
// Main
//----------------------------------------
int main() {
    ifstream inputFile("F:\\Coding\\University\\Fourth Semester\\Ds\\dataset\\small.input");
    ifstream timeFile("F:\\Coding\\University\\Fourth Semester\\Ds\\dataset\\small.times");
    ifstream refFile("F:\\Coding\\University\\Fourth Semester\\Ds\\dataset\\small.refout");
    ofstream myOut("F:\\Coding\\University\\Fourth Semester\\Ds\\dataset\\small.myout");

    if (!inputFile.is_open()) { cerr << "❌ Cannot open input file\n"; return 1; }
    if (!timeFile.is_open()) { cerr << "❌ Cannot open times file\n"; return 1; }
    if (!refFile.is_open())  { cerr << "❌ Cannot open reference file\n"; return 1; }

    vector<vector<double>> points;
    vector<pair<int, int>> times;
    string line;

    while (getline(inputFile, line)) {
        istringstream iss(line);
        vector<double> coords((istream_iterator<double>(iss)), istream_iterator<double>());
        points.push_back(coords);
    }

    while (getline(timeFile, line)) {
        istringstream iss(line);
        int a, d;
        iss >> a >> d;
        times.emplace_back(a, d);
    }

    const int max_time = 110;
    BJRTree tree(3, 1000);  // Cache size = 1000
    map<int, Node*> activeNodes;

    for (int t = 0; t < max_time; ++t) {
        for (auto it = activeNodes.begin(); it != activeNodes.end();) {
            if (times[it->first].second == t) {
                tree.eject(it->second);
                it = activeNodes.erase(it);
            } else {
                ++it;
            }
        }

        for (int id = 0; id < points.size(); ++id) {
            if (times[id].first == t) {
                Node* n = new Node(id, points[id]);
                tree.inject(n);
                activeNodes[id] = n;
            }
        }

        vector<Node*> skyline = tree.getSkyline();
        vector<int> ids;
        for (Node* n : skyline) ids.push_back(n->id);
        sort(ids.begin(), ids.end());
        for (int id : ids) myOut << id << " ";
        myOut << "\n";
    }

    // Compare output
    myOut.close();
    ifstream myFile("F:\\Coding\\University\\Fourth Semester\\Ds\\dataset\\small.myout");
    if (!myFile.is_open()) { cerr << "❌ Cannot open generated output\n"; return 1; }

    cout << "\n--- Skyline Comparison ---\n";
    string refLine, myLine;
    int t = 0, matched = 0, total = 0;

    while (getline(refFile, refLine) && getline(myFile, myLine)) {
        ++total;
        if (refLine == myLine) {
            ++matched;
        } else {
            cout << "❌ Time " << t << " mismatch\n";
            cout << "Expected: " << refLine << "\n";
            cout << "Got     : " << myLine << "\n";
        }
        ++t;
    }

    cout << "\n✔ Matched " << matched << " out of " << total << " timesteps.\n";
    return 0;
}
