#pragma once

#include <memory>
#include <utility>
#include <concepts>
#include <algorithm>
#include <stdexcept>

namespace gto {

/**
 * @brief Circular queue.
 * @details Iterators are invalidated by: push(), push_front(), emplace(), pop(), pop_back(), reset() and clear().
 * @see https://en.wikipedia.org/wiki/Circular_buffer
 * @see https://github.com/torrentg/cqueue
 * @note This class is not thread-safe.
 * @version 1.0.0
 */
template<std::semiregular T>
class cqueue {

  public: // declarations

    //! cqueue iterator.
    class iterator {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
      private:
        cqueue<T> *queue = nullptr;
        difference_type pos = 0;
      private:
        std::size_t cast(difference_type n) const { return (n < 0 ? queue->size() : static_cast<std::size_t>(n)); }
        difference_type size() const { return static_cast<difference_type>(queue->size()); }
      public:
        explicit iterator(cqueue<T> *o, difference_type p = 0) : queue(o), pos(p < 0 ? -1 : (p < size() ? p : size())) {}
        reference operator*() { return queue->operator[](cast(pos)); }
        pointer operator->() { return &(queue->operator[](cast(pos))); }
        reference operator[](difference_type rhs) const { return (queue->operator[](cast(pos + rhs))); }
        bool operator==(const iterator &rhs) const { return (queue == rhs.queue && pos == rhs.pos); }
        bool operator!=(const iterator &rhs) const { return (queue != rhs.queue || pos != rhs.pos); }
        bool operator >(const iterator &rhs) const { return (queue == rhs.queue && pos  > rhs.pos); }
        bool operator <(const iterator &rhs) const { return (queue == rhs.queue && pos  < rhs.pos); }
        bool operator>=(const iterator &rhs) const { return (queue == rhs.queue && pos >= rhs.pos); }
        bool operator<=(const iterator &rhs) const { return (queue == rhs.queue && pos <= rhs.pos); }
        iterator& operator++() { pos = (pos + 1 < size() ? pos + 1 : size()); return *this; }
        iterator& operator--() { pos = (pos < 0 ? -1 : pos -1); return *this; }
        iterator  operator++(int) { iterator tmp(queue, pos); operator++(); return tmp; }
        iterator  operator--(int) { iterator tmp(queue, pos); operator--(); return tmp; }
        iterator& operator+=(difference_type rhs) { pos = (pos + rhs < size() ? pos + rhs : size()); return *this; }
        iterator& operator-=(difference_type rhs) { pos = (pos - rhs < 0 ? -1 : pos - rhs); return *this; }
        iterator  operator+(difference_type rhs) const { return iterator(queue, pos + rhs); }
        iterator  operator-(difference_type rhs) const { return iterator(queue, pos - rhs); }
        friend iterator operator+(difference_type lhs, const iterator &rhs) { return iterator(rhs.queue, lhs + rhs.pos); }
        friend iterator operator-(difference_type lhs, const iterator &rhs) { return iterator(rhs.queue, lhs - rhs.pos); }
        difference_type operator-(const iterator &rhs) const { return (pos - rhs.pos); }
    };

