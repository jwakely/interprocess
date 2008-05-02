//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2008. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////
//
// This file comes from SGI's stl_vector.h file. Modified by Ion Gaztanaga.
// Renaming, isolating and porting to generic algorithms. Pointer typedef 
// set to allocator::pointer to allow placing it in shared memory.
//
///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1994
// Hewlett-Packard Company
// 
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  Hewlett-Packard Company makes no
// representations about the suitability of this software for any
// purpose.  It is provided "as is" without express or implied warranty.
// 
// 
// Copyright (c) 1996
// Silicon Graphics Computer Systems, Inc.
// 
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without fee,
// provided that the above copyright notice appear in all copies and
// that both that copyright notice and this permission notice appear
// in supporting documentation.  Silicon Graphics makes no
// representations about the suitability of this software for any
// purpose.  It is provided "as is" without express or implied warranty.

#ifndef BOOST_INTERPROCESS_VECTOR_HPP
#define BOOST_INTERPROCESS_VECTOR_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>

#include <cstddef>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <utility>
#include <boost/detail/no_exceptions_support.hpp>
#include <boost/type_traits/has_trivial_destructor.hpp>
#include <boost/type_traits/has_trivial_copy.hpp>
#include <boost/type_traits/has_trivial_assign.hpp>
#include <boost/type_traits/has_nothrow_copy.hpp>
#include <boost/type_traits/has_nothrow_assign.hpp>
#include <boost/interprocess/detail/version_type.hpp>
#include <boost/interprocess/allocators/allocation_type.hpp>
#include <boost/interprocess/detail/utilities.hpp>
#include <boost/interprocess/detail/iterators.hpp>
#include <boost/interprocess/detail/algorithms.hpp>
#include <boost/interprocess/detail/min_max.hpp>
#include <boost/interprocess/interprocess_fwd.hpp>
#include <boost/interprocess/detail/move_iterator.hpp>
#include <boost/interprocess/detail/move.hpp>
#include <boost/interprocess/detail/mpl.hpp>

namespace boost {
namespace interprocess {

/// @cond

namespace detail {

//! Const vector_iterator used to iterate through a vector. 
template <class Pointer>
class vector_const_iterator
   : public std::iterator<std::random_access_iterator_tag
                          ,const typename std::iterator_traits<Pointer>::value_type
                          ,typename std::iterator_traits<Pointer>::difference_type
                          ,typename pointer_to_other
                              <Pointer
                              ,const typename std::iterator_traits<Pointer>::value_type
                              >::type
                          ,const typename std::iterator_traits<Pointer>::value_type &>
{
   public:
   typedef const typename std::iterator_traits<Pointer>::value_type  value_type;
   typedef typename std::iterator_traits<Pointer>::difference_type   difference_type;
   typedef typename pointer_to_other<Pointer, value_type>::type      pointer;
   typedef value_type&                                               reference;

   /// @cond
   protected:
   Pointer m_ptr;

   public:
   Pointer get_ptr() const    {  return   m_ptr;  }
   explicit vector_const_iterator(Pointer ptr)  : m_ptr(ptr){}
   /// @endcond

   public:

   //Constructors
   vector_const_iterator() : m_ptr(0){}

   //Pointer like operators
   reference operator*()   const  
   {  return *m_ptr;  }

   const value_type * operator->()  const  
   {  return  detail::get_pointer(m_ptr);  }

   reference operator[](difference_type off) const
   {  return m_ptr[off];   }

   //Increment / Decrement
   vector_const_iterator& operator++()       
   { ++m_ptr;  return *this; }

   vector_const_iterator operator++(int)      
   {  Pointer tmp = m_ptr; ++*this; return vector_const_iterator(tmp);  }

   vector_const_iterator& operator--()
   {  --m_ptr; return *this;  }

   vector_const_iterator operator--(int)
   {  Pointer tmp = m_ptr; --*this; return vector_const_iterator(tmp); }

   //Arithmetic
   vector_const_iterator& operator+=(difference_type off)
   {  m_ptr += off; return *this;   }

   vector_const_iterator operator+(difference_type off) const
   {  return vector_const_iterator(m_ptr+off);  }

   friend vector_const_iterator operator+(difference_type off, const vector_const_iterator& right)
   {  return vector_const_iterator(off + right.m_ptr); }

   vector_const_iterator& operator-=(difference_type off)
   {  m_ptr -= off; return *this;   }

   vector_const_iterator operator-(difference_type off) const
   {  return vector_const_iterator(m_ptr-off);  }

   difference_type operator-(const vector_const_iterator& right) const
   {  return m_ptr - right.m_ptr;   }

   //Comparison operators
   bool operator==   (const vector_const_iterator& r)  const
   {  return m_ptr == r.m_ptr;  }

   bool operator!=   (const vector_const_iterator& r)  const
   {  return m_ptr != r.m_ptr;  }

   bool operator<    (const vector_const_iterator& r)  const
   {  return m_ptr < r.m_ptr;  }

   bool operator<=   (const vector_const_iterator& r)  const
   {  return m_ptr <= r.m_ptr;  }

   bool operator>    (const vector_const_iterator& r)  const
   {  return m_ptr > r.m_ptr;  }

   bool operator>=   (const vector_const_iterator& r)  const
   {  return m_ptr >= r.m_ptr;  }
};

//! Iterator used to iterate through a vector
template <class Pointer>
class vector_iterator
   :  public vector_const_iterator<Pointer>
{
   public:
   explicit vector_iterator(Pointer ptr)
      : vector_const_iterator<Pointer>(ptr)
   {}

   public:
   typedef typename std::iterator_traits<Pointer>::value_type        value_type;
   typedef typename vector_const_iterator<Pointer>::difference_type  difference_type;
   typedef Pointer                                                   pointer;
   typedef value_type&                                               reference;

   //Constructors
   vector_iterator()
   {}

   //Pointer like operators
   reference operator*()  const  
   {  return *this->m_ptr;  }

   value_type* operator->() const  
   {  return  detail::get_pointer(this->m_ptr);  }

   reference operator[](difference_type off) const 
   {  return this->m_ptr[off];   }

   //Increment / Decrement
   vector_iterator& operator++()  
   {  ++this->m_ptr; return *this;  }

   vector_iterator operator++(int)
   {  pointer tmp = this->m_ptr; ++*this; return vector_iterator(tmp);  }
   
   vector_iterator& operator--()
   {  --this->m_ptr; return *this;  }

   vector_iterator operator--(int)
   {  vector_iterator tmp = *this; --*this; return vector_iterator(tmp); }

   // Arithmetic
   vector_iterator& operator+=(difference_type off)
   {  this->m_ptr += off;  return *this;  }

   vector_iterator operator+(difference_type off) const
   {  return vector_iterator(this->m_ptr+off);  }

   friend vector_iterator operator+(difference_type off, const vector_iterator& right)
   {  return vector_iterator(off + right.m_ptr); }

   vector_iterator& operator-=(difference_type off)
   {  this->m_ptr -= off; return *this;   }

   vector_iterator operator-(difference_type off) const
   {  return vector_iterator(this->m_ptr-off);  }

   difference_type operator-(const vector_const_iterator<Pointer>& right) const
   {  return static_cast<const vector_const_iterator<Pointer>&>(*this) - right;   }
};

//!This struct deallocates and allocated memory
template <class A>
struct vector_alloc_holder 
{
   typedef typename A::pointer      pointer;
   typedef typename A::size_type    size_type;
   typedef typename A::value_type   value_type;

