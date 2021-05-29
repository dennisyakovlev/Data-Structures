from typing import Any
from python.pqueue import heap

# Notes:
# - build_max_heap and insert producde different
#   results

class KeyError(Exception):
    def __init__(self, *args: object) -> None:
        super().__init__(*args)

class PriorityQueue:

    def __init__(self, arr = []) -> None:
        self._heap = heap.Heap(arr)
        heap.build_max_heap(self._heap)

    def __str__(self) -> str:
        return str(self._heap)

    def insert(self, elem: Any) -> None:
        self._heap.push_back(elem)
        return self.set_priority(elem, len(self._heap) - 1)

    def max(self) -> Any:
        return self._heap[0]

    def get_max(self) -> Any:
        self._heap.swap(0, len(self._heap) - 1)
        max = self._heap.remove(len(self._heap) - 1)
        heap.max_heapify(self._heap, 0)
        return max

    def set_priority(self, elem: Any, i: int) -> int:
        ''' Change elem <i> to value <elem> and adjust position
        '''
        if not self._heap:
            self._heap.push_back(elem)
            return
            
        if (not len(self._heap) - 1 == i) and elem < self._heap[i]:
            print('err')
            raise KeyError(f'{elem} smaller than current elem {self._heap[i]}')

        self._heap[i] = elem
        parent = self._heap.parent(i)
        while i > 0 and self._heap[parent] < self._heap[i]:
            self._heap.swap(i, parent)
            i = parent
            parent = self._heap.parent(i)
        
        return i