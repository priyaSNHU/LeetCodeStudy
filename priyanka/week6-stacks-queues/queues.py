
class Queue:
    # queue class constructor
    def __init__(self):
    # initialise the queue
        self.queue = dequeue()
    
  # add data element to the queue
    def enqueue(self, data):
        self.queue.append(data)


        # check for duplicate entry
        # if data not in self.queue:
        #     self.queue.appendt(0, data)
        #     return True
        # else:
        #     return False
  
    # remove the first data element from the front  
    def dequeue(self):
        # check whether there is data in queue
        #Removes and returns the front item from the queue
        if not self.is_empty():
            return self.queue.pop()
   
    # check if queue is empty or not 
    def is_empty(self):
        if len(self.queue) > 0:
            return False
        else:
            return True
  
    # return the number of elements in the queue  
    def size(self):
        return len(self.queue)
   
    # print the complete queue 
    def show(self):
        print(self.queue)
    
    # search elemensts in queue
    def peek(self): 
        if not self.is_empty():
            return self.items[0]