   static const bool trivial_dctr = boost::has_trivial_destructor<value_type>::value;
   static const bool trivial_dctr_after_move = 
               has_trivial_destructor_after_move<value_type>::value || trivial_dctr;
   static const bool trivial_copy = has_trivial_copy<value_type>::value;
   static const bool nothrow_copy = has_nothrow_copy<value_type>::value;
   static const bool trivial_assign = has_trivial_assign<value_type>::value;
   static const bool nothrow_assign = has_nothrow_assign<value_type>::value;

   //Constructor, does not throw
   vector_alloc_holder(const A &a)
      : members_(a)
   {}

   //Constructor, does not throw
   vector_alloc_holder(const vector_alloc_holder<A> &h)
      : members_(h.alloc())
   {}

   //Destructor
   ~vector_alloc_holder()
   {  this->prot_deallocate(); }

   typedef detail::integral_constant<unsigned, 1>      allocator_v1;
   typedef detail::integral_constant<unsigned, 2>      allocator_v2;
   typedef detail::integral_constant<unsigned,
      boost::interprocess::detail::version<A>::value> alloc_version;
   std::pair<pointer, bool>
      allocation_command(allocation_type command,
                         size_type limit_size, 
                         size_type preferred_size,
                         size_type &received_size, const pointer &reuse = 0)
   {
      return allocation_command(command, limit_size, preferred_size,
                               received_size, reuse, alloc_version());
   }

   std::pair<pointer, bool>
      allocation_command(allocation_type command,
                         size_type limit_size, 
                         size_type preferred_size,
                         size_type &received_size,
                         const pointer &reuse,
                         allocator_v1)
   {
      (void)limit_size;
      (void)reuse;
      if(!(command & allocate_new))
         return std::pair<pointer, bool>(0, 0);
      received_size = preferred_size;
      return std::make_pair(this->alloc().allocate(received_size), false);
   }

   std::pair<pointer, bool>
      allocation_command(allocation_type command,
                         size_type limit_size, 
                         size_type preferred_size,
                         size_type &received_size,
                         const pointer &reuse,
                         allocator_v2)
   {
      return this->alloc().allocation_command
         (command, limit_size, preferred_size, received_size, reuse);
   }

   size_type next_capacity(size_type additional_objects) const
   {  return get_next_capacity(this->alloc().max_size(), this->members_.m_capacity, additional_objects);  }

   struct members_holder
      : public A
   {
      private:
      members_holder(const members_holder&);

      public:
      members_holder(const A &alloc)
         :  A(alloc), m_start(0), m_size(0), m_capacity(0)
      {}

      pointer     m_start;
      size_type   m_size;
      size_type   m_capacity;
   } members_;

   protected:
   void prot_deallocate()
   {
      if(!this->members_.m_capacity)   return;
      this->alloc().deallocate(this->members_.m_start, this->members_.m_capacity);
      this->members_.m_start     = 0;
      this->members_.m_size      = 0;
      this->members_.m_capacity  = 0;
   }

   void destroy(value_type* p)
   {
      if(!trivial_dctr)
         detail::get_pointer(p)->~value_type();
   }

   void destroy_n(value_type* p, size_type n)
   {
      if(!trivial_dctr)
         for(; n--; ++p)   p->~value_type();
   }

   A &alloc()
   {  return members_;  }

   const A &alloc() const
   {  return members_;  }
};

}  //namespace detail {
/// @endcond

//! A vector is a sequence that supports random access to elements, constant 
//! time insertion and removal of elements at the end, and linear time insertion 
//! and removal of elements at the beginning or in the middle. The number of 
//! elements in a vector may vary dynamically; memory management is automatic.
//! boost::interprocess::vector is similar to std::vector but it's compatible
//! with shared memory and memory mapped files.
template <class T, class A>
class vector : private detail::vector_alloc_holder<A>
{
   /// @cond
   typedef vector<T, A>                   self_t;
   typedef detail::vector_alloc_holder<A> base_t;
   /// @endcond
   public:
   //! The type of object, T, stored in the vector
   typedef T                                       value_type;
   //! Pointer to T
   typedef typename A::pointer                     pointer;
   //! Const pointer to T
   typedef typename A::const_pointer               const_pointer;
   //! Reference to T
   typedef typename A::reference                   reference;
   //! Const reference to T
   typedef typename A::const_reference             const_reference;
   //! An unsigned integral type
   typedef typename A::size_type                   size_type;
   //! A signed integral type
   typedef typename A::difference_type             difference_type;
   //! The allocator type
   typedef A                                       allocator_type;
   //! The random access iterator
   typedef detail::vector_iterator<pointer>        iterator;
   //! The random access const_iterator
   typedef detail::vector_const_iterator<pointer>  const_iterator;

   //! Iterator used to iterate backwards through a vector. 
   typedef std::reverse_iterator<iterator>   
      reverse_iterator;
   //! Const iterator used to iterate backwards through a vector. 
   typedef std::reverse_iterator<const_iterator>                 
      const_reverse_iterator;
   //! The stored allocator type
   typedef allocator_type                 stored_allocator_type;

   /// @cond
   private:

   typedef typename base_t::allocator_v1           allocator_v1;
   typedef typename base_t::allocator_v2           allocator_v2;
   typedef typename base_t::alloc_version          alloc_version;

   typedef constant_iterator<T, difference_type>   cvalue_iterator;
   typedef repeat_iterator<T, difference_type>     repeat_it;
   typedef detail::move_iterator<repeat_it>        repeat_move_it;
   //This is the anti-exception array destructor
   //to deallocate values already constructed
   typedef typename detail::if_c
      <base_t::trivial_dctr
      ,detail::null_scoped_destructor_n<allocator_type>
      ,detail::scoped_destructor_n<allocator_type>
      >::type   OldArrayDestructor;
   //This is the anti-exception array destructor
   //to destroy objects created with copy construction
   typedef typename detail::if_c
      <base_t::nothrow_copy
      ,detail::null_scoped_destructor_n<allocator_type>
      ,detail::scoped_destructor_n<allocator_type>
      >::type   UCopiedArrayDestructor;
   //This is the anti-exception array deallocator
   typedef typename detail::if_c
      <base_t::nothrow_copy
      ,detail::null_scoped_array_deallocator<allocator_type>
      ,detail::scoped_array_deallocator<allocator_type>
      >::type   UCopiedArrayDeallocator;

   //This is the optimized move iterator for copy constructors
   //so that std::copy and similar can use memcpy
   typedef typename detail::if_c
      <base_t::trivial_copy  || !is_movable<value_type>::value
      ,T*
      ,detail::move_iterator<T*>
      >::type   copy_move_it;

   //This is the optimized move iterator for assignments
   //so that std::uninitialized_copy and similar can use memcpy
   typedef typename detail::if_c
      <base_t::trivial_assign || !is_movable<value_type>::value
      ,T*
      ,detail::move_iterator<T*>
      >::type   assign_move_it;
   /// @endcond

   public:

   //! <b>Effects</b>: Constructs a vector taking the allocator as parameter.
   //! 
   //! <b>Throws</b>: If allocator_type's copy constructor throws.
   //! 
   //! <b>Complexity</b>: Constant.
   explicit vector(const A& a = A())
      : base_t(a)
   {}

