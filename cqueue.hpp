#pragma once

#include <memory>
#include <limits>
#include <compare>
#include <utility>
#include <concepts>
#include <algorithm>
#include <stdexcept>
#include <type_traits>

namespace gto {

/**
 * @brief Circular queue.
 * @details Iterators are invalidated by: 
 *          push(), push_back(), push_front(), 
 *          pop(), pop_back(), pop_front(),
 *          emplace(), emplace_back(), emplace_front(),
 *          reserve(), shrink_to_fit(), reset() and clear().
 * @see https://en.wikipedia.org/wiki/Circular_buffer
 * @see https://github.com/torrentg/cqueue
 * @note This class is not thread-safe.
 * @version 1.0.0
 */
template<std::copyable T, typename Allocator = std::allocator<T>>
class cqueue {

  private: // declarations

    //! cqueue iterator.
    template<typename U>
    class iter {
      public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = U;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type *;
        using reference = value_type &;
      private:
        friend class iter<std::add_const_t<value_type>>;
        using size_type = std::size_t;
        using queue_type = std::conditional_t<std::is_const_v<value_type>, const cqueue, cqueue>;
      private:
        queue_type *queue = nullptr;
        difference_type pos = 0;
      private:
        auto cast(difference_type n) const {
          return (n < 0 ? queue->size() : static_cast<size_type>(n));
        }
        auto size() const {
          return static_cast<difference_type>(queue->size());
        }
        auto clamp(difference_type p) const {
          return std::clamp<difference_type>(p, -1, size());
        }
      public:
        explicit iter(queue_type *o, difference_type p = 0) : 
            queue{o}, pos{clamp(p)} {}
        iter(const iter<std::remove_const_t<value_type>> &other) requires std::is_const_v<value_type> : 
            queue{other.queue}, pos{other.pos} {}
        iter(const iter<value_type> &other) = default;
        iter& operator=(const iter& other) = default;
        reference operator*() {
            return queue->operator[](cast(pos));
        }
        pointer operator->() {
            return &(queue->operator[](cast(pos)));
        }
        reference operator[](difference_type rhs) const {
            return queue->operator[](cast(pos + rhs));
        }
        auto operator<=>(const iter &rhs) const {
            return (queue == rhs.queue ? pos <=> rhs.pos : std::partial_ordering::unordered);
        }
        bool operator==(const iter &rhs) const { return ((*this <=> rhs) == 0); }
        iter& operator++() { return *this += 1; }
        iter& operator--() { return *this += -1; }
        [[nodiscard]] iter operator++(int) { iter tmp{queue, pos}; ++*this; return tmp; }
        [[nodiscard]] iter operator--(int) { iter tmp{queue, pos}; --*this; return tmp; }
        auto& operator+=(difference_type rhs) { pos = clamp(pos + rhs); return *this; }
        auto& operator-=(difference_type rhs) { pos = clamp(pos - rhs); return *this; }
        auto operator+(difference_type rhs) const { return iter{queue, pos + rhs}; }
        auto operator-(difference_type rhs) const { return iter{queue, pos - rhs}; }
        friend iter operator+(difference_type lhs, const iter &rhs) { return iter{rhs.queue, lhs + rhs.pos}; }
        friend iter operator-(difference_type lhs, const iter &rhs) { return iter{rhs.queue, lhs - rhs.pos}; }
        auto operator-(const iter &rhs) const { return pos - rhs.pos; }
    };

  public: // declarations

    // Aliases  
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using const_pointer = const pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using allocator_type = Allocator;
    using const_alloc_reference = const allocator_type &;
    using allocator_traits = std::allocator_traits<allocator_type>;
    using iterator = iter<value_type>;
    using const_iterator = iter<const value_type>;

  private: // static members

    //! Capacity increase factor.
    static constexpr size_type GROWTH_FACTOR = 2;
    //! Default initial capacity (power of 2).
    static constexpr size_type MIN_ALLOCATE = 8;
    //! Maximum capacity.
    static constexpr size_type MAX_CAPACITY = std::numeric_limits<difference_type>::max();

  private: // members

