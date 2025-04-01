#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <utility>
#include <cmath>

namespace sjtu {

template<class T> class double_list{
  public:
    struct Node{
      T* data;
      Node *pre, *next;
      Node() : data(nullptr), pre(nullptr), next(nullptr){}
      Node(T* d, Node* pre_, Node* next) : data(d), pre(pre_), next(next){};
      Node(const Node& other) {
        if(other.data)
          data = new T(*other.data);
        else
          data = nullptr;
        pre = other.pre, next = other.next;
      }
      Node(T* val_ptr) : data(val_ptr), pre(nullptr), next(nullptr) {}
      Node& operator=(const Node& other) {
        if(this == &other)
          return *this;
        delete data;
        if(other.data)
          data = new T(*other.data);
        else
          data = nullptr;
        pre = other.pre, next = other.next;
        return *this;
      }
      ~Node() { delete data; }
      bool operator==(const Node &other) const { return data == other.data; }
    };
    // --------------------------
    //注意前后两个点都要记录
    Node *head, *tail;
    size_t s;
    Node tail_node;
    // --------------------------
    //doublelist的构造需要深拷贝！
    double_list() : tail_node() {
      head = &tail_node, tail = &tail_node;
      s = 0;
    }
    double_list(Node* head_, Node* tail_, size_t s_) : double_list() {
      // double_list();
      this->add(head_, tail_, s_);
    }
    double_list(const double_list<T> &other) : double_list(){
      *this = other;
    }
    double_list& add(Node* add_head, Node* add_tail, size_t add_s) {
      if(add_head == add_tail)
        return *this;
      if(empty()) {
        head = add_head;
        head->pre = nullptr;
      }else {
        tail->pre->next = add_head;
        add_head->pre = tail->pre;
      }
      add_tail->pre->next = tail;
      tail->pre = add_tail->pre;
      s += add_s;
      return *this;
    }
    double_list& add(double_list<T> &&other) {
      add(other.head, other.tail, other.size());
      other.head = other.tail;
      other.s = 0;
      return *this;
    }
    double_list& operator=(const double_list<T>& other) {
      if (this == &other) 
        return *this;
      clear();
      for (auto it = other.begin(); it != other.end(); ++it)
        insert_tail(*it);
      return *this;
    }
    ~double_list() {clear(); }
  
    class iterator{
    public:
      Node *current;
      const double_list *current_list;
      // --------------------------
      iterator() : current(nullptr), current_list(nullptr){}
      iterator(Node *current_, const double_list *current_list_) : current(current_), current_list(current_list_) {}
      iterator(const iterator &t) : current(t.current), current_list(t.current_list) {}
      ~iterator(){}
          /**
       * iter++
       */
      iterator operator++(int) {
        iterator old = *this;
        ++*this;
        return old;
      }
          /**
       * ++iter
       */
      iterator &operator++() {
        current = current -> next;
        return *this;	
      }
          /**
       * iter--
       */
      iterator operator--(int) {
        iterator old = *this;
        --*this;
        return old;
      }
          /**
       * --iter
       */
      iterator &operator--() {
        current = current->pre;
        return *this;
      }
      /**
       * if the iter didn't point to a value
       * throw " invalid"
      */
      T &operator*() const {
        if(current == nullptr || current->data == nullptr)
          throw std::out_of_range("invalid dereference at end()");
        return *(current->data);
      }
          /**
           * other operation
          */
      T *operator->() const noexcept { return (current->data);}
      bool operator==(const iterator &rhs) const { return current == rhs.current;}
      bool operator!=(const iterator &rhs) const { return current != rhs.current;}
    };
    /**
     * return an iterator to the beginning
     */
    // iterator begin(){return iterator(head, this);}
    /**
     * return an iterator to the ending
     * in fact, it returns the iterator point to nothing,
     * just after the last element.
     */
    // iterator end(){return iterator(tail, this);}
    iterator begin() const { return iterator(head, this); }
    iterator end() const { return iterator(tail, this); }
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
      if(pos == iterator() || pos == end() || this != pos.current_list) {
        throw std::out_of_range("erase is invalid");
        return end();
      }
      // 不需要考虑删除头节点 因为前面有判断
      if(pos == begin()) {
        delete_head();
        return begin();
      }
      Node *tmp = pos.current, *tmp_ = tmp->next;
      tmp->pre->next = tmp->next;
      tmp->next->pre = tmp->pre;
      delete tmp;
      //把tmp_定义在这里next就错了
      s--;
      return iterator(tmp_, this);
    }
    /**
     * the following are operations of double list
    */
    iterator insert(iterator pos, const T &val) {
      if(pos == iterator() || pos.current_list != this)
        throw std::out_of_range("insert position invalid");
      if(pos == begin()) {
        insert_head(val);
        return begin();
      }
      else if(pos == end()) {
        insert_tail(val);
        return --end();
      }
      Node* it_ = pos.current;
      Node* substitute = new Node(new T(val), it_->pre, it_);
      if(it_->pre)
        it_->pre->next = substitute;
      it_->pre = substitute;
      s++;
      return iterator(substitute, this);
    }
    void insert_head(const T &val){
      Node* new_node = new Node(new T(val), nullptr, head);
      head->pre = new_node;
      head = new_node;
      s++;
    }
    void insert_tail(const T &val){
      Node *new_node = new Node(new T(val), tail->pre, tail);
      if(!tail->pre) //原来是空的
        head = new_node;
      else {
        // std::cout << "not empty()" << std::endl;
        tail->pre->next = new_node;
      }
        
      tail->pre = new_node;
      // new_node->next = tail;
      s++;
    }
    void delete_head(){
      if (empty())
        throw std::out_of_range("delete_head on empty list");
      Node *old = head;
      head = head->next;
      head->pre = nullptr;
      delete old;
      s--;
    }
    void delete_tail(){
      if (empty()) 
        throw std::out_of_range("delete_tail on empty list");
      Node *last = tail->pre;
      if (last->pre)
        last->pre->next = tail;
      else
        head = tail;
      tail->pre = last->pre;
      delete last;
      s--;
    }
    bool empty () const{
      return (s == 0) || (head == tail);
    }
    void clear() {
      Node* tmp = head;
      while (tmp != tail) {
        tmp = tmp -> next;
        delete tmp->pre;
      }
      tail_node = Node();
      head = &tail_node;
      tail = &tail_node;
      // head = tail;
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
    size_t size() { return s; }
    //merge version1
    // static double_list merge(double_list &a, double_list &b) { //因为它们操作的是参数，而不是类自身的成员变量->static
    //   double_list merge_ans;
    //   for(auto merge_it = a.begin(); merge_it != a.end(); merge_it++)
    //     merge_ans.insert_tail(*merge_it);
    //   for(auto merge_it = b.begin(); merge_it != b.end(); merge_it++)
    //     merge_ans.insert_tail(*merge_it);
    //   return merge_ans;
    // }
    //merge version2
    static double_list merge(double_list &a, double_list &b) {
      return std::move(a.add(std::move(b)));
    }
    static std::pair<double_list, double_list> split(double_list &a, size_t split_pos) { 
      if(a.size() < split_pos)
        throw std::out_of_range("split position doesn't exist");
      if(split_pos == 0)
        return std::make_pair(double_list(), a);
      else if(split_pos == a.size())
        return std::make_pair(a, double_list());
      Node *it_ = a.head;
      //这里的i要从1开始不然的话就不对了！！！
      size_t i = 1;
      while(i <= split_pos) {
        it_ = it_->next;
        i++;
      }
      double_list<T> front = double_list(a.head, it_, split_pos);
      //注意这里不包含最后一个元素的
      double_list<T> back = double_list(it_, a.tail, a.size() - split_pos);
      a.head = a.tail;
      a.s = 0;
      return std::make_pair(front, back);
    }
    static size_t init_size(const double_list &list_, const iterator& chunk_it_) { //同一个list的chunk到开头的距离
      size_t cnt = 0;
      for (iterator it_ = list_.begin(); it_ != chunk_it_ && it_ != list_.end(); it_++)
        cnt++;
      return cnt;
    }
};
template <class T> class deque {
public:
  using chunk_it_type = typename double_list<T>::iterator;
  using list_it_type = typename double_list<double_list<T>>::iterator;
  static constexpr double spilt_index = 1.5;
  static constexpr double merge_index = 0.5;

public:
  double_list<double_list<T>> data;
  size_t sum_s;
  size_t chunk_s;
  class const_iterator;
  size_t standard_size() {
    return floor(sqrt(sum_s)) + 1;
  }
  size_t init_size(const iterator& it) {
    size_t cnt = 0;
    list_it_type list_it_ = data.begin();
    while(list_it_ != it.list_it_ && list_it_ != data.end()) {
      cnt += list_it_->size();
      list_it_++;
    }
    if(list_it_ == data.end()) {
      if(it.list_it_ == data.end())
        return cnt;
      return -1;
    }
    cnt += double_list<T>::init_size(*list_it_, it.chunk_it);
    return cnt;
  }
  //------------------------------
  class iterator {
  public:
    deque *dq_it;
    list_it_type list_it;
    chunk_it_type chunk_it;
    //--------------------------
    iterator() : dq_it(nullptr), list_it(), chunk_it(){}
    iterator(deque* dq_it_, list_it_type list_it_, chunk_it_type chunk_it_) : dq_it(dq_it_), list_it(list_it_), chunk_it(chunk_it_){} 
    /**
     * return a new iterator which points to the n-next element.
     * if there are not enough elements, the behaviour is undefined.
     * same for operator-.
     */
    iterator operator+(const int &n) const {
      if(*this == iterator())
        throw std::out_of_range("+ is invalid");
      if(n == 0)
        return *this;
      else if(n < 0)
        return (*this) - (-n);
      // 注意这里是可以--end()的 所以对end的判断要写在这个之后
      else if(*this == dq_it->end())
        throw std::out_of_range("+ is invalid");
      int n_ = n;
      list_it_type list_it_ = list_it;
      chunk_it_type chunk_it_ = chunk_it;
      while(n_ > 0 && chunk_it_ != --(list_it_->end())) {
        chunk_it_++;
        n_--;
      }
      if(n_ == 0)
        return iterator(dq_it, list_it_, chunk_it_);
      if(list_it_ == --(dq_it->data.end())) {
        if(n_ > 1)
          throw std::out_of_range("add too much");
        else
          return (dq_it)->end();
      }else {
        list_it_++;
        while(n_ > list_it_->size() && list_it_ != (dq_it->data.end())) {
          n_ -= list_it->size();
          list_it_++;
        }
        if(list_it_ == (dq_it->data.end())) {
          if(n_ > 1)
            throw std::out_of_range("add too much");
          else
            return (dq_it)->end();
        }
        chunk_it_ = list_it_->begin();
        while(n_ > 1) {
          chunk_it_++;
          n_--;
        }
        return iterator(dq_it, list_it_, chunk_it_);
      }
    }
    iterator operator-(const int &n) const {
      if(*this == iterator())
        throw std::out_of_range("- is invalid"); //指向begin的之后未必直接throw
      if(n == 0)
        return *this;
      else if(n < 0)
        return (*this) + (-n);
      int n_ = n;
      list_it_type list_it_ = list_it;
      chunk_it_type chunk_it_ = chunk_it;
      if(*this == dq_it->end()) {
        list_it_--;
      }else {
        while(n_ > 0 && chunk_it_ != list_it_->begin()) {
          n_--;
          chunk_it_--;
        }
        if(n_ == 0)
          return iterator(dq_it, list_it_, chunk_it_);
        if(list_it_ == (dq_it->data.begin())) 
          throw std::out_of_range("minus too much");
        list_it_--;
      }
      while(n_ > int(list_it_->size()) && list_it_ != dq_it->data.begin()) {
        n_ -= list_it_->size();
        list_it_--;
      }
      if (n_ > list_it_->size())
        throw std::out_of_range("minus too much");
      chunk_it_ = --list_it_->end();
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
        if(list_it == dq_it->data.end())
          return 0;
        return static_cast<int>(double_list<T>::init_size(*(this->list_it), this->chunk_it)) - double_list<T>::init_size(*(rhs.list_it), rhs.chunk_it);
      }
      bool greater - double_list<double_list<T>>::init_size(dq_it->data, this->list_it) > double_list<double_list<T>>::init_size(dq_it->data. rhs.list_it);
      list_it_type ITX this->list_it, ITY = rhs.list_it;
      chunk_it_type itx = this->chunk_it, ity = rhs.chunk_it;
      if(greater) {
        std::swap(ITX, ITY);
        std::swap(itx, ity);
      }
      int res - static_cast<int>(ITX->size()) - double_list<T>::init_size(*ITX, itx);
      if(ITY != dq_it->data.end())
        res += double_list<T>::init_size(*ITY, ity);
      else
        res += 0;
      ++ITX;
      while(ITX != ITY) {
        res += ITX->size();
        ITX++;
      }
      if(greater)
        return res;
      else
        return -res;
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
      return *chunk_it;
    }
    /**
     * it->field
     */
    T *operator->() const noexcept {
      return chunk_it.operator->();
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
    list_it_type list_it;
    chunk_it_type chunk_it;
    //--------------------------
    const_iterator() : dq_it(nullptr), list_it(), chunk_it(){}
    const_iterator(const deque *dq_it_, const list_it_type list_it_, const chunk_it_type chunk_it_) : dq_it(dq_it_), list_it(list_it_), chunk_it(chunk_it_){} 
    const_iterator(const iterator &other) : dq_it(other.dq_it), list_it(other.list_it), chunk_it(other.chunk_it){}
    /**
     * return a new iterator which points to the n-next element.
     * if there are not enough elements, the behaviour is undefined.
     * same for operator-.
     */
    const_iterator operator+(const int &n) const {
      if(*this == iterator())
        throw std::out_of_range("+ is invalid");
      if(n == 0)
        return *this;
      else if(n < 0)
        return (*this) - (-n);
      // 注意这里是可以--end()的 所以对end的判断要写在这个之后
      else if(*this == dq_it->end())
        throw std::out_of_range("+ is invalid");
      int n_ = n;
      list_it_type list_it_ = list_it;
      chunk_it_type chunk_it_ = chunk_it;
      while(n_ > 0 && chunk_it_ != list_it->end()) {
        chunk_it_++;
        n_--;
      }
      if(n_ == 0)
        return const_iterator(dq_it, list_it_, chunk_it_);
      if(list_it_ == --(dq_it->data.end())) {
        if(n_ > 1)
          throw std::out_of_range("add too much");
        else
          return (dq_it)->cend();
      }else {
        list_it_++;
        while(n_ > (*list_it_).size() && list_it_ != (dq_it->data.end())) {
          n_ -= (*list_it_).size();
          list_it_++;
        }
        if(list_it_ == (dq_it->data.end())) {
          if(n_ > 1)
            throw std::out_of_range("add too much");
          else
            return (dq_it)->cend();
        }
        chunk_it_ = list_it_->begin();
        while(n_ > 1) {
          chunk_it_++;
          n_--;
        }
        return const_iterator(dq_it, list_it_, chunk_it_);
      }
    }
    const_iterator operator-(const int &n) const {
      if(*this == iterator())
        throw std::out_of_range("- is invalid"); //指向begin的之后未必直接throw
      if(n == 0)
        return *this;
      else if(n < 0)
        return (*this) + (-n);
      int n_ = n;
      list_it_type list_it_ = list_it;
      chunk_it_type chunk_it_ = chunk_it;
      if(*this == dq_it->end()) {
        list_it_--;
      }else {
        while(n_ > 0 && chunk_it_ != list_it_->begin()) {
          n_--;
          chunk_it_--;
        }
        if(n_ == 0)
          return const_iterator(dq_it, list_it_, chunk_it_);
        if(list_it_ == (dq_it->data.begin())) 
          throw std::out_of_range("minus too much");
        list_it_--;
      }
      while(n_ > (int)list_it_->size() && list_it_ != dq_it->data.begin()) {
        n_ -= list_it_->size();
        list_it_--;
      }
      if (n_ > list_it_->size())
        throw std::out_of_range("minus too much");
      chunk_it_ = --list_it_->end();
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
        if(list_it == dq_it->data.end())
          return 0;
        return static_cast<int>(double_list<T>::init_size(*(this->list_it), this->chunk_it)) - double_list<T>::init_size(*(rhs.list_it), rhs.chunk_it);
      }
      bool greater - double_list<double_list<T>>::init_size(dq_it->data, this->list_it) > double_list<double_list<T>>::init_size(dq_it->data. rhs.list_it);
      list_it_type ITX this->list_it, ITY = rhs.list_it;
      chunk_it_type itx = this->chunk_it, ity = rhs.chunk_it;
      if(greater) {
        std::swap(ITX, ITY);
        std::swap(itx, ity);
      }
      int res - static_cast<int>(ITX->size()) - double_list<T>::init_size(*ITX, itx);
      if(ITY != dq_it->data.end())
        res += double_list<T>::init_size(*ITY, ity);
      else
        res += 0;
      ++ITX;
      while(ITX != ITY) {
        res += ITX->size();
        ITX++;
      }
      if(greater)
        return res;
      else
        return -res;
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
      return *chunk_it;
    }
    /**
     * it->field
     */
    const T *operator->() const noexcept {
      return chunk_it.operator->();
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
      this->data.insert_tail(*it_);
    return *this;
  }
  //------------------------------
  bool if_split(const list_it_type pos) {
    return (pos->size() > standard_size() * spilt_index);
  }
  bool if_merge(const list_it_type pos) {
    if(pos == data.begin() || pos == --data.end())
      return false;
    //考虑首位情况不merge
    return (pos->size() < standard_size() * merge_index);
  }
  list_it_type do_split(const list_it_type pos) {
    double_list<T> split_front, split_back;
    split_front = double_list<T>::split(*pos, (pos->size() + 1) / 2).first;
    split_back = double_list<T>::split(*pos, (pos->size() + 1) / 2).second;
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
    }else if(del_back == --data.end()) {
      substitute = del_front;
      if_next = false;
    }else {
      if(del_front->size() < del_back->size()) {
        if_next = false;
        substitute = del_front;
      }else {
        if_next = true;
        substitute = del_back;
      }
    }
    double_list<T> substitute_;
    if(if_next)
      substitute_ = double_list<T>::merge(*pos, *substitute);
    else
      substitute_ = double_list<T>::merge(*substitute, *pos);
    data.erase(pos);
    list_it_type substitute_pos = data.erase(substitute);
    return data.insert(substitute_pos, substitute_);
  }
  //return the position of p after change, -1 by default
  size_t shape(const iterator& pos) {
    list_it_type list_pos = pos.list_it;
    if(list_pos == list_it_type() || list_pos == data.end())
      return -1;
    size_t ans = -1;
    if(if_split(list_pos)) {
      if(ans == -1)
        ans = list_distance(pos);
      list_pos = do_split(list_pos);
    }else if(if_merge(list_pos)) {
      if(ans == -1)
        ans = list_distance(pos);
      list_pos = do_merge(list_pos);
    }
    return ans;
  }
  iterator at_iterator(const size_t &pos) { 
    if(pos > sum_s)
      throw std::out_of_range("at_interator invalid");
    if(2 * pos < size())
      return begin() + pos;
    else
      return end() - (size() - pos);
  }
  //及时删掉空的chunk
  list_it_type check_empty(list_it_type pos) {
    if(pos == data.end())
      return pos;
    if(pos->empty())
      return data.erase(pos);
    else
      return pos;
  }
  //------------------------------
  /**
   * access a specified element with bound checking.
   * throw index_out_of_bound if out of bound.
   */
  T &at(const size_t &pos) {
      if(pos >= sum_s)
        throw std::out_of_range("at points to invalid");
      return *(at_iterator(pos));
  }
  const T &at(const size_t &pos) const {
    if(pos >= sum_s)
        throw std::out_of_range("at points to invalid")
    return *(at_iterator(pos));
  }
  T &operator[](const size_t &pos) {
    return *(at_iterator(pos));
  }
  const T &operator[](const size_t &pos) const {
    return *(at_iterator(pos));
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
  const_iterator begin() const {
    return cbegin();
  }
  iterator begin() {
    if(sum_s == 0)
      return end();
    return iterator(this, data.begin(), data.begin()->begin());
  }
  const_iterator cbegin() const {
    if(sum_s == 0)
      return cend();
    return const_iterator(this, data.begin(), data.begin()->begin());
  }

  /**
   * return an iterator to the end.
   */
  iterator end() {
    return iterator(this, data.end(), chunk_it_type());
  }
  const_iterator end() const{
    return cend();
  }
  const_iterator cend() const {
    return const_iterator(this, data.end(), chunk_it_type());
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
    if(pos.dq_it != this || pos == iterator())
      throw std::out_of_range("iterator is invalid");
    if(empty()) {
      data.insert_tail(double_list<T>());
      pos = end();
    }
    size_t shape_result = shape(pos);
    if(shape_result != size_t(-1))
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
    if(this != pos.dq_it || pos == end() || empty())
      throw std::out_of_range("the erase pos is invalid");
    size_t shape_result = shape(pos);
    if(shape_result != size_t(-1))
      pos = at_iterator(shape_result);
    chunk_it_type chunk_it_ = pos.list_it->erase(pos.chunk_it);
    list_it_type list_it_= check_empty(pos.list_it);
    if(list_it_ != pos.list_it) {
      if(list_it_ == data.end())
        chunk_it_ = chunk_it_type();
      else
        chunk_it_ = list_it_->begin();
    }else if(chunk_it_ == list_it_->end()) {
      if(list_it_ == --data.end())
        chunk_it_ = chunk_it_type();
      else
        chunk_it_ = list_it_->begin();
    }
    sum_s--;
    return iterator(this, list_it_, chunk_it_);
  }

  /**
   * add an element to the end.
   */
  void push_back(const T &value) {
    if (empty() || (--data.end())->size() > standard_size())
      data.insert_tail(double_list<T>());
    (--data.end())->insert_tail(value);
    sum_s++;
  }
  /**
   * remove the last element.
   * throw when the container is empty.
   */
  void pop_back() {
    if(empty())
      throw std::out_of_range("pop_back is invalid");
    (--data.end())->delete_tail();
    check_empty(--data.end());
    sum_s--;
  }

  /**
   * insert an element to the beginning.
   */
  void push_front(const T &value) {
    if(empty() || data.begin()->size() > standard_size())
      data.insert_head(double_list<T>());
    data.begin()->insert_head(value);
    sum_s++;
  }

  /**
   * remove the first element.
   * throw when the container is empty.
   */
  void pop_front() {
    if(empty())
      throw std::out_of_range("pop_front is invalid");
    data.begin()->delete_head();
    check_empty(data.begin());
    sum_s--;
  }
};

} // namespace sjtu

#endif
