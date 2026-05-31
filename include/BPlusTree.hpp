#ifndef B_PLUS_TREE_HPP
#define B_PLUS_TREE_HPP

#include "MemoryRiver.hpp"
#include "vector.hpp"
#include <cassert>
#include <iostream>
// #include <queue>
// #include <vector>

template <typename KeyType, typename ValueType> class BPlusTree
{
public:
  class KeyValue
  {
  public:
    KeyType key_;
    ValueType value_;
    KeyValue(KeyType key = KeyType(), ValueType value = ValueType())
        : key_(key), value_(value)
    {
    }
    bool operator<(
        const KeyValue &other) const // keys are not same, we only compare key
    {
      return key_ < other.key_;
    }

    bool operator==(const KeyValue &other) const { return key_ == other.key_; }

    bool operator!=(const KeyValue &other) const { return !(*this == other); }
    bool operator<=(const KeyValue &other) const { return !(other < *this); }
    bool operator>(const KeyValue &other) const { return other < *this; }
    bool operator>=(const KeyValue &other) const { return !(*this < other); }
    friend std::ostream &operator<<(std::ostream &os, const KeyValue &key_value)
    {
      os << key_value.key_;
      return os;
    }
  };

private:
  static constexpr int maxSize = (4096 - 5 * sizeof(int)) / (sizeof(KeyValue) + sizeof(int)) - 1;
  static constexpr int minSize = (maxSize + 1) / 2;

  struct Node
  {
    int type_ = 0; // 0 stands for index node, 1 stands for leaf node
    int size_ = 0;
    int father_ = -1;
    int left_ = -1;
    int right_ = -1;
    KeyValue key_value_[maxSize + 1]; // we resize one more to support
                                      // insert and then split
    int childs_[maxSize + 1];
    Node(int type = 0, int size = 0, int father = -1, int left = -1,
         int right = -1, KeyValue *key_value = nullptr, int *childs = nullptr)
        : type_(type), size_(size), father_(father), left_(left), right_(right)
    {
      for (int i = 0; i < size_; i++)
      {
        key_value_[i] = key_value[i];
        childs_[i] = childs[i];
      }
    }
  };
  MemoryRiver<Node, 3> io_; // first info is root, second info is leaf
  void UpdateIndex(Node &node, int pos, KeyType old_key, KeyType new_key)
  {
    bool flag = true;
    while (flag)
    {
      if (pos == -1) // root node
      {
        break;
      }
      for (int i = 0; i < node.size_; i++)
      {
        if (node.key_value_[i].key_ == old_key) // we need to update this index
        {
          node.key_value_[i].key_ = new_key;
          if (i == 0) // we need to update upper father node
          {
            flag = true;
          }
          else
          {
            flag = false;
          }
          break;
        }
      }
      io_.Update(node, pos);  // write back
      pos = node.father_;     // update pos
      if (pos == -1 || !flag) // root node
      {
        break;
      }
      io_.Read(node, pos); // update node
    }
  }

  void Split(Node &node, int pos)
  {
    // std::cerr << "\n\npos: " << pos << "\n\n";
    if (node.size_ <= maxSize)
    {
      return;
    }
    int root;
    io_.GetInfo(root, 1);
    if (root == pos) // root node, and we don't need to split new_root
    {
      Node right_node, new_root_node;
      if (node.type_ == 1) // leaf node to be the root, which means little tree
      {
        int size1 = (maxSize + 1) / 2, size2 = maxSize + 1 - size1;
        int new_root = io_.Write(new_root_node);
        // update right_node
        right_node.type_ = 1;
        right_node.left_ = pos;
        right_node.father_ = new_root;
        right_node.size_ = size2;
        for (int i = 0; i < size2; i++)
        {
          right_node.key_value_[i] = node.key_value_[i + size1];
        }
        int right_pos = io_.Write(right_node);
        // update node
        node.size_ = size1;
        node.father_ = new_root;
        node.right_ = right_pos;
        io_.Update(node, pos);
        // update new_root_node
        new_root_node.size_ = 2;
        new_root_node.key_value_[0] = node.key_value_[0];
        new_root_node.key_value_[1] = right_node.key_value_[0];
        new_root_node.childs_[0] = pos;
        new_root_node.childs_[1] = right_pos;
        io_.Update(new_root_node, new_root);
        // update root
        io_.WriteInfo(new_root, 1);
      }
      else // index node to be the root
      {
        int size1 = (maxSize + 1) / 2, size2 = maxSize + 1 - size1;
        int right_pos = io_.Write(right_node);
        // update new_root_node
        new_root_node.size_ = 2;
        new_root_node.key_value_[0] = node.key_value_[0];
        new_root_node.key_value_[1] = node.key_value_[size1];
        new_root_node.childs_[0] = pos;
        new_root_node.childs_[1] = right_pos;
        int new_root = io_.Write(new_root_node);
        // update node
        node.size_ = size1;
        node.father_ = new_root;
        node.right_ = right_pos;
        io_.Update(node, pos);
        // update right_node
        right_node.left_ = pos;
        right_node.father_ = new_root;
        right_node.size_ = size2;
        for (int i = 0; i < size2; i++)
        {
          right_node.key_value_[i] = node.key_value_[i + size1];
          right_node.childs_[i] = node.childs_[i + size1];
          // update child_node
          Node child_node;
          io_.Read(child_node, right_node.childs_[i]);
          child_node.father_ = right_pos;
          io_.Update(child_node, right_node.childs_[i]);
        }
        io_.Update(right_node, right_pos);
        // update root
        io_.WriteInfo(new_root, 1);
      }
    }
    else // not root node, we probably need to split father node, and
         // father_node exists
    {
      Node right_node, old_right_node, father_node;
      int father_pos = node.father_, old_right_pos = node.right_;
      io_.Read(father_node, father_pos);
      if (node.type_ == 0) // index node
      {
        int size1 = (maxSize + 1) / 2, size2 = maxSize + 1 - size1;
        int right_pos = io_.Write(right_node);
        // update old_right_node if it exists
        if (old_right_pos != -1) // old_right_node existss
        {
          io_.Read(old_right_node, old_right_pos);
          old_right_node.left_ = right_pos;
          io_.Update(old_right_node, old_right_pos);
        }
        // update father_node
        int pos_in_father;
        for (int i = 0; i < father_node.size_; i++)
        {
          // std::cerr << "\n\nnow_child: " << father_node.childs_[i] << "\n\n";
          if (father_node.childs_[i] == pos)
          {
            // std::cerr << "\n\nwe fucking find it\n\n";
            pos_in_father = i;
            break;
          }
        }
        for (int i = father_node.size_ - 1; i >= pos_in_father + 1; i--)
        {
          father_node.key_value_[i + 1] = father_node.key_value_[i];
          father_node.childs_[i + 1] = father_node.childs_[i];
        }
        father_node.key_value_[pos_in_father + 1] = node.key_value_[size1];
        father_node.childs_[pos_in_father + 1] = right_pos;
        father_node.size_++;
        io_.Update(father_node, father_pos);
        // update node
        node.size_ = size1;
        node.right_ = right_pos;
        io_.Update(node, pos);
        // update right_node
        right_node.type_ = 0;
        right_node.left_ = pos;
        right_node.right_ = old_right_pos;
        right_node.father_ = father_pos;
        right_node.size_ = size2;
        for (int i = 0; i < size2; i++)
        {
          right_node.key_value_[i] = node.key_value_[i + size1];
          right_node.childs_[i] = node.childs_[i + size1];
          // update child_node
          Node child_node;
          io_.Read(child_node, right_node.childs_[i]);
          child_node.father_ = right_pos;
          io_.Update(child_node, right_node.childs_[i]);
        }
        io_.Update(right_node, right_pos);
      }
      else // leaf node
      {
        int size1 = (maxSize + 1) / 2, size2 = maxSize + 1 - size1;
        int right_pos = io_.Write(right_node);
        // update old_right_node if it exists
        if (old_right_pos != -1) // old_right_node existss
        {
          io_.Read(old_right_node, old_right_pos);
          old_right_node.left_ = right_pos;
          io_.Update(old_right_node, old_right_pos);
        }
        // update father_node
        int pos_in_father;
        for (int i = 0; i < father_node.size_; i++)
        {
          if (father_node.childs_[i] == pos)
          {
            pos_in_father = i;
          }
        }
        for (int i = father_node.size_ - 1; i >= pos_in_father + 1; i--)
        {
          // std::cerr << "\n\n\n\n?????   " << i << "\n\n\n\n";
          father_node.key_value_[i + 1] = father_node.key_value_[i];
          father_node.childs_[i + 1] = father_node.childs_[i];
        }
        father_node.key_value_[pos_in_father + 1] = node.key_value_[size1];
        father_node.childs_[pos_in_father + 1] = right_pos;
        father_node.size_++;
        io_.Update(father_node, father_pos);
        // update node
        node.size_ = size1;
        node.right_ = right_pos;
        io_.Update(node, pos);
        // update right_node
        right_node.type_ = 1;
        right_node.left_ = pos;
        right_node.right_ = old_right_pos;
        right_node.father_ = father_pos;
        right_node.size_ = size2;
        for (int i = 0; i < size2; i++)
        {
          right_node.key_value_[i] = node.key_value_[i + size1];
        }
        io_.Update(right_node, right_pos);
      }
      Split(father_node, father_pos);
    }
  }
  bool AdjustOneNode(Node &node, int pos,
                     int root) // return true if we need to adjust upper node
  {
    if (node.size_ >= minSize) // do not need adjust
    {
      return false;
    }
    if (root == pos) // root node
    {
      if (node.size_ != 1) // root have just more than one element
      {
        return false;
      }
      Node child;
      int child_pos = node.childs_[0];
      io_.Read(child, child_pos);
      // update child
      child.father_ = -1;
      io_.Update(child, child_pos);
      // update root
      io_.WriteInfo(child_pos, 1);
      return false;
    }
    else // leaf node or index node
    {
      int left_pos = node.left_, right_pos = node.right_;
      Node left_node, right_node;
      bool read_right_node = false;
      if (left_pos != -1) // has left_node
      {
        io_.Read(left_node, left_pos);
        if (left_node.size_ >= minSize + 1 &&
            left_node.father_ == node.father_) // left_node has enough elements
                                               // and it is left brother
        {
          RentLeft(node, pos, left_node, left_pos);
          return false;
        }
      }
      if (right_pos != -1) // has right_node
      {
        io_.Read(right_node, right_pos);
        read_right_node = true;
        if (right_node.size_ >= minSize + 1 &&
            right_node.father_ ==
                node.father_) // right_node has enough elements and it is right
                              // brother
        {
          RentRight(node, pos, right_node, right_pos);
          return false;
        }
      }
      // merge
      // merge left brother
      if (left_pos != -1)
      {
        // already read in left_node
        if (left_node.father_ == node.father_)
        {
          Merge(left_node, left_pos, node, pos);
          return true;
        }
      }
      // merge right brother
      if (!read_right_node) // prevent from not reading right_node
      {
        io_.Read(right_node, right_pos);
      }
      // we definitely can merge in right_node in this situation
      Merge(node, pos, right_node, right_pos);
      return true;
    }
  }
  void Adjust(Node node, int pos, int root)
  {
    while (true)
    {
      if (AdjustOneNode(node, pos, root))
      {
        if (node.father_ != -1)
        {
          pos = node.father_;
          io_.Read(node, pos);
        }
      }
      else
      {
        break;
      }
    }
  }
  void RentLeft(Node &node, int pos, Node &left_node, int left_pos)
  {
    if (node.type_ == 1) // leaf node
    {
      KeyValue tar_key_value = left_node.key_value_[left_node.size_ - 1];
      KeyValue old_key_value = node.key_value_[0];
      // update left_node
      left_node.size_--;
      io_.Update(left_node, left_pos);
      // update node
      for (int i = node.size_ - 1; i >= 0; i--)
      {
        node.key_value_[i + 1] = node.key_value_[i];
      }
      node.key_value_[0] = tar_key_value;
      node.size_++;
      io_.Update(node, pos);
      // update father
      if (node.father_ != -1)
      {
        Node father_node;
        io_.Read(father_node, node.father_);
        int modify_pos = LastLessEqual(father_node.key_value_,
                                       father_node.size_, old_key_value);
        father_node.key_value_[modify_pos] = tar_key_value;
        io_.Update(father_node, node.father_);
      }
    }
    else // index node
    {
      KeyValue tar_key_value = left_node.key_value_[left_node.size_ - 1];
      int tar_child = left_node.childs_[left_node.size_ - 1];
      KeyValue old_key_value = node.key_value_[0];
      // update left_node
      left_node.size_--;
      io_.Update(left_node, left_pos);
      // update node
      for (int i = node.size_ - 1; i >= 0; i--)
      {
        node.key_value_[i + 1] = node.key_value_[i];
        node.childs_[i + 1] = node.childs_[i];
      }
      node.key_value_[0] = tar_key_value;
      node.childs_[0] = tar_child;
      node.size_++;
      io_.Update(node, pos);
      // update father
      if (node.father_ != -1)
      {
        Node father_node;
        io_.Read(father_node, node.father_);
        int modify_pos = LastLessEqual(father_node.key_value_,
                                       father_node.size_, old_key_value);
        father_node.key_value_[modify_pos] = tar_key_value;
        io_.Update(father_node, node.father_);
      }
      // update child node
      Node child_node;
      int child_pos = node.childs_[0];
      io_.Read(child_node, child_pos);
      child_node.father_ = pos;
      io_.Update(child_node, child_pos);
    }
  }
  void RentRight(Node &node, int pos, Node &right_node, int right_pos)
  {
    if (node.type_ == 1) // leaf node
    {
      KeyValue tar_key_value = right_node.key_value_[1];
      KeyValue old_key_value = right_node.key_value_[0];
      // update node
      node.size_++;
      node.key_value_[node.size_ - 1] = old_key_value;
      io_.Update(node, pos);
      // update right_node
      for (int i = 0; i <= right_node.size_ - 2; i++)
      {
        right_node.key_value_[i] = right_node.key_value_[i + 1];
      }
      right_node.size_--;
      io_.Update(right_node, right_pos);
      // update father_node
      if (node.father_ != -1)
      {
        Node father_node;
        io_.Read(father_node, node.father_);
        int modify_pos = LastLessEqual(father_node.key_value_,
                                       father_node.size_, old_key_value);
        father_node.key_value_[modify_pos] = tar_key_value;
        io_.Update(father_node, node.father_);
      }
    }
    else // index node
    {
      KeyValue tar_key_value = right_node.key_value_[1];
      int tar_child = right_node.childs_[0];
      KeyValue old_key_value = right_node.key_value_[0];
      // update node
      node.size_++;
      node.key_value_[node.size_ - 1] = old_key_value;
      node.childs_[node.size_ - 1] = tar_child;
      io_.Update(node, pos);
      // update right_node
      for (int i = 0; i <= right_node.size_ - 2; i++)
      {
        right_node.key_value_[i] = right_node.key_value_[i + 1];
        right_node.childs_[i] = right_node.childs_[i + 1];
      }
      right_node.size_--;
      io_.Update(right_node, right_pos);
      // update father_node
      if (node.father_ != -1)
      {
        Node father_node;
        io_.Read(father_node, node.father_);
        int modify_pos = LastLessEqual(father_node.key_value_,
                                       father_node.size_, old_key_value);
        father_node.key_value_[modify_pos] = tar_key_value;
        io_.Update(father_node, node.father_);
      }
      // update child node
      Node child_node;
      int child_pos = node.childs_[node.size_ - 1];
      io_.Read(child_node, child_pos);
      child_node.father_ = pos;
      io_.Update(child_node, child_pos);
    }
  }
  void Merge(Node &node1, int pos1, Node &node2, int pos2)
  {
    int size1 = node1.size_, size2 = node2.size_;
    if (node1.type_ == 1) // leaf node
    {
      KeyValue old_key_value = node2.key_value_[0];
      // update node1
      for (int i = 0; i <= size2 - 1; i++)
      {
        node1.key_value_[i + size1] = node2.key_value_[i];
      }
      node1.right_ = node2.right_;
      node1.size_ = size1 + size2;
      io_.Update(node1, pos1);
      // update right_node if it exists
      if (node2.right_ != -1)
      {
        Node right_node;
        io_.Read(right_node, node2.right_);
        right_node.left_ = pos1;
        io_.Update(right_node, node2.right_);
      }
      // update father_node
      Node father_node;
      int father_pos = node1.father_;
      io_.Read(father_node, father_pos);
      int modify_pos = LastLessEqual(father_node.key_value_, father_node.size_,
                                     old_key_value);
      for (int i = modify_pos; i <= father_node.size_ - 2; i++)
      {
        father_node.key_value_[i] = father_node.key_value_[i + 1];
        father_node.childs_[i] = father_node.childs_[i + 1];
      }
      father_node.size_--;
      io_.Update(father_node, father_pos);
    }
    else // index node
    {
      KeyValue old_key_value = node2.key_value_[0];
      // update node1
      for (int i = 0; i <= size2 - 1; i++)
      {
        node1.key_value_[i + size1] = node2.key_value_[i];
        node1.childs_[i + size1] = node2.childs_[i];
        // update child_node
        Node child_node;
        int child_pos = node2.childs_[i];
        io_.Read(child_node, child_pos);
        child_node.father_ = pos1;
        io_.Update(child_node, child_pos);
      }
      node1.right_ = node2.right_;
      node1.size_ = size1 + size2;
      io_.Update(node1, pos1);
      // update right_node if it exists
      if (node2.right_ != -1)
      {
        Node right_node;
        io_.Read(right_node, node2.right_);
        right_node.left_ = pos1;
        io_.Update(right_node, node2.right_);
      }
      // update father_node
      Node father_node;
      int father_pos = node1.father_;
      io_.Read(father_node, father_pos);
      int modify_pos = LastLessEqual(father_node.key_value_, father_node.size_,
                                     old_key_value);
      for (int i = modify_pos; i <= father_node.size_ - 2; i++)
      {
        father_node.key_value_[i] = father_node.key_value_[i + 1];
        father_node.childs_[i] = father_node.childs_[i + 1];
      }
      father_node.size_--;
      io_.Update(father_node, father_pos);
    }
  }

  int LastLessEqual(const KeyValue *a, int n, const KeyValue &x) // a[0, n)
  {
    int l = -1, r = n - 1;
    while (l < r)
    {
      int mid = l + (r - l + 1) / 2;
      if (a[mid] <= x)
      {
        l = mid;
      }
      else
      {
        r = mid - 1;
      }
    }
    return l;
  }

public:
  BPlusTree() = default;
  BPlusTree(std::string filename) { io_.Initialise(filename); }
  ~BPlusTree() = default;
  bool Find(KeyType key,
            KeyValue &key_value) // return false if not found, return true if
                                 // found, and the result is restored in
                                 // key_value(by quoting)
  {
    int root;
    io_.GetInfo(root, 1);
    if (root == -1) // empty tree
    {
      return false;
    }
    Node now_node;
    io_.Read(now_node, root);
    while (now_node.type_ == 0) // index node
    {
      int pos =
          LastLessEqual(now_node.key_value_, now_node.size_, KeyValue(key));
      if (pos == -1)
      {
        return false;
      }
      io_.Read(now_node, now_node.childs[pos]);
    }
    // now_node is leaf node
    int tar_pos =
        LastLessEqual(now_node.key_value_, now_node.size_, KeyValue(key));
    if (tar_pos == -1)
    {
      return false;
    }
    if (now_node.key_value_[tar_pos].key_ == key)
    {
      key_value = now_node.key_value_[tar_pos];
      return true;
    }
    return false;
  }
  sjtu::vector<KeyValue> IntervalFind(KeyType key1,
                    KeyType key2) // find key in [key1, key2]
  {
    // std::cerr << "key1: " << key1 << "  key2: " << key2 << '\n';
    int root;
    io_.GetInfo(root, 1);
    if (root == -1) // empty tree
    {
      return sjtu::vector<KeyValue>{};
    }
    Node now_node;
    io_.Read(now_node, root);
    while (now_node.type_ == 0) // index node
    {
      int pos = LastLessEqual(now_node.key_value_, now_node.size_, key1);
      // std::cerr << "we find it in pos = " << pos << '\n';
      if (pos == -1) // key1 is smaller than the smallest one in tree
      {
        pos = 0;
      }
      io_.Read(now_node, now_node.childs_[pos]);
    }
    // now_node is leaf node
    bool flag = false; // to record whether need to cout "null"
    bool flag_find = true;
    int cnt = 0;
    sjtu::vector<KeyValue> ans;
    while (true)
    {
      for (int i = 0; i <= now_node.size_ - 1; i++)
      {
        if (now_node.key_value_[i].key_ >= key1 &&
            now_node.key_value_[i].key_ <= key2)
        {
          // std::cerr << "we find the first in block = " << i << '\n';
          if (cnt > 1 && !flag)
          {
            assert(false);
          }
          flag = true;
          ans.push_back(now_node.key_value_[i]);
        }
        if (now_node.key_value_[i].key_ > key2) // if encounter a bigger one
        {
          flag_find = false;
          break;
        }
      }
      if (now_node.right_ == -1 || !flag_find) // if now_node is the last leaf
      {
        // std::cerr << "shit\n";
        break;
      }
      io_.Read(now_node, now_node.right_);
      cnt++;
    }
    if (!flag)
    {
      return sjtu::vector<KeyValue>{};
    }
    return ans;
  }
  void Insert(KeyType key, ValueType value)
  {
    int root;
    io_.GetInfo(root, 1);

    if (root == -1) // empty tree
    {
      int child = -1;
      KeyValue key_value(key);
      Node node(1, 1, -1, -1, -1, &key_value, &child); // leaf node
      root = io_.Write(node);
      // update root
      io_.WriteInfo(root, 1);
      // update leaf
      io_.WriteInfo(root, 2);
      return;
    }
    // not empty tree
    Node now_node;
    int now_pos = root; // for split, we need to record the pos of now_node
    io_.Read(now_node, root);
    while (now_node.type_ == 0) // index node
    {
      int pos =
          LastLessEqual(now_node.key_value_, now_node.size_, KeyValue(key));
      if (pos == -1) // insert a smallest element
      {
        pos = 0;
      }
      now_pos = now_node.childs_[pos];
      io_.Read(now_node, now_pos);
    }
    // now_node is leaf node
    int pos = LastLessEqual(now_node.key_value_, now_node.size_, KeyValue(key));
    bool flag = false;
    KeyType old_key;
    if (pos == -1) // insert a smallest element in this node, so we need to
                   // modify father's index element
    {
      flag = true;
      old_key = now_node.key_value_[0].key_;
    }
    if (pos != -1 &&
        now_node.key_value_[pos].key_ ==
            key) // we find the same key, which means we don't need insert
    {
      return;
    }
    // insert data
    for (int i = now_node.size_ - 1; i >= pos + 1; i--)
    {
      now_node.key_value_[i + 1] = now_node.key_value_[i];
    }
    now_node.key_value_[pos + 1] = KeyValue(key, value);
    now_node.size_++;
    // write back node first
    io_.Update(now_node, now_pos);
    // modify index
    Node modify_node;
    int modify_pos;
    if (flag && now_node.father_ != -1) // need update and not root node
    {
      io_.Read(modify_node, now_node.father_);
      modify_pos = now_node.father_;
      UpdateIndex(modify_node, modify_pos, old_key, key);
    }
    // split
    if (now_node.size_ == maxSize + 1)
    {
      Split(now_node, now_pos);
    }
  }
  void Delete(KeyType key)
  {
    int root;
    io_.GetInfo(root, 1);
    if (root == -1) // empty tree
    {
      return;
    }
    // not empty tree
    Node now_node;
    int now_pos = root; // for split, we need to record the pos of now_node
    io_.Read(now_node, root);
    while (now_node.type_ == 0) // index node
    {
      int pos =
          LastLessEqual(now_node.key_value_, now_node.size_, KeyValue(key));
      if (pos == -1) // sorry we can't not find it
      {
        return;
      }
      now_pos = now_node.childs_[pos];
      io_.Read(now_node, now_pos);
    }
    // now_node is leaf node
    int tar_pos = -1;
    int i = LastLessEqual(now_node.key_value_, now_node.size_, KeyValue(key));
    if (i == -1) // sorry we can't not find it
    {
      return;
    }
    if (now_node.key_value_[i].key_ == key) // we find it!
    {
      tar_pos = i;
    }
    if (tar_pos == -1) // sorry we did not find it
    {
      return;
    }
    KeyType new_key;
    if (tar_pos == 0) // we have to delete the first element, which means we
                      // need to modify index!
    {
      // record new_key
      new_key = now_node.key_value_[1].key_;
    }
    // delete the target element
    for (int i = tar_pos; i <= now_node.size_ - 2; i++)
    {
      now_node.key_value_[i] = now_node.key_value_[i + 1];
      now_node.childs_[i] = now_node.childs_[i + 1];
    }
    // modify size
    now_node.size_--;
    // write back node first
    io_.Update(now_node, now_pos);
    // modify index
    Node modify_node;
    int modify_pos;
    if (tar_pos == 0 && now_node.father_ != -1) // need update and not root node
    {
      io_.Read(modify_node, now_node.father_);
      modify_pos = now_node.father_;
    }
    if (tar_pos == 0 && now_node.father_ != -1) // we have to update index
    {
      UpdateIndex(modify_node, modify_pos, key, new_key);
    }
    // adjust
    if (now_node.size_ <= minSize - 1)
    {
      if (now_node.size_ == 0) // delete into an empty tree
      {
        io_.WriteInfo(-1, 1);
        io_.WriteInfo(-1, 2);
      }
      else if (root == now_pos) // little tree, nothing happens
      {
        return;
      }
      else
      {
        Adjust(now_node, now_pos, root);
      }
    }
  }
  // struct NodePos
  // {
  //   Node node_;
  //   int pos_;
  // };
  // void PrintTree()
  // {
  //   int root;
  //   io_.GetInfo(root, 1);
  //   std::cerr << "root: " << root << "\n\n";
  //   if (root == -1)
  //   {
  //     std::cerr << "empty tree!\n";
  //   }
  //   else
  //   {
  //     Node now_node;
  //     io_.Read(now_node, root);
  //     int pos = root;
  //     std::queue<NodePos> q;
  //     q.push(NodePos{now_node, pos});
  //     while (!q.empty())
  //     {
  //       NodePos now = q.front();
  //       q.pop();
  //       if (PrintNode(now.node_, now.pos_))
  //       {
  //         Node new_node;
  //         int new_pos;
  //         for (int i = 0; i <= now.node_.size_ - 1; i++)
  //         {
  //           NodePos new_ob;
  //           new_pos = now.node_.childs_[i];
  //           io_.Read(new_node, new_pos);
  //           new_ob.node_ = new_node;
  //           new_ob.pos_ = new_pos;
  //           q.push(new_ob);
  //         }
  //       }
  //     }
  //   }
  // }
  // bool PrintNode(Node node, int pos)
  // {
  //   std::cerr << "pos: " << pos << '\n';
  //   std::cerr << "type: " << ((node.type_ == 1) ? "leaf node" : "index node")
  //             << '\n';
  //   std::cerr << "size: " << node.size_ << '\n';
  //   std::cerr << "father: " << node.father_ << '\n';
  //   std::cerr << "left: " << node.left_ << '\n';
  //   std::cerr << "right: " << node.right_ << '\n';
  //   for (int i = 0; i <= node.size_ - 1; i++)
  //   {
  //     std::cerr << "key_value[" << i << "] = {" << node.key_value_[i].key_
  //               << "}  ";
  //   }
  //   std::cerr << '\n';
  //   for (int i = 0; i <= node.size_ - 1; i++)
  //   {
  //     std::cerr << "childs[" << i << "] = " << node.childs_[i] << "  ";
  //   }
  //   std::cerr << "\n\n";
  //   if (node.type_ == 1)
  //   {
  //     return false;
  //   }
  //   else
  //   {
  //     return true;
  //   }
  // }
};

#endif // B_PLUS_TREE_HPP