   //! <b>Effects</b>: Constructs a vector that will use a copy of allocator a
   //!   and inserts n copies of value.
   //!
   //! <b>Throws</b>: If allocator_type's default constructor or copy constructor
   //!   throws or T's default or copy constructor throws.
   //! 
   //! <b>Complexity</b>: Linear to n.
   vector(size_type n, const T& value = T(),
          const allocator_type& a = allocator_type()) 
      :  base_t(a)
   {  this->insert(this->end(), n, value); }

   //! <b>Effects</b>: Copy constructs a vector.
   //!
   //! <b>Postcondition</b>: x == *this.
   //! 
   //! <b>Complexity</b>: Linear to the elements x contains.
   vector(const vector<T, A>& x) 
      :  base_t((base_t&)x)
   {  *this = x;  }

   //! <b>Effects</b>: Move constructor. Moves mx's resources to *this.
   //!
   //! <b>Throws</b>: If allocator_type's copy constructor throws.
   //! 
   //! <b>Complexity</b>: Constant.
   #ifndef BOOST_INTERPROCESS_RVALUE_REFERENCE
   vector(const detail::moved_object<vector<T, A> >& mx) 
      :  base_t(mx.get())
   {  this->swap(mx.get());   }
   #else
   vector(vector<T, A> && mx) 
      :  base_t(mx)
   {  this->swap(mx);   }
   #endif

   //! <b>Effects</b>: Constructs a vector that will use a copy of allocator a
   //!   and inserts a copy of the range [first, last) in the vector.
   //!
   //! <b>Throws</b>: If allocator_type's default constructor or copy constructor
   //!   throws or T's constructor taking an dereferenced InIt throws.
   //!
   //! <b>Complexity</b>: Linear to the range [first, last).
   template <class InIt>
   vector(InIt first, InIt last, const allocator_type& a = allocator_type())
      :  base_t(a)
   {  this->assign(first, last); }

   //! <b>Effects</b>: Destroys the vector. All stored values are destroyed
   //!   and used memory is deallocated.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Linear to the number of elements.
   ~vector() 
   {  this->priv_destroy_all();  }

   //! <b>Effects</b>: Returns an iterator to the first element contained in the vector.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   iterator begin()      
   { return iterator(this->members_.m_start); }

   //! <b>Effects</b>: Returns a const_iterator to the first element contained in the vector.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_iterator begin() const
   { return const_iterator(this->members_.m_start); }

   //! <b>Effects</b>: Returns an iterator to the end of the vector.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   iterator end()        
   { return iterator(this->members_.m_start + this->members_.m_size); }

   //! <b>Effects</b>: Returns a const_iterator to the end of the vector.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_iterator end()   const
   { return const_iterator(this->members_.m_start + this->members_.m_size); }

   //! <b>Effects</b>: Returns a reverse_iterator pointing to the beginning 
   //! of the reversed vector. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   reverse_iterator rbegin()     
   { return reverse_iterator(this->end());      }

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the beginning 
   //! of the reversed vector. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_reverse_iterator rbegin()const
   { return const_reverse_iterator(this->end());}

   //! <b>Effects</b>: Returns a reverse_iterator pointing to the end
   //! of the reversed vector. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   reverse_iterator rend()       
   { return reverse_iterator(this->begin());       }

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the end
   //! of the reversed vector. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_reverse_iterator rend()  const
   { return const_reverse_iterator(this->begin()); }

   //! <b>Requires</b>: !empty()
   //!
   //! <b>Effects</b>: Returns a reference to the first element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   reference         front()       
   { return *this->members_.m_start; }

   //! <b>Requires</b>: !empty()
   //!
   //! <b>Effects</b>: Returns a const reference to the first element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_reference   front() const 
   { return *this->members_.m_start; }

   //! <b>Requires</b>: !empty()
   //!
   //! <b>Effects</b>: Returns a reference to the first element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   reference         back()        
   { return this->members_.m_start[this->members_.m_size - 1]; }

   //! <b>Requires</b>: !empty()
   //!
   //! <b>Effects</b>: Returns a const reference to the first element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_reference   back()  const 
   { return this->members_.m_start[this->members_.m_size - 1]; }

   //! <b>Effects</b>: Returns the number of the elements contained in the vector.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   size_type size() const 
   { return this->members_.m_size; }

   //! <b>Effects</b>: Returns the largest possible size of the vector.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   size_type max_size() const 
   { return this->alloc().max_size(); }

   //! <b>Effects</b>: Number of elements for which memory has been allocated.
   //!   capacity() is always greater than or equal to size().
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   size_type capacity() const 
   { return this->members_.m_capacity; }

   //! <b>Effects</b>: Returns true if the vector contains no elements.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   bool empty() const 
   { return !this->members_.m_size; }

   //! <b>Requires</b>: size() < n.
   //!
   //! <b>Effects</b>: Returns a reference to the nth element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   reference operator[](size_type n)         
   { return this->members_.m_start[n]; }

   //! <b>Requires</b>: size() < n.
   //!
   //! <b>Effects</b>: Returns a const reference to the nth element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_reference operator[](size_type n) const   
   { return this->members_.m_start[n]; }

   //! <b>Requires</b>: size() < n.
   //!
   //! <b>Effects</b>: Returns a reference to the nth element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: std::range_error if n >= size()
   //! 
   //! <b>Complexity</b>: Constant.
   reference at(size_type n)
   { this->priv_check_range(n); return this->members_.m_start[n]; }

   //! <b>Requires</b>: size() < n.
   //!
   //! <b>Effects</b>: Returns a const reference to the nth element 
   //!   from the beginning of the container.
   //! 
   //! <b>Throws</b>: std::range_error if n >= size()
   //! 
   //! <b>Complexity</b>: Constant.
   const_reference at(size_type n) const
   { this->priv_check_range(n); return this->members_.m_start[n]; }

   //! <b>Effects</b>: Returns a copy of the internal allocator.
   //! 
   //! <b>Throws</b>: If allocator's copy constructor throws.
   //! 
   //! <b>Complexity</b>: Constant.
   allocator_type get_allocator() const 
   { return this->alloc();  }

   const stored_allocator_type &get_stored_allocator() const 
   {  return this->alloc(); }

   stored_allocator_type &get_stored_allocator()
   {  return this->alloc(); }

   //! <b>Effects</b>: If n is less than or equal to capacity(), this call has no
   //!   effect. Otherwise, it is a request for allocation of additional memory.
   //!   If the request is successful, then capacity() is greater than or equal to
   //!   n; otherwise, capacity() is unchanged. In either case, size() is unchanged.
   //! 
   //! <b>Throws</b>: If memory allocation allocation throws or T's copy constructor throws.
   void reserve(size_type new_cap)
   {
      if (this->capacity() < new_cap){
         //There is not enough memory, allocate a new
         //buffer or expand the old one.
         bool same_buffer_start;
         size_type real_cap = 0;
         std::pair<pointer, bool> ret =
            this->allocation_command
               (allocate_new | expand_fwd | expand_bwd,
                  new_cap, new_cap, real_cap, this->members_.m_start);

         //Check for forward expansion
         same_buffer_start = ret.second && this->members_.m_start == ret.first;
         if(same_buffer_start){
            #ifdef BOOST_INTERPROCESS_VECTOR_ALLOC_STATS
            ++this->num_expand_fwd;
            #endif
            this->members_.m_capacity  = real_cap;
         }
         //If there is no forward expansion, move objects
         else{
            //We will reuse insert code, so create a dummy input iterator
            copy_move_it dummy_it(detail::get_pointer(this->members_.m_start));
            //Backwards (and possibly forward) expansion
            if(ret.second){
               #ifdef BOOST_INTERPROCESS_VECTOR_ALLOC_STATS
               ++this->num_expand_bwd;
               #endif
               this->priv_range_insert_expand_backwards
                  ( detail::get_pointer(ret.first)
                  , real_cap
                  , detail::get_pointer(this->members_.m_start)
                  , dummy_it
                  , dummy_it
                  , 0);
            }
            //New buffer
            else{
               #ifdef BOOST_INTERPROCESS_VECTOR_ALLOC_STATS
               ++this->num_alloc;
               #endif
               this->priv_range_insert_new_allocation
                  ( detail::get_pointer(ret.first)
                  , real_cap
                  , detail::get_pointer(this->members_.m_start)
                  , dummy_it
                  , dummy_it);
            }
         }
      }
   }

