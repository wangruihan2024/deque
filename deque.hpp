#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

/*
  deque supports moveable only type
*/

#include "exceptions.hpp"

#include <cstddef>

#include <iostream>
#include <utility>
#include <cmath>
#include <tuple>
// #define LIST_DBG 1
// #define DEQUE_DBG 1

namespace sjtu {

// double list
template<class T> class list{
protected:
		struct node {
		T* data_;
		node *pre_, *next_;
		bool operator==(const node& other) const {
			return data_ == other.data_;
		}
		node(T* data, node* pre, node* next) 
			: data_(data), pre_(pre), next_(next) {}
		node() : node(nullptr, nullptr, nullptr) {}
		node(node&& rhs) {
			data_ = rhs.data_;
			rhs.data_ = nullptr;
		}
		node(const node& rhs) {
			data_ = new T(*rhs.data_);
			pre_ = rhs.pre_, next_ = rhs.next_;
		}
		~node() {
			delete data_;
		}
		node& operator=(const node& other) = default;
	};
private:
	node tail_node_, *head_, *tail_;
  size_t size_;

  list(node* head, node* tail, size_t size) : list() {
    link(head, tail, size);
  }
  list& link(node* head, node* tail, size_t size)  {
    if (head == tail)
      return *this;
    size_ += size;
    
    if (empty()) {
      head_ = head;
      head_->pre_ = nullptr;
    } else {
      tail_->pre_->next_ = head;
      head->pre_ = tail_->pre_;
    }

    tail->pre_->next_ = tail_;
    tail_->pre_ = tail->pre_;
    return *this;
  }
  void invalidate() {
    head_ = tail_;
    size_ = 0;
  }
public:
	/**
	 * elements
	 * add whatever you want
	*/

// --------------------------
	/**
	 * the follows are constructors and destructors
	 * you can also add some if needed.
	*/
	list()
		: tail_node_(), head_(&tail_node_), tail_(&tail_node_), size_(0) {
	}
	list(const list<T> &other) : list() {
    *this = other;
	}
	list(list<T> &&other) : list() {
    *this = std::move(other);
	}
	~list(){
		clear();
	}

	list& operator=(const list<T> &other) {
#ifdef LIST_DBG
    std::cerr << "LIST COPY" << std::endl;
#endif
    if (this == &other)
      return *this;
		this->clear();
		for (const auto &item : other)
			insert_tail(item);
		return *this;
	}

  list& operator=(list<T> &&other) {
#ifdef LIST_DBG
    std::cerr << "LIST MOVE" << std::endl;
#endif
    if (this == &other)
      return *this;
    this->clear();
    this->link(std::move(other));
    other.invalidate();
		return *this;
  }
  
  list& link(list<T> &&other) {
    link(other.head_, other.tail_, other.size());
    other.invalidate();
    return *this;
  }

	class iterator{
		friend list;
	protected:
    const list* lst_;
		node* nd_;
		iterator(const list* lst, node* nd)
			: lst_(lst), nd_(nd) {}
	public:
    	/**
		 * elements
		 * add whatever you want
		*/
	    // --------------------------
        /**
		 * the follows are constructors and destructors
		 * you can also add some if needed.
		*/
		iterator() 
			: lst_(nullptr), nd_(nullptr) {}
		iterator(const iterator &t) = default;
		~iterator() { }
        /**
		 * iter++
		 */

		iterator operator++(int) {
			iterator hold = *this;
			++*this;
			return hold;
		}
        /**
		 * ++iter
		 */
		iterator &operator++() {
			nd_ = nd_->next_;
			return *this;
		}
        /**
		 * iter--
		 */
		iterator operator--(int) {
			iterator hold = *this;
			--*this;
			return hold;
		}
        /**
		 * --iter
		 */
		iterator &operator--() {
			nd_ = nd_->pre_;
			return *this;
		}
		/**
		 * if the iter didn't point to a value
		 * throw " invalid"
		*/
		T &operator*() const {
			if (!nd_ || !(nd_->data_))
				throw "invalid";
			return *(nd_->data_);
		}

		// const	T &operator*() const {
		// 	return *(nd_->data_);
		// }
        /**
         * other operation
        */
		T *operator->() const noexcept {
			return nd_->data_;
		}
		bool operator==(const iterator &rhs) const {
			return nd_ == rhs.nd_;
		}
		bool operator!=(const iterator &rhs) const {
			return nd_ != rhs.nd_;
		}
	};
	/**
	 * return an iterator to the beginning
	 */
	iterator begin() const {
		return iterator(this, head_);
	}
	/**
	 * return an iterator to the ending
	 * in fact, it returns the iterator point to nothing,
	 * just after the last element.
	 */
	iterator end() const {
		return iterator(this, tail_);
	}
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
	iterator erase(iterator pos) {
		if (pos == iterator() || pos.lst_ != this || pos == end()) {
      throw std::runtime_error("list::erase : bad iterator");
    	return end();
    }
		if (pos == begin()) {
			delete_head();
			return begin();
		}
    size_--; // 之前写到 if 上面了，导致 size 维护出错
		node *now = pos.nd_, *ret = now->next_;
		now->pre_->next_ = now->next_;
		now->next_->pre_ = now->pre_;
		delete now;
		return iterator(this, ret);
	}

  iterator insert(iterator pos, const T& val) {
    if (pos == iterator() || pos.lst_ != this)
      throw "invalid insert iterator";
    if (pos == begin())
      return insert_head(val), begin();
    if (pos == end())
      return insert_tail(val), --end();
    size_++;
    node* next = pos.nd_;
    node* new_node = new node(new T(val), next->pre_, next);
    if (next->pre_)
      next->pre_->next_ = new_node;
    next->pre_ = new_node;
    return iterator(this, new_node);
  }
  iterator insert(iterator pos, T&& val) {
    if (pos == iterator() || pos.lst_ != this)
      throw "invalid insert iterator";
    if (pos == begin())
      return insert_head(std::move(val)), begin();
    if (pos == end())
      return insert_tail(std::move(val)), --end();
    size_++;
    node* next = pos.nd_;
    node* new_node = new node(new T(std::move(val)), next->pre_, next);
    if (next->pre_)
      next->pre_->next_ = new_node;
    next->pre_ = new_node;
    return iterator(this, new_node);
  }

	/**
	 * the following are operations of double list
	*/
	void insert_head(const T &val) {
    size_++;
		node* new_node = new node(new T(val), nullptr, head_);
		head_->pre_ = new_node;
		head_ = new_node;
	}
	void insert_head(T &&val) {
    size_++;
		node* new_node = new node(new T(std::move(val)), nullptr, head_);
		head_->pre_ = new_node;
		head_ = new_node;
	}

	void insert_tail(const T &val) {
    size_++;
		node* new_node = new node(new T(val), tail_->pre_, tail_);
		if (tail_->pre_)
			tail_->pre_->next_ = new_node;
		else 
			head_ = new_node;
		tail_->pre_ = new_node;
	}
	void insert_tail(T &&val) {
    size_++;
		node* new_node = new node(new T(std::move(val)), tail_->pre_, tail_);
		if (tail_->pre_)
			tail_->pre_->next_ = new_node;
		else 
			head_ = new_node;
		tail_->pre_ = new_node;
	}

	void delete_head() {
		if (head_ == tail_)
			throw std::runtime_error("list::delete_head : delete empty list");
    size_--;
		node* hold = head_;
		head_ = head_->next_;
		head_->pre_ = nullptr;
		delete hold;
	}
	void delete_tail() {
		if (head_ == tail_)
			throw std::runtime_error("list::delete_tail : delete empty list");
    size_--;
		node* pre = tail_->pre_;
		if (pre->pre_)
			pre->pre_->next_ = tail_;
		else 
			head_ = tail_;
		tail_->pre_ = pre->pre_;
    delete pre;
	}
	/**
	 * if didn't contain anything, return true, 
	 * otherwise false.
	 */
	bool empty() const {
		return head_ == tail_;
	}

  size_t size() {
    return size_;
  }

	void clear() {
		node* p = head_;
		while (p != tail_) {
			p = p->next_;
			delete p->pre_;
		}
		tail_node_ = node();
		head_ = tail_ = &tail_node_;
    size_ = 0;
	}

  static // return concatenation of l1, l2 (l1, l2 will be invalidated)
  list merge(list& l1, list& l2) {
    return std::move(l1.link(std::move(l2)));
  }

  static // split l into [1, s] and [s+1, end) (s must <= l.size(), l will be invalidated)
  std::pair<list, list> split(list& l, size_t s) {
    if (l.size() < s)
      throw std::runtime_error("invalid split");
    if (s == 0)
      return std::make_pair(list(), std::move(l));
    else if (s == l.size())
      return std::make_pair(std::move(l), list());
    node* p = l.head_;
    for (size_t i = 1; i <= s; i++)
      p = p->next_;
    list l1 = list(l.head_, p, s);
    list l2 = list(p, l.tail_, l.size() - s); // 乐，这里 p 用完之后 p->pre 就失效了，所以 l1 一定要在 l2 前面构造。
    l.invalidate();
    return std::make_pair(std::move(l1), std::move(l2));
  }

  static // return the index of the given iterator in list l
  size_t index_of_iterator(const list& l, const iterator& pos) {
    iterator it = l.begin();
    size_t n = 0;
    while (it != pos && it != l.end()) 
      n++, ++it;
    return n;
  }
};


template <class T> class deque {
public:
  using chunk = list<T>;
  static constexpr double expand_factor = 1.75, compress_factor = 0.5;
  // expand_factor / 2 - comparess_factor >> 0 ; compress_factor >= 0.5
  using list_it = typename list<chunk>::iterator;
  using chunk_it = typename list<T>::iterator;
private:
  list<chunk> data_;
  size_t size_;
  size_t last_bs_;

public:
  class const_iterator;
  class iterator {
    friend deque;
  protected:
    /**
     * add data members.
     * just add whatever you want.
     */
    const deque* dq_;
    list_it it1_;
    chunk_it it2_;
    iterator(const deque* dq, const list_it& it1, const chunk_it& it2) 
      : dq_(dq), it1_(it1), it2_(it2) {
    }
  public:
    iterator() 
      : dq_(nullptr), it1_(), it2_() {}

    /**
     * return a new iterator which points to the n-next element.
     * if there are not enough elements, the behaviour is undefined.
     * same for operator-.
     */
    iterator operator+(int n) const {
      if (*this == iterator())
        throw invalid_iterator();
      if (n == 0)
        return *this;
      if (n < 0)
        return *this - (-n);
      if (*this == dq_->end())
        throw index_out_of_bound();
      list_it it1 = it1_;
      chunk_it it2 = it2_;
      while (n && it2 != --(it1->end()))
        n--, ++it2;
      if (!n)
        return iterator(dq_, it1, it2);
      if (it1 == --(dq_->data_.end())) {
        if (n != 1)
          throw index_out_of_bound();
        return dq_->end();
      } else 
        ++it1;
      while (it1 != dq_->data_.end() && n > it1->size())
        n -= it1->size(), ++it1;
      if (it1 == dq_->data_.end()) {
        if (n > 1)
          throw index_out_of_bound();
        return dq_->end(); 
      }
      it2 = it1->begin();
      it2 = dq_->locate_in_chunk(it1, n - 1);
      return iterator(dq_, it1, it2);
    }
    iterator operator-(int n) const {
      if (*this == iterator())
        throw invalid_iterator();
      if (n == 0)
        return *this;
      if (n < 0)
        return *this + (-n);
      list_it it1 = it1_;
      chunk_it it2 = it2_;
      if (*this == dq_->end()) {
        // n--, --it1; // no need for n--
        --it1;
      } else {
        while (n && it2 != it1->begin())
          n--, --it2;
        if (!n)
          return iterator(dq_, it1, it2);
        if (it1 == dq_->data_.begin())
          throw index_out_of_bound();
        --it1;
      }
      while (n > it1->size() && it1 != dq_->data_.begin())
        n -= it1->size(), --it1;
      if (n > it1->size())
        throw index_out_of_bound();
      it2 = dq_->locate_in_chunk(it1, it1->size() - n);
      return iterator(dq_, it1, it2);
    }

    /**
     * return the distance between two iterators.
     * if they point to different dequs, throw
     * invaild_iterator.
     */
    int operator-(const iterator &rhs) const {
      if (this->dq_ != rhs.dq_)
        throw invalid_iterator();
      if (this->it1_ == rhs.it1_) {
        if (this->it1_ != dq_->data_.end())
          return static_cast<int>(chunk::index_of_iterator(*(this->it1_), this->it2_)) 
            - chunk::index_of_iterator(*(rhs.it1_), rhs.it2_);
        else 
          return 0;
      }
      bool greater = list<chunk>::index_of_iterator(dq_->data_, this->it1_) > list<chunk>::index_of_iterator(dq_->data_, rhs.it1_);
      list_it ITX = this->it1_, ITY = rhs.it1_;
      chunk_it itx = this->it2_, ity = rhs.it2_;
      if (greater)
        std::swap(ITX, ITY), std::swap(itx, ity);
      // 忘记特判 ITY = end() 了
      int res = static_cast<int>(ITX->size()) - chunk::index_of_iterator(*ITX, itx);
      if (ITY != dq_->data_.end())
        res += chunk::index_of_iterator(*ITY, ity);
      else 
        res += 0; // end()
      ++ITX; // 之前把 ++ITX 放在上一行前了
      while (ITX != ITY)
        res += ITX->size(), ++ITX;
      return greater ? res : -res;
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
      iterator ret = *this;
      *this = *this + 1;
      return ret;
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
      iterator ret = *this;
      *this = *this - 1;
      return ret;
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
      return *it2_;
    }
    /**
     * it->field
     */
    T *operator->() const noexcept {
      return it2_.operator->();
    }

    /**
     * check whether two iterators are the same (pointing to the same
     * memory).
     */
    bool operator==(const iterator &rhs) const {
      return this->dq_ == rhs.dq_ && this->it1_ == rhs.it1_ && this->it2_ == rhs.it2_;
    }
    // bool operator==(const const_iterator &rhs) const {} // for const_iterator convert to base and then compare
    /**
     * some other operator for iterators.
     */
    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }
    // bool operator!=(const const_iterator &rhs) const {}
  };

