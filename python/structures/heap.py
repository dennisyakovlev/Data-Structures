import math
from typing import Any

# Introduction to Algorithms 3rd edition 6.1 - 6.3

# Notes:
#  - To turn into c++, implement as contiguous memory array
#  - Do same trick with ending element, decrementing
#    iter.end to get last elem in heap

class Heap:

    def __init__(self, arr = []) -> None:
        self._arr = arr

    def __getitem__(self, i: int) -> Any:
        return self._arr[i]

    def __setitem__(self, i: int, elem: Any) -> None:
        if (i == len(self._arr)):
            self._arr.append(elem)
        else:
            self._arr[i] = elem

    def __len__(self) -> int:
        return len(self._arr)

    def __str__(self) -> str:
        return f'{self._arr}'

    def __bool__(self) -> bool:
        return bool(self._arr)

    def parent(self, i: int) -> int:
        return math.ceil(i / 2) - 1

    # return -1 when out of bounds
    def left(self, i: int) -> int:
        ret = (2 * i) + 1
        if ret > len(self) - 1:
            return -1
        return ret

    # return -1 when out of bounds
    def right(self, i: int) -> int:
        ret = (2 * i) + 2
        if ret > len(self) - 1:
            return -1
        return ret

    def swap(self, a: int, b: int) -> None:
        self._arr[a], self._arr[b] = self._arr[b], self._arr[a]

    def push_back(self, elem: Any) -> None:
        self._arr.append(elem)

    def remove(self, i: int) -> Any:
        return self._arr.pop(i)

def max_heapify(heap: Heap, i: int) -> None:

    left = heap.left(i)
    # safe to have right = -1 since heap[-1] (last elem) 
    # <= any other element in the heap, because of 
    # max_heapify precondition that left and right are
    # max heaps
    right = heap.right(i)
    
    if left == -1: # no leafs
        return None

    largest = i
    if heap[left] > heap[largest]:
        largest = left
    if heap[right] > heap[largest]:
        largest = right

    if not largest == i:
        heap.swap(i, largest)
        max_heapify(heap, largest)

def build_max_heap(heap: Heap) -> None:
    size = len(heap)
    for i in range(math.floor(size / 2), -1, -1):
        max_heapify(heap, i)