   //! <b>Effects</b>: Makes *this contain the same elements as x.
   //!
   //! <b>Postcondition</b>: this->size() == x.size(). *this contains a copy 
   //! of each of x's elements. 
   //!
   //! <b>Throws</b>: If memory allocation throws or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to the number of elements in x.
   vector<T, A>& operator=(const vector<T, A>& x)
   {
      if (&x != this){
         this->assign(x.members_.m_start, x.members_.m_start + x.members_.m_size);
      }
      return *this;
   }

   //! <b>Effects</b>: Move assignment. All mx's values are transferred to *this.
   //!
   //! <b>Postcondition</b>: x.empty(). *this contains a the elements x had
   //!   before the function.
   //!
   //! <b>Throws</b>: If allocator_type's copy constructor throws.
   //!
   //! <b>Complexity</b>: Constant.
   #ifndef BOOST_INTERPROCESS_RVALUE_REFERENCE
   vector<T, A>& operator=(const detail::moved_object<vector<T, A> >& mx)
   {
      vector<T, A> &x = mx.get();

      if (&x != this){
         this->swap(x);
         x.clear();
      }
      return *this;
   }
   #else
   vector<T, A>& operator=(vector<T, A> && mx)
   {
      vector<T, A> &x = mx;

      if (&x != this){
         this->swap(x);
         x.clear();
      }
      return *this;
   }
   #endif

   //! <b>Effects</b>: Assigns the n copies of val to *this.
   //!
   //! <b>Throws</b>: If memory allocation throws or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to n.
   void assign(size_type n, const value_type& val)
   {  this->assign(cvalue_iterator(val, n), cvalue_iterator());   }

   //! <b>Effects</b>: Assigns the the range [first, last) to *this.
   //!
   //! <b>Throws</b>: If memory allocation throws or
   //!   T's constructor from dereferencing InpIt throws.
   //!
   //! <b>Complexity</b>: Linear to n.
   template <class InIt>
   void assign(InIt first, InIt last) 
   {
      //Dispatch depending on integer/iterator
      const bool aux_boolean = detail::is_convertible<InIt, std::size_t>::value;
      typedef detail::bool_<aux_boolean> Result;
      this->priv_assign_dispatch(first, last, Result());
   }

   //! <b>Effects</b>: Inserts a copy of x at the end of the vector.
   //!
   //! <b>Throws</b>: If memory allocation throws or
   //!   T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Amortized constant time.
   void push_back(const T& x) 
   {
      if (this->members_.m_size < this->members_.m_capacity){
         //There is more memory, just construct a new object at the end
         new((void*)(detail::get_pointer(this->members_.m_start) + this->members_.m_size))value_type(x);
         ++this->members_.m_size;
      }
      else{
         this->insert(this->end(), x);
      }
   }

   //! <b>Effects</b>: Constructs a new element in the end of the vector
   //!   and moves the resources of mx to this new element.
   //!
   //! <b>Throws</b>: If memory allocation throws.
   //!
   //! <b>Complexity</b>: Amortized constant time.
   #ifndef BOOST_INTERPROCESS_RVALUE_REFERENCE
   void push_back(const detail::moved_object<T> & mx) 
   {
      if (this->members_.m_size < this->members_.m_capacity){
         //There is more memory, just construct a new object at the end
         new((void*)detail::get_pointer(this->members_.m_start + this->members_.m_size))value_type(mx);
         ++this->members_.m_size;
      }
      else{
         this->insert(this->end(), mx);
      }
   }
   #else
   void push_back(T && mx) 
   {
      if (this->members_.m_size < this->members_.m_capacity){
         //There is more memory, just construct a new object at the end
         new((void*)detail::get_pointer(this->members_.m_start + this->members_.m_size))value_type(move(mx));
         ++this->members_.m_size;
      }
      else{
         this->insert(this->end(), move(mx));
      }
   }
   #endif
  
   //! <b>Effects</b>: Swaps the contents of *this and x.
   //!   If this->allocator_type() != x.allocator_type()
   //!   allocators are also swapped.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   void swap(vector<T, A>& x) 
   {
      allocator_type &this_al = this->alloc(), &other_al = x.alloc();
      //Just swap internals
      detail::do_swap(this->members_.m_start, x.members_.m_start);
      detail::do_swap(this->members_.m_size, x.members_.m_size);
      detail::do_swap(this->members_.m_capacity, x.members_.m_capacity);

      if (this_al != other_al){
         detail::do_swap(this_al, other_al);
      }
   }

   //! <b>Effects</b>: Swaps the contents of *this and x.
   //!   If this->allocator_type() != x.allocator_type()
   //!   allocators are also swapped.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant.
   #ifndef BOOST_INTERPROCESS_RVALUE_REFERENCE
   void swap(const detail::moved_object<vector<T, A> >& mx) 
   {
      vector<T, A> &x = mx.get();
      this->swap(x);
   }
   #else
   void swap(vector<T, A> && mx) 
   {
      vector<T, A> &x = mx;
      this->swap(x);
   }
   #endif

   //! <b>Requires</b>: position must be a valid iterator of *this.
   //!
   //! <b>Effects</b>: Insert a copy of x before position.
   //!
   //! <b>Throws</b>: If memory allocation throws or x's copy constructor throws.
   //!
   //! <b>Complexity</b>: If position is begin() or end(), amortized constant time
   //!   Linear time otherwise.
   iterator insert(iterator position, const T& x) 
   {
      //Just call more general insert(pos, size, value) and return iterator
      size_type n = position - begin();
      this->insert(position, (size_type)1, x);
      return iterator(this->members_.m_start + n);
   }

   //! <b>Requires</b>: position must be a valid iterator of *this.
   //!
   //! <b>Effects</b>: Insert a new element before position with mx's resources.
   //!
   //! <b>Throws</b>: If memory allocation throws.
   //!
   //! <b>Complexity</b>: If position is begin() or end(), amortized constant time
   //!   Linear time otherwise.
   #ifndef BOOST_INTERPROCESS_RVALUE_REFERENCE
   iterator insert(iterator position, const detail::moved_object<T> &mx) 
   {
      //Just call more general insert(pos, size, value) and return iterator
      size_type n = position - begin();
      this->insert(position
                  ,repeat_move_it(repeat_it(mx.get(), 1))
                  ,repeat_move_it(repeat_it()));
      return iterator(this->members_.m_start + n);
   }
   #else
   iterator insert(iterator position, T &&mx) 
   {
      //Just call more general insert(pos, size, value) and return iterator
      size_type n = position - begin();
      this->insert(position
                  ,repeat_move_it(repeat_it(mx, 1))
                  ,repeat_move_it(repeat_it()));
      return iterator(this->members_.m_start + n);
   }
   #endif

