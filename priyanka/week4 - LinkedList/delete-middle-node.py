# Definition for singly-linked list.
class ListNode:
    def __init__(self, val=0, next=None):
        self.val = val
        self.next = next

## UMPIRE: 
# U: undestand  :
#. Practice: ask questions and discuss corner cases:
# M: Match :
# practice: always match with known DS or Algorithms to solve problem , run with an example
#P: Planning / pseudocode : 
# practice: come up with steps on how to solve/approach problem
# funtion length of lll
# d middle node :
#  length=  fun(len):
#    mi = l //2
## I: Implement
## R: Revise: Current Implementation:
#  Time complexity O(n) as i need to perform 2 traversals ( finding length and delete moddle node doing another traversal)

## Improvements : May be instead of 2 traversals can reduce to 1 travesal by using double pointers
# Space Complexity O(1)

class Solution:
    def lenll(head):
        #ll None
        i = 0
        while head != None:
            head = head.next
            i += 1
        return i
    
    def deleteMiddle(self, head: Optional[ListNode]) -> Optional[ListNode]:
        ### Corner cases 1. if node is empty
        if head == None or head.next == None:
            return None
        ## 2. if list has only 1 node
        # if head.next == None:
        #     return None
        
        current = head
        #length of LL
        i = Solution.lenll(head)
        mid_len = i // 2

        ## delete middle node while looping through list
        while mid_len > 1 :
            mid_len -= 1
            head = head.next
        head.next = head.next.next

        return current
    
    def display(self):
        current = self.head
        while current:
            print(current.value)
            current = current.next



### With 1 traversal:

def deleteMiddle(self,head):
    ### Corner cases 1. if node is empty
    if head == None or head.next == None:
        return None
        ## 2. if list has only 1 node
        # if head.next == None:
        #     return None
    
    left_pointer = head
    right_pointer = head
    mid_pointer = None

    ## Traversal for LL to find and delete mid node

    while (right_pointer != None) and (right_pointer.next != None):
        right_pointer = right_pointer.next.next
        mid_pointer = left_pointer
        left_pointer = left_pointer.next
    
    mid_pointer.next = left_pointer.next
    return head


