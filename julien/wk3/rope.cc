#include <cassert>
#include <iostream>
#include <memory>
#include <queue>

enum RopeNodeType {
  LeafNodeType = 0,
  ConcatNodeType = 1,
};

class LeafNode;
class ConcatNode;

class RopeNode {
public:
  RopeNode(RopeNodeType type, int prefixLength)
    : m_type(type)
    , m_prefixLength(prefixLength) {}

  virtual ~RopeNode() {}

  bool isLeaf() const { return m_type == LeafNodeType; }
  bool isConcat() const { return m_type == ConcatNodeType; }

  int prefixLength() const { return m_prefixLength; }

  LeafNode* toLeafNode();
  ConcatNode* toConcatNode();

  const LeafNode* toLeafNode() const;
  const ConcatNode* toConcatNode() const;

private:
  RopeNodeType m_type;

protected:
  // This is the prefix length, ie the length of all strings on the left branch.
  int m_prefixLength;

  friend int insertRecursive(ConcatNode* curr, int offset, const std::string& s);
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
    m_prefixLength += s.length();
  }

private:
  std::string m_s;
};

class ConcatNode : public RopeNode {
public:
  ConcatNode(RopeNode* left, RopeNode* right)
    : RopeNode(ConcatNodeType, left->prefixLength())
    , m_left(left)
    , m_right(right) {
  }

  // Left is guaranteed to non-null.
  RopeNode* left() const { return m_left.get(); }
  // Right can be null.
  RopeNode* right() const { return m_right.get(); }

  RopeNode* releaseLeft() { return m_left.release(); m_prefixLength = 0; }
  RopeNode* releaseRight() { return m_right.release(); }

  // TODO: Should we check that left/right are empty here?
  void setLeft(RopeNode* left) {
    // It's possible for left to be 0 when we released it above.
    assert(!m_left || m_prefixLength == m_left->prefixLength());

    m_left.reset(left);
  }

  void setRight(RopeNode* right) {
    // We should never set right to nullptr here.
    assert(right);

    m_right.reset(right);
  }

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

  void dumpTree(std::ostream&) const;

private:
   std::unique_ptr<RopeNode> m_root;
};

int Rope::length() const {
  RopeNode* curr = root();
  int l = 0;
  while (curr) {
    l += curr->prefixLength();
    // Check for terminal node.
    if (curr->isLeaf()) {
      break;
    }

    ConcatNode* cn = curr->toConcatNode();
    curr = cn->right();
  }
  return l;
}

int insertRecursive(ConcatNode* curr, int offset, const std::string& s) {
  // Check that we're not walking past the insertion point.
  if (offset <= curr->prefixLength()) {
    RopeNode* next = curr->left();
    if (next->isLeaf()) {
      // Are we prepending?
      if (offset == 0) {
        int rightLength = curr->left()->prefixLength();
        std::unique_ptr<LeafNode> left{new LeafNode(s)};
        std::unique_ptr<RopeNode> newNext = std::make_unique<ConcatNode>(left.release(), curr->releaseLeft());
        curr->setLeft(newNext.release());
        curr->m_prefixLength = s.length() + rightLength;
        return s.length();
      }

      // Are we appending?
      if (offset == curr->prefixLength()) {
        int leftLength = curr->left()->prefixLength();
        std::unique_ptr<LeafNode> right{new LeafNode(s)};
        std::unique_ptr<RopeNode> newNext = std::make_unique<ConcatNode>(curr->releaseLeft(), right.release());
        curr->setLeft(newNext.release());
        curr->m_prefixLength = leftLength + s.length();
        return s.length();
      }

      // We are inserting in the middle of the string so we need to split it first.
      LeafNode* ln = next->toLeafNode();
      int length = ln->s().length();
      std::unique_ptr<LeafNode> pre{new LeafNode(ln->s().substr(0, offset))};
      std::unique_ptr<LeafNode> post{new LeafNode(ln->s().substr(offset))};
      std::unique_ptr<RopeNode> bottomConcat = std::make_unique<ConcatNode>(new LeafNode(s), post.release());
      std::unique_ptr<RopeNode> newNext = std::make_unique<ConcatNode>(pre.release(), bottomConcat.release());
      curr->setLeft(newNext.release());
      curr->m_prefixLength = length + s.length();
      return s.length();
    }

    int delta = insertRecursive(next->toConcatNode(), offset, s);
    curr->m_prefixLength += delta;
    return delta;
  }

  offset -= curr->prefixLength();
  RopeNode* next = curr->right();
  if (next->isLeaf()) {
    LeafNode* ln = next->toLeafNode();

    // Are we appending?
    if (offset >= ln->prefixLength()) {
      int leftLength = curr->left()->prefixLength();
      std::unique_ptr<LeafNode> right{new LeafNode(s)};
      std::unique_ptr<RopeNode> newRoot = std::make_unique<ConcatNode>(curr->releaseRight(), right.release());
      curr->setRight(newRoot.release());
      curr->m_prefixLength = leftLength + s.length();
      return s.length();
    }

    // Are we prepending?
    if (offset == 0) {
      int rightLength = curr->right()->prefixLength();
      std::unique_ptr<LeafNode> left{new LeafNode(s)};
      std::unique_ptr<RopeNode> newRoot = std::make_unique<ConcatNode>(left.release(), curr->releaseRight());
      curr->setRight(newRoot.release());
      curr->m_prefixLength = s.length() + rightLength;
      return s.length();
    }

    // We are inserting in the middle of the string so we need to split it first.
    int length = ln->s().length();
    std::unique_ptr<LeafNode> pre{new LeafNode(ln->s().substr(0, offset))};
    std::unique_ptr<LeafNode> post{new LeafNode(ln->s().substr(offset))};
    std::unique_ptr<RopeNode> bottomConcat = std::make_unique<ConcatNode>(new LeafNode(s), post.release());
    std::unique_ptr<RopeNode> newRoot = std::make_unique<ConcatNode>(pre.release(), bottomConcat.release());
    curr->setRight(newRoot.release());
    curr->m_prefixLength = length + s.length();
    return s.length();
  }

  ConcatNode* concatNext = next->toConcatNode();
  if (!concatNext->right()) {
    std::unique_ptr<LeafNode> right{new LeafNode(s)};
    concatNext->setRight(right.release());
    return s.length();
  }

  insertRecursive(concatNext, offset, s);
  //curr->m_prefixLength += delta;
  return 0;
}