  class const_iterator : public iterator {
      friend deque;
    protected:
      const_iterator(const deque* dq, const list_it& it1, const chunk_it& it2) 
      : iterator(dq, it1, it2) {}
    public:
      const_iterator() : iterator() {}
      const_iterator(const iterator& it) : iterator(it) {}

    const T& operator*() const  {
      return this->iterator::operator*();
    }

    const T* operator->() const noexcept {
      return this->iterator::operator->();
    }
  };

private:

  size_t index_of_iterator(const iterator& it) {
    list_it it1 = data_.begin();
    size_t n = 0;
    while (it1 != it.it1_ && it1 != data_.end())
      n += it1->size(), ++it1;
    if (it1 == data_.end())
      return it.it1_ == data_.end() ? n : -1;
    n += chunk::index_of_iterator(*it1, it.it2_);
    return n;
  }

  size_t block_size() {
    return floor(sqrt(size_)) + 1; // +1 to avoid problem of zero
  }

  bool check_expand(const list_it& pos) {
    if (pos->size() > block_size() * expand_factor)
      return true;
    else 
      return false;
  }
  bool check_compress(const list_it& pos) {
    if (pos == data_.begin() || pos == --data_.end())
      return false;
    if (pos->size() < block_size() * compress_factor)
      return true;
    else
      return false;
  }