    //! cqueue const iterator.
    class const_iterator {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = const T;
        using pointer = const T *;
        using reference = const T &;
      private:
        const cqueue<T> *queue = nullptr;
        difference_type pos = 0;
      private:
        std::size_t cast(difference_type n) const { return (n < 0 ? queue->size() : static_cast<std::size_t>(n)); }
        difference_type size() const { return static_cast<difference_type>(queue->size()); }
      public:
        explicit const_iterator(const cqueue<T> *o, difference_type p = 0) : queue(o), pos(p < 0 ? -1 : (p < size() ? p : size())) {}
        reference operator*() { return queue->operator[](cast(pos)); }
        pointer operator->() { return &(queue->operator[](cast(pos))); }
        reference operator[](difference_type rhs) const { return (queue->operator[](cast(pos + rhs))); }
        bool operator==(const const_iterator &rhs) const { return (queue == rhs.queue && pos == rhs.pos); }
        bool operator!=(const const_iterator &rhs) const { return (queue != rhs.queue || pos != rhs.pos); }
        bool operator >(const const_iterator &rhs) const { return (queue == rhs.queue && pos  > rhs.pos); }
        bool operator <(const const_iterator &rhs) const { return (queue == rhs.queue && pos  < rhs.pos); }
        bool operator>=(const const_iterator &rhs) const { return (queue == rhs.queue && pos >= rhs.pos); }
        bool operator<=(const const_iterator &rhs) const { return (queue == rhs.queue && pos <= rhs.pos); }
        const_iterator& operator++() { pos = (pos + 1 < size() ? pos + 1 : size()); return *this; }
        const_iterator& operator--() { pos = (pos < 0 ? -1 : pos -1); return *this; }
        const_iterator  operator++(int) { const_iterator tmp(queue, pos); operator++(); return tmp; }
        const_iterator  operator--(int) { const_iterator tmp(queue, pos); operator--(); return tmp; }
        const_iterator& operator+=(difference_type rhs) { pos = (pos + rhs < size() ? pos + rhs : size()); return *this; }
        const_iterator& operator-=(difference_type rhs) { pos = (pos - rhs < 0 ? -1 : pos - rhs); return *this; }
        const_iterator  operator+(difference_type rhs) const { return const_iterator(queue, pos + rhs); }
        const_iterator  operator-(difference_type rhs) const { return const_iterator(queue, pos - rhs); }
        friend const_iterator operator+(difference_type lhs, const const_iterator &rhs) { return const_iterator(rhs.queue, lhs + rhs.pos); }
        friend const_iterator operator-(difference_type lhs, const const_iterator &rhs) { return const_iterator(rhs.queue, lhs - rhs.pos); }
        difference_type operator-(const const_iterator &rhs) const { return (pos - rhs.pos); }
    };

  private: // static members

    //! Capacity increase factor.
    static constexpr std::size_t GROWTH_FACTOR = 2;
    //! Default initial capacity (power of 2).
    static constexpr std::size_t DEFAULT_RESERVED = 8;
    //! Max capacity (user-defined power of 2).
    static constexpr std::size_t MAX_CAPACITY = 67'108'864;

  private: // members

    //! Buffer.
    std::unique_ptr<T[]> mData;
    //! Buffer size.
    std::size_t mReserved = 0;
    //! Maximum number of elements (always > 0).
    std::size_t mCapacity = 0;
    //! Index representing first entry (0 <= mFront < mReserved).
    std::size_t mFront = 0;
    //! Number of entries in the queue (empty = 0, full = mReserved).
    std::size_t mLength = 0;

  private: // methods

    //! Convert from pos to index (throw exception if out-of-bounds).
    std::size_t getCheckedIndex(std::size_t pos) const noexcept(false);
    //! Convert from pos to index.
    std::size_t getUncheckedIndex(std::size_t pos) const noexcept;
    //! Compute memory size to reserve.
    std::size_t getNewMemoryLength(std::size_t n) const;
    //! Ensure buffer size.
    void reserve(std::size_t n);

  public: // static methods

    //! Maximum capacity the container is able to hold.
    static std::size_t max_capacity() noexcept { return MAX_CAPACITY; }

  public: // methods

    //! Constructor (0 means unlimited).
    cqueue(std::size_t capacity = 0);
    //! Copy constructor.
    cqueue(const cqueue &other);
    //! Move constructor.
    cqueue(cqueue &&other) noexcept { this->swap(other); }
    //! Destructor.
    ~cqueue() = default;

    //! Copy assignment.
    cqueue & operator=(const cqueue &other);
    //! Move assignment.
    cqueue & operator=(cqueue &&other) { this->swap(other); return *this; }