   //! <b>Requires</b>: pos must be a valid iterator of *this.
   //!
   //! <b>Effects</b>: Insert a copy of the [first, last) range before pos.
   //!
   //! <b>Throws</b>: If memory allocation throws, T's constructor from a
   //!   dereferenced InpIt throws or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to std::distance [first, last).
   template <class InIt>
   void insert(iterator pos, InIt first, InIt last)
   {
      //Dispatch depending on integer/iterator
      const bool aux_boolean = detail::is_convertible<InIt, std::size_t>::value;
      typedef detail::bool_<aux_boolean> Result;
      this->priv_insert_dispatch(pos, first, last, Result());
   }

   //! <b>Requires</b>: pos must be a valid iterator of *this.
   //!
   //! <b>Effects</b>: Insert n copies of x before pos.
   //!
   //! <b>Throws</b>: If memory allocation throws or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to n.
   void insert (iterator p, size_type n, const T& x)
   {  this->insert(p, cvalue_iterator(x, n), cvalue_iterator()); }

   //! <b>Effects</b>: Removes the last element from the vector.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Constant time.
   void pop_back() 
   {
      //Destroy last element
      --this->members_.m_size;
      this->destroy(detail::get_pointer(this->members_.m_start) + this->members_.m_size);
   }

   //! <b>Effects</b>: Erases the element at position pos.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Linear to the elements between pos and the 
   //!   last element. Constant if pos is the first or the last element.
   iterator erase(const_iterator position) 
   {
      T *pos = detail::get_pointer(position.get_ptr());
      T *beg = detail::get_pointer(this->members_.m_start);

		std::copy(assign_move_it(pos + 1), assign_move_it(beg + this->members_.m_size), pos);
      --this->members_.m_size;
      //Destroy last element
      base_t::destroy(detail::get_pointer(this->members_.m_start) + this->members_.m_size);
      return iterator(position.get_ptr());
   }

   //! <b>Effects</b>: Erases the elements pointed by [first, last).
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Linear to the distance between first and last.
   iterator erase(const_iterator first, const_iterator last) 
   {
		if (first != last){	// worth doing, copy down over hole
         T* end_pos = detail::get_pointer(this->members_.m_start) + this->members_.m_size;
         T* ptr = detail::get_pointer(std::copy
            (assign_move_it(detail::get_pointer(last.get_ptr()))
            ,assign_move_it(end_pos)
				,detail::get_pointer(first.get_ptr())
            ));
         size_type destroyed = (end_pos - ptr);
         this->destroy_n(ptr, destroyed);
         this->members_.m_size -= destroyed;
      }
      return iterator(first.get_ptr());
   }

   //! <b>Effects</b>: Inserts or erases elements at the end such that
   //!   the size becomes n. New elements are copy constructed from x.
   //!
   //! <b>Throws</b>: If memory allocation throws, or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to the difference between size() and new_size.
   void resize(size_type new_size, const T& x) 
   {
      pointer finish = this->members_.m_start + this->members_.m_size;
      if (new_size < size()){
         //Destroy last elements
         this->erase(iterator(this->members_.m_start + new_size), this->end());
      }
      else{
         //Insert new elements at the end
         this->insert(iterator(finish), new_size - this->size(), x);
      }
   }

   //! <b>Effects</b>: Inserts or erases elements at the end such that
   //!   the size becomes n. New elements are default constructed.
   //!
   //! <b>Throws</b>: If memory allocation throws, or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to the difference between size() and new_size.
   void resize(size_type new_size) 
   {
      if (new_size < this->size()){
         //Destroy last elements
         this->erase(iterator(this->members_.m_start + new_size), this->end());
      }
      else{
         size_type n = new_size - this->size();
         this->reserve(new_size);
         T *ptr = detail::get_pointer(this->members_.m_start + this->members_.m_size);
         while(n--){
            //Default construct
            new((void*)ptr++)T();
            ++this->members_.m_size;
         }
      }
   }

   //! <b>Effects</b>: Erases all the elements of the vector.
   //!
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Linear to the number of elements in the vector.
   void clear() 
   {  this->priv_destroy_all();  }

   /// @cond

   //! <b>Effects</b>: Tries to deallocate the excess of memory created
   //!   with previous allocations. The size of the vector is unchanged
   //!
   //! <b>Throws</b>: If memory allocation throws, or T's copy constructor throws.
   //!
   //! <b>Complexity</b>: Linear to size().
   void shrink_to_fit()
   {  priv_shrink_to_fit(alloc_version());   }

   private:
   void priv_shrink_to_fit(allocator_v1)
   {
      if(this->members_.m_capacity){
         if(!size()){
            this->prot_deallocate();
         }
         else{
            //This would not work with stateful allocators
            vector<T, A>(*this).swap(*this);
         }
      }
   }

   void priv_shrink_to_fit(allocator_v2)
   {
      if(this->members_.m_capacity){
         if(!size()){
            this->prot_deallocate();
         }
         else{
            size_type received_size;
            this->alloc().allocation_command(shrink_in_place, this->size(), this->capacity()
                                      ,received_size, this->members_.m_start);
         }
      }
   }

   void priv_destroy_all()
   {
      this->destroy_n(detail::get_pointer(this->members_.m_start), this->members_.m_size);
      this->members_.m_size = 0;
   }

   template <class FwdIt>
   void priv_range_insert(pointer pos,     FwdIt first, 
                          FwdIt last, std::forward_iterator_tag)
   {
      if (first != last){
         size_type n = std::distance(first, last);
         //Check if we have enough memory or try to expand current memory
         size_type remaining = this->members_.m_capacity - this->members_.m_size;
         bool same_buffer_start;
         std::pair<pointer, bool> ret;
         size_type real_cap = this->members_.m_capacity;

         //Check if we already have room
         if (n <= remaining){
            same_buffer_start = true;
         }
         else{
            //There is not enough memory, allocate a new
            //buffer or expand the old one.
            size_type new_cap = this->next_capacity(n);
            ret = this->allocation_command
                  (allocate_new | expand_fwd | expand_bwd,
                   this->members_.m_size + n, new_cap, real_cap, this->members_.m_start);

            //Check for forward expansion
            same_buffer_start = ret.second && this->members_.m_start == ret.first;
            if(same_buffer_start){
               this->members_.m_capacity  = real_cap;
            }
         }
         
         //If we had room or we have expanded forward
         if (same_buffer_start){
            #ifdef BOOST_INTERPROCESS_VECTOR_ALLOC_STATS
            ++this->num_expand_fwd;
            #endif
            this->priv_range_insert_expand_forward
               (detail::get_pointer(pos), first, last, n);
         }
         //Backwards (and possibly forward) expansion
         else if(ret.second){
            #ifdef BOOST_INTERPROCESS_VECTOR_ALLOC_STATS
            ++this->num_expand_bwd;
            #endif
            this->priv_range_insert_expand_backwards
               ( detail::get_pointer(ret.first)
               , real_cap
               , detail::get_pointer(pos)
               , first
               , last
               , n);
         }
         //New buffer
         else{
            #ifdef BOOST_INTERPROCESS_VECTOR_ALLOC_STATS
            ++this->num_alloc;
            #endif
            this->priv_range_insert_new_allocation
               ( detail::get_pointer(ret.first)
               , real_cap
               , detail::get_pointer(pos)
               , first
               , last);
         }
      }
   }