  bool check_reconstruct() {
    size_t bs = block_size();
    if (last_bs_ < bs * compress_factor || last_bs_ > bs * expand_factor)
      return true;
    return false;
  }

  // return -1 if no change happens, otherwise idx of p
  size_t check_rebalance(const iterator& p) {
    // return -1;
    list_it pos = p.it1_;
    if (pos == list_it() || pos == data_.end())
      return -1;
    size_t n = -1;
    if (check_compress(pos)) {
      n = (n == -1) ? index_of_iterator(p) : n;
      // n = 1;
      pos = compress(pos);
    }
    if (check_expand(pos)) {
      n = (n == -1) ? index_of_iterator(p) : n;
      // n = 1;
      pos = expand(pos);      
    }
    if (check_reconstruct()) {
      n = (n == -1) ? index_of_iterator(p) : n;
      // n = 1;
      reconstruct();
    } 
    return n;
  }

  list_it check_empty_chunk(list_it pos)  {
    if (pos == data_.end())
      return pos;
    if (pos->empty()) {
      return data_.erase(pos);
    } else 
      return pos;
  }

  list_it expand(const list_it& pos) { // split a chunk to two of even size
#ifdef DEQUE_DBG
    std::cerr << "EXPAND CALLED" << std::endl;
#endif
    chunk c1, c2;
    std::tie(c1, c2) = chunk::split(*pos, (pos->size() + 1) / 2);
    list_it next = data_.erase(pos);
    next = data_.insert(next, std::move(c2));
    return data_.insert(next, std::move(c1));
  } 
  list_it compress(const list_it& pos) { // merge two chunks
#ifdef DEQUE_DBG
    std::cerr << "COMPRESS CALLED" << std::endl;
#endif
    list_it other;
    list_it pre = pos, next = pos;
    bool pre_next = false;
    --pre, ++next;
    if (pre == data_.begin())
      other = next, pre_next = true;
    else if (next == --data_.end())
      other = pre, pre_next = false;
    else 
      other = (pre_next = next->size() <= pre->size()) ? next : pre;
    chunk c = pre_next ? chunk::merge(*pos, *other) : chunk::merge(*other, *pos);
    data_.erase(pos);
    list_it erase_next = data_.erase(other);
    return data_.insert(erase_next, std::move(c));
  }
  void reconstruct() { // re-partition into exactly bs size
#ifdef DEQUE_DBG
    std::cerr << "RECONSTRUCT CALLED" << std::endl;
#endif
    
    chunk cat_ck;
    for (chunk& ck : data_)
      cat_ck.link(std::move(ck));
    data_.clear();
    size_t bs = block_size();
    while (cat_ck.size() >= bs) {
      chunk ck;
      std::tie(ck, cat_ck) = chunk::split(cat_ck, bs);
      data_.insert_tail(std::move(ck));
    }
    if (cat_ck.size())
      data_.insert_tail(cat_ck);

    last_bs_ = bs;
  }

