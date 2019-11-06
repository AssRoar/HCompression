#include <string>
#include <cassert>
#include <set>
#include <fstream>
#include <vector>
#include <iostream>
#include <unordered_map>

class HTree{
    struct node{
        bool list;
        int64_t sz;
        char name;
        node(char, int64_t);
        node(node *, node *);
        node * l;
        node * r;
    };
    std::unordered_map<std::string, int> decode;
    bool ended = false;
    node * root = nullptr;
    struct NodeCMP{
        bool operator()(const node * a, const node * b) const;
    };

    unsigned int cnt[256];
    std::vector<std::string> code;

    void addChar(char);
    void build();
    void setValues(std::string pref, node * rt);
    std::string toString(int bit, int cnt);
public:
    std::vector<std::pair<char, std::string> > dump();
    HTree(std::string);
    std::vector<unsigned char> toCode(std::string & s);
    std::string toStdString(std::vector<unsigned char>, int);
    void initByFile(std::string name);
    HTree();
};
