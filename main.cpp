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

unsigned char * to_code(unsigned char c, std::string & s){ // size =
    std::cout << int(c) << ' ' << s.size() << ' ' << s << '\n';
    int bytes = s.size() / 8 + !!s.size();
    int bits = s.size();
    unsigned char * l = new unsigned char[6];
    for (int i = 0; i<6; i++)
        l[i] = 0;
    l[0] = c;
    for (int bit = 0; bit < 16; ++bit)
        if (bits & (1 << bit))
            l[bit / 8 + 1] |= (1 << (bit % 8));
    l[1] = bits;
    l[2] = bits >> 8;
    std::cout << bits << ' ' << (int)l[1] << ' ';
    for (int bit = 0; bit < s.size(); ++bit){
        if (s[bit] == '1')
            l[bit/8 + 3] |= (1 << (bit % 8));
    }
    return l;
}

unsigned char * to_code(size_t n){ // 8 bytes
    unsigned char * ans = new unsigned char [8];
    for (int i = 0; i<8; i++)
        ans[i] = 0;

    ans[0] = n;
    ans[1] = n >> 8;
    ans[2] = n >> 16;
    ans[3] = n >> 32;
    ans[4] = n >> 40;
    ans[5] = n >> 48;
    return ans;
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
    auto * ans = new unsigned char[8];
    for (int i = 0; i<8; i++)
        ans[i] = 0;
    for (int bit = 0; bit < 50; ++bit){
        int byte = bit/8;
        if (rcnt & (1 << bit)){
            ans[byte] |= (1 << (bit % 8));
        }
    }
    std::cerr << rcnt << '\n';
    out.write((char *)&ans[0], 8);


    for (int i = 0; i < 256; i++)
        if (cnt[i] != 0){
            unsigned char * re = to_code(i, code[i]);
            out.write((char *)re, 6);
        }
    int64_t code_len = 0;
    for (auto c:input)
        code_len += code[c].size();

    if (code_len % 8 != 0)
        code_len = (code_len + 8) - code_len % 8;

    code_len/=8;
    auto * res = new unsigned char[code_len];
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
    auto a = to_code(input.size());
    auto b = to_code(code_len);
    out.write((char *)a, 8);
    out.write((char *)b, 8);
    out.write((char*)res, code_len);
    std::cout << "ST: " << (int) res[0] << '\n';
}

void decompress(std::string name, std::string out_name){
    std::vector<std::string> code(256);
    std::ifstream fin;
    fin.open(name, std::ios::binary);
    std::vector<unsigned char> input(std::istreambuf_iterator<char>(fin), {});
    size_t n = 0;
    for (int bit = 0; bit < 50; ++bit){
        int byte = bit/8;
        if (input[byte] & (1 << (bit % 8)))
            n |= (1 << bit);
    }
    node * root = new node;
    node * cur = root;
    for (int i = 0; i<n; i++){
     std::vector<unsigned char> v(6);
     int st = 6 * i + 8;
     for (int byte = 0; byte < 8; ++byte)
         v[byte] = input[st + byte];
     std::cout << (int) v[1] << ' ';
     int ch = v[0];
     int len = 0;
     for (int bit = 0; bit < 16; ++bit)
         if (v[1 + bit/8] & (1 << ((bit) % 8)))
            len |= (1 << bit);
     std::string res_code;
     for (int bit = 0; bit < len; ++bit){
         int byte = bit / 8 + 3;
         int b = bit % 8;
         if (v[byte] & (1 << b))
             res_code += '1';
         else
             res_code += '0';
     }
     std::cout << (int) ch <<' ' << len <<  ' ' << res_code << '\n';
     code[ch] = res_code;
    }

    uint64_t ans_size = 0, code_len = 0;
    {
        uint64_t st = 8 + 6 * n;
        std::vector<unsigned char> a(8);
        std::vector<unsigned char> b(8);

        for (uint64_t i = 0; i < 8; i++)
            a[i] = input[i + st];
        for (uint64_t i = 0; i < 8; i++)
            b[i] = input[i + st + 8];

        for (uint64_t bit = 0; bit < 50; ++bit){
            uint64_t byte = bit/8;
            uint64_t B = bit % 8;
            if (a[byte] & ((uint64_t)1 << B))
                ans_size |= ((uint64_t)1 << bit);
        }
        for (uint64_t bit = 0; bit < 50; ++bit){
            uint64_t byte = bit/8;
            uint64_t B = bit % 8;
            if (b[byte] & ((uint64_t)1 << B))
                code_len |= ((uint64_t)1 << bit);
        }
    }

    assert(ans_size < 1e8);

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

     std::ofstream out(out_name, std::ios::out | std::ios::binary);
     int stpos = 24 + n * 6;
     std::cout << "ST: " << (int) input[stpos] << '\n';

     int byte = 0;
     int bit = 0;
     cur = root;
     auto * ans = new unsigned char[ans_size];
     for (int i = 0; i < ans_size; i++)
         ans[i] = 0;
     int cur_pos = 0;
     for (byte = stpos; byte<input.size() && cur_pos < ans_size; byte++){
         for (bit = 0; bit < 8 && cur_pos < ans_size; bit++){
             if (input[byte] & (1 << bit)){
                 cur = cur -> l;
             } else
                 cur = cur -> r;
             if (cur -> term){
                 std::cout << cur -> value;
                 ans[cur_pos++] = cur -> value;
                 cur = root;
             }
         }
     }
     std::cerr << cur_pos << '\n';
     out.write((char *)&ans[0], ans_size);
}

int main() {
    compress("test.jpg", "test.myrar");
    decompress("test.myrar", "ans.jpg");
    return 0;
}