    //! Memory allocator.
    [[no_unique_address]]
    allocator_type mAllocator = {};
    //! Buffer.
    pointer mData = nullptr;
    //! Buffer size.
    size_type mReserved = 0;
    //! Maximum number of elements (always > 0).
    size_type mCapacity = MAX_CAPACITY;
    //! Index representing first entry (0 <= mFront < mReserved).
    size_type mFront = 0;
    //! Number of entries in the queue (empty = 0, full = mReserved).
    size_type mLength = 0;

  private: // methods

    //! Convert from pos to index (throw exception if out-of-bounds).
    constexpr auto getCheckedIndex(size_type pos) const noexcept(false);
    //! Convert from pos to index.
    constexpr auto getUncheckedIndex(size_type pos) const noexcept;
    //! Compute memory size to reserve.
    constexpr auto getNewMemoryLength(size_type n) const noexcept;
    //! Resize buffer.
    constexpr void resizeIfRequired(size_type n);
    //! Resize buffer.
    void resize(size_type n);
    //! Clear and dealloc memory (preserve capacity and allocator).
    void reset() noexcept;

  public: // static methods

    //! Maximum capacity the container is able to hold.
    static constexpr auto max_capacity() noexcept { return MAX_CAPACITY; }

  public: // methods

    //! Constructor.
    constexpr explicit cqueue(const_alloc_reference alloc = Allocator()) : cqueue(0, alloc) {}
    //! Constructor (capacity=0 means unlimited).
    constexpr explicit cqueue(size_type capacity, const_alloc_reference alloc = Allocator());
    //! Copy constructor.
    constexpr cqueue(const cqueue &other) : 
        cqueue{other, allocator_traits::select_on_container_copy_construction(other.get_allocator())} {}
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
    constexpr allocator_type get_allocator() const noexcept { return mAllocator; }
    //! Return queue capacity.
    constexpr auto capacity() const noexcept { return (mCapacity == MAX_CAPACITY ? 0 : mCapacity); }
    //! Return the number of items.
    constexpr auto size() const noexcept { return mLength; }
    //! Current reserved size (numbers of items).
    constexpr auto reserved() const noexcept { return mReserved; }
    //! Check if there are items in the queue.
    [[nodiscard]] constexpr bool empty() const noexcept { return (mLength == 0); }

    //! Return the first element.
    constexpr const_reference front() const { return operator[](0); }
    //! Return the first element.
    constexpr reference front() { return operator[](0); }
    //! Return the last element.
    constexpr const_reference back() const { return operator[](mLength-1); }
    //! Return the last element.
    constexpr reference back() { return operator[](mLength-1); }

    //! Construct and insert an element at the end.
    template <class... Args>
    constexpr reference emplace_back(Args&&... args);
    //! Construct and insert an element at the front.
    template <class... Args>
    constexpr reference emplace_front(Args&&... args);

    //! Insert an element at the end.
    constexpr void push_back(const T &val);
    //! Insert an element at the end.
    constexpr void push_back(T &&val);
    //! Insert an element at the front.
    constexpr void push_front(const T &val);
    //! Insert an element at the front.
    constexpr void push_front(T &&val);

    //! Remove the front element.
    constexpr bool pop_front();
    //! Remove the back element.
    constexpr bool pop_back();

    //! Alias to emplace_back.
    template <class... Args>
    constexpr reference emplace(Args&&... args) { return emplace_back(std::forward<Args>(args)...); }
    //! Alias to push_back.
    constexpr void push(const T &val) { return push_back(val); }
    //! Alias to push_back.
    constexpr void push(T &&val) { return push_back(std::move(val)); }
    //! Alias to pop_front.
    constexpr bool pop() { return pop_front(); }

    //! Returns a reference to the element at position n.
    constexpr reference operator[](size_type n) { return mData[getCheckedIndex(n)]; }
    //! Returns a const reference to the element at position n.
    constexpr const_reference operator[](size_type n) const { return mData[getCheckedIndex(n)]; }

