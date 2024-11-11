#include <cassert>
#include <iostream>
#include <memory>

enum RopeNodeType {
  LeafNodeType = 0,
  ConcatNodeType = 1,
};

class LeafNode;
class ConcatNode;

class RopeNode {
public:
  RopeNode(RopeNodeType type, int length)
    : m_type(type)
    , m_length(length) {}

  virtual ~RopeNode() {}

  bool isLeaf() const { return m_type == LeafNodeType; }
  bool isConcat() const { return m_type == ConcatNodeType; }

  int length() const { return m_length; }

  LeafNode* toLeafNode();
  ConcatNode* toConcatNode();

  const LeafNode* toLeafNode() const;
  const ConcatNode* toConcatNode() const;

private:
  RopeNodeType m_type;

protected:
  // We have 2 length: prefix length and total length.
  // To keep an accurate count, we may need both. Right now, this is the prefix length.
  int m_length;
};

class LeafNode : public RopeNode {
public:
  // TODO: Get std::string as reference.
  LeafNode(std::string s)
    : RopeNode(LeafNodeType, s.length())
    , m_s(s) {}

  const std::string& s() const { return m_s; }

  void concat(const std::string& s) {
    m_s.append(s);
    m_length += s.length();
  }

private:
  std::string m_s;
};

class ConcatNode : public RopeNode {
public:
  ConcatNode(RopeNode* left, RopeNode* right)
    : RopeNode(ConcatNodeType, left ? left->length() : 0)
    , m_left(left)
    , m_right(right) {}

  // Left is guaranteed to non-null.
  RopeNode* left() const { return m_left.get(); }
  // Right can be null.
  RopeNode* right() const { return m_right.get(); }

  RopeNode* releaseLeft() { return m_left.release(); }
  RopeNode* releaseRight() { return m_right.release(); }

  // TODO: Should we check that left/right are empty here?
  void setLeft(RopeNode* left) { m_left.reset(left); m_length = m_left->length(); }
  void setRight(RopeNode* right) {  m_right.reset(right); }

private:
  std::unique_ptr<RopeNode> m_left;
  std::unique_ptr<RopeNode> m_right;
};

inline LeafNode* RopeNode::toLeafNode() {
  assert(isLeaf());
  return static_cast<LeafNode*>(this);
}

inline ConcatNode* RopeNode::toConcatNode() {
  assert(isConcat());
  return static_cast<ConcatNode*>(this);
}

inline const LeafNode* RopeNode::toLeafNode() const {
  assert(isLeaf());
  return static_cast<const LeafNode*>(this);
}

inline const ConcatNode* RopeNode::toConcatNode() const {
  assert(isConcat());
  return static_cast<const ConcatNode*>(this);
}

// Rope is the base class for manipulating ropes.
// Ideally it should abstract the nodes away from callers.
class Rope {
public:
  Rope() : m_root(new LeafNode(std::string())) {}

  // TODO: Make noncopyable for now.
  Rope(const Rope&) = delete;
  void operator=(const Rope&) = delete;

  RopeNode* root() const { return m_root.get(); }

  int length() const;

  void append(const std::string& s);
  void insert(int offset, const std::string& s);

private:
   std::unique_ptr<RopeNode> m_root;
};

int Rope::length() const {
  RopeNode* curr = root();
  int l = 0;
  while (curr) {
    l += curr->length();
    // Check for terminal node.
    if (curr->isLeaf()) {
      break;
    }

    ConcatNode* cn = curr->toConcatNode();
    curr = cn->right();
  }
  return l;
}