void Rope::insert(int offset, const std::string& s) {
  // TODO: validate offset.
  if (m_root->isLeaf()) {
    LeafNode* ln = m_root->toLeafNode();
    if (ln->prefixLength()) {
      ln->concat(s);
      return;
    }

    // Are we appending?
    if (offset >= ln->prefixLength()) {
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

  insertRecursive(m_root.get()->toConcatNode(), offset, s);
}

struct RopeNodeInfo {
  const RopeNode* n;
  int level;
  bool isRight;
};

void Rope::dumpTree(std::ostream& o) const {
  std::queue<RopeNodeInfo> q;
  q.push(RopeNodeInfo{m_root.get(), 0, false});
  int lastLevel = 0;
  while (!q.empty()) {
    RopeNodeInfo i = q.front();
    q.pop();
    int level = i.level;
    if (level != lastLevel) {
      o << std::endl;
      lastLevel = level;
    } else {
      o << "  ";
    }
    if (i.n->isLeaf()) {
      const LeafNode* ln = i.n->toLeafNode();
      if (level > 0) {
        if (i.isRight) {
          o << "R";
        } else {
          o << "L";
        }
      }
      o << "l\"" << ln->s() << "\" (p=" << ln->prefixLength() << ")";
    } else {
      if (level > 0) {
        if (i.isRight) {
          o << "R";
        } else {
          o << "L";
        }
      }
      const ConcatNode* cn = i.n->toConcatNode();
      if (cn->left()) {
        q.push(RopeNodeInfo{cn->left(), level + 1, false});
      }
      if (cn->right()) {
        q.push(RopeNodeInfo{cn->right(), level + 1, true});
      }
      o << "c (p=" << cn->prefixLength() << ")";
    }
  }
}

void Rope::append(const std::string& s) {
  if (m_root->isLeaf()) {
    LeafNode* ln = m_root->toLeafNode();
    // TODO: Append short strings too?
    if (!ln->prefixLength()) {
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
  r.append("b");
  std::cout << "After appending ab to rope: " << r << ", length: " << r.length() << std::endl;
  r.append("cd");
  std::cout << "After appending cd to rope: " << r << ", length: " << r.length() << std::endl;
  r.append("fh");
  std::cout << "After appending fg to rope: " << r << ", length: " << r.length() << std::endl;
  std::cout << std::endl;
  r.dumpTree(std::cout);
  std::cout << std::endl;
  r.insert(3, "e");
  std::cout << "After insert e to rope: " << r << ", length: " << r.length() << std::endl;
  std::cout << std::endl;
  r.dumpTree(std::cout);
  std::cout << std::endl;
  r.insert(666, "z");
  std::cout << "After appending z to rope: " << r << ", length: " << r.length() << std::endl;
  r.insert(0, "a");
  std::cout << "After prepending a to rope: " << r << ", length: " << r.length() << std::endl;
  r.insert(5, "g");
  std::cout << "After inserting/splitting g to rope: " << r << ", length: " << r.length() << std::endl;

  return 0;
}
