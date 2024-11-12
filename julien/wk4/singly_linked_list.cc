// Build using:
//   g++ -Wall -Werror --sanitize=address -g -o singly_linked_list singly_linked_list.cc && ./singly_linked_list
#include <cassert>
#include <iostream>
#include <memory>

// Singly linked list implementation.

// TODO: Templatize payload.
class SinglyLinkedListNode {
public:
  SinglyLinkedListNode(int value) : m_value(value), m_next(nullptr) {}

  // Make non-copiable for now.
  SinglyLinkedListNode(const SinglyLinkedListNode&) = delete;
  void operator=(const SinglyLinkedListNode&) = delete;

  const SinglyLinkedListNode* next() const { return m_next.get(); }
  SinglyLinkedListNode* next() { return m_next.get(); }

  int value() const { return m_value; }

  // insert takes ownership of the passed-in SinglyLinkedListNode.
  void insert(SinglyLinkedListNode* n) {
    n->m_next.reset(m_next.release());
    m_next.reset(n);
  }

  void setNext(SinglyLinkedListNode* next) { m_next.reset(next); }
  SinglyLinkedListNode* releaseNext() { return m_next.release(); }

private:
  int m_value;
  std::unique_ptr<SinglyLinkedListNode> m_next;
};

class SinglyLinkedList {
public:
  SinglyLinkedList() : m_head(nullptr) {}

  // Make non-copiable for now.
  SinglyLinkedList(const SinglyLinkedList&) = delete;
  void operator=(const SinglyLinkedList&) = delete;

  const SinglyLinkedListNode* head() const { return m_head.get(); }
  SinglyLinkedListNode* head() { return m_head.get(); }

  void append(SinglyLinkedListNode* n) {
    if (!m_head) {
      m_head.reset(n);
    } else {
      SinglyLinkedListNode* curr = m_head.get();
      assert(m_head);
      while (true) {
        SinglyLinkedListNode* next = curr->next();
        if (!next) {
          break;
        }
        curr = next;
      }
      curr->insert(n);
    }
  }

  // TODO: Ownership is not clear here. We should allow multiple ownership.
  // Right now we deallocate |n|.
  void remove(SinglyLinkedListNode* n) {
    if (!m_head) {
      // We should never hit this as we don't have any pointer to remove.
      assert(false);
      return;
    }

    if (head() == n) {
      SinglyLinkedListNode* next = m_head->releaseNext();
      m_head.reset(next);
    } else {
      SinglyLinkedListNode* prev = m_head.get();
      SinglyLinkedListNode* curr = m_head->next();
      while (curr) {
        if (curr == n) {
          SinglyLinkedListNode* next = curr->releaseNext();
          prev->setNext(next);
          return;
        }
        prev = curr;
        curr = curr->next();
      }
      delete n;
    }
  }

  void insertBefore(SinglyLinkedListNode* before, SinglyLinkedListNode* newNode) {
    // This is a safety as the logic will delete whatever comes after...
    assert(!newNode->next());

    if (head() == before) {
      m_head.release();
      newNode->setNext(before);
      m_head.reset(newNode);
      return;
    }

    SinglyLinkedListNode* prev = head();
    SinglyLinkedListNode* curr = prev->next();
    while (curr) {
      if (curr == before) {
        SinglyLinkedListNode* next = prev->releaseNext();
        prev->setNext(newNode);
        newNode->setNext(next);
        return;
      }
      prev = curr;
      curr = curr->next();
    }
    // Probably a bug if we didn't find |before|.
    assert(false);
  }

private:
  std::unique_ptr<SinglyLinkedListNode> m_head;
};

std::ostream& operator<<(std::ostream& o, const SinglyLinkedList& l) {
  const SinglyLinkedListNode* head = l.head();
  o << "{";
  const SinglyLinkedListNode* curr = head;
  while (curr) {
    if (curr != head) {
      o << " -> ";
    }
    o << curr->value(); 
    curr = curr->next();
  }
  o << "}";
  return o;
}

int main() {
  SinglyLinkedList l;
  std::cout << "Empty linked list: " << l << std::endl;
  SinglyLinkedListNode* first = new SinglyLinkedListNode(10);
  l.append(first);
  std::cout << "After adding 10 to list: " << l << std::endl;
  SinglyLinkedListNode* second = new SinglyLinkedListNode(50);
  l.append(second);
  std::cout << "After adding 50 to list: " << l << std::endl;
  l.insertBefore(second, new SinglyLinkedListNode(25));
  std::cout << "After inserting 25 to list: " << l << std::endl;
  l.insertBefore(first, new SinglyLinkedListNode(5));
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
