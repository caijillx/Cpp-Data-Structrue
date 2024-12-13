#include "src/rbtree/rbtree.h"

#include <stdio.h>

#include <cassert>
#include <queue>
#include <vector>

Node* RBTree::find(int val) const {
  Node* cur = root;
  while (cur != nullptr) {
    if (val < cur->value) {
      cur = cur->lchild;
    } else if (val > cur->value) {
      cur = cur->rchild;
    } else {
      return cur;
    }
  }
  return nullptr;
}

bool RBTree::insert(int val) {
  Node** pparent = &root;  // 二级指针：用于指向Node应该所在的位置
  Node* parent = nullptr;  // 维护要插入的node的父亲节点
  while (*pparent != nullptr) {
    parent = *pparent;
    if (val < parent->value) {
      pparent = &parent->lchild;
    } else if (val > parent->value) {
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

void RBTree::insert_fix(Node* node) {
  while (true) {
    Node* parent = node->parent;
    if (parent == nullptr) {  // case 1 : root
      node->color = Color::BLACK;
      return;
    }
    if (node->color == Color::BLACK ||
        parent->color ==
            Color::BLACK) {  // 当前节点和其父亲都是红色才有调整的必要
      return;
    }
    Node* uncle;
    Node* grandpa = parent->parent;
    assert(grandpa);
    Direction parent_diretion =
        grandpa->lchild == parent ? Direction::LEFT : Direction::RIGHT;
    if (parent_diretion == Direction::LEFT) {
      uncle = grandpa->rchild;
    } else {
      uncle = grandpa->lchild;
    }
    Direction node_direction =
        parent->lchild == node ? Direction::LEFT : Direction::RIGHT;
    if (uncle != nullptr &&
        uncle->color == Color::RED) {  // case 2 : uncle is red
      uncle->color = Color::BLACK;
      parent->color = Color::BLACK;
      grandpa->color = Color::RED;
      node = grandpa;
    } else {  // case 3 : uncle is black
      if (node_direction == parent_diretion) {
        if (node_direction == Direction::RIGHT) {  // RR
          left_rotate(grandpa);
        } else {  // LL
          right_rotate(grandpa);
        }
        std::swap(grandpa->color, parent->color);
        node = grandpa;
      } else {
        if (node_direction == Direction::RIGHT) {  // LR
          left_rotate(parent);
        } else {  // RL
          right_rotate(parent);
        }
        node = parent;
      }
    }
  }
}

/*
          node                     rchild
        /      \                  /       \
       x      rchild     =>      node       z
              /     \           /     \
             y       z         x        y
*/
void RBTree::left_rotate(Node* node) {
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

/*
          node                  lchild
        /       \              /       \
      lchild     z     =>     x         node
      /     \                         /       \
    x         y                      y          z
*/
void RBTree::right_rotate(Node* node) {
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

// replace 代替 node, replace != nullptr
void RBTree::transplant(Node* node, Node* replace) noexcept {
  if (node->parent == nullptr) {
    root = replace;
  } else if (node == node->parent->lchild) {
    node->parent->lchild = replace;
  } else {
    node->parent->rchild = replace;
  }
  replace->parent = node->parent;
}

Node* RBTree::successor(Node* node) noexcept {
  if (node->rchild != nullptr) {
    Node* p = node->rchild;
    while (p->lchild != nullptr) {
      p = p->lchild;
    }
    return p;
  } else {  // 找到第一个node在其左子树的祖先
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

// 使用 https://graphviz.christine.website/ 可视化
void RBTree::print_graphvis() const {
  if (root == nullptr) {
    printf("empty tree!\n");
    return;
  }

  printf("digraph prof {\nratio = fill;\nnode [style=filled];\n");
  std::queue<Node*> q;
  q.push(root);
  auto print_label = [](int val, Color color) {
    const char* c = color == Color::RED ? "red" : "black";
    printf("%d [label=\"%d\", style=filled, fillcolor=%s, fontcolor=white];\n",
           val, val, c);
  };
  std::vector<std::pair<int, int>> edges;
  while (!q.empty()) {
    Node* node = q.front();
    q.pop();
    print_label(node->value, node->color);
    if (node->lchild) {
      q.push(node->lchild);
      edges.emplace_back(node->value, node->lchild->value);
    }
    if (node->rchild) {
      q.push(node->rchild);
      edges.emplace_back(node->value, node->rchild->value);
    }
  }
  printf("\n");
  for (auto& edge : edges) {
    printf("%d -> %d\n", edge.first, edge.second);
  }
  printf("}\n");
}

void RBTree::check() const {
  if (root == nullptr) return;
  assert(root->color == Color::BLACK);
  post_travel(root);
}

int RBTree::post_travel(Node* node) const {
  if (node == nullptr) return 1;
  if (node->color == Color::RED) {
    if (node->lchild != nullptr) {
      assert(node->lchild->color == Color::BLACK);
      assert(node->lchild->parent == node);
    }
    if (node->rchild != nullptr) {
      assert(node->rchild->color == Color::BLACK);
      assert(node->rchild->parent == node);
    }
  }
  auto lcnt = post_travel(node->lchild);
  auto rcnt = post_travel(node->rchild);
  assert(lcnt == rcnt);
  return node->color == Color::BLACK ? lcnt + 1 : lcnt;
}

bool RBTree::remove(int val) {
  Node* node = find(val);
  if (node == nullptr) return false;
  erase(node);
  return true;
}

void RBTree::erase(Node* node) {
  // 如果左右子树都不为空，那么找后继，用后继替换当前节点，删除后继
  if (node->lchild != nullptr && node->rchild != nullptr) {
    Node* s = successor(node);
    node->value = s->value;
    node = s;
  }
  // 此时左右子树一定有一个为空
  Node* replace = node->lchild != nullptr ? node->lchild : node->rchild;
  // 选择非空的那一个进行直接代替父亲
  if (replace != nullptr) {
    transplant(node, replace);
    // 如果父亲的节点是黑色，那么说明删除该节点，将违反黑路同性质，需要修复。
    if (node->color == Color::BLACK) {
      remove_fix(replace);
    }
  } else if (node->parent == nullptr) {  // 左右子树都为空，先看是否是根节点
    root = nullptr;
  } else {  // 如果不是根节点，是红色节点就直接删除，黑色节点，也需要修复。
    if (node->color == Color::BLACK) {
      remove_fix(node);
    }
    // 删除node节点的之前，需要保证node->parent原本指向node的指针指向nullptr
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

void RBTree::remove_fix(Node* node) {
  while (node != root && node->color == Color::BLACK) {
    Node *parent = node->parent, *sibling;
    if (node == parent->lchild) {
      sibling = parent->rchild;
      if (sibling->color == Color::RED) {
        sibling->color = Color::BLACK;
        parent->color = Color::RED;
        left_rotate(parent);
        // parent绕着node左旋，左旋完成后，原来的node一定还是新节点的左儿子，因此这里sibling直接取rchild
        parent = node->parent;
        sibling = parent->rchild;
      }
      if ((!sibling->lchild || sibling->lchild->color == Color::BLACK) &&
          (!sibling->rchild || sibling->rchild->color == Color::BLACK)) {
        sibling->color = Color::RED;
        node = parent;
      } else {
        if (!sibling->rchild || sibling->rchild->color == Color::BLACK) {  // RL
          // sibling->lchild->color = Color::BLACK;
          // sibling->color = Color::RED;
          right_rotate(sibling);
          // parent = node->parent;
          sibling = parent->rchild;
        }
        // r变s s变p p变黑 因为r是红色，所以s一定是黑色，p可能是红色可能是黑色。
        sibling->color = parent->color;
        parent->color = Color::BLACK;
        sibling->rchild->color = Color::BLACK;
        left_rotate(parent);
        node = root;
      }
    } else {  // 镜像
      sibling = parent->lchild;
      if (sibling->color == Color::RED) {
        sibling->color = Color::BLACK;
        parent->color = Color::RED;
        right_rotate(parent);
        parent = node->parent;
        sibling = parent->lchild;
      }
      if ((!sibling->rchild || sibling->rchild->color == Color::BLACK) &&
          (!sibling->lchild || sibling->lchild->color == Color::BLACK)) {
        sibling->color = Color::RED;
        node = parent;
      } else {
        if (!sibling->lchild || sibling->lchild->color == Color::BLACK) {  // LR
          // sibling->rchild->color = Color::BLACK;
          // sibling->color = Color::RED;
          left_rotate(sibling);
          // parent = node->parent;
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

RBTree::~RBTree() {
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