#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu {

template <class T> class deque {
public:
    size_t size_;
    int max_size_ = 500;
    struct block{
        int max_size_ = 500;
        int cap;
        block *prev,*next;
        T **list,**head,**tail;
        block():cap(0),prev(nullptr),next(nullptr){
            list = new T*[max_size_];
            head = list;
            tail = list - 1;
        }
        ~block(){
            T** t = this->head;
            while(t != this->tail + 1){
                delete (*t);
                t++;
            }
            delete[] list;
        }
        // void print(){
        //   T** t = head;
        //   while(t != nullptr) {
        //   if(t == tail) break;
        //    t++;}
        // } 
        block &operator=(const block &other){
            if(this == &other) return *this;
            for(int i = 0;i < cap;i++){
                delete list[i];
            }
            delete[] list;
            cap = other.cap;
            list = new T*[max_size_]; 
            head = list;
            tail = list;
            for (int i = 0; i < cap; i++) {
                list[i] = new T(*other.list[i]);
            }
            prev = other.prev;
            next = other.next;
            return *this; 
        }
    };
    block *fir,*lst; 
    void bl_delete(block *tmp){
        // std::cerr<<"  in";
        if(lst == tmp) lst = tmp->prev;
        if(fir == tmp) fir = tmp->next;
        if(tmp->prev != nullptr) tmp->prev->next = tmp->next;
        if(tmp->next != nullptr) tmp->next->prev = tmp->prev;
        delete tmp;
        // std::cerr<<"  out";
    }
    block* bl_insert(block *pre,block *nxt){
        block* new_bl = new block;
        if(nxt == fir) fir = new_bl;
        if(pre == lst) lst = new_bl;
        if(pre != nullptr)    pre->next = new_bl;
        // std::cerr<<"  pre";
        new_bl->next = nxt;
        if(nxt != nullptr)    nxt->prev = new_bl;
        new_bl->prev = pre;
        // std::cerr<<"\033[31mNEWBL\033[0m\n"<<std::endl;
        return new_bl;
    }
  // void show(){
  //   block *b = fir;
  //   while(b != nullptr) {b->print(),std::cerr<<std::endl;b = b->next;}
  // }
  class const_iterator;
  class iterator {
  public:
    deque *dq;
    block *bl;
    T **p;
    int cur;
    iterator():dq(nullptr),bl(nullptr),p(nullptr),cur(0){}
    iterator(const deque* dq_,const block* bl_,const T** p_,const int cur_)
    :dq(dq_),bl(bl_),p(p_),cur(cur_){}
    iterator( deque* dq_, block* bl_,T** p_, int cur_)
    :dq(dq_),bl(bl_),p(p_),cur(cur_){}
    /**
     * return a new iterator which points to the n-next element.
     * if there are not enough elements, the behaviour is undefined.
     * same for operator-.
     */
    iterator operator+(const int &n) const {
      // if(n == -1 || *this == dq->end()) return iterator(dq,dq->lst,dq->lst->tail,dq->size_);
        iterator tmp = *this;
        return (tmp += n);
    }
    iterator operator-(const int &n) const {
      // if(n == 1 || *this == dq->end()) return iterator(dq,dq->lst,dq->lst->tail,dq->size_);
        iterator tmp = *this;
        return (tmp -= n);    
    }

    /**
     * return the distance between two iterators.
     * if they point to different vectors, throw
     * invaild_iterator.
     */
    int operator-(const iterator &rhs) const {
        if(this->dq != rhs.dq) throw invalid_iterator();
        return (int)(this->cur - rhs.cur);
    }
    iterator &operator+=(const int &n_) {
        int n = n_;
        if(cur == dq->size_ && n != 0){//是end()
          *this = iterator(dq,dq->lst,dq->lst->tail,dq->size_-1);
          return (*this += (n+1));
        }
        cur += n;
        if(n == 0) return *this;
        if(n > 0){
            if(this->p + n <= this->bl->tail) {
                // std::cerr<<"it: "<<(**bl->tail)<<std::endl;
                // std::cerr<<this->bl->cap<<" "<<(**(this->bl->head))<<" "<<(**(this->bl->tail))<<std::endl;
                p =  p + n;
            } 
            else{
                n -= bl->tail - p + 1;
                if(bl->next == nullptr ) {
                  if(n == 0) return *this = dq->end();
                  else throw index_out_of_bound();
                }
                bl = bl->next;
                p = bl->head;
                
                while(n >= bl->cap){
                    // std::cerr<<bl->cap<<" next_bl ";
                    n -= bl->cap;
                    if(bl->next == nullptr){
                      if(n == 0) return *this = dq->end();
                      else throw index_out_of_bound();
                    }
                    bl = bl->next;
                    p = bl->head;
                }
                // std::cerr<<n<<std::endl;
                p = p + n ;
            }
        }
        else{
          // std::cerr<<" ccc"<<std::endl;
            if(p + n >= bl->head){
                p = p + n;
            }
            else{
                n += p - bl->head + 1;
                bl = bl->prev;
                p =  bl->tail;
                while((-n) >= bl->cap){
                    n += bl->cap;
                    // if(bl->prev == nullptr) std::cerr<<"op+=";
                    bl = bl->prev;
                    p = bl->tail;
                }
                p = p + n ;
            }
        }
        // std::cerr<<n_<<" end"<<std::endl;
        return *this;
    }
    iterator &operator-=(const int &n) {
        return *this += (-n);
    }

    /**
     * iter++
     */
    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }
    /**
     * ++iter
     */
    iterator &operator++() {
      if(*this == iterator(dq,dq->lst,dq->lst->tail,dq->size_-1)) //是end()-1
        return *this = dq->end();
      return *this = *this + 1;
    }
    /**
     * iter--
     */
    iterator operator--(int) {
      iterator tmp = *this;
      --(*this);
      return tmp;
    }
    /**
     * --iter
     */
    iterator &operator--() {
      if(*this == dq->begin()) throw invalid_iterator();
      if(this->cur == dq->size_ ) {
        *this =  iterator(dq,dq->lst,dq->lst->tail,dq->size_-1);
        return *this;}
      return *this = *this - 1;
    }

    /**
     * *it
     */
    T &operator*() const {
        if(*this == dq->end()) throw invalid_iterator();
        return (**p);
    }
    /**
     * it->field
     */
    T *operator->() const noexcept {
        return &(operator *());
    }

    /**
     * check whether two iterators are the same (pointing to the same
     * memory).
     */
    bool operator==(const iterator &rhs) const {
        return (dq == rhs.dq) && (cur == rhs.cur);
    }
    bool operator==(const const_iterator &rhs) const {
        return (dq == rhs.dq) && (cur == rhs.cur);
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
  public:
    const deque *dq;
    const block *bl;
    T **p;
    int cur;
    const_iterator():dq(nullptr),bl(nullptr),p(nullptr),cur(0){}
    const_iterator(const deque* dq_,const block* bl_, T** p_,const int cur_)
    :dq(dq_),bl(bl_),p(p_),cur(cur_){}
    const_iterator( deque* dq_, block* bl_,T** p_, int cur_)
    :dq(dq_),bl(bl_),p(p_),cur(cur_){}
    /**
     * return a new iterator which points to the n-next element.
     * if there are not enough elements, the behaviour is undefined.
     * same for operator-.
     */
    const_iterator operator+(const int &n) const {
        const_iterator tmp = *this;
        return (tmp += n);
    }
    const_iterator operator-(const int &n) const {
        const_iterator tmp = *this;
        return (tmp -= n);    
    }

    /**
     * return the distance between two iterators.
     * if they point to different vectors, throw
     * invaild_iterator.
     */
    int operator-(const const_iterator &rhs) const {
        if(this->dq != rhs.dq) throw invalid_iterator();
        return (this->cur - rhs.cur);
    }
    const_iterator &operator+=(const int &n_) {
        int n = n_;
        if(cur == dq->size_ && n != 0){//是end()
          *this = const_iterator(dq,dq->lst,dq->lst->tail,dq->size_-1);
          return (*this += (n+1));
        }
        cur += n;
        if(n == 0) return *this;
        if(n > 0){
            if(this->p + n <= this->bl->tail) {
                // std::cerr<<"it: "<<(**bl->tail)<<std::endl;
                // std::cerr<<this->bl->cap<<" "<<(**(this->bl->head))<<" "<<(**(this->bl->tail))<<std::endl;
                p =  p + n;
            } 
            else{
                n -= bl->tail - p + 1;
              // std::cerr<<n<<std::endl;
                if(bl->next == nullptr ) {
                  if(n == 0) return *this = dq->cend();
                  else throw index_out_of_bound();
                }
                bl = bl->next;
                p = bl->head;
                
                while(n >= bl->cap){
                    // std::cerr<<bl->cap<<" next_bl ";
                    n -= bl->cap;
                    if(bl->next == nullptr){
                      if(n == 0) return *this = dq->cend();
                      else throw index_out_of_bound();
                    }
                    bl = bl->next;
                    p = bl->head;
                }
                // std::cerr<<n<<std::endl;
                p = p + n ;
            }
        }
        else{
          // std::cerr<<" ccc"<<std::endl;
            if(p + n >= bl->head){
                p = p + n;
            }
            else{
                n += p - bl->head + 1;
                bl = bl->prev;
                p =  bl->tail;
                while((-n) >= bl->cap){
                    n += bl->cap;
                    // if(bl->prev == nullptr) std::cerr<<"op+=";
                    bl = bl->prev;
                    p = bl->tail;
                }
                p = p + n ;
            }
        }
        // std::cerr<<n_<<" end"<<std::endl;
        return *this;
    }
    const_iterator &operator-=(const int &n) {
        return *this += (-n);
    }

    /**
     * iter++
     */
    const_iterator operator++(int) {
      const_iterator tmp = *this;
      ++(*this);
      return tmp;
    }
    /**
     * ++iter
     */
    const_iterator &operator++() {
      if(*this == const_iterator(dq,dq->lst,dq->lst->tail,dq->size_-1)) //是end()-1
        return *this = const_iterator();
      return *this = *this + 1;
    }
    /**
     * iter--
     */
    const_iterator operator--(int) {
      const_iterator tmp = *this;
      --(*this);
      return tmp;
    }
    /**
     * --iter
     */
    const_iterator &operator--() {
      if(*this == dq->cbegin()) throw invalid_iterator();
      if(*this == const_iterator()) return *this = const_iterator(dq,dq->lst,dq->lst->tail,dq->size_-1);
      return *this = *this - 1;
    }

    /**
     * *it
     */
    const T &operator*() const {
        if(*this == dq->cend()) throw invalid_iterator();
        return (**p);
    }
    /**
     * it->field
     */
    const T *operator->() const noexcept {
        return &(operator *());
    }

    /**
     * check whether two iterators are the same (pointing to the same
     * memory).
     */
    bool operator==(const iterator &rhs) const {
        return (dq == rhs.dq) && (cur == rhs.cur);
    }
    bool operator==(const const_iterator &rhs) const {
        return (dq == rhs.dq) && (cur == rhs.cur);
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
  deque() :size_(0){
    fir = lst = new block();
  }
  deque(const deque &other) {
    if(other.empty()) {
      fir = lst = new block();
      size_ = 0;
      return;
    }
    size_ = other.size_;
    fir = lst = nullptr;
    block *cur = other.fir,*last_one = nullptr;
    while(cur != nullptr){
        block *new_bl = bl_insert(last_one,nullptr);
        int i0 = cur->head - cur->list;
        for (int i = 0; i < cur->cap ; i++) {
            *(new_bl->list + i0 + i) = new T(**(cur->list + i0 + i));
        }
        new_bl->cap = cur->cap;
        new_bl->head = new_bl->list + i0;
        new_bl->tail = new_bl->head + new_bl->cap - 1;
        // if(fir == nullptr){
        //     fir = new_bl; lst = fir;
        // // std::cerr<<"bl1"<<std::endl;
        // }
        // else{
        //     lst->next = new_bl;
        //     new_bl->prev = lst;
        //     lst = new_bl;
        // // std::cerr<<"bl2"<<std::endl; 
        // }
        cur = cur->next;
        last_one = new_bl;
    }

  }

  /**
   * deconstructor.
   */
  ~deque() {
    block *p = fir, *q;
    while (p)
    {
        q = p->next;
        delete p;
        if(q != nullptr) q->prev = nullptr;
        p = q;
    }
  }

  /**
   * assignment operator.
   */
  deque &operator=(const deque &other) {
    if(fir == other.fir && lst == other.lst) return *this;
    block *p = fir, *q;
    while (p)
    {
        q = p->next;
        delete p;
        if(q != nullptr) q->prev = nullptr;
        p = q;
    }

    size_ = other.size_;
    fir = lst = nullptr;
    block *cur = other.fir,*last_one = nullptr;
    int cnt = 1;
    while(cur != nullptr){
      // std::cerr<<cnt<<" ";cnt++;
        block *new_bl = bl_insert(last_one,nullptr);
        int i0 = cur->head - cur->list;
        for (int i = 0; i < cur->cap ; i++) {
            *(new_bl->list + i0 + i) = new T(**(cur->list + i0 + i));
        }
        new_bl->cap = cur->cap;
        new_bl->head = new_bl->list + i0;
        new_bl->tail = new_bl->head + new_bl->cap - 1;
        // if(fir == nullptr){
        //     fir = new_bl; lst = fir;
        // // std::cerr<<"bl1"<<std::endl;
        // }
        // else{
        //     lst->next = new_bl;
        //     new_bl->prev = lst;
        //     lst = new_bl;
        // // std::cerr<<"bl2"<<std::endl; 
        // }
        cur = cur->next;
        last_one = new_bl;
    }
    // if(fir == nullptr || lst == nullptr) 
    //     std::cerr<<"copy=_null"<<std::endl;
    return *this;
  }

  /**
   * access a specified element with bound checking.
   * throw index_out_of_bound if out of bound.
   */
  T &at(const size_t &pos) {
    if(pos < 0||pos >= size_) throw index_out_of_bound();
    return *(begin()+pos);
  }
  const T &at(const size_t &pos) const {
    if(pos < 0||pos >= size_) throw index_out_of_bound();
    return *(cbegin()+pos);
  }
  T &operator[](const size_t &pos) {
    if(pos < 0||pos >= size_) throw index_out_of_bound();
    return *(begin()+pos);
  }
  const T &operator[](const size_t &pos) const {
    if(pos < 0||pos >= size_) throw index_out_of_bound();
    return *(cbegin()+pos);
  }

  /**
   * access the first element.
   * throw container_is_empty when the container is empty.
   */
  const T &front() const {
    if(size_ == 0) throw container_is_empty();
    // if(fir->cap == 0) std::cerr<<"front_empty";
    return **(fir->head);
  }
  /**
   * access the last element.
   * throw container_is_empty when the container is empty.
   */
  const T &back() const {
    if(size_ == 0) throw container_is_empty();
    if(lst->cap == 0) {
      block* tmp = lst->prev;
      return **(tmp->tail);
    }
    return **(lst->tail);
  }

  /**
   * return an iterator to the beginning.
   */
  iterator begin() {
    return iterator(this,fir,fir->head,0);
  }
  const_iterator cbegin() const {
    return const_iterator(this,fir,fir->head,0);
  }
  /**
   * return an iterator to the end.
   */
  iterator end() {
    return iterator(this,nullptr,nullptr,size_);
    // return iterator();
  }
  const_iterator cend() const {
    return const_iterator(this,nullptr,nullptr,size_);
    // return iterator();
  }

  /**
   * check whether the container is empty.
   */
  bool empty() const {
    return (size_ == 0 );
  }

  /**
   * return the number of elements.
   */
  size_t size() const {
    return size_;
  }

  /**
   * clear all contents.
   */
  void clear() {
    block *p = fir, *q;
    while (p)
    {
        q = p->next;
        delete p;
        if(q != nullptr) q->prev = nullptr;
        p = q;
    }
    size_ = 0;
    fir = lst = new block();
  }

  /**
   * insert value before pos.
   * return an iterator pointing to the inserted value.
   * throw if the iterator is invalid or it points to a wrong place.
   */
  iterator insert(iterator pos, const T &value) {
    int it_cur = pos.cur;
    // std::cerr<<"pos_cur  "<<pos.cur<<std::endl;
    deque* it_dq = pos.dq;
    block* it_bl = pos.bl;
    T** it_p = pos.p;
    if(it_dq != this) throw "wrong";
    if(it_cur < 0 || (size_ > 0 && it_cur > size_)) throw index_out_of_bound();
    if(it_cur == 0 ){
      // std::cerr<<"insert_pushfront"<<std::endl;
        push_front(value);
        return iterator(it_dq,this->fir,fir->head,0);
    }
    if(it_cur == size_ ){
      // std::cerr<<"insert_pushback"<<std::endl;
        push_back(value);
        return iterator(it_dq,this->lst,lst->tail,size_-1);
    }
    size_++;
    if(it_bl->cap >= max_size_){//这块满了,在后面开新块
        block* new_bl = bl_insert(it_bl,it_bl->next);
        // std::cerr<<"insert_newbl: "<<it_bl->cap<<std::endl;
        int i0 = (it_bl->cap)/2;
        for(int i = i0;i < it_bl->cap ;i++){
          *(new_bl->head + i-i0) = new T(**(it_bl->head + i));
          // std::cerr<<"delete "<<(**(it_bl->head + i))<<std::endl;
          delete (*(it_bl->head + i));
          new_bl->cap++;
          it_bl->tail--;
        }
        new_bl->tail = new_bl->head + new_bl->cap - 1;
        it_bl->cap -= new_bl->cap;
        int dis = (it_p - it_bl->head) - it_bl->cap;
        // std::cerr<<"dis : "<<(it_p - (it_bl->list))<<" - "<<it_bl->cap<<" it_p "<<(**it_p)<<std::endl;
        if(dis>0){//被移动了
          it_p = new_bl->head + (it_p - (it_bl->list)) - it_bl->cap;
          it_bl = new_bl;
        }
        if(it_bl == it_dq->lst) it_dq->lst = new_bl;
    }
    
      if(it_bl->head-it_bl->list > 0){//前面有空位
          // std::cerr<<"front  "<<(**it_bl->head)<<std::endl;
          T** tmp = it_bl->head - 1;
          (*tmp) = new T(**(it_bl->head));
          it_bl->cap++;
          it_bl->head--;
          while(tmp != it_p - 1){
              // std::cerr<<(**tmp)<<" = "<<(**(tmp - 1))<<std::endl;
              (**tmp) = (**(tmp + 1));
              tmp++;
          }
          (**tmp) = value;
          return iterator(it_dq,it_bl,tmp,it_cur);
      }
      else{//后面有空位
          // std::cerr<<"back  "<<it_bl->cap<<std::endl;
          // std::cerr<<"cap"<<" = "<<it_bl->cap<<std::endl;
          T** tmp = it_bl->head + it_bl->cap;

          (*tmp) = new T(**(tmp-1));
              // std::cerr<<"new"<<" = "<<(**tmp)<<std::endl;
          // std::cerr<<"it_cur:  "<<it_cur<<std::endl;
          // std::cerr<<"it_p:  "<<(**it_p)<<std::endl;
          it_bl->cap++;
          while(tmp != it_p ){
              // std::cerr<<(**tmp)<<" = "<<(**(tmp - 1))<<std::endl;
              (**tmp) = (**(tmp - 1));
              tmp--;
          }
              // std::cerr<<"cap"<<" = "<<it_bl->cap<<std::endl;
              // std::cerr<<(**tmp)<<" = "<<value<<std::endl;
          (**tmp) = value;
          it_bl->tail = it_bl->head + it_bl->cap - 1;
          return iterator(it_dq,it_bl,tmp,it_cur);

    }
  }

  /**
   * remove the element at pos.
   * return an iterator pointing to the following element. if pos points to
   * the last element, return end(). throw if the container is empty,
   * the iterator is invalid, or it points to a wrong place.
   */
  iterator erase(iterator pos) {
    if(empty()) throw container_is_empty();
    if(pos.dq == nullptr || pos.bl == nullptr || pos.p == nullptr) 
    throw invalid_iterator();
    int it_cur = pos.cur;
    deque* it_dq = pos.dq;
    block* it_bl = pos.bl;
    T** it_p = pos.p;
    if(it_cur < 0 || it_cur >= size_) 
      throw index_out_of_bound();
    if(it_cur == 0 && size_ > 1){
        iterator res = pos + 1;
        pop_front();
        return res;
    }
    if(it_cur == size_ - 1){
        pop_back();
        return end();
    }
    size_--;
    int dis = it_p - it_bl->head;
    if(it_bl->cap > 1){
        iterator res = pos + 1;
        if(it_p == it_bl->tail){
          it_bl->tail--;it_bl->cap--;
          delete *(it_p);
          res.cur--;
          return res;
        }
        // std::cerr<<"res "<<(**(res))<<"dis "<<dis<<std::endl;
        for(int i = dis; i <= it_bl->cap - 2;i++)
        {
            **(it_bl->head + i) = **(it_bl->head + i + 1);
        }
        it_bl->cap--;
        it_bl->tail--;
        delete *(it_bl->head + it_bl->cap);
        // if(it_p == it_bl->tail) return iterator(it_dq,it_bl->next,it_p,it_cur);
        return iterator(it_dq,it_bl,it_p,it_cur);
    }
    else{
        // delete *(it_p);
        block *tmp = it_bl->next;
        bl_delete(it_bl);
        return iterator(it_dq,tmp,tmp->head,it_cur);
    }
  }

  /**
   * add an element to the end.
   */
  void push_back(const T &value) {
    if(lst == nullptr || this->lst->cap >= max_size_ || lst->tail == lst->list + max_size_){
      // std::cerr<<"push_back_newbl"<<std::endl;
      // if(lst->tail == lst->list + max_size_) std::cerr<<"nooo";
    
        // if(lst == nullptr) std::cerr<<"nooo";
        block *new_bl = bl_insert(lst,nullptr);
        // std::cerr<<"push_back"<<std::endl;
        new_bl->list[0] = new T(value);
        new_bl->tail = new_bl->head = &new_bl->list[0];
        new_bl->cap = 1;
        lst = new_bl;
        if(fir == nullptr) fir = lst;
    }
    else{
      T** tar = lst->head + lst->cap;
      (*tar) = new T(value);//这样才能放进正确地址 tar = new T*(value)就在别的地址创建新的指针了
      lst->tail = tar;
      (lst->cap)++;
    }
    size_++;
    // std::cerr<<size_<<" ";
  }

  /**
   * remove the last element.
   * throw when the container is empty.
   */
  void pop_back() {
    if(empty()) throw container_is_empty();
    size_--;
    lst->cap--;
    lst->tail = lst->tail - 1;
    // std::cerr<<"del "<<(**(lst->tail+1))<<std::endl;
    delete *(lst->tail + 1);
    if(lst->cap == 0){
        if(lst == fir) {
          fir->head = fir->list;
          fir->tail = fir->list - 1;
          return;
        }
        block* tar = lst;
        lst = lst->prev;
        bl_delete(tar);
    }
  }

  /**
   * insert an element to the beginning.
   */
  void push_front(const T &value) {
    if(size_ == 0){
      (*fir->head) = new T(value);
      fir->cap++;
      fir->tail = fir->head;
      size_++;return;
    }
    if((fir->head == fir->list && fir->head != nullptr)|| fir->cap >= max_size_)
    {
        block *new_bl = bl_insert(nullptr,fir);
        new_bl->tail = new_bl->list + max_size_ - 1;
        *(new_bl->tail) = new T(value);
        new_bl->head = new_bl->tail;
        new_bl->cap = 1;
        fir = new_bl;
        if(lst == nullptr) lst = fir;
    }
    else{
        fir->head = fir->head - 1;
        (*fir->head) = new T(value);
        fir->cap++;
    }
    size_++;
  }

  /**
   * remove the first element.
   * throw when the container is empty.
   */
  void pop_front() {
    if(empty()) throw container_is_empty();
    if(fir->cap == 1){
    // std::cerr<<"last_del "<<(**(fir->tail))<<std::endl;
        delete *(fir->tail);
        if(fir == lst){  
          fir->cap = 0;
          fir->head = fir->list;
          fir->tail = fir->list - 1;
          size_ = 0;
          return;
        }
        fir->cap--;
        fir->head = fir->tail + 1; 
        block *tmp = fir;
        fir = fir->next;
        bl_delete(tmp);
    }
    else{
    // std::cerr<<"del "<<(**(fir->head))<<std::endl;
        delete *(fir->head);
        fir->cap--;
        fir->head++;
    }
    size_--;
    // std::cerr<<"pop_end"<<std::endl;
  }
};

} // namespace sjtu

#endif