#removing stars from string

##UMPIRE
# remove 
##
#Q/corner cases
# empty string
# string with only stars
# The string might contain consecutive stars, which should remove multiple characters.
# stars at begining of string, should handle the case
# Match: use stacks
#P: pseudocode
#1. Intialize empty stack
#2. Iterate through string
#if char != *
#push char to stack
#else
#pop elements from stack
#Join the characters in the stack: 
# Implementation

def remove_stars(star_string):
    string_stack = []
    for each_char in star_string:
        if each_char != '*':
            string_stack.push(each_char)
        else:
            string_stack.pop()
    return ''.join(string_stack)

# Time complexity:
# length of string O(n)

#space complexity
# 1 stack, and need to fill all elements if string doesn't have *

