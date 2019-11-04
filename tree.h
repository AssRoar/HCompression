#include <string>

class HTree{
	struct node{
		bool list;
		int64_t sz;
		std::string name;
		node(char, int64_t);
		node(node *, node *);
		node * l;
		node * r;
	};

	node * root = nullptr;

	struct NodeCMP{
		bool operator()(const node * a, const node * b) const;
	};

	int cnt[256];

	void addChar(char);
};