   template <class FwdIt>
   void priv_range_insert_expand_forward
         (T* pos, FwdIt first, FwdIt last, size_type n)
   {
      //There is enough memory
      T* old_finish = detail::get_pointer(this->members_.m_start) + this->members_.m_size;
      const size_type elems_after = old_finish - pos;

      if (elems_after > n){
         //New elements can be just copied.
         //Move to uninitialized memory last objects
         std::uninitialized_copy(copy_move_it(old_finish - n), copy_move_it(old_finish), old_finish);
         this->members_.m_size += n;
         //Copy previous to last objects to the initialized end
         std::copy_backward(assign_move_it(pos), assign_move_it(old_finish - n), old_finish);
         //Insert new objects in the pos
         std::copy(first, last, pos);
      }
      else {
         //The new elements don't fit in the [pos, end()) range. Copy
         //to the beginning of the unallocated zone the last new elements.
         FwdIt mid = first;
         std::advance(mid, elems_after);
         std::uninitialized_copy(mid, last, old_finish);
         this->members_.m_size += n - elems_after;
         //Copy old [pos, end()) elements to the uninitialized memory
         std::uninitialized_copy
            ( copy_move_it(pos)
            , copy_move_it(old_finish)
            , detail::get_pointer(this->members_.m_start) + this->members_.m_size);
         this->members_.m_size += elems_after;
         //Copy first new elements in pos
         std::copy(first, mid, pos);
      }
   }

   template <class FwdIt>
   void priv_range_insert_new_allocation
      (T* new_start, size_type new_cap, T* pos, FwdIt first, FwdIt last)
   {
      T* new_finish = new_start;
      T *old_finish;
      //Anti-exception rollbacks
      UCopiedArrayDeallocator scoped_alloc(new_start, this->alloc(), new_cap);
      UCopiedArrayDestructor construted_values_destroyer(new_start, 0u);

      //Initialize with [begin(), pos) old buffer 
      //the start of the new buffer
      new_finish = std::uninitialized_copy
         ( copy_move_it(detail::get_pointer(this->members_.m_start))
         , copy_move_it(pos)
         , old_finish = new_finish);
      construted_values_destroyer.increment_size(new_finish - old_finish);
      //Initialize new objects, starting from previous point
      new_finish = std::uninitialized_copy
         (first, last, old_finish = new_finish);
      construted_values_destroyer.increment_size(new_finish - old_finish);
      //Initialize from the rest of the old buffer, 
      //starting from previous point
      new_finish = std::uninitialized_copy
         ( copy_move_it(pos)
         , copy_move_it(detail::get_pointer(this->members_.m_start) + this->members_.m_size)
         , new_finish);

      //All construction successful, disable rollbacks
      construted_values_destroyer.release();
      scoped_alloc.release();
      //Destroy and deallocate old elements
      //If there is allocated memory, destroy and deallocate
      if(this->members_.m_start != 0){
		   if(!base_t::trivial_dctr_after_move)
			   this->destroy_n(detail::get_pointer(this->members_.m_start), this->members_.m_size); 
         this->alloc().deallocate(this->members_.m_start, this->members_.m_capacity);
      }
      this->members_.m_start     = new_start;
      this->members_.m_size      = new_finish - new_start;
      this->members_.m_capacity  = new_cap;
   }

