#include "tree.h"

HTree::node::node(char c_, int64_t sz_):
	name(),
	sz(sz_),
	list(true),
	l(nullptr),
	r(nullptr){
		name += c_;
};
	
HTree::node::node(node * left, node * right):
	name(left -> name + right -> name),
	sz(left -> sz + right -> sz),
	list(false),
	l(left),
	r(right){};

bool HTree::NodeCMP::operator()(const node * a, const node * b)const{
                        if (a -> sz == b -> sz)
                                return a -> name < b -> name;
                        return a -> sz < b -> sz;
}

void HTree::addChar(char nc){
	++cnt[nc];
}
