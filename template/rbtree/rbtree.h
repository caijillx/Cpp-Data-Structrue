#include <functional>
#include <queue>

#include "template/define.h"

template <Comparable T>
struct RBTreeNode {
  RBTreeNode* parent = nullptr;
  RBTreeNode* lchild = nullptr;
  RBTreeNode* rchild = nullptr;
  Color color = Color::RED;
  T value;

  template <class Args>
  RBTreeNode(Args&& args) : value(std::forward<Args>(args)) {}
};

template <Comparable T, class Comparator = std::less<T>>
class RBTree {
 public:
  using Node = RBTreeNode<T>;

  auto find(T val) const -> Node*;
  bool insert(T val);
  bool remove(T val);
  void erase(Node* node);

  ~RBTree();

 private:
  inline bool compare(Node* a, Node* b) const noexcept {
    return comp_(a->value, b->value);
  }

  inline bool is_black(Node* node) {
    return node == nullptr || node->color == Color::BLACK;
  }

  void left_rotate(Node* node) noexcept;
  void right_rotate(Node* node) noexcept;
  void transplant(Node* node, Node* replace) noexcept;
  auto successor(Node* node) noexcept -> Node*;

  void insert_fix(Node* node) noexcept;
  void remove_fix(Node* node) noexcept;

  Node* root = nullptr;
  Comparator comp_;
};

template <Comparable T, class U>
auto RBTree<T, U>::find(T val) const -> Node* {
  Node* cur = root;
  while (cur != nullptr) {
    if (comp_(val, cur->value)) {
      cur = cur->lchild;
    } else if (comp_(cur->value, val)) {
      cur = cur->rchild;
    } else {
      return cur;
    }
  }
  return nullptr;
}

template <Comparable T, class U>
bool RBTree<T, U>::insert(T val) {
  Node** pparent = &root;
  Node* parent = nullptr;
  while (*pparent != nullptr) {
    parent = *pparent;
    if (comp_(val, parent->value)) {
      pparent = &parent->lchild;
    } else if (comp_(parent->value, val)) {
      pparent = &parent->rchild;
    } else {
      return false;
    }
  }
  Node* node = new Node(val);
  *pparent = node;
  node->parent = parent;
  insert_fix(node);
  return true;
}

template <Comparable T, class U>
bool RBTree<T, U>::remove(T val) {
  Node* node = find(val);
  if (node == nullptr) return false;
  erase(node);
  return true;
}

template <Comparable T, class U>
void RBTree<T, U>::erase(Node* node) {
  if (node == nullptr) return;
  if (node->lchild != nullptr && node->rchild != nullptr) {
    Node* s = successor(node);
    node->value = s->value;
    node = s;
  }
  Node* replace = node->lchild != nullptr ? node->lchild : node->rchild;
  if (replace != nullptr) {
    transplant(node, replace);
    if (node->color == Color::BLACK) {
      remove_fix(replace);
    }
  } else if (node->parent == nullptr) {
    root = nullptr;
  } else {
    if (node->color == Color::BLACK) {
      remove_fix(node);
    }
    if (node->parent != nullptr) {
      if (node == node->parent->lchild) {
        node->parent->lchild = nullptr;
      } else {
        node->parent->rchild = nullptr;
      }
    }
  }
  delete node;
}

template <Comparable T, class U>
void RBTree<T, U>::left_rotate(Node* node) noexcept {
  Node* rchild = node->rchild;
  node->rchild = rchild->lchild;
  if (rchild->lchild != nullptr) {
    rchild->lchild->parent = node;
  }
  Node* parent = node->parent;
  rchild->parent = parent;
  if (parent == nullptr) {
    root = rchild;
  } else if (node == parent->lchild) {
    parent->lchild = rchild;
  } else {
    parent->rchild = rchild;
  }
  rchild->lchild = node;
  node->parent = rchild;
}

template <Comparable T, class U>
void RBTree<T, U>::right_rotate(Node* node) noexcept {
  Node* lchild = node->lchild;
  node->lchild = lchild->rchild;
  if (lchild->rchild != nullptr) {
    lchild->rchild->parent = node;
  }
  Node* parent = node->parent;
  lchild->parent = parent;
  if (parent == nullptr) {
    root = lchild;
  } else if (node == parent->lchild) {
    parent->lchild = lchild;
  } else {
    parent->rchild = lchild;
  }
  lchild->rchild = node;
  node->parent = lchild;
}

