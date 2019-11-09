#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <cassert>

const int N = 600;

struct node{
    node * l;
    node * r;
    int sz;
    bool term;
    unsigned char value = 0;

    node(unsigned char value_, int sz_):
        l(nullptr), r(nullptr), sz(sz_), term(true), value(value_){};

    node(node * l_, node * r_):
        l(l_), r(r_), sz(l_ -> sz + r -> sz), term(false){};
    node():
        term (false), l(nullptr), r(nullptr), sz(0), value(0){};
};

struct cmp{
    bool operator()(const node * l, const node * r)const{
        return l -> sz < r -> sz;
    }
};

void set_code(node *root, std::string s, std::vector<std::string> & code){
    if (root -> term){
        code[root->value] = s;
    } else{
        set_code(root->l, s + '1', code);
        set_code(root->r, s + '0', code);
    }
}

void compress(std::string name, std::string out_name){
    std::ifstream fin(name, std::ios::binary);
    std::vector<unsigned char> input(std::istreambuf_iterator<char>(fin), {});
    if (input.empty()){
        std::cout << "empty file\n";
        return;
    }
    std::vector<int64_t> cnt(256, 0);
    for (auto c : input)
        ++cnt[c];
    std::vector<std::string> code(256);
    std::multiset<node *, cmp> st;
    for (int i = 0; i < 256; i++)
        if (cnt[i] != 0)
            st.insert(new node(i, cnt[i]));
    node * root = nullptr;
    while (st.size() > 1){
        node * l = *st.begin();
        st.erase(st.begin());
        node * r = *st.begin();
        st.erase(st.begin());
        st.insert(new node(l, r));
    }
    root = *st.begin();

    int rcnt = 0;
    if (root -> term){
        code[root->value] = "1";
        rcnt = 1;
    } else {
        set_code(root, std::string(), code);
        for (int i = 0; i < 256; i++)
            if (cnt[i] != 0) {
                rcnt++;
            }
    }

    std::ofstream out(out_name, std::ios::out | std::ios::binary);
    std::string num = std::to_string(rcnt);
    out.write(num.c_str(), num.size());
    out.write("\n", 1);
    for (int i = 0; i < 256; i++)
        if (cnt[i] != 0){
            out.write(std::to_string(i).c_str(), std::to_string(i).size());
            out.write(" ", 1);
            out.write(code[i].c_str(), code[i].size());
            out.write("\n", 1);
        }
    int64_t code_len = 0;
    for (auto c:input)
        code_len += code[c].size();

    if (code_len % 8 != 0)
        code_len = (code_len + 8) - code_len % 8;

    code_len/=8;
    auto * res = new char[code_len];
    for (int i = 0; i < code_len; i++)
        res[i] = 0;
    int byte = 0;
    unsigned int bit = 0;
    for (unsigned char sym:input)
        for (char c:code[sym]){
            if (c == '1')
                res[byte] |= (1 << bit);
            ++bit;
            if (bit == 8){
                ++byte;
                bit = 0;
            }
        }
    std::string nums = std::to_string(input.size()) + ' ' + std::to_string(code_len) + '\n';
    out.write(nums.c_str(), nums.size());
    out.write(res, code_len);
}

void decompress(std::string name, std::string out_name){
    std::vector<std::string> code(256);
    std::ifstream fin;
    fin.open(name);
    int n;
    fin >> n;
    node * root = new node;
    node * cur = root;
    for (int i = 0; i<n; i++){
        int val;
        fin >> val;
        fin >> code[val];
    }
    for (int i = 0; i<256; i++)
        if (!code[i].empty()){
            for (auto c:code[i]){
                if (c == '1'){
                    if (cur -> l == nullptr)
                        cur -> l = new node;
                    cur = cur -> l;
                }
                if (c == '0'){
                    if (cur -> r == nullptr)
                        cur -> r = new node;
                    cur = cur -> r;
                }
            }
            cur -> value = i;
            cur -> term = true;
            cur = root;
        }
    int ans_size, code_len;
    fin >> ans_size >> code_len;
    fin.close();
    fin.open(name, std::ios::binary);
    std::vector<unsigned char> input(std::istreambuf_iterator<char>(fin), {});
    std::ofstream out(out_name, std::ios::out | std::ios::binary);
    int stpos = 0;
    int enter_cnt = 0;
    for (stpos =  0; enter_cnt < n + 2; stpos++)
        enter_cnt += (input[stpos] == '\n');
    int byte = 0;
    int bit = 0;
    cur = root;
    auto * ans = new unsigned char[ans_size];
    int cur_pos = 0;
    for (byte = stpos; byte<input.size() && cur_pos < ans_size; byte++){
        for (bit = 0; bit < 8 && cur_pos < ans_size; bit++){
            if (input[byte] & (1 << bit)){
                cur = cur -> l;
            } else
                cur = cur -> r;
            if (cur -> term){
                ans[cur_pos++] = cur -> value;
                cur = root;
            }
        }
    }
    std::cerr << cur_pos << '\n';
    out.write((char *)&ans[0], ans_size);
}

int main() {
    compress("test.png", "test.myrar");
    decompress("test.myrar", "ans.png");
    return 0;
}