void Rope::insert(int offset, const std::string& s) {
  // TODO: validate offset.
  if (m_root->isLeaf()) {
    LeafNode* ln = m_root->toLeafNode();
    if (ln->length()) {
      ln->concat(s);
      return;
    }

    // Are we appending?
    if (offset >= ln->length()) {
      std::unique_ptr<LeafNode> right{new LeafNode(s)};
      std::unique_ptr<RopeNode> newRoot = std::make_unique<ConcatNode>(m_root.release(), right.release());
      m_root.swap(newRoot);
      return;
    }

    // Are we prepending?
    if (offset == 0) {
      std::unique_ptr<LeafNode> left{new LeafNode(s)};
      std::unique_ptr<RopeNode> newRoot = std::make_unique<ConcatNode>(left.release(), m_root.release());
      m_root.swap(newRoot);
      return;
    }

    // We are inserting in the middle of the string so we need to split it first.
    std::unique_ptr<LeafNode> pre{new LeafNode(ln->s().substr(0, offset))};
    std::unique_ptr<LeafNode> post{new LeafNode(ln->s().substr(offset))};
    std::unique_ptr<RopeNode> bottomConcat = std::make_unique<ConcatNode>(new LeafNode(s), post.release());
    std::unique_ptr<RopeNode> newRoot = std::make_unique<ConcatNode>(pre.release(), bottomConcat.release());
    m_root.swap(newRoot);
    return;
   }

  // Find insertion point.
  ConcatNode* curr = m_root.get()->toConcatNode();
  while (true) {
    RopeNode* next = curr->right();
    // Check that we're not walking past the insertion point.
    if (offset <= curr->length()) {
      next = curr->left();
      if (next->isLeaf()) {
        // Are we prepending?
        if (offset == 0) {
          std::unique_ptr<LeafNode> left{new LeafNode(s)};
          std::unique_ptr<RopeNode> newNext = std::make_unique<ConcatNode>(left.release(), curr->releaseLeft());
          curr->setLeft(newNext.release());
          return;
        }

        // We are inserting in the middle of the string so we need to split it first.
        LeafNode* ln = next->toLeafNode();
        std::unique_ptr<LeafNode> pre{new LeafNode(ln->s().substr(0, offset))};
        std::unique_ptr<LeafNode> post{new LeafNode(ln->s().substr(offset))};
        std::unique_ptr<RopeNode> bottomConcat = std::make_unique<ConcatNode>(new LeafNode(s), post.release());
        std::unique_ptr<RopeNode> newNext = std::make_unique<ConcatNode>(pre.release(), bottomConcat.release());
        curr->setLeft(newNext.release());
        return;
      }
      curr = next->toConcatNode();
      continue;
    }
    offset -= curr->length();

    if (next->isLeaf()) {
      LeafNode* ln = next->toLeafNode();

      // Are we appending?
      if (offset >= ln->length()) {
        std::unique_ptr<LeafNode> right{new LeafNode(s)};
        std::unique_ptr<RopeNode> newRoot = std::make_unique<ConcatNode>(curr->releaseRight(), right.release());
        curr->setRight(newRoot.release());
        return;
      }

      // Are we prepending?
      if (offset == 0) {
        std::unique_ptr<LeafNode> left{new LeafNode(s)};
        std::unique_ptr<RopeNode> newRoot = std::make_unique<ConcatNode>(left.release(), curr->releaseRight());
        curr->setRight(newRoot.release());
        return;
      }

      // We are inserting in the middle of the string so we need to split it first.
      std::unique_ptr<LeafNode> pre{new LeafNode(ln->s().substr(0, offset))};
      std::unique_ptr<LeafNode> post{new LeafNode(ln->s().substr(offset))};
      std::unique_ptr<RopeNode> bottomConcat = std::make_unique<ConcatNode>(new LeafNode(s), post.release());
      std::unique_ptr<RopeNode> newRoot = std::make_unique<ConcatNode>(pre.release(), bottomConcat.release());
      curr->setRight(newRoot.release());
      return;
    }

    ConcatNode* concatNext = next->toConcatNode();
    if (!concatNext->right()) {
      std::unique_ptr<LeafNode> right{new LeafNode(s)};
      concatNext->setRight(right.release());
      return;
    }

    curr = concatNext;
  }
  // Not reached.
}

void Rope::append(const std::string& s) {
  if (m_root->isLeaf()) {
    LeafNode* ln = m_root->toLeafNode();
    // TODO: Append short strings too?
    if (!ln->length()) {
      ln->concat(s);
      return;
    }

    std::unique_ptr<LeafNode> right{new LeafNode(s)};
    std::unique_ptr<RopeNode> newRoot = std::make_unique<ConcatNode>(m_root.release(), right.release());
    m_root.swap(newRoot);
    return;
  }

  // Find insertion point.
  ConcatNode* curr = m_root.get()->toConcatNode();
  while (true) {
    RopeNode* next = curr->right();
    if (next->isLeaf()) {
      std::unique_ptr<LeafNode> right{new LeafNode(s)};
      std::unique_ptr<RopeNode> newNext = std::make_unique<ConcatNode>(curr->releaseRight(), right.release());
      curr->setRight(newNext.release());
      return;
    }
    ConcatNode* concatNext = next->toConcatNode();
    if (!concatNext->right()) {
      std::unique_ptr<LeafNode> right{new LeafNode(s)};
      concatNext->setRight(right.release());
      return;
    }

    curr = concatNext;
  }
  // Not reached.
}

void dfs(std::ostream& o, const RopeNode* n) {
  if (n->isLeaf()) {
    o << n->toLeafNode()->s();
  } else {
    const ConcatNode* cn = n->toConcatNode();
    dfs(o, cn->left());
    dfs(o, cn->right());
  }
}

std::ostream& operator<<(std::ostream& o, const Rope& r) {
  o << "\"";
  dfs(o, r.root());
  o << "\"";
  return o;
}

int main() {
  Rope r;
  std::cout << "Empty rope: " << r << ", length: " << r.length() << std::endl;
  r.append("ab");
  std::cout << "After appending ab to rope: " << r << ", length: " << r.length() << std::endl;
  r.append("cd");
  std::cout << "After appending cd to rope: " << r << ", length: " << r.length() << std::endl;
  r.append("fg");
  std::cout << "After appending fg to rope: " << r << ", length: " << r.length() << std::endl;
  r.insert(4, "e");
  std::cout << "After insert e to rope: " << r << ", length: " << r.length() << std::endl;

  return 0;
}
