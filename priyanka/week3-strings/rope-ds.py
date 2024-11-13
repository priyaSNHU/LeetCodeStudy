class Rope(object):
    def __init__(self, var=''):
        self.var = var
        self.left = left
        self.right = right
        // length of the variable/string defined
        self.length = len(var)

        ## TO_DO:
        ## 1. consider cases like what if given vaule is not string:


    def __str__(self):
        if self.left is None and self.right is None:
            return self.var
        else:
            return str(self.left) + str(self.right)

    def __search__(self, i):
        if i < len(self.left):
            return self.left[i]
        else:
            return self.right[i - len(self.left)]

        ## TODO:
        if i < 0:
            i += self.length

    def __insert__ (self, var):
        return Rope([str(self), str(var)])

    def __split__ (self, i):
        return Rope([str(left), str(self.var)]), Rope(str(right))

        ## todo cases
        if i == 0:
            return Rope(''),self
        if i < 0 or > len(self.var):
            return "Not Valid Index"

