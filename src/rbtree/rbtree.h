#pragma once

enum class Color { RED, BLACK };
enum class Direction { LEFT, RIGHT };
struct Node {
  Node* parent;
  Node* lchild;
  Node* rchild;
  int value;
  Color color;
  Node(int val = 0)
      : parent(nullptr),
        lchild(nullptr),
        rchild(nullptr),
        value(val),
        color(Color::RED) {}
};

class RBTree {
 public:
  Node* find(int val) const;
  bool insert(int val);
  bool remove(int val);
  void erase(Node* node);

  // for debug
  void print_graphvis() const;
  void check() const;

  ~RBTree();

 private:
  // 左旋：以node为旋转点 node->rchild为中心点，向左进行旋转
  void left_rotate(Node* node);
  // 右旋：以node为旋转点 node->lchild为旋转点，向右进行旋转
  void right_rotate(Node* node);
  // 移植：
  void transplant(Node* node, Node* replace) noexcept;
  // 后继：
  Node* successor(Node* node) noexcept;
  // 后序遍历：
  int post_travel(Node* node) const;

  /*
  插入节点默认为红色节点
  1. 插入为根节点，则红->黑
  2. 若自己和父亲都为红节点，则开始修复，此时需要关注叔叔节点的颜色
    a. 看叔叔节点，如果叔叔节点是红色，则叔父爷 变成当前相反的颜色， 同时 cur
  指向 爷爷节点，继续向上判定 b.
  看叔叔节点，如果叔叔节点是黑色，则根据LL，LR，RL，RR进行旋转，只需要对最后一次旋转点和参考点
  取相反颜色即可。
      对于LL，RR，只需要旋转一次即可。随后交换grandpa 和 parent的颜色即可。
      对于LR，RL，需要先对parent进行旋转，在对grandpa进行旋转，再对grandpa旋转的时候，交换颜色即可。
  */
  void insert_fix(Node* node);

  /*
  进入remove_fix的情况，说明node是一个双黑节点或根节点
  1. 若node为根节点，则将node颜色改为黑色
  2. 看兄弟节点，如果兄弟节点是红色，那么兄弟的两个孩子一定是黑色，父亲节点一定是黑色。
  此时需要对兄弟节点和父亲节点变色，并将父亲节点向着node方向旋转。此时会进入情况3。
  3. 兄弟节点是黑色
    a. 兄弟节点的孩子全为黑色，空节点视作黑色。
    因为删除node，会导致node一侧黑色节点少一个，所以将兄弟变红进行平衡。
    随后继续判断父亲节点是否平衡，如果父亲是红色节点，那么就双黑变单黑调整完毕。
    b. 兄弟节点的孩子有一个为红。分为LL，LR，RL，RR情况。
    因为LR和RL需要旋转两次，可以旋转一次后，变成LL，RR的情况，因此不需要分成四类。
    只需要先判断是否为LR或RL，调整完毕后，直接走LL和RL的逻辑即可。
    LL和RR结束后直接双黑变单黑，结束循环。
    但是由于LL和RR进行了旋转操作，有可能导致根节点发生变化，因此有必要检查根节点是否为黑色节点
  */
  void remove_fix(Node* node);

  Node* root = nullptr;
};