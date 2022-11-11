#pragma once

#include <memory>
#include <limits>
#include <utility>
#include <concepts>
#include <algorithm>
#include <stdexcept>

namespace gto {

/**
 * @brief Circular queue.
 * @details Iterators are invalidated by: 
 *          push(), push_front(), emplace(), pop(), pop_back(), reserve(), reset() and clear().
 * @see https://en.wikipedia.org/wiki/Circular_buffer
 * @see https://github.com/torrentg/cqueue
 * @note This class is not thread-safe.
 * @version 1.0.0
 */
template<std::copyable T, typename Allocator = std::allocator<T>>
class cqueue {

  public: // declarations

    // Aliases  
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = T *;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Allocator;
    using const_alloc_reference = const allocator_type &;

    //! cqueue iterator.
    class iterator {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;
      private:
        cqueue *queue = nullptr;
        difference_type pos = 0;
      private:
        size_type cast(difference_type n) const { return (n < 0 ? queue->size() : static_cast<size_type>(n)); }
        difference_type size() const { return static_cast<difference_type>(queue->size()); }
      public:
        explicit iterator(cqueue *o, difference_type p = 0) : queue(o), pos(p < 0 ? -1 : (p < size() ? p : size())) {}
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
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;
      private:
        const cqueue *queue = nullptr;
        difference_type pos = 0;
      private:
        size_type cast(difference_type n) const { return (n < 0 ? queue->size() : static_cast<size_type>(n)); }
        difference_type size() const { return static_cast<difference_type>(queue->size()); }
      public:
        explicit const_iterator(const cqueue *o, difference_type p = 0) : queue(o), pos(p < 0 ? -1 : (p < size() ? p : size())) {}
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
    static constexpr size_type GROWTH_FACTOR = 2;
    //! Default initial capacity (power of 2).
    static constexpr size_type DEFAULT_RESERVED = 8;

  private: // members

    //! Memory allocator.
    [[no_unique_address]]
    allocator_type mAllocator;
    //! Buffer.
    pointer mData = nullptr;
    //! Buffer size.
    size_type mReserved = 0;
    //! Maximum number of elements (always > 0).
    size_type mCapacity = 0;
    //! Index representing first entry (0 <= mFront < mReserved).
    size_type mFront = 0;
    //! Number of entries in the queue (empty = 0, full = mReserved).
    size_type mLength = 0;

  private: // methods

    //! Convert from pos to index (throw exception if out-of-bounds).
    constexpr size_type getCheckedIndex(size_type pos) const noexcept(false);
    //! Convert from pos to index.
    constexpr size_type getUncheckedIndex(size_type pos) const noexcept;
    //! Compute memory size to reserve.
    constexpr size_type getNewMemoryLength(size_type n) const noexcept;
    //! Resize buffer.
    constexpr void resizeIfRequired(size_type n);
    //! Resize buffer.
    void resize(size_type n);
    //! Clear and dealloc memory (preserve capacity and allocator).
    void reset() noexcept;

  public: // static methods

    //! Maximum capacity the container is able to hold.
    static constexpr size_type max_capacity() noexcept { return (std::numeric_limits<difference_type>::max()); }

  public: // methods

    //! Constructor.
    constexpr explicit cqueue(const_alloc_reference alloc = Allocator()) : cqueue(0, alloc) {}
    //! Constructor (capacity=0 means unlimited).
    constexpr explicit cqueue(size_type capacity, const_alloc_reference alloc = Allocator());
    //! Copy constructor.
    constexpr cqueue(const cqueue &other);
    //! Copy constructor with allocator.
    constexpr cqueue(const cqueue &other, const_alloc_reference alloc);
    //! Move constructor.
    constexpr cqueue(cqueue &&other) noexcept { this->swap(other); }
    //! Move constructor.
    constexpr cqueue(cqueue &&other, const_alloc_reference alloc);
    //! Destructor.
    ~cqueue() noexcept { reset(); };

    //! Copy assignment.
    constexpr cqueue & operator=(const cqueue &other);
    //! Move assignment.
    constexpr cqueue & operator=(cqueue &&other) { this->swap(other); return *this; }

    //! Return container allocator.
    [[nodiscard]] constexpr allocator_type get_allocator() const noexcept { return mAllocator; }
    //! Return queue capacity.
    [[nodiscard]] constexpr size_type capacity() const noexcept { return (mCapacity == max_capacity() ? 0 : mCapacity); }
    //! Return the number of items.
    [[nodiscard]] constexpr size_type size() const noexcept { return mLength; }
    //! Current reserved size (numbers of items).
    [[nodiscard]] constexpr size_type reserved() const noexcept { return mReserved; }
    //! Check if there are items in the queue.
    [[nodiscard]] constexpr bool empty() const noexcept { return (mLength == 0); }

