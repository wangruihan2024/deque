#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {

template<class T> class double_list{
  public:
    struct Node{
      T data;
      Node *pre, *next;
      Node(const T& data):data(data), pre(nullptr), next(nullptr){}
    };
    Node *head, *tail;
    size_t s;
    // --------------------------
    double_list():head(nullptr), tail(nullptr), s(0){
    }
    double_list(const double_list<T> &other){
          // clear();
          head = nullptr;
      tail = nullptr;
      Node *tmp = other.head;
      while(tmp) {
        insert_tail(tmp->data);
        tmp = tmp->next;
      }
          s = other.s;
      }
    ~double_list(){
      clear();
    }
  
    class iterator{
    public:
      Node *current;
      // --------------------------
      iterator(){}
      iterator(Node* t) : current(t){}
      iterator(const iterator &t) : current(t.current){}
      ~iterator(){}
          /**
       * iter++
       */
      iterator operator++(int) {
        iterator old = *this;
        if(current)
          current = current->next;
        else
          throw std::out_of_range("invalid++");
        return old;
      }
          /**
       * ++iter
       */
      iterator &operator++() {
        if(current)
          current = current -> next;
        else
          throw std::out_of_range("++invalid");
        return *this;	
      }
          /**
       * iter--
       */
      iterator operator--(int) {
        iterator old = *this;
        if(!current || !(current->pre))
          throw std::out_of_range("invalid--");
        else
          current = current->pre;
        return old;
      }
          /**
       * --iter
       */
      iterator &operator--() {
        if(!current || !(current->pre))
          throw std::out_of_range("--invalid");
        else {
          current = current->pre;
          return *this;
        }	
        
      }
      /**
       * if the iter didn't point to a value
       * throw " invalid"
      */
      T &operator*() const {
        if(!current)
          throw std::out_of_range("invalid");
        return this->current->data;
      }
          /**
           * other operation
          */
      T *operator->() const noexcept {
        if(!current)
          throw std::out_of_range("invalid");
        return &(current->data);
      }
      bool operator==(const iterator &rhs) const {
        return current == rhs.current;
      }
      bool operator!=(const iterator &rhs) const {
        return current != rhs.current;
      }
    };
    /**
     * return an iterator to the beginning
     */
    iterator begin(){
      return iterator(head);
    }
    /**
     * return an iterator to the ending
     * in fact, it returns the iterator point to nothing,
     * just after the last element.
     */
    iterator get_tail() {
      return iterator(tail);
    }
    iterator end(){
      return iterator(nullptr);
    }
    iterator begin() const { return iterator(head); }
    iterator get_tail() const { return iterator(tail); }
    iterator end() const { return iterator(nullptr); }
    /**
     * if the iter didn't point to anything, do nothing,
     * otherwise, delete the element pointed by the iter
     * and return the iterator point at the same "index"
     * e.g.
     * 	if the origin iterator point at the 2nd element
     * 	the returned iterator also point at the
     *  2nd element of the list after the operation
     *  or nothing if the list after the operation
     *  don't contain 2nd elememt.
    */
    iterator erase(iterator pos){
      if(!pos.current)
        return end();
      Node *tmp = pos.current;
      if(tmp->pre)
        tmp->pre->next = tmp->next;
      else
        head = tmp->next;
      if(tmp->next)
        tmp->next->pre = tmp->pre;
      else 
        tail = tmp->pre;
      iterator tmp_next = iterator(tmp->next);
      delete tmp;
      s--;
      return tmp_next;
    }
    /**
     * the following are operations of double list
    */
    void insert_head(const T &val){
      Node *new_node = new Node(val);
      if(!head) {
        head = new_node;
        tail = new_node;
      }else {
        head->pre = new_node;
        new_node->next = head;
        head = new_node;
      }
      s++;
    }
    void insert_tail(const T &val){
      Node *new_node = new Node(val);
      if(!tail) {
        head = new_node;
        tail = new_node;
      }else {
        tail->next = new_node;
        new_node->pre = tail;
        tail = new_node;
      }
      s++;
    }
    void delete_head(){
      if(!head)
        return;
      if(s == 1)
        head = tail = nullptr;
      else {
        Node* tmp = head;
        head = head->next;
        head->pre = nullptr;
        s--;
        delete tmp;
      }
    }
    void delete_tail(){
      if(!tail)
        return;
      if(s == 1)
        head = tail = nullptr;
      else {
        Node *tmp = tail;
        tail = tail->pre;
        tail->next = nullptr;
        s--;
        delete tmp;
      }
    }
    bool empty () const{
      return s == 0;
    }
    void clear() {
      Node* tmp = head;
      while(tmp) {
        Node* tmp_aft = tmp;
        tmp = tmp->next;
        delete tmp_aft;
      }
      head = nullptr;
      tail = nullptr;
      s = 0;
    }
    void print() const {
      Node *tmp = head;
      while(tmp) {
        std::cout << tmp->data << " ";
        tmp = tmp->next;
      }
      std::cout << std::endl;
    }
    size_t size() const { return s; }
    //--------------------------
    size_t init_size(const double_list &outer, iterator &pos) {
      size_t cnt_ = 0;
      iterator it_ = outer.begin();
      while(it_ != outer.end() && it_ != pos) {
        it_++;
        cnt_++;
      }
      delete it_;
      return cnt_;
    }
  };

