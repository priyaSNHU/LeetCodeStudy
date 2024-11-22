class HashTable:
    def __init__(self):
        self.table = {}

    def add(self, key, value):
        hash_value = hash(key)
        bitmask = 0b11111111  # 8-bit mask
        masked_hash = hash_value & bitmask
        self.table[masked_hash] = value

    def get(self, key):
        hash_value = hash(key)
        bitmask = 0b11111111
        masked_hash = hash_value & bitmask
        return self.table.get(masked_hash)