#Given an encoded string, return its decoded string.

## UMPIRE
# decode given encoded string 
#The encoding rule is: k[encoded_string], where the encoded_string inside the square brackets is being repeated exactly k times
#1. empty string
#2. Nested Brackets 3[a4[ac]]
#3. Multiple Digits in the Multiplier 55[c]
#4. zeros in the inputs 001[c]
#5. unbalanced inputs for ex, missing brackets
#6. Nondigit/non-letter characters 3[p@]
#M:
# We can use stacks to iterate the through string and place number in 1 stack and chars in other stack
#p:
#pseudocode:
# 1. iterate through string 
# if current = digit:
# build a number by concatenating digits until a non-digit character is reached. Push this number onto the integer stack. 
#if current = '['
# append count to number stack and char to char stack and reset num and str values
#if current = '['
#1. Pop the count from the number stack. 
#2. pop the char from char stack
#3. Repeat the popped char var the specified number of times and push the repeated char back onto the char stack. 
# else current = char
#  append it to the current char var. 
#I
## using stacks
def decode_string(encode_string):
    char_stack = []
    num_stack = []
    curr_num = 0
    decode_string = ""

    for each_char in encode_string:
        if each_char.isdigit():
            curr_num = curr_num * 10 + int(each_char)
        elif each_char == '[':
            char_stack.push(each_char)
            num_stack.push(curr_num)
            curr_num = 0
            decode_string = ""
        elif each_char == ']':
            prev_char = char_stack.pop()
            prev_num = num_stack.pop()
            decode_string = prev_char + decode_string * prev_num
        else:
            decode_string += each_char
    return decode_string

## Time Complexity
# length of string O(n)

## Space:
# 2 stacks and 4 variables


### Using recurssion:
def decode_string(encoded_string):
    curr_num = 0
    decode_string = ""

    for i in range(len(encoded_string)):
        if encoded_string[i].isdigit():
            curr_num = curr_num * 10 + int(each_char)
        elif encoded_string[i] == '[':
            string = decode_string()


        elif char == ']':
        else:
            decode_string = prev_char 




        


