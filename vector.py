import math

class Vector(object):
  def __init__(self, size: int, d: int):
    self.size = 0
    self.capacity = 0
    self.backing = []
    self._growIfNeeded(size)
    # TODO: We could do a one-off size * [d] to avoid the for-loop.
    for i in range(0, size):
      self.backing[i] = d
    self.size = size

  def __len__(self):
    return self.size

  def __getitem__(self, i: int) -> int:
    if i < 0 or i >= self.size:
      raise IndexError("")

    return self.backing[i]

  def __setitem__(self, i: int, v: int):
    if i < 0 or i >= self.size:
      raise IndexError("")

    self.backing[i] = v    

  def __delitem__(self, i: int):
    if i < 0 or i >= self.size:
      raise IndexError("")

    for j in range(i + 1, self.size):
      self.backing[j] = self.backing[j + 1]
    self.size -= 1
    
  # TODO: Add __add__ and all to be more Pythonistic.
  def insert(self, i: int, v: int):
    if i < 0 or i > self.size:
      raise IndexError("")

    self._growIfNeeded(self.size + 1)
    for j in range(self.size - 1, i - 1, -1):
      self.backing[j + 1] = self.backing[j]
    self.size += 1
    self.backing[i] = v

  def _growIfNeeded(self, capacity: int):
    if self.capacity >= capacity:
      return

    newCapacity = math.floor(1.4 * capacity)
    newBacking = newCapacity * [0]
    for i in range(0, self.size):
      newBacking[i] = self.backing[i]
    self.backing = newBacking
    self.capacity = newCapacity

  def append(self, v: int):
    self.insert(self.size, v)

  def dump(self, prefix : str):
    s = prefix
    s += "["
    for i in range(0, self.size):
      if i > 0:
        s += ", "
      s += str(self.backing[i])
    s += "]"
    print(s)

def main():
  s = Vector(2, 10)
  s.dump("Initial vector: ")
  s[1] = 20
  s.dump("After mutating 2nd element: ")
  s.insert(0, 30)
  s.dump("After inserting: ")
  del s[1]
  s.dump("After deleting 2nd element: ")
  s.append(50)
  s.dump("After appending: ")

main()
