class HashTable:
    def __init__(self, size=10):
        self.size = size
        self.table = [[] for _ in range(size)]  # Create a list of empty buckets

    #returns the index of the key in the table 
    def _hash(self, key):
        """Compute the hash index for a given key using the built-in hash function."""
        # print("hash(key):", hash(key))
        return hash(key) % self.size

    def insert(self, key, value):
        """Insert a key-value pair into the hashtable."""
        index = self._hash(key)
        # print("index:", index)
        # we have for loop for pair since we may have multiple [key, value] in one index of table
        for pair in self.table[index]:
            if pair[0] == key:  # Update if the key already exists
                pair[1] = value
                return
        self.table[index].append([key, value])  # Add new key-value pair

    def find(self, key):
        """Find the value associated with a key."""
        index = self._hash(key)
        for pair in self.table[index]:
            if pair[0] == key:
                return pair[1]
        return None  # Key not found

    def update(self, key, value):
        """Update the value for an existing key."""
        index = self._hash(key)
        for pair in self.table[index]:
            if pair[0] == key:
                pair[1] = value
                return True
        return False  # Key not found

    def delete(self, key):
        """Delete a key-value pair from the hashtable."""
        index = self._hash(key)
        for i, pair in enumerate(self.table[index]):
            if pair[0] == key:
                self.table[index].pop(i)
                return True
        return False  # Key not found

    def display(self):
        """Display the hashtable for debugging."""
        for i, bucket in enumerate(self.table):
            print(f"Bucket {i}: {bucket}")


# Example Usage
if __name__ == "__main__":
    ht = HashTable(size=5)

    # Insert key-value pairs
    ht.insert("apple", 10)
    ht.insert("banana", 20)
    ht.insert("orange", 30)
    
    # Display table
    ht.display()
    
    # Find values
    print("Find 'apple':", ht.find("apple"))
    print("Find 'grape':", ht.find("grape"))
    
    # Update value
    ht.update("apple", 40)
    print("After updating 'apple':")
    ht.display()
    
    # Delete key
    ht.delete("banana")
    print("After deleting 'banana':")
    ht.display()
