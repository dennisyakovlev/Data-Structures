from math import floor, log2
from typing import Any

# assume complete binary tree for now
class dfs:

    def __init__(self, arr) -> None:
        self._curr = 0
        self._arr = arr[:]
        self._unvisited = len(arr)
        #self._arr.append(None)

    def _right_parent(self, index: int) -> int:
        return self._right_child(self._parent(self._curr))

    def _parent(self, index: int) -> int:
        return (index - 1) // 2

    def _left_child(self, index: int) -> int:
        return (index * 2) + 1

    def _right_child(self, index: int) -> int:
        return self._left_child(index) + 1

    def deref(self):
        return self._arr[self._curr]

    def next(self) -> Any:
        
        if self._unvisited == 0:
            return -1

        if self._curr >= len(self._arr) // 2: # on bottom row
            while self._right_child(self._parent(self._curr)) == self._curr \
                or self._right_child(self._parent(self._curr)) == len(self._arr): # incase bottom row is not fully filled
                                                                                  # and on left child
                self._curr = self._parent(self._curr)
            self._curr += 1
            self._unvisited -= 1
            return None

        self._curr = self._left_child(self._curr) # not on bottom row, go to left child
        self._unvisited -= 1
        return None