  chunk_it locate_in_chunk(const list_it& ck, size_t n) const { // approach n-th(0 indexed) position from near end
    chunk_it it;
    if (n < ck->size() - n) {
      it = ck->begin();
      while (n--)
        ++it;
      return it;
    } else {
      it = ck->end();
      n = ck->size() - n;
      while (n--)
        --it;
      return it;
    }
  }

  iterator at_iterator(size_t n) const {
#ifdef DEQUE_DBG
    if (n > size()) // 可以等于，是 end()
      throw index_out_of_bound();
#endif
    if (n < size() - n)
      return begin() + n;
    else 
      return end() - (size() - n);
  }

public:

  /**
   * constructors.
   */
  deque() : size_(0), last_bs_(block_size() /* 1 */) {}
  deque(const deque &other) : deque() {
    *this = other;
  }
  deque(deque&& other) : deque() {
    *this = std::move(other);
  }

  /**
   * deconstructor.
   */
  ~deque() {
  }

  /**
   * assignment operator.
   */
  deque& operator=(const deque &other) {
    if (this == &other)
      return *this;
    this->clear();
    size_ = other.size_;
    last_bs_ = other.last_bs_;
    for (const chunk& ck : other.data_)
      this->data_.insert_tail(ck);
    return *this;
  }

