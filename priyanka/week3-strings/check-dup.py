## https://leetcode.com/problems/contains-duplicate/description/
def check_dup (arr_list):
    len_arr = len(arr_list)

    for i in range(len_arr - 1):
        for j in (i+1 , len_arr):
            if arr_list[i] == arr_list[j]:
                return True
    return False
## O(n2)

def check_dup (arr_list):
    len_arr = len(arr_list)

    set_arr = set()

    for i in range(len_arr):
        if arr_list[i] in set_arr:
            return True
        else:
            set_arr.add
    return False