    //! Return queue capacity.
    std::size_t capacity() const { return (mCapacity == MAX_CAPACITY ? 0 : mCapacity); }
    //! Return the number of items.
    std::size_t size() const { return mLength; }
    //! Current reserved size (numbers of items).
    std::size_t reserved() const { return mReserved; }
    //! Check if there are items in the queue.
    bool empty() const { return (mLength == 0); }

    //! Return the first element.
    const T & front() const { return operator[](0); }
    //! Return the first element.
    T & front() { return operator[](0); }
    //! Return the last element.
    const T & back() const { return operator[](mLength-1); }
    //! Return the last element.
    T & back() { return operator[](mLength-1); }

    //! Insert an element at the end.
    void push(const T &val);
    //! Insert an element at the end.
    void push(T &&val);
    //! Insert an element at the front.
    void push_front(const T &val);
    //! Insert an element at the front.
    void push_front(T &&val);

    //! Construct and insert an element at the end.
    template <class... Args>
    void emplace(Args&&... args);
    //! Remove the front element.
    bool pop();
    //! Remove the back element.
    bool pop_back();

    //! Returns a reference to the element at position n.
    T & operator[](std::size_t n) { return mData[getCheckedIndex(n)]; }
    //! Returns a const reference to the element at position n.
    const T & operator[](std::size_t n) const { return mData[getCheckedIndex(n)]; }

    //! Returns an iterator to the first element.
    iterator begin() noexcept { return iterator(this, 0); }
    //! Returns an iterator to the element following the last element.
    iterator end() noexcept { return iterator(this, static_cast<std::ptrdiff_t>(size())); }
    //! Returns an iterator to the first element.
    const_iterator begin() const noexcept { return const_iterator(this, 0); }
    //! Returns an iterator to the element following the last element.
    const_iterator end() const noexcept { return const_iterator(this, static_cast<std::ptrdiff_t>(size())); }

    //! Clear content.
    void clear() noexcept;
    //! Swap content.
    void swap (cqueue<T> &x) noexcept;
};

} // namespace gto

/**
 * @param[in] capacity Container capacity.
 */
template<std::semiregular T>
gto::cqueue<T>::cqueue(std::size_t capacity) {
  if (capacity > MAX_CAPACITY) {
    throw std::length_error("cqueue max capacity exceeded");
  } else {
    mCapacity = (capacity == 0 ? MAX_CAPACITY : capacity);
  }
}

/**
 * @param[in] other Queue to copy.
 */
template<std::semiregular T>
gto::cqueue<T>::cqueue(const cqueue &other) {
  mCapacity = other.mCapacity;
  reserve(mLength);
  for (std::size_t i = 0; i < other.size(); i++) {
    push(other[i]);
  }
}

/**
 * @param[in] other Queue to copy.
 */
template<std::semiregular T>
gto::cqueue<T> & gto::cqueue<T>::operator=(const cqueue &other) {
  clear();
  mCapacity = other.mCapacity;
  for (std::size_t i = 0; i < other.size(); i++) {
    push(other[i]);
  }
  return *this;
}

/**
 * @param[in] num Element position.
 * @return Index in buffer.
 */
template<std::semiregular T>
std::size_t gto::cqueue<T>::getUncheckedIndex(std::size_t pos) const noexcept {
  return (mFront + pos) % (mReserved == 0 ? 1 : mReserved);
}

/**
 * @param[in] num Element position.
 * @return Index in buffer.
 * @exception std::out_of_range Invalid position.
 */
template<std::semiregular T>
std::size_t gto::cqueue<T>::getCheckedIndex(std::size_t pos) const noexcept(false) {
  if (pos >= mLength) {
    throw std::out_of_range("cqueue access out-of-range");
  } else {
    return getUncheckedIndex(pos);
  }
}

/**
 * @details Remove all elements.
 */
template<std::semiregular T>
void gto::cqueue<T>::clear() noexcept {
  for (std::size_t i = 0; i < mLength; i++) {
    mData[getUncheckedIndex(i)] = T{};
  }
  mFront = 0;
  mLength = 0;
}

