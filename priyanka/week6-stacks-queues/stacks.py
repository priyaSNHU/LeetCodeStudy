class Stack:

    #LIFO
    def __init__(self):
        self.items = []
    
    #Add elements to stack
    def push(self, data):
        self.items.append(data)
    
    #Delete Elements from stack
    def pop(self):
        if not self.is_empty():
            return self.items.pop()
    
    #search element in stack
    def peek(self):
        if not self.is_empty():
            return self.items[-1]
    
    #check if stack is empty
    def is_empty(self):
        return len(self.items) == 0
    
    #Size of stack
    def size(self):
        return len(self.stack)