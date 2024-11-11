#include <cassert>
#include <iostream>
#include <memory>

// Doubly linked list implementation.

// TODO: Templatize payload.
class DoublyLinkedListNode {
public:
  DoublyLinkedListNode(int value) : m_value(value), m_prev(nullptr), m_next(nullptr) {}

  // Make non-copiable for now.
  DoublyLinkedListNode(const DoublyLinkedListNode&) = delete;
  void operator=(const DoublyLinkedListNode&) = delete;

  const DoublyLinkedListNode* next() const { return m_next.get(); }
  DoublyLinkedListNode* next() { return m_next.get(); }

  const DoublyLinkedListNode* prev() const { return m_prev; }
  DoublyLinkedListNode* prev() { return m_prev; }

  int value() const { return m_value; }

  // insert takes ownership of the passed-in DoublyLinkedListNode.
  void insertAfter(DoublyLinkedListNode* n) {
    n->m_next.reset(m_next.release());
    m_next.reset(n);
    n->m_prev = this;
  }

  void setPrev(DoublyLinkedListNode* prev) { m_prev = prev; }
  void setNext(DoublyLinkedListNode* next) { m_next.reset(next); }
  DoublyLinkedListNode* releaseNext() { return m_next.release(); }

private:
  int m_value;
  // TODO: We could use shared_ptr here.
  DoublyLinkedListNode* m_prev;
  std::unique_ptr<DoublyLinkedListNode> m_next;
};

class DoublyLinkedList {
public:
  DoublyLinkedList() : m_head(nullptr), m_tail(nullptr) {}

  // Make non-copiable for now.
  DoublyLinkedList(const DoublyLinkedList&) = delete;
  void operator=(const DoublyLinkedList&) = delete;

  const DoublyLinkedListNode* head() const { return m_head.get(); }
  DoublyLinkedListNode* head() { return m_head.get(); }

  const DoublyLinkedListNode* tail() const { return m_tail; }
  DoublyLinkedListNode* tail() { return m_tail; }

  void append(DoublyLinkedListNode* n) {
    if (!m_head) {
      m_head.reset(n);
      m_tail = n;
    } else {
      DoublyLinkedListNode* curr = m_head.get();
      assert(m_head);
      while (true) {
        DoublyLinkedListNode* next = curr->next();
        if (!next) {
          break;
        }
        curr = next;
      }
      curr->insertAfter(n);
      if (curr == m_tail) {
        m_tail = n;
      }
    }
  }

  // TODO: Ownership is not clear here. We should allow multiple ownership.
  // Right now we deallocate |n|.
  void remove(DoublyLinkedListNode* n) {
    if (!m_head) {
      // We should never hit this as we don't have any pointer to remove.
      assert(false);
      return;
    }

    if (head() == n) {
      DoublyLinkedListNode* next = m_head->releaseNext();
      next->setPrev(nullptr);
      m_head.reset(next);
    } else if (m_tail == n) {
      DoublyLinkedListNode* prev = m_tail->prev();
      prev->setNext(nullptr);
      m_tail = prev;
    } else {
      DoublyLinkedListNode* prev = m_head.get();
      DoublyLinkedListNode* curr = m_head->next();
      while (curr) {
        if (curr == n) {
          DoublyLinkedListNode* next = curr->releaseNext();
          prev->setNext(next);
          next->setPrev(prev);
          return;
        }
        prev = curr;
        curr = curr->next();
      }
      delete n;
    }
  }

  void insertBefore(DoublyLinkedListNode* before, DoublyLinkedListNode* newNode) {
    // This is a safety as the logic will delete whatever comes after...
    assert(!newNode->next());

    if (head() == before) {
      if (head() == m_tail) {
        m_tail = newNode;
      }
      m_head.release();
      newNode->setNext(before);
      before->setPrev(newNode);
      m_head.reset(newNode);
      return;
    }

    if (before == m_tail) {
      m_tail->setNext(newNode);
      newNode->setPrev(m_tail);
      m_tail = newNode;
      return;
    }

    DoublyLinkedListNode* prev = head();
    DoublyLinkedListNode* curr = prev->next();
    while (curr) {
      if (curr == before) {
        DoublyLinkedListNode* next = prev->releaseNext();
        prev->setNext(newNode);
        newNode->setNext(next);
        newNode->setPrev(prev);
        return;
      }
      prev = curr;
      curr = curr->next();
    }
    // Probably a bug if we didn't find |before|.
    assert(false);
  }

private:
  std::unique_ptr<DoublyLinkedListNode> m_head;
  DoublyLinkedListNode* m_tail;
};

std::ostream& operator<<(std::ostream& o, const DoublyLinkedList& l) {
  const DoublyLinkedListNode* head = l.head();
  o << std::endl;
  o << "  fwd{";
  const DoublyLinkedListNode* curr = head;
  while (curr) {
    if (curr != head) {
      o << " -> ";
    }
    o << curr->value(); 
    curr = curr->next();
  }
  o << "}" << std::endl;

  o << "  backwd{";
  curr = l.tail();
  while (curr) {
    if (curr != l.tail()) {
      o << " -> ";
    }
    o << curr->value(); 
    curr = curr->prev();
  }
  o << "}" << std::endl;
#ifndef NDEBUG
  o << " [head=" << l.head() << ", " << l.tail() << "]";
#endif
  return o;
}

int main() {
  DoublyLinkedList l;
  std::cout << "Empty linked list: " << l << std::endl;
  DoublyLinkedListNode* first = new DoublyLinkedListNode(10);
  l.append(first);
  std::cout << "After adding 10 to list: " << l << std::endl;
  DoublyLinkedListNode* second = new DoublyLinkedListNode(50);
  l.append(second);
  std::cout << "After adding 50 to list: " << l << std::endl;
  l.insertBefore(second, new DoublyLinkedListNode(25));
  std::cout << "After inserting 25 to list: " << l << std::endl;
  l.insertBefore(first, new DoublyLinkedListNode(5));
  std::cout << "After inserting 5 to list: " << l << std::endl;
  l.remove(first);
  first = nullptr;
  std::cout << "After removing 10 to list: " << l << std::endl;
  l.remove(second);
  second = nullptr;
  std::cout << "After removing last to list: " << l << std::endl;
  l.remove(l.head());
  std::cout << "After removing head to list: " << l << std::endl;

  return 0;
}
