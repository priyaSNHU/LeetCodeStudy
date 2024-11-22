class Solution:
    def findDifference(self, nums1: List[int], nums2: List[int]) -> List[List[int]]:

        ##UMPIRE:
        # 2 lists return 2 lists which doesn't exist in each other array
        # Corner cases:
        # one of the list empty
        # strings/etc
        # same list
        # M: Using sets and lists, hashtables
        #Pseudocode:
        # set of list to eliminate duplicates
        # iterate each element from nums1 to check if key present in num2 and copy in nums_list1
        # Implement:
        s1 = set(nums1)
        s2 = set(nums2)
        diff_nums1 = [key for key in s1 if key not in s2]
        diff_nums2 = [key for key in s2 if key not in s1]
        return [diff_nums1 , diff_nums2]

        ## Timecomplexity:  O(m + n) [length of nums1 and length of nums2]






        