  deque& operator=(deque&& other) {
    if (this == &other)
      return *this;
    this->clear();
    data_ = std::move(other.data_);
    size_ = other.size_;
    last_bs_ = other.last_bs_;
    return *this;
  }
  /**
   * access a specified element with bound checking.
   * throw index_out_of_bound if out of bound.
   */
  T &at(const size_t &pos) { 
    if (pos >= size_)
      throw index_out_of_bound();
    return *at_iterator(pos);
  }
  const T &at(const size_t &pos) const {
    if (pos >= size_)
      throw index_out_of_bound();
    return *at_iterator(pos);
  }
  T &operator[](const size_t &pos) {
    return *at_iterator(pos);
  }
  const T &operator[](const size_t &pos) const {
    return *at_iterator(pos);
  }

  /**
   * access the first element.
   * throw container_is_empty when the container is empty.
   */
  const T &front() const {
    if (empty())
      throw container_is_empty();
    return *cbegin();
  }
  /**
   * access the last element.
   * throw container_is_empty when the container is empty.
   */
  const T &back() const {
    if (empty())
      throw container_is_empty();
    return *--cend();
  }

  /**
   * return an iterator to the beginning.
   */
  iterator begin() {
    if (empty())
      return end();
    return iterator(this, data_.begin(), data_.begin()->begin());
  }
  const_iterator begin() const {
    return cbegin();
  }
  const_iterator cbegin() const {
    if (empty())
      return cend();
    return const_iterator(this, data_.begin(), data_.begin()->begin());
  }
  /**
   * return an iterator to the end.
   */
  iterator end() {
    return iterator(this, data_.end(), chunk_it());
  }
  const_iterator end() const {
    return cend();
  }
  const_iterator cend() const {
    return const_iterator(this, data_.end(), chunk_it());
  }