    //! Return the first element.
    [[nodiscard]] constexpr const_reference front() const { return operator[](0); }
    //! Return the first element.
    [[nodiscard]] constexpr reference front() { return operator[](0); }
    //! Return the last element.
    [[nodiscard]] constexpr const_reference back() const { return operator[](mLength-1); }
    //! Return the last element.
    [[nodiscard]] constexpr reference back() { return operator[](mLength-1); }

    //! Insert an element at the end.
    constexpr void push(const T &val);
    //! Insert an element at the end.
    constexpr void push(T &&val);
    //! Insert an element at the front.
    constexpr void push_front(const T &val);
    //! Insert an element at the front.
    constexpr void push_front(T &&val);

    //! Construct and insert an element at the end.
    template <class... Args>
    constexpr void emplace(Args&&... args);
    //! Remove the front element.
    constexpr bool pop();
    //! Remove the back element.
    constexpr bool pop_back();

    //! Returns a reference to the element at position n.
    [[nodiscard]] constexpr reference operator[](size_type n) { return mData[getCheckedIndex(n)]; }
    //! Returns a const reference to the element at position n.
    [[nodiscard]] constexpr const_reference operator[](size_type n) const { return mData[getCheckedIndex(n)]; }

    //! Returns an iterator to the first element.
    [[nodiscard]] constexpr iterator begin() noexcept { return iterator(this, 0); }
    //! Returns an iterator to the element following the last element.
    [[nodiscard]] constexpr iterator end() noexcept { return iterator(this, static_cast<difference_type>(size())); }
    //! Returns an iterator to the first element.
    [[nodiscard]] constexpr const_iterator begin() const noexcept { return const_iterator(this, 0); }
    //! Returns an iterator to the element following the last element.
    [[nodiscard]] constexpr const_iterator end() const noexcept { return const_iterator(this, static_cast<difference_type>(size())); }