   template <class FwdIt>
   void priv_range_insert_expand_backwards
         (T* new_start, size_type new_capacity,
          T* pos, FwdIt first, FwdIt last, size_type n)
   {
      //Backup old data
      T* old_start  = detail::get_pointer(this->members_.m_start);
      T* old_finish = old_start + this->members_.m_size;
      size_type old_size = this->members_.m_size;

      //We can have 8 possibilities:
      const size_type elemsbefore   = (size_type)(pos - old_start);
      const size_type s_before      = (size_type)(old_start - new_start);

      //Update the vector buffer information to a safe state
      this->members_.m_start      = new_start;
      this->members_.m_capacity   = new_capacity;
      this->members_.m_size = 0;

      //If anything goes wrong, this object will destroy
      //all the old objects to fulfill previous vector state
      OldArrayDestructor old_values_destroyer(old_start, old_size);

      //Check if s_before is so big that even copying the old data + new data
      //there is a gap between the new data and the old data
      if(s_before >= (old_size + n)){
         //Old situation:
         // _________________________________________________________
         //|            raw_mem                | old_begin | old_end |
         //| __________________________________|___________|_________|
         //
         //New situation:
         // _________________________________________________________
         //| old_begin |    new   | old_end |         raw_mem        |
         //|___________|__________|_________|________________________|
         //
         //Copy first old values before pos, after that the 
         //new objects
         boost::interprocess::uninitialized_copy_copy
            (copy_move_it(old_start), copy_move_it(pos), first, last, new_start);
         UCopiedArrayDestructor new_values_destroyer(new_start, elemsbefore);
         //Now initialize the rest of memory with the last old values
         std::uninitialized_copy
            (copy_move_it(pos), copy_move_it(old_finish), new_start + elemsbefore + n);
         //All new elements correctly constructed, avoid new element destruction
         new_values_destroyer.release();
         this->members_.m_size = old_size + n;
         //Old values destroyed automatically with "old_values_destroyer"
         //when "old_values_destroyer" goes out of scope unless the have trivial
         //destructor after move.
         if(base_t::trivial_dctr_after_move)
            old_values_destroyer.release();
      }
      //Check if s_before is so big that divides old_end
      else if(difference_type(s_before) >= difference_type(elemsbefore + n)){
         //Old situation:
         // __________________________________________________
         //|            raw_mem         | old_begin | old_end |
         //| ___________________________|___________|_________|
         //
         //New situation:
         // __________________________________________________
         //| old_begin |   new    | old_end |  raw_mem        |
         //|___________|__________|_________|_________________|
         //
         //Copy first old values before pos, after that the 
         //new objects
         boost::interprocess::uninitialized_copy_copy
            (copy_move_it(old_start), copy_move_it(pos), first, last, new_start);
         UCopiedArrayDestructor new_values_destroyer(new_start, elemsbefore);
         size_type raw_gap = s_before - (elemsbefore + n);
         //Now initialize the rest of s_before memory with the
         //first of elements after new values
         std::uninitialized_copy
            (copy_move_it(pos), copy_move_it(pos + raw_gap), new_start + elemsbefore + n);
         //All new elements correctly constructed, avoid new element destruction
         new_values_destroyer.release();
         //All new elements correctly constructed, avoid old element destruction
         old_values_destroyer.release();
         //Update size since we have a contiguous buffer
         this->members_.m_size = old_size + s_before;
         //Now copy remaining last objects in the old buffer begin
         T *to_destroy = std::copy(assign_move_it(pos + raw_gap), assign_move_it(old_finish), old_start);
         //Now destroy redundant elements except if they were moved and
         //they have trivial destructor after move
         size_type n_destroy =  old_finish - to_destroy;
         if(!base_t::trivial_dctr_after_move)
            this->destroy_n(to_destroy, n_destroy);
         this->members_.m_size -= n_destroy;
      }
      else{
         //Check if we have to do the insertion in two phases
         //since maybe s_before is not big enough and
         //the buffer was expanded both sides
         //
         //Old situation:
         // _________________________________________________
         //| raw_mem | old_begin + old_end |  raw_mem        |
         //|_________|_____________________|_________________|
         //
         //New situation with do_after:
         // _________________________________________________
         //|     old_begin + new + old_end     |  raw_mem    |
         //|___________________________________|_____________|
         //
         //New without do_after:
         // _________________________________________________
         //| old_begin + new + old_end  |  raw_mem           |
         //|____________________________|____________________|
         //
         bool do_after    = n > s_before;
         FwdIt before_end = first;
         //If we have to expand both sides,
         //we will play if the first new values so
         //calculate the upper bound of new values
         if(do_after){
            std::advance(before_end, s_before);
         }

         //Now we can have two situations: the raw_mem of the
         //beginning divides the old_begin, or the new elements:
         if (s_before <= elemsbefore) {
            //The raw memory divides the old_begin group:
            //
            //If we need two phase construction (do_after)
            //new group is divided in new = new_beg + new_end groups
            //In this phase only new_beg will be inserted
            //
            //Old situation:
            // _________________________________________________
            //| raw_mem | old_begin | old_end |  raw_mem        |
            //|_________|___________|_________|_________________|
            //
            //New situation with do_after(1):
            //This is not definitive situation, the second phase
            //will include
            // _________________________________________________
            //| old_begin | new_beg | old_end |  raw_mem        |
            //|___________|_________|_________|_________________|
            //
            //New situation without do_after:
            // _________________________________________________
            //| old_begin | new | old_end |  raw_mem            |
            //|___________|_____|_________|_____________________|
            //
            //Copy the first part of old_begin to raw_mem
            T *start_n = old_start + difference_type(s_before); 
            std::uninitialized_copy
               (copy_move_it(old_start), copy_move_it(start_n), new_start);
            //The buffer is all constructed until old_end,
            //release destroyer and update size
            old_values_destroyer.release();
            this->members_.m_size = old_size + s_before;
            //Now copy the second part of old_begin overwriting himself
            T* next = std::copy(assign_move_it(start_n), assign_move_it(pos), old_start);
            if(do_after){
               //Now copy the new_beg elements
               std::copy(first, before_end, next);
            }
            else{
               //Now copy the all the new elements
               T* move_start = std::copy(first, last, next);
               //Now displace old_end elements
               T* move_end   = std::copy(assign_move_it(pos), assign_move_it(old_finish), move_start);
               //Destroy remaining moved elements from old_end except if
               //they have trivial destructor after being moved
               difference_type n_destroy = s_before - n;
               if(!base_t::trivial_dctr_after_move)
                  this->destroy_n(move_end, n_destroy);
               this->members_.m_size -= n_destroy;
            }
         }
         else {
            //The raw memory divides the new elements
            //
            //If we need two phase construction (do_after)
            //new group is divided in new = new_beg + new_end groups
            //In this phase only new_beg will be inserted
            //
            //Old situation:
            // _______________________________________________________
            //|   raw_mem     | old_begin | old_end |  raw_mem        |
            //|_______________|___________|_________|_________________|
            //
            //New situation with do_after():
            // ____________________________________________________
            //| old_begin |    new_beg    | old_end |  raw_mem     |
            //|___________|_______________|_________|______________|
            //
            //New situation without do_after:
            // ______________________________________________________
            //| old_begin | new | old_end |  raw_mem                 |
            //|___________|_____|_________|__________________________|
            //
            //First copy whole old_begin and part of new to raw_mem
            FwdIt mid = first;
            size_type n_new_init = difference_type(s_before) - elemsbefore;
            std::advance(mid, n_new_init);
            boost::interprocess::uninitialized_copy_copy
               (copy_move_it(old_start), copy_move_it(pos), first, mid, new_start);
            //The buffer is all constructed until old_end,
            //release destroyer and update size
            old_values_destroyer.release();
            this->members_.m_size = old_size + s_before;

            if(do_after){
               //Copy new_beg part
               std::copy(mid, before_end, old_start);
            }
            else{
               //Copy all new elements
               T* move_start = std::copy(mid, last, old_start);
               //Displace old_end
               T* move_end   = std::copy(copy_move_it(pos), copy_move_it(old_finish), move_start);
               //Destroy remaining moved elements from old_end except if they
               //have trivial destructor after being moved
               difference_type n_destroy = s_before - n;
               if(!base_t::trivial_dctr_after_move)
                  this->destroy_n(move_end, n_destroy);
               this->members_.m_size -= n_destroy;
            }
         }

         //This is only executed if two phase construction is needed
         //This can be executed without exception handling since we
         //have to just copy and append in raw memory and
         //old_values_destroyer has been released in phase 1.
         if(do_after){
            //The raw memory divides the new elements
            //
            //Old situation:
            // ______________________________________________________
            //|   raw_mem    | old_begin |  old_end   |  raw_mem     |
            //|______________|___________|____________|______________|
            //
            //New situation with do_after(1):
            // _______________________________________________________
            //| old_begin   +   new_beg  | new_end |old_end | raw_mem |
            //|__________________________|_________|________|_________|
            //
            //New situation with do_after(2):
            // ______________________________________________________
            //| old_begin      +       new            | old_end |raw |
            //|_______________________________________|_________|____|
            //
            const size_type n_after  = n - s_before;
            const difference_type elemsafter = old_size - elemsbefore;

            //The new_end part is [first + (n - n_after), last)
            std::advance(first, n - n_after);

            //We can have two situations:
            if (elemsafter > difference_type(n_after)){
               //The raw_mem from end will divide displaced old_end
               //
               //Old situation:
               // ______________________________________________________
               //|   raw_mem    | old_begin |  old_end   |  raw_mem     |
               //|______________|___________|____________|______________|
               //
               //New situation with do_after(1):
               // _______________________________________________________
               //| old_begin   +   new_beg  | new_end |old_end | raw_mem |
               //|__________________________|_________|________|_________|
               //
               //First copy the part of old_end raw_mem
               T* finish_n = old_finish - difference_type(n_after);
               std::uninitialized_copy
                  (copy_move_it(finish_n), copy_move_it(old_finish), old_finish);
               this->members_.m_size += n_after;
               //Displace the rest of old_end to the new position
               std::copy_backward(assign_move_it(pos), assign_move_it(finish_n), old_finish);
               //Now overwrite with new_end
               std::copy(first, last, pos);
            }
            else {
               //The raw_mem from end will divide new_end part
               //
               //Old situation:
               // _____________________________________________________________
               //|   raw_mem    | old_begin |  old_end   |  raw_mem            |
               //|______________|___________|____________|_____________________|
               //
               //New situation with do_after(2):
               // _____________________________________________________________
               //| old_begin   +   new_beg  |     new_end   |old_end | raw_mem |
               //|__________________________|_______________|________|_________|
               //
               FwdIt mid = first;
               std::advance(mid, elemsafter);
               //First initialize data in raw memory
               boost::interprocess::uninitialized_copy_copy
                  ( mid, last, copy_move_it(pos), copy_move_it(old_finish), old_finish);
               this->members_.m_size += n_after;
               //Now copy the part of new_end over constructed elements
               std::copy(first, mid, pos);
            }
         }
      }
   }

   template <class InIt>
   void priv_range_insert(iterator pos,
                          InIt first, InIt last,
                          std::input_iterator_tag)
   {
      //Insert range before the pos position
      std::copy(std::inserter(*this, pos), first, last);
   }

   template <class InIt>
   void priv_assign_aux(InIt first, InIt last,
                        std::input_iterator_tag)
   {
      //Overwrite all elements we can from [first, last)
      iterator cur = begin();
      for ( ; first != last && cur != end(); ++cur, ++first){
         *cur = *first;
      }
      
      if (first == last){
         //There are no more elements in the sequence, erase remaining
         this->erase(cur, end());
      }
      else{
         //There are more elements in the range, insert the remaining ones
         this->insert(this->end(), first, last);
      }
   }

