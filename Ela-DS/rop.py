class RopeNode:
    def __init__(self, value=""):
        self.value = value
        self.left = None
        self.right = None
        self.weight = len(value)

class Rope:
    def __init__(self, initial_string=""):
        self.root = RopeNode(initial_string)

    def _split(self, node, index):
        if not node:
            return None, None
        
        if index <= node.weight:
            left, right = self._split(node.left, index)
            new_right = RopeNode(node.value + (right.value if right else ""))
            new_right.right = node.right
            new_right.left = right
            new_right.weight = len(new_right.value)
            return left, new_right
        
        else:
            left, right = self._split(node.right, index - node.weight)
            new_left = RopeNode(node.value)
            new_left.left = node.left
            new_left.right = left
            new_left.weight = len(new_left.value)
            return new_left, right

    def _concatenate(self, left, right):
        if not left:
            return right
        if not right:
            return left
        root = RopeNode()
        root.left = left
        root.right = right
        root.weight = len(left.value)
        return root

    def insert(self, index, value):
        left, right = self._split(self.root, index)
        new_node = RopeNode(value)
        self.root = self._concatenate(self._concatenate(left, new_node), right)

    def _find(self, node, index):
        if not node:
            return ""
        if index < node.weight:
            return self._find(node.left, index)
        else:
            return self._find(node.right, index - node.weight)

    def lookup(self, index):
        return self._find(self.root, index)

    def _delete(self, node, start, end):
        left, middle = self._split(node, start)
        middle, right = self._split(middle, end - start)
        return self._concatenate(left, right)

    def delete(self, start, end):
        self.root = self._delete(self.root, start, end)

    def _update(self, node, index, value):
        if not node:
            return RopeNode(value)
        if index < node.weight:
            node.left = self._update(node.left, index, value)
        elif index < node.weight + len(node.value):
            node.value = node.value[:index - node.weight] + value + node.value[index - node.weight + len(value):]
        else:
            node.right = self._update(node.right, index - node.weight, value)
        node.weight = len(node.value)
        return node

    def update(self, index, value):
        self.root = self._update(self.root, index, value)

    def to_string(self, node=None):
        if node is None:
            node = self.root
        
        
        if node is None:
            return ""
        
        
        left_string = self.to_string(node.left) if node.left else ""
        right_string = self.to_string(node.right) if node.right else ""
        
        return left_string + node.value + right_string


# Example usage:
rope = Rope("Hello, World!")
print("Initial string:", rope.to_string())  # Outputs: "Hello, World!"

# Insert
rope.insert(7, "beautiful ")
print("After insert:", rope.to_string())  # Outputs: "Hello, beautiful World!"

# Lookup
print("Character at index 7:", rope.lookup(7))  # Outputs: "b"

# Update
rope.update(7, "wonderful")
print("After update:", rope.to_string())  # Outputs: "Hello, wonderful World!"

# Delete
rope.delete(7, 16)
print("After delete:", rope.to_string())  # Outputs: "Hello, World!"
