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
  bool search(int val);
  int seqAccess(int x, int N);

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
  cout << endl;
}

void BTree::insert(int val) {
  std::shared_ptr<Node> node = root;
  while (!node->is_leaf) {
    auto it = node->children.begin();
    for (int i = 0; i < node->keys.size() && val > node->keys[i]; ++i) {
      ++it;
    }
    node = (*it);
  }
  auto it = node->keys.begin();
  int index = 0;
  for (; it != node->keys.end() && val > *it; ++it, ++index)
    ;

  node->keys.insert(it, val);
  while (node->keys.size() >= order) {
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
      nl->right = node->right;
      nl->parent = parent;
      nl->keys.assign(node->keys.begin() + middle_i, node->keys.end());
      node->keys.erase(node->keys.begin() + middle_i, node->keys.end());
      node->parent = parent;
      node->right = nl.get();
      *(it_c) = std::move(nl);
      parent->children.insert(it_c, node);

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

    node = node->parent.lock();
  }
}

bool BTree::search(int val) {
  std::shared_ptr<Node> node = root;
  while (!node->is_leaf) {
    auto it = node->children.begin();
    for (int i = 0; i < node->keys.size() && val >= node->keys[i]; ++i) {
      ++it;
    }
    std::cout << "(";
    for (int i = 0; i < node->keys.size(); ++i) {
      if (i != 0) {
        std::cout << " ";
      }
      std::cout << node->keys[i];
    }
    std::cout << ")" << std::endl;
    node = (*it);
  }
  bool found = false;
  std::cout << "(";
  for (int i = 0; i < node->keys.size(); ++i) {
    if (node->keys[i] == val) {
      found = true;
    }
    if (i != 0) {
      std::cout << " ";
    }
    std::cout << node->keys[i];
  }
  std::cout << ")" << std::endl;
  return found;
}

int BTree::seqAccess(int x, int N) {
  std::shared_ptr<Node> node = root;
  while (!node->is_leaf) {
    auto it = node->children.begin();
    for (int i = 0; i < node->keys.size() && x >= node->keys[i]; ++i) {
      ++it;
    }
    node = (*it);
  }

  int counter = -1;
  for (int i = 0; i < node->keys.size(); ++i) {
    if (counter == -1 && node->keys[i] == x) {
      counter = 0;
    }
    if (counter >= 0 && counter < N) {
      if (counter != 0) {
        std::cout << " ";
      }
      std::cout << node->keys[i];
      ++counter;
    }
  }
  Node *read = node->right;

  while (read != nullptr && counter >= 0 && counter < N) {
    for (int i = 0; i < read->keys.size() && counter < N; ++i) {
      std::cout << " ";
      std::cout << read->keys[i];
      ++counter;
    }
    read = read->right;
  }
  if (counter >= 0) {
    std::cout << std::endl;
  }

  return counter;
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
    int x, N, read;
    bool f;
    ss >> op;
    switch (op) {
      case 'i':
        ss >> x;
        bt.insert(x);
        break;
      case 's':
        ss >> x;
        f = bt.search(x);
        if (f) {
          cout << "Found" << endl << endl;
        } else {
          cout << "QAQ" << endl << endl;
        }
        break;
      case 'p':
        bt.printTree();
        break;
      case 'a':
        ss >> x >> N;
        read = bt.seqAccess(x, N);
        if (read < 0) {
          cout << "Access Failed" << endl << endl;
        } else if (read < N) {
          cout << "N is too large" << endl << endl;
        } else {
          cout << endl;
        }
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