  /**
   * check whether the container is empty.
   */
  bool empty() const {
    return size_ == 0;
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
    data_.clear();
    size_ = 0;
    last_bs_ = 1;
  }

  /**
   * insert value before pos.
   * return an iterator pointing to the inserted value.
   * throw if the iterator is invalid or it points to a wrong place.
   */
  iterator insert(iterator pos, const T &value) {
    if (pos == iterator() || pos.dq_ != this)
      throw invalid_iterator();
    if (empty())
      data_.insert_tail(chunk()), pos = end();
    size_t n = check_rebalance(pos);
    if (n != size_t(-1))
      pos = begin() + n;
    if (pos == end())
      --pos.it1_, pos.it2_ = pos.it1_->end();
    chunk_it it2 = pos.it1_->insert(pos.it2_, value);

    size_++;
    return iterator(this, pos.it1_, it2);
  }
  iterator insert(iterator pos, T&& value) {
    if (pos == iterator() || pos.dq_ != this)
      throw invalid_iterator();
    if (empty())
      data_.insert_tail(chunk()), pos = end();
    size_t n = check_rebalance(pos);
    if (n != size_t(-1))
      pos = begin() + n;
    if (pos == end())
      --pos.it1_, pos.it2_ = pos.it1_->end();
    chunk_it it2 = pos.it1_->insert(pos.it2_, std::move(value));

    size_++;
    return iterator(this, pos.it1_, it2); 
  } // finish and examine last insert, then copy to this one.

  /**
   * remove the element at pos.
   * return an iterator pointing to the following element. if pos points to
   * the last element, return end(). throw if the container is empty,
   * the iterator is invalid, or it points to a wrong place.
   */
  iterator erase(iterator pos) {
    if (pos.dq_ != this || empty() || pos == end())
      throw invalid_iterator();
    size_t n = check_rebalance(pos);
    if (n != -1)
      pos = at_iterator(n);
    chunk_it it2 = pos.it1_->erase(pos.it2_);
    list_it it1 = check_empty_chunk(pos.it1_);
    if (it1 != pos.it1_) {
      it2 = (it1 == data_.end()) ? chunk_it() /* i.e. deque::end() */ : it1->begin();
    } else if (it2 == it1->end()) {
      it2 = (++it1 == data_.end()) ? chunk_it() : it1->begin();
    }

    size_--;
    return iterator(this, it1, it2);
  }

  /**
   * add an element to the end.
   */
  void push_back(const T &value) {
    if (empty() || (--data_.end())->size() > block_size())
      data_.insert_tail(chunk());
    (--data_.end())->insert_tail(value);
    size_++;
  }
  void push_back(T &&value) {
    if (empty() || (--data_.end())->size() > block_size())
      data_.insert_tail(chunk());
    (--data_.end())->insert_tail(std::move(value));
    size_++;
  }

  /**
   * remove the last element.
   * throw when the container is empty.
   */
  void pop_back() {
    if (empty())
      throw std::runtime_error("deque::pop_back : empty container");
    (--data_.end())->delete_tail();
    check_empty_chunk(--data_.end());
    size_--;
  }

  /**
   * insert an element to the beginning.
   */
  void push_front(const T &value) {
    if (empty() || data_.begin()->size() > block_size())
      data_.insert_head(chunk());
    data_.begin()->insert_head(value);
    size_++;
  }
  void push_front(T &&value) {
    if (empty() || data_.begin()->size() > block_size())
      data_.insert_head(chunk());
    data_.begin()->insert_head(std::move(value));
    size_++;
  }

  /**
   * remove the first element.
   * throw when the container is empty.
   */
  void pop_front() {
    if (empty())
      throw std::runtime_error("deque::pop_back : empty container");
    size_--;
    data_.begin()->delete_head();
    check_empty_chunk(data_.begin());
  }
};

} // namespace sjtu

#endif