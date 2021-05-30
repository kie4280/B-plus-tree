#include <iostream>
#include <memory>
#include <vector>

struct Node;

struct Node {
  Node *parent = nullptr, *left = nullptr, *right = nullptr;
  int order;
  bool is_leaf = false;
  std::vector<std::unique_ptr<Node>> children;
  std::vector<int> keys;

  Node(int order) : order(order), children(order + 1), keys(order) {}
  Node(const Node &old) {
    // children = old.children;
    keys = old.keys;
    parent = old.parent;
    left = old.left;
    right = old.right;
    order = old.order;
  }
};

class BTree {
 public:
  BTree(int order);
  ~BTree();
  BTree(const BTree &old);
  void insert(int val);
  Node *search(int val);
  Node *traverse(int val);

 private:
  std::unique_ptr<Node> root;
  int order;
};

BTree::BTree(int order)  :order(order) {
  Node *n = new Node(order);
  n->is_leaf = true;
  root.reset(n);
}

BTree::~BTree() {}

inline Node *BTree::traverse(int val) {
  Node *cur = root.get();
  while (!cur->is_leaf) {
    auto it = cur->children.begin();
    for (int i = 0; i < cur->keys.size() && val > cur->keys[i]; ++i) {
      ++it;
    }
    cur = (*it).get();
  }
  return cur;
}

void BTree::insert(int val) {
  Node *node = traverse(val);
  auto it = node->keys.begin();
  int index = 0;
  for (; it != node->keys.end() && val > *it; ++it, ++index)
    ;

  node->keys.insert(it, val);
  while (true) {
    if (node->keys.size() >= order) {
      std::unique_ptr<Node> new_r;
      Node *parent = node->parent;
      if (parent == nullptr) {
        new_r.reset(new Node(order));
        parent = new_r.get();
      }
      if (node->is_leaf) {
        int middle_i = order / 2;
        int middle = node->keys[middle_i];
        auto it_k = parent->keys.begin();
        auto it_c = parent->children.begin();
        for (; it_k != node->keys.end() && middle > *it_k; ++it_k, ++it_c)
          ;
        parent->keys.insert(it_k, middle);
        std::unique_ptr<Node> nl(new Node(order));
        nl->is_leaf = true;
        nl->right = node;
        nl->parent = parent;
        nl->keys.assign(node->keys.begin(), node->keys.begin() + middle_i);
        node->keys.erase(node->keys.begin(), node->keys.begin() + middle_i + 1);
        node->parent = parent;
        parent->children.insert(it_c, std::move(nl));
      } else {
        int middle_i = order / 2;
        int middle = node->keys[middle_i];
        std::unique_ptr<Node> left(new Node(order)), right(new Node(order));
        left->parent = parent;
        right->parent = parent;
        left->keys.assign(node->keys.begin(), node->keys.begin() + middle_i);
        right->keys.assign(node->keys.begin() + middle_i + 1, node->keys.end());
        
      }
    } else {
      break;
    }
    node = node->parent;
  }
}

Node *BTree::search(int val) {
  Node *leaf = traverse(val);
  return nullptr;
}

int main(int, char **) { std::cout << "Hello, world!\n"; }
