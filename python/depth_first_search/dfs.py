from math import floor
from typing import Any

# assume complete binary tree for now
class dfs:

    def __init__(self, arr) -> None:
        self._mid = floor(len(arr) / 2)
        self._curr = 0
        self._arr = arr
        self._second_time = False

    def _right_parent(self, index: int) -> int:
        return self._right_child(self._parent(self._curr))

    def _parent(self, index: int) -> int:
        return (index - 1) // 2

    def _left_child(self, index: int) -> int:
        return (index * 2) + 1

    def _right_child(self, index: int) -> int:
        return self._left_child(index) + 1

    def next(self) -> Any:
        # check for end case
        # think of some better solution
        # perhaps with the go up loop being implemented
        # such that it accounts for end case and goes past
        # end of array
        if self._curr == len(self._arr) - 1:
            to_ret = self._curr
            self._curr = None
            return self._arr[to_ret]
        if self._curr is None:
            return None
        
        to_ret = self._curr
        self._second_time = True

        if self._curr >= len(self._arr) // 2:
            if self._curr % 2 == 1: # on bottom row as left child, go right one
                self._curr += 1
                return self._arr[to_ret]

            # on bottom row as right child, go up to not yet seen element
            while self._right_child(self._parent(self._curr)) == self._curr:
                self._curr = self._parent(self._curr)
            self._curr += 1
            return self._arr[to_ret]

        self._curr = self._left_child(self._curr) # not on bottom row, go to left child
        return self._arr[to_ret]

    def test(self):
        num = 12
        while self._right_child(self._parent(num)) == num:
            num = self._parent(num)
        print(num)
            
