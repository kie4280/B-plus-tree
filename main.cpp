#include <iostream>
#include <memory>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

struct Node;

struct Node {
  Node *right = nullptr;
  int order;
  bool is_leaf = false;
  std::vector<std::shared_ptr<Node>> children;
  std::vector<int> keys;
  std::weak_ptr<Node> parent;
  int level = 0;

  Node(int order) : order(order) {
    children.reserve(order + 1);
    keys.reserve(order);
  }
  Node(const Node &old) {
    children = old.children;
    keys = old.keys;
    parent = old.parent;
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
  void printTree();
  std::shared_ptr<Node> search(int val);
  std::shared_ptr<Node> traverse(int val);

 private:
  std::shared_ptr<Node> root;
  int order;
};

BTree::BTree(int order) : order(order) {
  root = std::make_shared<Node>(order);
  root->is_leaf = true;
}

BTree::~BTree() {}

void BTree::printTree() {
  using namespace std;

  std::stack<std::shared_ptr<Node>> explore;
  int root_level = root->level;
  explore.emplace(root);
  while (!explore.empty()) {
    auto cur = explore.top();
    explore.pop();
    for (int a = 0; a < root_level - cur->level; ++a) {
      cout << "  ";
    }
    cout << "(";
    for (int a = 0; a < cur->keys.size(); ++a) {
      if (a != 0) {
        cout << " ";
      }
      cout << cur->keys[a];
    }
    cout << ")" << endl;
    for (auto it = cur->children.rbegin(); it != cur->children.rend(); ++it) {
      explore.emplace(*it);
    }
  }
  cout << endl << endl;
}

inline std::shared_ptr<Node> BTree::traverse(int val) {
  std::shared_ptr<Node> cur = root;
  while (!cur->is_leaf) {
    auto it = cur->children.begin();
    for (int i = 0; i < cur->keys.size() && val > cur->keys[i]; ++i) {
      ++it;
    }
    cur = (*it);
  }
  return cur;
}

void BTree::insert(int val) {
  std::shared_ptr<Node> node = traverse(val);
  auto it = node->keys.begin();
  int index = 0;
  for (; it != node->keys.end() && val > *it; ++it, ++index)
    ;

  node->keys.insert(it, val);
  while (true) {
    if (node->keys.size() >= order) {
      std::shared_ptr<Node> parent = node->parent.lock();
      if (parent == nullptr) {
        parent = std::make_shared<Node>(order);
        parent->children.push_back(node);
        root = parent;
        parent->level = node->level + 1;
      }
      if (node->is_leaf) {
        int middle_i = order / 2;
        int middle = node->keys[middle_i];

        auto it_k = parent->keys.begin();
        auto it_c = parent->children.begin();
        for (; it_k != parent->keys.end() && middle > *it_k; ++it_k, ++it_c)
          ;
        parent->keys.insert(it_k, middle);
        std::shared_ptr<Node> nl = std::make_shared<Node>(order);
        nl->is_leaf = true;
        nl->right = node.get();
        nl->parent = parent;
        nl->keys.assign(node->keys.begin(), node->keys.begin() + middle_i);
        node->keys.erase(node->keys.begin(), node->keys.begin() + middle_i);
        node->parent = parent;
        parent->children.insert(it_c, std::move(nl));

      } else {
        int middle_i = order / 2;
        int middle = node->keys[middle_i];
        auto it_k = parent->keys.begin();
        auto it_c = parent->children.begin();
        for (; it_k != parent->keys.end() && middle > *it_k; ++it_k, ++it_c)
          ;
        parent->keys.insert(it_k, middle);

        std::shared_ptr<Node> nl = std::make_shared<Node>(order);
        nl->parent = parent;
        nl->level = node->level;
        nl->keys.assign(node->keys.begin(), node->keys.begin() + middle_i);
        node->keys.erase(node->keys.begin(), node->keys.begin() + middle_i + 1);
        nl->children.assign(node->children.begin(),
                            node->children.begin() + middle_i + 1);
        node->children.erase(node->children.begin(),
                             node->children.begin() + middle_i + 1);
        for (auto &c : nl->children) {
          c->parent = nl;
        }
        node->parent = parent;
        parent->children.insert(it_c, std::move(nl));
      }
    } else {
      break;
    }
    node = node->parent.lock();
  }
}

std::shared_ptr<Node> BTree::search(int val) {
  std::shared_ptr<Node> cur = traverse(val);
  return nullptr;
}

int main(int, char **) {
  using namespace std;
  int ORDER;
  cin >> ORDER;

  BTree bt(ORDER);
  string line;
  getline(cin, line);
  while (getline(cin, line)) {
    stringstream ss(line);
    char op;
    int x, N;
    ss >> op;
    switch (op) {
      case 'i':

        ss >> x;
        bt.insert(x);
        break;
      case 's':

        ss >> x;
        break;
      case 'p':
        bt.printTree();
        break;
      case 'a':

        ss >> x >> N;
        break;
      case 'q':
        goto exitProg;
        break;

      default:
        break;
    }
  }
exitProg:;
}
