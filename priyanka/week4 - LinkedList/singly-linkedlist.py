class Node:
    def __init__(self,value):
        self.value = value
        self.next = None

class LinkedList:
    def __init__(self,head=None):
        self.head = head
    
    def insert(self, value , index):
        current = self.head
        i = 0
        if index == 0:
            current = self.head
        elif index == 1:
            value.next = self.head
            self.head = value
        
        while current:
            if i+1 == index:
                value.next = current.next
                current.next = value
                return
            else:
                i+=1
                current = current.next
        pass

    def update(self, value, index):
        current = self.head
        i = 0
        if i == index:
            current.value = value
        else:
            while (current != None and i != index):
                i = i+1
                current = current.next
            if current != None:
                current.value = value
            else:
                return ("Invalid update operation")

    ### Need to implement with index as well
    def delete(self, value):
        current = self.head
        if current.value == value:
            self.head = current.next
        else:
            while current:
                if current.value == value:
                    break
                prev = current
                current = current.next
            if current == None:
                return
            prev.next = current.next
            current = None

    def display(self):
        current = self.head
        while current:
            print(current.value)
            current = current.next
    

ll = LinkedList()

ll.insert('a')
ll.update('b', 1)
ll.display()
ll.delete('b')