    //! Ensure buffer size.
    constexpr void reserve(size_type n);
    //! Clear content.
    void clear() noexcept;
    //! Swap content.
    constexpr void swap (cqueue &x) noexcept;
};

} // namespace gto

/**
 * @param[in] capacity Container capacity.
 * @param[in] alloc Allocator to use.
 */
template<std::copyable T, typename Allocator>
constexpr gto::cqueue<T, Allocator>::cqueue(size_type capacity, const_alloc_reference alloc) : 
    mAllocator(alloc), mData{nullptr}, mReserved{0}, mCapacity{max_capacity()}, mFront{0}, mLength{0}
{
  if (capacity > max_capacity()) {
    throw std::length_error("cqueue max capacity exceeded");
  } else {
    mCapacity = (capacity == 0 ? max_capacity() : capacity);
  }
}

/**
 * @param[in] other Queue to copy.
 */
template<std::copyable T, typename Allocator>
constexpr gto::cqueue<T, Allocator>::cqueue(const cqueue &other) :
    mAllocator{std::allocator_traits<allocator_type>::select_on_container_copy_construction(other.get_allocator())},
    mData{nullptr}, mReserved{0}, mCapacity{other.mCapacity}, mFront{0}, mLength{0}
{
  resizeIfRequired(other.mLength);
  for (size_type i = 0; i < other.size(); ++i) {
    push(other[i]);
  }
}

/**
 * @param[in] other Queue to copy.
 * @param[in] alloc Allocator to use.
 */
template<std::copyable T, typename Allocator>
constexpr gto::cqueue<T, Allocator>::cqueue(const cqueue &other, const_alloc_reference alloc) : 
    mAllocator{alloc}, mData{nullptr}, mReserved{0}, mCapacity{other.mCapacity}, mFront{0}, mLength{0}
{
  resizeIfRequired(other.mLength);
  for (size_type i = 0; i < other.size(); ++i) {
    push(other[i]);
  }
}

/**
 * @param[in] other Queue to copy.
 * @param[in] alloc Allocator to use
 */
template<std::copyable T, typename Allocator>
constexpr gto::cqueue<T, Allocator>::cqueue(cqueue &&other, const_alloc_reference alloc) :
    mData{nullptr}, mReserved{0}, mCapacity{max_capacity()}, mFront{0}, mLength{0}
{
  if (alloc == other.mAllocator) {
    this->swap(other);
  } else {
    mAllocator = alloc;
    mCapacity = other.mCapacity;
    resizeIfRequired(other.mLength);
    for (size_type i = 0; i < other.size(); ++i) {
      push(std::move(other[i]));
    }
    other.reset();
    other.mCapacity = 0;
  }
}

/**
 * @param[in] other Queue to copy.
 */
template<std::copyable T, typename Allocator>
constexpr gto::cqueue<T, Allocator> & gto::cqueue<T, Allocator>::operator=(const cqueue &other) {
  if (this != &other) {
    cqueue tmp(other);
    this->swap(tmp);
  }
  return *this;
}

/**
 * @param[in] num Element position.
 * @return Index in buffer.
 */
template<std::copyable T, typename Allocator>
constexpr typename gto::cqueue<T, Allocator>::size_type gto::cqueue<T, Allocator>::getUncheckedIndex(size_type pos) const noexcept {
  return ((mFront + pos) % (mReserved == 0 ? 1 : mReserved));
}

/**
 * @param[in] num Element position.
 * @return Index in buffer.
 * @exception std::out_of_range Invalid position.
 */
template<std::copyable T, typename Allocator>
constexpr typename gto::cqueue<T, Allocator>::size_type gto::cqueue<T, Allocator>::getCheckedIndex(size_type pos) const noexcept(false) {
  if (pos >= mLength) {
    throw std::out_of_range("cqueue access out-of-range");
  } else {
    return getUncheckedIndex(pos);
  }
}

/**
 * @details Remove all elements.
 */
template<std::copyable T, typename Allocator>
void gto::cqueue<T, Allocator>::clear() noexcept {
  for (size_type i = 0; i < mLength; ++i) {
    size_type index = getUncheckedIndex(i);
    std::allocator_traits<allocator_type>::destroy(mAllocator, mData + index);
  }
  mFront = 0;
  mLength = 0;
}

/**
 * @details Remove all elements and frees memory.
 */
template<std::copyable T, typename Allocator>
void gto::cqueue<T, Allocator>::reset() noexcept {
  clear();
  std::allocator_traits<allocator_type>::deallocate(mAllocator, mData, mReserved);
  mData = nullptr;
  mReserved = 0;
}

/**
 * @details Swap content with another same-type cqueue.
 */
template<std::copyable T, typename Allocator>
constexpr void gto::cqueue<T, Allocator>::swap(cqueue &other) noexcept {
  if (this == &other) {
    return;
  }
  if constexpr (std::allocator_traits<allocator_type>::propagate_on_container_swap::value) {
    std::swap(mAllocator, other.mAllocator);
  }
  std::swap(mData, other.mData);
  std::swap(mFront, other.mFront);
  std::swap(mLength, other.mLength);
  std::swap(mReserved, other.mReserved);
  std::swap(mCapacity, other.mCapacity);
}

/**
 * @brief Compute the new buffer size.
 * @param[in] n New queue size.
 */
template<std::copyable T, typename Allocator>
constexpr typename gto::cqueue<T, Allocator>::size_type gto::cqueue<T, Allocator>::getNewMemoryLength(size_type n) const noexcept {
  size_type ret = (mReserved == 0 ? std::min(mCapacity, DEFAULT_RESERVED) : mReserved);
  while (ret < n) {
    ret *= GROWTH_FACTOR;
  }
  return std::min(ret, mCapacity);
}

/**
 * @param[in] n Expected future queue size.
 * @exception std::length_error Capacity exceeded.
 * @exception ... Error throwed by move contructors.
 */
template<std::copyable T, typename Allocator>
constexpr void gto::cqueue<T, Allocator>::resizeIfRequired(size_type n) {
  if (n < mReserved) {
    [[likely]]
    return;
  } else if (n > mCapacity) {
    [[unlikely]] 	
    throw std::length_error("cqueue capacity exceeded");
  } else {
    size_type len = getNewMemoryLength(n);
    resize(len);
  }
}

/**
 * @param[in] n Expected future queue size.
 * @exception std::length_error Capacity exceeded.
 * @exception ... Error throwed by move contructors.
 */
template<std::copyable T, typename Allocator>
constexpr void gto::cqueue<T, Allocator>::reserve(size_type n) {
  if (n < mReserved) {
    return;
  } else if (n > mCapacity) {
    throw std::length_error("cqueue capacity exceeded");
  } else {
    resize(n);
  }
}

/**
 * @param[in] n New reserved size.
 * @details Provides strong exception guarantee.
 * @see https://en.cppreference.com/w/cpp/language/exceptions#Exception_safety
 * @exception ... Error throwed by move contructors.
 */
template<std::copyable T, typename Allocator>
void gto::cqueue<T, Allocator>::resize(size_type len)
{
  pointer tmp = std::allocator_traits<allocator_type>::allocate(mAllocator, len);

  if constexpr (std::is_nothrow_move_constructible<T>::value) {
    // move elements from mData to tmp
    for (size_type i = 0; i < mLength; ++i) {
      size_type index = getUncheckedIndex(i);
      std::allocator_traits<allocator_type>::construct(mAllocator, tmp + i, std::move(mData[index]));
    }
  }
  else {
    // copy elements from mData to tmp
    size_type i = 0;
    try {
      for (i = 0; i < mLength; ++i) {
        size_type index = getUncheckedIndex(i);
        std::allocator_traits<allocator_type>::construct(mAllocator, tmp + i, mData[index]);
      }
    } catch (...) {
      for (size_type j = 0; j < i; ++j) {
        std::allocator_traits<allocator_type>::destroy(mAllocator, tmp + j);
      }
      std::allocator_traits<allocator_type>::deallocate(mAllocator, tmp, len);
      throw;
    }
  }

  // destroy mData elements
  for (size_type j = 0; j < mLength; ++j) {
    size_type index = getUncheckedIndex(j);
    std::allocator_traits<allocator_type>::destroy(mAllocator, mData + index);
  }

  // deallocate mData
  std::allocator_traits<allocator_type>::deallocate(mAllocator, mData, mReserved);

  // assign new content
  mData = tmp;
  mReserved = len;
  mFront = 0;
}

/**
 * @param[in] val Value to add.
 * @exception std::length_error Number of values exceed queue capacity.
 */
template<std::copyable T, typename Allocator>
constexpr void gto::cqueue<T, Allocator>::push(const T &val) {
  resizeIfRequired(mLength + 1);
  size_type index = getUncheckedIndex(mLength);
  std::allocator_traits<allocator_type>::construct(mAllocator, mData + index, val);
  ++mLength;
}

/**
 * @param[in] val Value to add.
 * @exception std::length_error Number of values exceed queue capacity.
 */
template<std::copyable T, typename Allocator>
constexpr void gto::cqueue<T, Allocator>::push(T &&val) {
  resizeIfRequired(mLength + 1);
  size_type index = getUncheckedIndex(mLength);
  std::allocator_traits<allocator_type>::construct(mAllocator, mData + index, std::move(val));
  ++mLength;
}

/**
 * @param[in] val Value to add.
 * @exception std::length_error Number of values exceed queue capacity.
 */
template<std::copyable T, typename Allocator>
constexpr void gto::cqueue<T, Allocator>::push_front(const T &val) {
  resizeIfRequired(mLength + 1);
  size_type index = (mLength == 0 ? 0 : (mFront == 0 ? mReserved : mFront) - 1);
  std::allocator_traits<allocator_type>::construct(mAllocator, mData + index, val);
  mFront = index;
  ++mLength;
}

/**
 * @param[in] val Value to add.
 * @exception std::length_error Number of values exceed queue capacity.
 */
template<std::copyable T, typename Allocator>
constexpr void gto::cqueue<T, Allocator>::push_front(T &&val) {
  resizeIfRequired(mLength + 1);
  size_type index = (mLength == 0 ? 0 : (mFront == 0 ? mReserved : mFront) - 1);
  std::allocator_traits<allocator_type>::construct(mAllocator, mData + index, std::move(val));
  mFront = index;
  ++mLength;
}

/**
 * @param[in] args Arguments of the new item.
 * @exception std::length_error Number of values exceed queue capacity.
 */
template<std::copyable T, typename Allocator>
template <class... Args>
constexpr void gto::cqueue<T, Allocator>::emplace(Args&&... args) {
  resizeIfRequired(mLength + 1);
  size_type index = getUncheckedIndex(mLength);
  std::allocator_traits<allocator_type>::construct(mAllocator, mData + index, std::forward<Args>(args)...);
  ++mLength;
}

/**
 * @return true = an element was erased, false = no elements in the queue.
 */
template<std::copyable T, typename Allocator>
constexpr bool gto::cqueue<T, Allocator>::pop() {
  if (mLength == 0) {
    return false;
  }
  std::allocator_traits<allocator_type>::destroy(mAllocator, mData + mFront);
  mFront = getUncheckedIndex(1);
  --mLength;
  return true;
}

/**
 * @return true = an element was erased, false = no elements in the queue.
 */
template<std::copyable T, typename Allocator>
constexpr bool gto::cqueue<T, Allocator>::pop_back() {
  if (mLength == 0) {
    return false;
  }
  size_type index = getUncheckedIndex(mLength - 1);
  std::allocator_traits<allocator_type>::destroy(mAllocator, mData + index);
  --mLength;
  return true;
}