    //! Returns an iterator to the first element.
    constexpr iterator begin() noexcept { return iterator(this, 0); }
    //! Returns an iterator to the element following the last element.
    constexpr iterator end() noexcept { return iterator(this, static_cast<difference_type>(size())); }
    //! Returns a constant iterator to the first element.
    constexpr const_iterator begin() const noexcept { return const_iterator(this, 0); }
    //! Returns a constant iterator to the element following the last element.
    constexpr const_iterator end() const noexcept { return const_iterator(this, static_cast<difference_type>(size())); }
    //! Returns a constant iterator to the first element.
    constexpr const_iterator cbegin() const noexcept { return const_iterator(this, 0); }
    //! Returns a constant iterator to the element following the last element.
    constexpr const_iterator cend() const noexcept { return const_iterator(this, static_cast<difference_type>(size())); }

    //! Clear content.
    void clear() noexcept;
    //! Swap content.
    constexpr void swap (cqueue &x) noexcept;
    //! Ensure buffer size.
    constexpr void reserve(size_type n);
    //! Shrink reserved memory to current size.
    constexpr void shrink_to_fit();
};

} // namespace gto

/**
 * @param[in] capacity Container capacity.
 * @param[in] alloc Allocator to use.
 */
template<std::copyable T, typename Allocator>
constexpr gto::cqueue<T, Allocator>::cqueue(size_type capacity, const_alloc_reference alloc) :
    mAllocator(alloc)
{
  if (capacity > MAX_CAPACITY) {
    throw std::length_error("cqueue max capacity exceeded");
  } else {
    mCapacity = (capacity == 0 ? MAX_CAPACITY : capacity);
  }
}

/**
 * @param[in] other Queue to copy.
 * @param[in] alloc Allocator to use.
 */
template<std::copyable T, typename Allocator>
constexpr gto::cqueue<T, Allocator>::cqueue(const cqueue &other, const_alloc_reference alloc) : 
    mAllocator{alloc},
    mCapacity{other.mCapacity}
{
  resizeIfRequired(other.mLength);
  for (size_type i = 0; i < other.size(); ++i) {
    push_back(other[i]);
  }
}

/**
 * @param[in] other Queue to copy.
 * @param[in] alloc Allocator to use
 */
template<std::copyable T, typename Allocator>
constexpr gto::cqueue<T, Allocator>::cqueue(cqueue &&other, const_alloc_reference alloc) {
  if (alloc == other.mAllocator) {
    swap(other);
  } else {
    cqueue q{other, alloc};
    swap(q);
  }
}

/**
 * @param[in] other Queue to copy.
 */
template<std::copyable T, typename Allocator>
constexpr gto::cqueue<T, Allocator> & gto::cqueue<T, Allocator>::operator=(const cqueue &other) {
  cqueue tmp(other);
  this->swap(tmp);
  return *this;
}

/**
 * @param[in] num Element position.
 * @return Index in buffer.
 */
template<std::copyable T, typename Allocator>
constexpr auto gto::cqueue<T, Allocator>::getUncheckedIndex(size_type pos) const noexcept {
  return ((mFront + pos) % (mReserved == 0 ? 1 : mReserved));
}

/**
 * @param[in] num Element position.
 * @return Index in buffer.
 * @exception std::out_of_range Invalid position.
 */
template<std::copyable T, typename Allocator>
constexpr auto gto::cqueue<T, Allocator>::getCheckedIndex(size_type pos) const noexcept(false) {
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
    allocator_traits::destroy(mAllocator, mData + index);
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
  allocator_traits::deallocate(mAllocator, mData, mReserved);
  mData = nullptr;
  mReserved = 0;
}

/**
 * @details Swap content with another same-type cqueue.
 */