template <class T> class deque {
  using list_it_type = double_list<T *>::iterator;
  using chunk_it_type = double_list<double_list<T *>>::iterator;
  static const double spilt_index = 1.5;
  static const double merge_index = 0.5;

public:
  double_list<double_list<T *>> data;
  size_t sum_s;
  size_t chunk_s;
  class const_iterator;
  class iterator {
  public:
    const deque *dq_it;
    double_list<T*>::iterator list_it;
    double_list<double_list<T*>>::iterator chunk_it;
    //--------------------------
    iterator() : dq_it(), list_it(), chunk_it(){}
    iterator(const deque *dq_it_, list_it_type list_it_, chunk_it_type chunk_it_) : dq_it(dq_it_), list_it(list_it_), chunk_it(chunk_it_){} 
    /**
     * return a new iterator which points to the n-next element.
     * if there are not enough elements, the behaviour is undefined.
     * same for operator-.
     */
    iterator operator+(const int &n) const {
      if(*this == dq_it->end())
        throw std::out_of_range();
      if(n == 0)
        return *this;
      else if(n < 0)
        return (*this) - (-n);
      int n_ = n;
      list_it_type list_it_ = list_it;
      chunk_it_type chunk_it_ = chunk_it;
      while(n_ > 0 && chunk_it_ != --(list_it_->end())) {
        chunk_it_++;
        n_--;
      }
      if(n == 0)
        return iterator(dq_it, list_it_, chunk_it_);
      if(list_it_ == --(dq_it->data.end())) {
        if(n > 1)
          throw std::out_of_range("add too much");
        else
          return dq_it->end();
      }else {
        list_it_++;
        while(n > list_it_->size() && list_it_ != --(dq_it->end())) {
          n -= list_it_->size();
          list_it_++;
        }
        if(list_it_ == --(dq_it->data.end())) {
          if(n > 1)
            throw std::out_of_range("add too much");
          else
            return dq_it->end();
        }
        chunk_it_ = list_it_->begin();
        while(n > 1) {
          chunk_it_++;
          n--;
        }
        return iterator(dq_it, list_it_, chunk_it_);
      }
    }
    iterator operator-(const int &n) const {
      if(*this == dq_it->begin())
        throw std::out_of_range();
      if(n == 0)
        return *this;
      else if(n < 0)
        return (*this) + (-n);
      int n_ = n;
      list_it_type list_it_ = list_it;
      chunk_it_type chunk_it_ = chunk_it;
      while(n_ > 0 && chunk_it != list_it->begin()) {
        n_--;
        chunk_it_--;
      }
      if(list_it == ++(dq_it->begin())) {
        if(n_ == 1)
          return dq_it->begin();
        else
          throw std::out_of_range("minus too much");
      }
      list_it--;
      while(n_ > list_it_->size() && list_it_ != dq_it->begin()) {
        n_ -= list_it_->size();
        list_it_--;
      }
      if (n_ > list_it_->size())
        throw std::out_of_range("minus too much");
      chunk_it_ = list_it_->end();
      while(n_ > 0) {
        chunk_it_ --;
        n_--;
      }
      return iterator(dq_it, list_it_, chunk_it_);
    }
    /**
     * return the distance between two iterators.
     * if they point to different vectors, throw
     * invaild_iterator.
     */
    int operator-(const iterator &rhs) const {
      if(dq_it != rhs.dq_it)
        throw std::out_of_range("distance from two deque");
      if(list_it == rhs.list_it) {
        size_t s1 = chunk_it->init_size((*list_it), chunk_it);
        size_t s2 = rhs.chunk_it->init_size((*rhs.list_it), rhs.chunk_it);
        return std::max(s1, s2) - std::min(s1, s2);
      }
      if(list_it->init_size(dq_it->data, list_it) < rhs.list_it->init_size(rhs.dq_it->data, rhs.list_it)) {
        size_t s1 = chunk_it->init_size((*list_it), chunk_it);
        size_t s2 = rhs.chunk_it->init_size((*rhs.list_it), rhs.chunk_it);
        size_t cnt = 0;
        for(auto it = list_it; it != rhs.list_it; it++)
          cnt += it->size();
        cnt += s2;
        cnt -= s1;
        return cnt;
      }else {
        return rhs - (*this);
      }
    }
    iterator &operator+=(const int &n) {
      return *this = *this + n;
    }
    iterator &operator-=(const int &n) {
      return *this = *this - n;
    }

    /**
     * iter++
     */
    iterator operator++(int) {
      iterator old = *this;
      *this = *this + 1;
      return old;
    }
    /**
     * ++iter
     */
    iterator &operator++() {
      return *this = *this + 1;
    }
    /**
     * iter--
     */
    iterator operator--(int) {
      iterator old = *this;
      *this = *this - 1;
      return old;
    }
    /**
     * --iter
     */
    iterator &operator--() {
      return *this = *this - 1;
    }

    /**
     * *it
     */
    T &operator*() const {
      if(chunk_it == list_it->end())
        throw std::out_of_range("invalid");
      return *chunk_it;
    }
    /**
     * it->field
     */
    T *operator->() const noexcept {
      if(chunk_it == list_it->end())
        throw std::out_of_range("invalid");
      return chunk_it->operator->();
    }

    /**
     * check whether two iterators are the same (pointing to the same
     * memory).
     */
    bool operator==(const iterator &rhs) const {
      if(dq_it == rhs.dq_it && list_it == rhs.list_it && chunk_it == rhs.chunk_it)
        return true;
      return false;
    }
    bool operator==(const const_iterator &rhs) const {
      if(dq_it == rhs.dq_it && list_it == rhs.list_it && chunk_it == rhs.chunk_it)
        return true;
      return false;
    }
    /**
     * some other operator for iterators.
     */
    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }
    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }
  };

  class const_iterator {
    /**
     * it should has similar member method as iterator.
     * you can copy them, but with care!
     * and it should be able to be constructed from an iterator.
     */
   public:
    const deque *dq_it;
    double_list<T*>::const_iterator list_it;
    double_list<double_list<T*>>::const_iterator chunk_it;
    //--------------------------
    const_iterator() : dq_it(), list_it(), chunk_it(){}
    const_iterator(const deque *dq_it_, const list_it_type list_it_, const chunk_it_type chunk_it_) : dq_it(dq_it_), list_it(list_it_), chunk_it(chunk_it_){} 
    const_iterator(const iterator &other) : dq_it(other.dq_it), list_it(other.list_it), chunk_it(other.chunk_it){}
    /**
     * return a new iterator which points to the n-next element.
     * if there are not enough elements, the behaviour is undefined.
     * same for operator-.
     */
    const_iterator operator+(const int &n) const {
      if(*this == dq_it->end())
        throw std::out_of_range();
      if(n == 0)
        return *this;
      else if(n < 0)
        return (*this) - (-n);
      int n_ = n;
      list_it_type list_it_ = list_it;
      chunk_it_type chunk_it_ = chunk_it;
      while(n_ > 0 && chunk_it_ != --(list_it_->end())) {
        chunk_it_++;
        n_--;
      }
      if(n == 0)
        return const_iterator(dq_it, list_it_, chunk_it_);
      if(list_it_ == --(dq_it->data.end())) {
        if(n > 1)
          throw std::out_of_range("add too much");
        else
          return dq_it->end();
      }else {
        list_it_++;
        while(n > list_it_->size() && list_it_ != --(dq_it->end())) {
          n -= list_it_->size();
          list_it_++;
        }
        if(list_it_ == --(dq_it->data.end())) {
          if(n > 1)
            throw std::out_of_range("add too much");
          else
            return dq_it->end();
        }
        chunk_it_ = list_it_->begin();
        while(n > 1) {
          chunk_it_++;
          n--;
        }
        return const_iterator(dq_it, list_it_, chunk_it_);
      }
    }
    const_iterator operator-(const int &n) const {
      if(*this == dq_it->begin())
        throw std::out_of_range();
      if(n == 0)
        return *this;
      else if(n < 0)
        return (*this) + (-n);
      int n_ = n;
      list_it_type list_it_ = list_it;
      chunk_it_type chunk_it_ = chunk_it;
      while(n_ > 0 && chunk_it != list_it->begin()) {
        n_--;
        chunk_it_--;
      }
      if(list_it == ++(dq_it->begin())) {
        if(n_ == 1)
          return dq_it->begin();
        else
          throw std::out_of_range("minus too much");
      }
      list_it--;
      while(n_ > list_it_->size() && list_it_ != dq_it->begin()) {
        n_ -= list_it_->size();
        list_it_--;
      }
      if (n_ > list_it_->size())
        throw std::out_of_range("minus too much");
      chunk_it_ = list_it_->end();
      while(n_ > 0) {
        chunk_it_ --;
        n_--;
      }
      return const_iterator(dq_it, list_it_, chunk_it_);
    }
    /**
     * return the distance between two iterators.
     * if they point to different vectors, throw
     * invaild_iterator.
     */
    int operator-(const const_iterator &rhs) const {
      if(dq_it != rhs.dq_it)
        throw std::out_of_range("distance from two deque");
      if(list_it == rhs.list_it) {
        size_t s1 = chunk_it->init_size((*list_it), chunk_it);
        size_t s2 = rhs.chunk_it->init_size((*rhs.list_it), rhs.chunk_it);
        return std::max(s1, s2) - std::min(s1, s2);
      }
      if(list_it->init_size(dq_it->data, list_it) < rhs.list_it->init_size(rhs.dq_it->data, rhs.list_it)) {
        size_t s1 = chunk_it->init_size((*list_it), chunk_it);
        size_t s2 = rhs.chunk_it->init_size((*rhs.list_it), rhs.chunk_it);
        size_t cnt = 0;
        for(auto it = list_it; it != rhs.list_it; it++)
          cnt += it->size();
        cnt += s2;
        cnt -= s1;
        return cnt;
      }else {
        return rhs - (*this);
      }
    }
    const_iterator &operator+=(const int &n) {
      return *this = *this + n;
    }
    const_iterator &operator-=(const int &n) {
      return *this = *this - n;
    }

    /**
     * iter++
     */
    const_iterator operator++(int) {
      const_iterator old = *this;
      *this = *this + 1;
      return old;
    }
    /**
     * ++iter
     */
    const_iterator &operator++() {
      return *this = *this + 1;
    }
    /**
     * iter--
     */
    const_iterator operator--(int) {
      const_iterator old = *this;
      *this = *this - 1;
      return old;
    }
    /**
     * --iter
     */
    const_iterator &operator--() {
      return *this = *this - 1;
    }

    /**
     * *it
     */
    const T &operator*() const {
      if(chunk_it == list_it->end())
        throw std::out_of_range("invalid");
      return *chunk_it;
    }
    /**
     * it->field
     */
    const T *operator->() const noexcept {
      if(chunk_it == list_it->end())
        throw std::out_of_range("invalid");
      return chunk_it->operator->();
    }

    /**
     * check whether two iterators are the same (pointing to the same
     * memory).
     */
    bool operator==(const iterator &rhs) const {
      if(dq_it == rhs.dq_it && list_it == rhs.list_it && chunk_it == rhs.chunk_it)
        return true;
      return false;
    }
    bool operator==(const const_iterator &rhs) const {
      if(dq_it == rhs.dq_it && list_it == rhs.list_it && chunk_it == rhs.chunk_it)
        return true;
      return false;
    }
    /**
     * some other operator for iterators.
     */
    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }
    bool operator!=(const const_iterator &rhs) const {
      return !(*this == rhs);
    }
  };

  /**
   * constructors.
   */
  deque() {}
  deque(const deque &other) {}

  /**
   * deconstructor.
   */
  ~deque() {}

  /**
   * assignment operator.
   */
  deque &operator=(const deque &other) {}

  /**
   * access a specified element with bound checking.
   * throw index_out_of_bound if out of bound.
   */
  T &at(const size_t &pos) {}
  const T &at(const size_t &pos) const {}
  T &operator[](const size_t &pos) {}
  const T &operator[](const size_t &pos) const {}

  /**
   * access the first element.
   * throw container_is_empty when the container is empty.
   */
  const T &front() const {}
  /**
   * access the last element.
   * throw container_is_empty when the container is empty.
   */
  const T &back() const {}

  /**
   * return an iterator to the beginning.
   */
  iterator begin() {}
  const_iterator cbegin() const {}

  /**
   * return an iterator to the end.
   */
  iterator end() {}
  const_iterator cend() const {}

  /**
   * check whether the container is empty.
   */
  bool empty() const {}

  /**
   * return the number of elements.
   */
  size_t size() const {
    return sum_s;
  }

  /**
   * clear all contents.
   */
  void clear() {}

  /**
   * insert value before pos.
   * return an iterator pointing to the inserted value.
   * throw if the iterator is invalid or it points to a wrong place.
   */
  iterator insert(iterator pos, const T &value) {}

  /**
   * remove the element at pos.
   * return an iterator pointing to the following element. if pos points to
   * the last element, return end(). throw if the container is empty,
   * the iterator is invalid, or it points to a wrong place.
   */
  iterator erase(iterator pos) {}

  /**
   * add an element to the end.
   */
  void push_back(const T &value) {}

  /**
   * remove the last element.
   * throw when the container is empty.
   */
  void pop_back() {}

  /**
   * insert an element to the beginning.
   */
  void push_front(const T &value) {}

  /**
   * remove the first element.
   * throw when the container is empty.
   */
  void pop_front() {}
};

} // namespace sjtu

#endif
