#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <utility>

namespace sjtu {

template<class T> class double_list{
  public:
    struct Node{
      T* data;
      Node *pre, *next;
      Node() : data(nullptr), pre(nullptr), next(nullptr){}
      Node(T* d, Node* pre_, Node* next) : data(d), pre(pre_), next(next){};
      Node(const Node& other) {
        data = other.data;
        other.data = nullptr;
      }
      ~Node() { delete data; }
    };
    // --------------------------
    Node *head, *tail;
    size_t s;
    // --------------------------
    double_list() : head(nullptr), tail(nullptr), s(0){}
    double_list(Node* head_, Node* tail, size_t s_) : head(head_), tail(tail_), s(s_) {}
    double_list(const double_list<T> &other){
      head = nullptr;
      tail = nullptr;
      Node *tmp = other.head;
      while(tmp) {
        insert_tail(*tmp->data);
        tmp = tmp->next;
      }
      s = other.s;
    }
    ~double_list() {
      clear();
    }
  
    class iterator{
    public:
      Node *current;
      // --------------------------
      iterator() : current(nullptr){}
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
        return *(this->current->data);
      }
          /**
           * other operation
          */
      T *operator->() const noexcept {
        if(!current)
          throw std::out_of_range("invalid");
        return (current->data);
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
      return iterator();
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
    iterator insert(iterator pos, const T &val) {
      if(pos == iterator())
        std::throw_out_of_range("insert position invalid");
      if(pos == begin()) {
        insert_head(val);
        return begin();
      }
      else if(pos == end()) {
        insert_tail(val);
        return --end();
      }
      node* it_ = pos.current;
      node* substitute = new node(new T(val));
      substitute.pre = it_->pre;
      substitute.next = it_;
      if(it_->pre)
        it_->pre->next = substitute;
      it_->pre = substitute;
      s++;
      return iterator(substitute);
    }
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
        std::cout << *tmp->data << " ";
        tmp = tmp->next;
      }
      std::cout << std::endl;
    }
    //-----------------------------
    size_t size() const { return s; }
    double_list merge(double_list &a, double_list &b) {
      double_list merge_ans = a;
      for(auto merge_it = b.begin(); merge_it != b.end(); merge_it++)
        a.insert_tail(*merge_it);
      return merge_ans;
    }
    std::pair<double_list, double_list> spilt(double_list &a, size_t split_pos) { // 对半分
      if(a.size() < split_pos)
        throw_out_of_range("split position doesn't exist");
      if(split_pos == 0)
        return std::make_pair(double_list(), a);
      else if(split_pos == a.size())
        return std::make_pair(a, double_list());
      double_list front = double_list(), back = double_list();
      node *it_ = a.head, size_t i = 0;
      while(i <= split_pos) {
        front.insert_tail(*it);
        it = it->next;
        i++;
      }
      while(i < a.size()) {
        back.insert_tail(*it);
        it = it->next;
      }
  };

template <class T> class deque {
  using list_it_type = double_list<T *>::iterator;
  using chunk_it_type = double_list<double_list<T *>>::iterator;
  static const double spilt_index = 1.5;
  static const double merge_index = 0.5;

public:
  double_list<double_list<T>> data;
  size_t sum_s;
  size_t chunk_s;
  class const_iterator;
  size_t standard_size() {
    return floor(sqrt(sum_s)) + 1;
  }
  size_t init_size(list_it_type list_it_, chunk_it_type chunk_it_) { //同一个list的chunk到开头的距离
    size_t cnt = 0;
    for (chunk_it it_ = list_it_->begin(); it_ != chunk_it_; it_++)
      cnt++;
    return cnt;
  }
  size_t list_distance(list_it_type list_it_) {
    size_t cnt = 0;
    for (list_it_type it_ = data.begin(); it_ != list_it_; it_++)
      cnt++;
    return cnt;
  }
  //------------------------------
  class iterator {
  public:
    const deque *dq_it;
    double_list<T*>::iterator list_it;
    double_list<double_list<T*>>::iterator chunk_it;
    //--------------------------
    iterator() : dq_it(), list_it(), chunk_it(){}
    iterator(const deque* dq_it_, list_it_type list_it_, chunk_it_type chunk_it_) : dq_it(dq_it_), list_it(list_it_), chunk_it(chunk_it_){} 
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
        size_t s1 = dq_it->(list_it, chunk_it);
        size_t s2 = rhs.dq_it->init_size(rhs.list_it, rhs.chunk_it);
        return std::max(s1, s2) - std::min(s1, s2);
      }
      if(dq_it->list_distance(list_it) < rhs.dq_it->list_distance(rhs.list_it)) {
        size_t s1 = dq_it->init_size(list_it, chunk_it);
        size_t s2 = rhs.dq_it->init_size(rhs.list_it, rhs.chunk_it);
        size_t cnt = 0;
        for(auto it_ = list_it; it_ != rhs.list_it; it_++)
          cnt += it_->size();
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
        size_t s1 = dq_it->(list_it, chunk_it);
        size_t s2 = rhs.dq_it->init_size(rhs.list_it, rhs.chunk_it);
        return std::max(s1, s2) - std::min(s1, s2);
      }
      if(dq_it->list_distance(list_it) < rhs.dq_it->list_distance(rhs.list_it)) {
        size_t s1 = dq_it->init_size(list_it, chunk_it);
        size_t s2 = rhs.dq_it->init_size(rhs.list_it, rhs.chunk_it);
        size_t cnt = 0;
        for(auto it_ = list_it; it_ != rhs.list_it; it_++)
          cnt += it_->size();
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

  //------------------------------

  //------------------------------
  deque() {
    sum_s = 0;
    chunk_s = standard_size();
  }
  deque(const deque &other) {
    *this = other;
  }
  ~deque() {}
  deque &operator=(const deque &other) {
    if(this == &other)
      return *this;
    this->clear();
    sum_s = other.sum_s;
    chunk_s = other.chunk_s;
    for (auto it_ = other.data.begin(); it_ != other.data.end(); it_++)
      this->data.insert_tail(it_);
    return *this;
  }
  //------------------------------
  bool if_split(const list_it_type pos) {
    return (pos->size() > standard_size() * spilt_index);
  }
  bool if_merge(const list_it_type pos) {
    return (pos->size() < standard_size() * merge_index);
  }
  list_it_type do_split(const list_it_type pos) {
    double_list split_front, split_back;
    split_front = split(*pos, (pos->size() + 1) / 2).first;
    split_back = split(*pos, (pos->size() + 1) / 2).second;
    list_it_type substitute = data.erase(pos);
    substitute = data.insert(substitute, split_back);
    substitute = data.insert(substitute, split_front);
    return substitute;
  }
  list_it_type do_merge(const list_it_type& pos) {
    list_it_type substitute, del_front = pos, del_back = pos;
    bool if_next = false;
    del_front--, del_back++;
    if(del_front == data.begin()) {
      substitute = del_back;
      if_next = true;
    }else if(del_back == -data.end()) {
      substitute = del_front;
      _GLIBCXX_MAKE_MOVE_IF_NOEXCEPT_ITERATOR = false;
    }else {
      if(del_front.size() < del_back.size()) {
        if_next = false;
        substitute = del_front;
      }else {
        if_next = true;
        substitute = del_back;
      }
    }
    double_list<T> substitute_;
    if(if_next)
      substitute_ = merge(*pos, *substitute);
    else
      substitute_ = merge(*substitute, *pos);
    data.erase(pos);
    list_it_type substitute_pos = data.erase(substitute);
    return data.insert(substitute_pos, substitute_);
  }
  size_t shape(const iterator& pos) {
    list_it_type list_pos = p.list_it;
    if(list_pos == list_it_type() || list_pos == data.end())
      return -1;
    size_t ans = -1;
    if(if_split(list_pos)) {
      ans = list_distance(pos);
      list_pos = do_split(list_pos);
      return ans;
    }else if(if_merge(list_pos)) {
      ans = list_distance(pos);
      list_pos = do_split(list_pos);
      return ans;
    }
    return ans;
  }
  //------------------------------
  /**
   * access a specified element with bound checking.
   * throw index_out_of_bound if out of bound.
   */
  T &at(const size_t &pos) {
    if(pos > sum_s) 
      throw index_out_of_bound();
    else if(pos == sum_s)
      return end();
    else
      return *(iterator(begin() + n));
  }
  const T &at(const size_t &pos) const {
    if(pos > sum_s) 
      throw index_out_of_bound();
    else if(pos == sum_s)
      return end();
    else
      return *(iterator(begin() + n));
  }
  T &operator[](const size_t &pos) {
    if(pos > sum_s) 
      throw index_out_of_bound();
    else if(pos == sum_s)
      return end();
    else
      return *(iterator(begin() + n));
  }
  const T &operator[](const size_t &pos) const {
    if(pos > sum_s) 
      throw index_out_of_bound();
    else if(pos == sum_s)
      return end();
    else
      return *(iterator(begin() + n));
  }

  /**
   * access the first element.
   * throw container_is_empty when the container is empty.
   */
  const T &front() const {
    if(sum_s == 0)
      throw container_is_empty();
    return *cbegin();
  }
  /**
   * access the last element.
   * throw container_is_empty when the container is empty.
   */
  const T &back() const {
    if(sum_s == 0)
      throw container_is_empty();
    return *(--cend());
  }

  /**
   * return an iterator to the beginning.
   */
  iterator begin() {
    if(sum_s == 0)
      return end();
    return iterator(this, data.begin(), data.begin()->begin());
  }
  const_iterator cbegin() const {
    if(sum_s == 0)
      return end();
    return const_iterator(this, data.begin(), data.begin()->begin());
  }

  /**
   * return an iterator to the end.
   */
  iterator end() {
    if(sum_s == 0)
      return end();
    return iterator(this, data.end(), data.end()->end());
  }
  const_iterator cend() const {
    if(sum_s == 0)
      return end();
    return const_iterator(this, data.end(), data.end()->end());
  }

  /**
   * check whether the container is empty.
   */
  bool empty() const {
    return sum_s == 0;
  }

  /**
   * return the number of elements.
   */
  size_t size() const {
    return sum_s;
  }

  /**
   * clear all contents.
   */
  void clear() {
    data.clear();
    sum_s = 0;
    chunk_s = 1;
  }

  /**
   * insert value before pos.
   * return an iterator pointing to the inserted value.
   * throw if the iterator is invalid or it points to a wrong place.
   */
  iterator insert(iterator pos, const T &value) {
    if(pos->dq_it != this || pos == iterator())
      std::throw_out_of_range(iterator is invalid);
    if(empty()) {
      data.insert_tail(chunk());
      pos = end();
    }
    size_t shape_result = shape(pos);
    if(shape_result != (-1))
      pos = begin() + shape_result;
    if(pos == end()) {
      --pos.list_it;
      pos.chunk_it = pos.list_it->end();
    }
    chunk_it_type it_ = pos.list_it->insert(pos.chunk_it, value);
    sum_s++;
    return iterator(this, pos.list_it, it_);
  }

  /**
   * remove the element at pos.
   * return an iterator pointing to the following element. if pos points to
   * the last element, return end(). throw if the container is empty,
   * the iterator is invalid, or it points to a wrong place.
   */
  iterator erase(iterator pos) {
    if(this != pos.dq_it || pos == end())
      std::throw_out_of_range("the erase pos is invalid");
    size_t shape_result = shape(pos);
    if(shape_result != size_t(-1))
      pos = at_iterator(shape_result);
    chunk_it_type chunk_it_ = pos.list_it->erase(pos.chunk_it);
    list_it_type list_it_= pos.list_it;
    sum_s--;
    return iterator(this, list_it_, chunk_it_);
  }

  /**
   * add an element to the end.
   */
  void push_back(const T &value) {
    (--data.end())->insert_tail(value);
    sum_s++;
  }

  /**
   * remove the last element.
   * throw when the container is empty.
   */
  void pop_back() {
    if(empty())
      std::throw_out_of_range("pop_back is invalid");
    (--data.end())->delete_tail();
    sum_s--;
  }

  /**
   * insert an element to the beginning.
   */
  void push_front(const T &value) {
    data.begin()->insert_head(value);
    sum_s++;
  }

  /**
   * remove the first element.
   * throw when the container is empty.
   */
  void pop_front() {
    if(empty())
      std::throw_out_of_range("pop_front is invalid");
    data.begin()->delete_head();
    sum_s--;
  }
};

} // namespace sjtu

#endif