template<std::copyable T, typename Allocator>
constexpr void gto::cqueue<T, Allocator>::swap(cqueue &other) noexcept {
  if constexpr (allocator_traits::propagate_on_container_swap::value) {
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
constexpr auto gto::cqueue<T, Allocator>::getNewMemoryLength(size_type n) const noexcept {
  size_type ret = (mReserved == 0 ? std::min(mCapacity, MIN_ALLOCATE) : mReserved);
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
  if (n <= mReserved) {
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
  if (n <= mReserved) {
    return;
  } else if (n > mCapacity) {
    throw std::length_error("cqueue capacity exceeded");
  } else {
    resize(n);
  }
}

/**
 * @exception ... Error throwed by move contructors.
 */
template<std::copyable T, typename Allocator>
constexpr void gto::cqueue<T, Allocator>::shrink_to_fit() {
  if (mReserved == 0) {
    return;
  } if (mLength == 0) {
    reset();
  } else if (mLength == mReserved || mReserved <= MIN_ALLOCATE) {
    return;
  } else {
    resize(mLength);
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
  pointer tmp = allocator_traits::allocate(mAllocator, len);

  if constexpr (std::is_nothrow_move_constructible<T>::value) {
    // move elements from mData to tmp
    for (size_type i = 0; i < mLength; ++i) {
      size_type index = getUncheckedIndex(i);
      allocator_traits::construct(mAllocator, tmp + i, std::move(mData[index]));
    }
  }
  else {
    // copy elements from mData to tmp
    size_type i = 0;
    try {
      for (i = 0; i < mLength; ++i) {
        size_type index = getUncheckedIndex(i);
        allocator_traits::construct(mAllocator, tmp + i, mData[index]);
      }
    } catch (...) {
      while (i-- > 0) {
        allocator_traits::destroy(mAllocator, tmp + i);
      }
      allocator_traits::deallocate(mAllocator, tmp, len);
      throw;
    }
  }

  // destroy mData elements
  for (size_type j = 0; j < mLength; ++j) {
    size_type index = getUncheckedIndex(j);
    allocator_traits::destroy(mAllocator, mData + index);
  }

  // deallocate mData
  allocator_traits::deallocate(mAllocator, mData, mReserved);

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
constexpr void gto::cqueue<T, Allocator>::push_back(const T &val) {
  resizeIfRequired(mLength + 1);
  size_type index = getUncheckedIndex(mLength);
  allocator_traits::construct(mAllocator, mData + index, val);
  ++mLength;
}

/**
 * @param[in] val Value to add.
 * @exception std::length_error Number of values exceed queue capacity.
 */
template<std::copyable T, typename Allocator>
constexpr void gto::cqueue<T, Allocator>::push_back(T &&val) {
  resizeIfRequired(mLength + 1);
  size_type index = getUncheckedIndex(mLength);
  allocator_traits::construct(mAllocator, mData + index, std::move(val));
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
  allocator_traits::construct(mAllocator, mData + index, val);
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
  allocator_traits::construct(mAllocator, mData + index, std::move(val));
  mFront = index;
  ++mLength;
}

/**
 * @param[in] args Arguments of the new item.
 * @exception std::length_error Number of values exceed queue capacity.
 */
template<std::copyable T, typename Allocator>
template <class... Args>
constexpr typename gto::cqueue<T, Allocator>::reference gto::cqueue<T, Allocator>::emplace_back(Args&&... args) {
  resizeIfRequired(mLength + 1);
  size_type index = getUncheckedIndex(mLength);
  allocator_traits::construct(mAllocator, mData + index, std::forward<Args>(args)...);
  ++mLength;
  return mData[index];
}

/**
 * @param[in] args Arguments of the new item.
 * @exception std::length_error Number of values exceed queue capacity.
 */
template<std::copyable T, typename Allocator>
template <class... Args>
constexpr typename gto::cqueue<T, Allocator>::reference gto::cqueue<T, Allocator>::emplace_front(Args&&... args) {
  resizeIfRequired(mLength + 1);
  size_type index = (mLength == 0 ? 0 : (mFront == 0 ? mReserved : mFront) - 1);
  allocator_traits::construct(mAllocator, mData + index, std::forward<Args>(args)...);
  mFront = index;
  ++mLength;
  return mData[index];
}

/**
 * @return true = an element was erased, false = no elements in the queue.
 */
template<std::copyable T, typename Allocator>
constexpr bool gto::cqueue<T, Allocator>::pop_front() {
  if (mLength == 0) {
    return false;
  }
  allocator_traits::destroy(mAllocator, mData + mFront);
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
  allocator_traits::destroy(mAllocator, mData + index);
  --mLength;
  return true;
}
