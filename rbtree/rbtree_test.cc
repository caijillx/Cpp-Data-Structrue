#include "rbtree/rbtree.h"

#include <cassert>
#include <iostream>
#include <random>
#include <set>
#include <unordered_set>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(1, 1000000);
int random_int() { return dis(gen); }

void insert_test() {
  {
    RBTree s;
    s.insert(1);
    s.insert(2);
    s.insert(3);
    s.insert(4);
    assert(s.find(3) != nullptr);
    assert(s.find(5) == nullptr);
    assert(s.insert(3) == false);
  }
  {
    RBTree rbtree;
    std::vector<int> nums = {17, 18, 23, 34, 27, 15, 9, 6, 8, 5, 25};
    for (auto num : nums) {
      rbtree.insert(num);
      rbtree.check();
    }
  }
}

void remove_test() {
  RBTree s;
  s.insert(1);
  s.insert(2);
  s.insert(3);
  s.insert(4);
  s.insert(5);
  s.remove(4);
  assert(s.find(4) == nullptr);
  assert(s.find(3) != nullptr);
  s.remove(4);
  s.remove(3);
  s.check();
  s.remove(2);
  s.remove(1);
  assert(s.find(5) != nullptr);
  s.remove(5);
  assert(s.find(5) == nullptr);
}

void batch_test() {
  RBTree rbtree;
  std::set<int> s;
  std::unordered_set<int> s2;
  for (int i = 0; i < 100000; ++i) {
    int temp = random_int();
    s.insert(temp);
    s2.insert(temp);
  }
  for (auto num : s) {
    if (rbtree.insert(num)) rbtree.check();
  }

  for (auto num : s2) {
    if (rbtree.remove(num)) rbtree.check();
  }
}

int main() {
  insert_test();
  remove_test();
  batch_test();
  return 0;
}