/**
 * @details Swap content with another same-type cqueue.
 */
template<std::semiregular T>
void gto::cqueue<T>::swap(cqueue<T> &x) noexcept {
  mData.swap(x.mData);
  std::swap(mFront, x.mFront);
  std::swap(mLength, x.mLength);
  std::swap(mReserved, x.mReserved);
  std::swap(mCapacity, x.mCapacity);
}

/**
 * @brief Compute the new buffer size.
 * @param[in] n New queue size.
 */
template<std::semiregular T>
std::size_t gto::cqueue<T>::getNewMemoryLength(std::size_t n) const {
  std::size_t ret = (mReserved == 0 ? std::min(mCapacity, DEFAULT_RESERVED) : mReserved);
  while (ret < n) {
    ret *= GROWTH_FACTOR;
  }
  return std::min(ret, mCapacity);
}

/**
 * @param[in] n Expected future queue size.
 * @exception std::length_error Capacity exceeded.
 */
template<std::semiregular T>
void gto::cqueue<T>::reserve(std::size_t n) {
  if (n < mReserved) {
    return;
  } else if (n > mCapacity) {
    throw std::length_error("cqueue capacity exceeded");
  }

  std::size_t len = getNewMemoryLength(n);
  auto tmp = std::make_unique<T[]>(len);
  for (std::size_t i = 0; i < mLength; i++) {
    tmp[i] = std::move(mData[getUncheckedIndex(i)]);
  }

  mReserved = len;
  mData.swap(tmp);
  mFront = 0;
}

/**
 * @param[in] val Value to add.
 * @exception std::length_error Number of values exceed queue capacity.
 */
template<std::semiregular T>
void gto::cqueue<T>::push(const T &val) {
  reserve(mLength + 1);
  mData[getUncheckedIndex(mLength)] = val;
  mLength++;
}

/**
 * @param[in] val Value to add.
 * @exception std::length_error Number of values exceed queue capacity.
 */
template<std::semiregular T>
void gto::cqueue<T>::push(T &&val) {
  reserve(mLength + 1);
  mData[getUncheckedIndex(mLength)] = std::move(val);
  mLength++;
}

/**
 * @param[in] val Value to add.
 * @exception std::length_error Number of values exceed queue capacity.
 */
template<std::semiregular T>
void gto::cqueue<T>::push_front(const T &val) {
  reserve(mLength + 1);
  mFront = (mLength == 0 ? 0 : (mFront == 0 ? mReserved : mFront) - 1);
  mData[mFront] = val;
  mLength++;
}

/**
 * @param[in] val Value to add.
 * @exception std::length_error Number of values exceed queue capacity.
 */
template<std::semiregular T>
void gto::cqueue<T>::push_front(T &&val) {
  reserve(mLength + 1);
  mFront = (mLength == 0 ? 0 : (mFront == 0 ? mReserved : mFront) - 1);
  mData[mFront] = std::move(val);
  mLength++;
}

/**
 * @param[in] args Arguments of the new item.
 * @exception std::length_error Number of values exceed queue capacity.
 */
template<std::semiregular T>
template <class... Args>
void gto::cqueue<T>::emplace(Args&&... args) {
  reserve(mLength + 1);
  mData[getUncheckedIndex(mLength)] = T{std::forward<Args>(args)...};
  mLength++;
}

/**
 * @return true = an element was erased, false = no elements in the queue.
 */
template<std::semiregular T>
bool gto::cqueue<T>::pop() {
  if (mLength == 0) {
    return false;
  }
  
  mData[mFront] = T{};
  mFront = getUncheckedIndex(1);
  mLength--;
  return true;
}

/**
 * @return true = an element was erased, false = no elements in the queue.
 */
template<std::semiregular T>
bool gto::cqueue<T>::pop_back() {
  if (mLength == 0) {
    return false;
  }
  mLength--;
  mData[getUncheckedIndex(mLength)] = T{};
  return true;
}