   template <class FwdIt>
   void priv_assign_aux(FwdIt first, FwdIt last,
                        std::forward_iterator_tag)
   {
      size_type n = std::distance(first, last);
      //Check if we have enough memory or try to expand current memory
      size_type remaining = this->members_.m_capacity - this->members_.m_size;
      bool same_buffer_start;
      std::pair<pointer, bool> ret;
      size_type real_cap = this->members_.m_capacity;

      if (n <= remaining){
         same_buffer_start = true;
      }
      else{
         //There is not enough memory, allocate a new buffer
         size_type new_cap = this->next_capacity(n);
         ret = this->allocation_command
               (allocate_new | expand_fwd | expand_bwd,
                  this->size() + n, new_cap, real_cap, this->members_.m_start);
         same_buffer_start = ret.second && this->members_.m_start == ret.first;
         if(same_buffer_start){
            this->members_.m_capacity  = real_cap;
         }
      }
      
      if(same_buffer_start){
         T *start = detail::get_pointer(this->members_.m_start);
         if (this->size() >= n){
            //There is memory, but there are more old elements than new ones
            //Overwrite old elements with new ones
            std::copy(first, last, start);
            //Destroy remaining old elements
            this->destroy_n(start + n, this->members_.m_size - n);
            this->members_.m_size = n;
         }
         else{
            //There is memory, but there are less old elements than new ones
            //First overwrite some old elements with new ones
            FwdIt mid = first;
            std::advance(mid, this->size());
            T *end = std::copy(first, mid, start);
            //Initialize the remaining new elements in the uninitialized memory
            std::uninitialized_copy(mid, last, end);
            this->members_.m_size = n;
         }
      }
      else if(!ret.second){
         UCopiedArrayDeallocator scoped_alloc(ret.first, this->alloc(), real_cap);
         std::uninitialized_copy(first, last, detail::get_pointer(ret.first));
         scoped_alloc.release();
         //Destroy and deallocate old buffer
         if(this->members_.m_start != 0){
			   this->destroy_n(detail::get_pointer(this->members_.m_start), this->members_.m_size); 
            this->alloc().deallocate(this->members_.m_start, this->members_.m_capacity);
         }
         this->members_.m_start     = ret.first;
         this->members_.m_size      = n;
         this->members_.m_capacity  = real_cap;
      }
      else{
         //Backwards expansion
         //If anything goes wrong, this object will destroy
         //all old objects
         T *old_start         = detail::get_pointer(this->members_.m_start);
         size_type old_size   = this->members_.m_size;
         OldArrayDestructor old_values_destroyer(old_start, old_size);
         //If something goes wrong size will be 0
         //but holding the whole buffer
         this->members_.m_size  = 0;
         this->members_.m_start = ret.first;
         this->members_.m_capacity = real_cap;
         
         //Backup old buffer data
         size_type old_offset    = old_start - detail::get_pointer(ret.first);
         size_type first_count   = min_value(n, old_offset);
         FwdIt mid = boost::interprocess::n_uninitialized_copy_n
            (first, first_count, detail::get_pointer(ret.first));

         if(old_offset > n){
            //All old elements will be destroyed by "old_values_destroyer" 
            this->members_.m_size = n;
         }
         else{
            //We have constructed objects from the new begin until
            //the old end so release the rollback destruction
            old_values_destroyer.release();
            this->members_.m_start  = ret.first;
            this->members_.m_size   = first_count + old_size;
            //Now overwrite the old values
            size_type second_count = min_value(old_size, n - first_count);
            mid = copy_n(mid, second_count, old_start);
            
            //Check if we still have to append elements in the
            //uninitialized end
            if(second_count == old_size){
               boost::interprocess::n_uninitialized_copy_n
                  ( mid
                  , n - first_count - second_count
                  , old_start + old_size); 
            }
            else{
               //We have to destroy some old values
               this->destroy_n
                  (old_start + second_count, old_size - second_count);
               this->members_.m_size = n;
            }
            this->members_.m_size = n;                        
         }
      }
   }

   template <class Integer>
   void priv_assign_dispatch(Integer n, Integer val, detail::true_)
   { this->assign((size_type) n, (T) val); }

   template <class InIt>
   void priv_assign_dispatch(InIt first, InIt last, detail::false_)
   { 
      //Dispatch depending on integer/iterator
      typedef typename 
         std::iterator_traits<InIt>::iterator_category ItCat;
      this->priv_assign_aux(first, last, ItCat()); 
   }

   template <class Integer>
   void priv_insert_dispatch( iterator pos, Integer n, Integer val, detail::true_) 
   {  this->insert(pos, (size_type)n, (T)val);  }

   template <class InIt>
   void priv_insert_dispatch(iterator pos,   InIt first, 
                             InIt last,      detail::false_)
   {
      //Dispatch depending on integer/iterator
      typedef typename 
         std::iterator_traits<InIt>::iterator_category ItCat;
      this->priv_range_insert(pos.get_ptr(), first, last, ItCat());
   }

   void priv_check_range(size_type n) const 
   {
      //If n is out of range, throw an out_of_range exception
      if (n >= size())
         throw std::out_of_range("vector::at");
   }

   #ifdef BOOST_INTERPROCESS_VECTOR_ALLOC_STATS
   public:
   unsigned int num_expand_fwd;
   unsigned int num_expand_bwd;
   unsigned int num_alloc;
   void reset_alloc_stats()
   {  num_expand_fwd = num_expand_bwd = num_alloc = 0;   }                 
   #endif
   /// @endcond
};

template <class T, class A>
inline bool 
operator==(const vector<T, A>& x, const vector<T, A>& y)
{
   //Check first size and each element if needed
   return x.size() == y.size() &&
          std::equal(x.begin(), x.end(), y.begin());
}

template <class T, class A>
inline bool 
operator!=(const vector<T, A>& x, const vector<T, A>& y)
{
   //Check first size and each element if needed
  return x.size() != y.size() ||
         !std::equal(x.begin(), x.end(), y.begin());
}

template <class T, class A>
inline bool 
operator<(const vector<T, A>& x, const vector<T, A>& y)
{
   return std::lexicographical_compare(x.begin(), x.end(), 
                                       y.begin(), y.end());
}

#ifndef BOOST_INTERPROCESS_RVALUE_REFERENCE
template <class T, class A>
inline void swap(vector<T, A>& x, vector<T, A>& y)
{  x.swap(y);  }

template <class T, class A>
inline void swap(const detail::moved_object<vector<T, A> >& x, vector<T, A>& y)
{  x.get().swap(y);  }

template <class T, class A>
inline void swap(vector<T, A> &x, const detail::moved_object<vector<T, A> >& y)
{  x.swap(y.get());  }
#else
template <class T, class A>
inline void swap(vector<T, A>&&x, vector<T, A>&&y)
{  x.swap(y);  }
#endif

/// @cond

//!This class is movable
template <class T, class A>
struct is_movable<vector<T, A> >
{
   enum {   value = true };
};

//!has_trivial_destructor_after_move<> == true_type
//!specialization for optimizations
template <class T, class A>
struct has_trivial_destructor_after_move<vector<T, A> >
{
   enum {   value = has_trivial_destructor<A>::value  };
};
/// @endcond

}  //namespace interprocess {
}  //namespace boost {

#include <boost/interprocess/detail/config_end.hpp>

#endif //   #ifndef  BOOST_INTERPROCESS_VECTOR_HPP

