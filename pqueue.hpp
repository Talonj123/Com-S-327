#ifndef _PQUEUE_H_
#define _PQUEUE_H_
#include <iostream>
#include <algorithm>
#include <vector>

template<typename T>
class PriorityQueue
{
private:
  std::vector<T> data;
  bool (*comparer)(T a, T b);

public:
  inline PriorityQueue(bool (*compare)(T a, T b)) : data()
  {
    comparer = compare;
    std::make_heap(data.begin(), data.end(), comparer);
  }

  inline void enqueue(T item)
  {
    data.push_back(item);
    std::push_heap(data.begin(), data.end(), comparer);
  }
  
  inline T peek()
  {
    return data.front();
  }
  
  inline T dequeue()
  {
    T element = data.front();
    std::pop_heap(data.begin(), data.end(), comparer);
    data.pop_back();
    return element;
  }

  inline bool empty()
  {
    return data.empty();
  }

  inline int size()
  {
    return data.size();
  }

  inline void decrease_priority(T element, char (*equals)(T a, T b))
  {
    int i;
    for (i = 0; i < data.size(); i++)
    {
      if (equals(data[i], element))
      {
	data[i] = element;
	break;
      }
    }
    std::make_heap(data.begin(), data.end(), comparer);
  }
};

#endif //_PQUEUE_H_
