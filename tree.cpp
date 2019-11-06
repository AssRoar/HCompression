#include "tree.h"

HTree::node::node(char c_, int64_t sz_):
        sz(sz_),
        list(true),
        l(nullptr),
        r(nullptr){
    name = c_;
};

HTree::node::node(node * left, node * right):
        sz(left -> sz + right -> sz),
        list(false){l = left; r = right;};

bool HTree::NodeCMP::operator()(const node * a, const node * b)const{
    return a -> sz < b -> sz;
}

void HTree::addChar(char nc){
    assert(ended == false);
    ++cnt[nc];
}

void HTree::build(){
    ended = true;
    std::multiset<node *, NodeCMP> order;
    for (int i = 0; i <=  255; i++)
        if (cnt[i] != 0){
            order.insert(new node(i, cnt[i]));
        }
    if (order.size() == 1){
        node * nl = *order.begin();
        root = nl;
        code[nl->name] += '1';
        root = *order.begin();
        return;
    }
    while (order.size() > 1){
        node * nl = *order.begin();
        order.erase(order.begin());
        node * nr = *order.begin();
        order.erase(order.begin());
        order.insert(new node(nl, nr));
    }
    root = *order.begin();
    setValues(std::string(), root);
}

void HTree::setValues(std::string pref, node * rt) {
    if (rt->list) {
        code[rt->name] = pref;
    } else {
        setValues(pref + '0', rt->l);
        setValues(pref + '1', rt->r);
    }
}

std::vector<std::pair<char, std::string> > HTree::dump() {
    std::vector<std::pair<char, std::string> > res;
    for (int c = 0; c < 256; c++)
        if (cnt[c]) {
            res.emplace_back(c, code[c]);
            std::cerr << "dmp: " << c << ' ' << code[c] << '\n';
        }
    return res;
}

std::string HTree::toString(int bit, int cnt) {
    std::string res;
    for (int i = 0; i < cnt; i++)
        res += ('0' + (cnt & (1 << i) ? 1 : 0));
    return res;
}

HTree::HTree(std::string s) {
    for (int i = 0; i<256; i++)
        cnt[i] = 0;
    code.assign(256, std::string());
    for (char c:s)
        addChar(c);
    build();
    for (int i = 0; i < 256; i++)
        if (cnt[i] != 0)
            decode[code[i]] = i;
}

HTree::HTree() {
    for (int i = 0; i<256; i++)
        cnt[i] = 0;
    code.assign(256, std::string());
    root = nullptr;
}

std::vector<unsigned char> HTree::toCode(std::string & s) {
    int n = 0;
    for (char c:s)
        n += code[c].size();
    if (n % 8 != 0){
        n += 8;
        n -= n % 8;
    }
    std::vector<unsigned char> res(n / 8, 0);
    int curByte = 0;
    int bit = 0;
    std::cerr << "\n----codind----\n";
    for (char c:s) {
        for (char mybit:code[c]) {
            std::cerr << mybit;
            if (mybit == '1') {
                res[curByte] |= (1 << bit);
            }
            ++bit;
            if (bit == 8){
                bit = 0;
                curByte++;
            }
        }
    }
    std::cerr << '\n';
    for (auto c:res)
        std::cerr << int(c) << ' ';
    std::cerr << '\n';
    std::cerr << "\n----end----\n";
    return res;
}

std::string HTree::toStdString(std::vector<unsigned char> v, int cnt) {
    std::cerr << "\n----dekoding---- " << cnt << '\n';
    for (auto c:v)
        std::cerr << int(c) << ' ';
    std::cerr << '\n';
    int byte = 1;
    int bit = 0;
    std::string cur;
    std::string ans;
    while(cnt--){
        while(!decode.count(cur)){
            cur += ((v[byte] & (1 << bit)) ? '1' : '0');
            std::cerr << cur.back();
            ++bit;
            if (bit == 8){
                bit = 0;
                byte++;
            }
        }
        ans += decode[cur];
//        std::cerr << "find: " << cur << ' ' << char(decode[cur]) << '\n';
        cur.resize(0);
    }
    std::cerr << "\n----end----\n";
    return ans;
}

void HTree::initByFile(std::string name) {
    std::ifstream fin(name);
    std::cerr << "start init\n";
    int c;
    while (fin >> c){
        fin >> code[c];
        std::cerr << code[c] << '\n';
        decode[code[c]] = c;
    }
}