template <Comparable T, class U>
void RBTree<T, U>::transplant(Node* node, Node* replace) noexcept {
  if (node->parent == nullptr) {
    root = replace;
  } else if (node == node->parent->lchild) {
    node->parent->lchild = replace;
  } else {
    node->parent->rchild = replace;
  }
  replace->parent = node->parent;
}

template <Comparable T, class U>
auto RBTree<T, U>::successor(Node* node) noexcept -> Node* {
  if (node->rchild != nullptr) {
    Node* p = node->rchild;
    while (p->lchild != nullptr) {
      p = p->lchild;
    }
    return p;
  } else {
    Node* p = node->parent;
    Node* cur = node;
    while (p != nullptr && cur == p->rchild) {
      cur = p;
      p = p->parent;
    }
    return p;
  }
  return nullptr;
}

template <Comparable T, class U>
void RBTree<T, U>::insert_fix(Node* node) noexcept {
  while (true) {
    Node* parent = node->parent;
    if (parent == nullptr) {
      node->color = Color::BLACK;
      return;
    }
    if (node->color == Color::BLACK || parent->color == Color::BLACK) {
      return;
    }
    Node* grandpa = parent->parent;
    bool is_parent_left = grandpa->lchild == parent;
    Node* uncle = is_parent_left ? grandpa->rchild : grandpa->lchild;
    bool is_node_left = parent->lchild == node;
    if (uncle != nullptr && uncle->color == Color::RED) {
      uncle->color = Color::BLACK;
      parent->color = Color::BLACK;
      grandpa->color = Color::RED;
      node = grandpa;
    } else {
      if (is_node_left == is_parent_left) {
        is_node_left ? right_rotate(grandpa) : left_rotate(grandpa);
        std::swap(grandpa->color, parent->color);
        node = grandpa;
      } else {
        is_node_left ? right_rotate(parent) : left_rotate(parent);
        node = parent;
      }
    }
  }
}

template <Comparable T, class U>
void RBTree<T, U>::remove_fix(Node* node) noexcept {
  while (node != root && node->color == Color::BLACK) {
    Node *parent = node->parent, *sibling;
    if (node == parent->lchild) {
      sibling = parent->rchild;
      if (sibling->color == Color::RED) {
        sibling->color = Color::BLACK;
        parent->color = Color::RED;
        left_rotate(parent);
        parent = node->parent;
        sibling = parent->rchild;
      }
      if (is_black(sibling->lchild) && is_black(sibling->rchild)) {
        sibling->color = Color::RED;
        node = parent;
      } else {
        if (is_black(sibling->rchild)) {
          right_rotate(sibling);
          sibling = parent->rchild;
        }
        sibling->color = parent->color;
        parent->color = Color::BLACK;
        sibling->rchild->color = Color::BLACK;
        left_rotate(parent);
        node = root;
      }
    } else {
      sibling = parent->lchild;
      if (sibling->color == Color::RED) {
        sibling->color = Color::BLACK;
        parent->color = Color::RED;
        right_rotate(parent);
        parent = node->parent;
        sibling = parent->lchild;
      }
      if (is_black(sibling->lchild) && is_black(sibling->rchild)) {
        sibling->color = Color::RED;
        node = parent;
      } else {
        if (is_black(sibling->lchild)) {
          left_rotate(sibling);
          sibling = parent->lchild;
        }
        sibling->color = parent->color;
        parent->color = Color::BLACK;
        sibling->lchild->color = Color::BLACK;
        right_rotate(parent);
        node = root;
      }
    }
  }
  node->color = Color::BLACK;
}

template <Comparable T, class U>
RBTree<T, U>::~RBTree() {
  if (root == nullptr) return;
  std::queue<Node*> q;
  q.push(root);
  while (!q.empty()) {
    Node* node = q.front();
    q.pop();
    if (node->lchild != nullptr) q.push(node->lchild);
    if (node->rchild != nullptr) q.push(node->rchild);
    delete node;
  }
}