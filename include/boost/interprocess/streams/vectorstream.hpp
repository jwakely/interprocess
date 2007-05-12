//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2005-2007. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////
//
// This file comes from SGI's sstream file. Modified by Ion Gaztanaga 2005.
// Changed internal SGI string to a generic, templatized vector. Added efficient
// internal buffer get/set/swap functions, so that we can obtain/establish the
// internal buffer without any reallocation or copy. Kill those temporaries!
///////////////////////////////////////////////////////////////////////////////
/*
 * Copyright (c) 1998
 * Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Silicon Graphics makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 */

/*!\file
   This file defines basic_vectorbuf, basic_ivectorstream,
   basic_ovectorstream, and basic_vectorstreamclasses.  These classes
   represent streamsbufs and streams whose sources or destinations are
   STL-like vectors that can be swapped with external vectors to avoid 
   unnecessary allocations/copies.
*/

#ifndef BOOST_INTERPROCESS_VECTORSTREAM_HPP
#define BOOST_INTERPROCESS_VECTORSTREAM_HPP

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>

#include <iosfwd>
#include <ios>
#include <istream>
#include <ostream>
#include <string>    // char traits            
#include <cstddef>   // ptrdiff_t
#include <boost/interprocess/interprocess_fwd.hpp>
#include <assert.h>

namespace boost {  namespace interprocess {

/*!A streambuf class that controls the transmission of elements to and from
   a basic_ivectorstream, basic_ovectorstream or basic_vectorstream. 
   It holds a character vector specified by CharVector template parameter
   as its formatting buffer. The vector must have contiguous storage, like 
   std::vector, boost::interprocess::vector or boost::interprocess::basic_string*/
template <class CharVector, class CharTraits>
class basic_vectorbuf
   : public std::basic_streambuf<typename CharVector::value_type, CharTraits>
{
   public:
   typedef CharVector                        vector_type;
   typedef typename CharVector::value_type   char_type;
   typedef typename CharTraits::int_type     int_type;
   typedef typename CharTraits::pos_type     pos_type;
   typedef typename CharTraits::off_type     off_type;
   typedef CharTraits                        traits_type;

   /// @cond
   private:
   typedef std::basic_streambuf<char_type, traits_type> base_t;

   basic_vectorbuf(const basic_vectorbuf&);
   basic_vectorbuf & operator =(const basic_vectorbuf&);
   /// @endcond

   public:
   /*!Constructor. Throws if vector_type default constructor throws.*/
   explicit basic_vectorbuf(std::ios_base::openmode mode
                              = std::ios_base::in | std::ios_base::out)
      :  base_t(), m_mode(mode)
   {  this->set_pointers();   }

   /*!Constructor. Throws if vector_type(const VectorParameter &param) throws.*/
   template<class VectorParameter>
   explicit basic_vectorbuf(const VectorParameter &param,
                            std::ios_base::openmode mode
                                 = std::ios_base::in | std::ios_base::out)
      :  base_t(), m_mode(mode), m_vect(param)
   {  this->set_pointers();   }

   virtual ~basic_vectorbuf(){}

   public:

   /*!Swaps the underlying vector with the passed vector. 
      This function resets the read/write position in the stream.
      Does not throw.*/
   void swap_vector(vector_type &vect)
   {  
      //Update high water if necessary
      //And resize vector to remove extra size
      if (this->m_mode & std::ios_base::out){
         if (mp_high_water < base_t::pptr()){
            //Restore the vector's size if necessary
            mp_high_water = base_t::pptr();
         }

         m_vect.resize(mp_high_water - (m_vect.size() ? &m_vect[0] : 0));
      }

      //Now swap vector
      m_vect.swap(vect);

      //Now update pointer data
      typename vector_type::size_type old_size = m_vect.size();
      m_vect.resize(m_vect.capacity());
      this->set_pointers();
      mp_high_water = old_size ? &m_vect[0] + old_size : 0;
   }

   /*!Returns a const reference to the internal vector.
      Does not throw.*/
   const vector_type &vector() const 
   {  
      if (this->m_mode & std::ios_base::out){
         if (mp_high_water < base_t::pptr()){
            //Restore the vector's size if necessary
            mp_high_water = base_t::pptr();
         }
         m_vect.resize(mp_high_water - (m_vect.size() ? &m_vect[0] : 0));
         const_cast<basic_vectorbuf * const>(this)->set_pointers();
      }
      return m_vect; 
   }

   /*!Preallocates memory from the internal vector.
      Resets the stream to the first position.
      Throws if the internals vector's memory allocation throws.*/
   void reserve(typename vector_type::size_type size) 
   {
      m_vect.reserve(size);
      //Now update pointer data
      typename vector_type::size_type old_size = m_vect.size();
      m_vect.resize(m_vect.capacity());
      this->set_pointers();
      mp_high_water = old_size ? &m_vect[0] + old_size : 0;
   }

   /*!Calls clear() method of the internal vector.
      Resets the stream to the first position.*/
   void clear() 
   {  m_vect.clear();   this->set_pointers();   }

   /// @cond
   private:
   void set_pointers()
   {
      // The initial read position is the beginning of the vector.
      if(m_mode & std::ios_base::in){
         if(m_vect.empty()){
            this->setg(0, 0, 0);
         }
         else{
            this->setg(&m_vect[0], &m_vect[0], &m_vect[0] + m_vect.size());
         }
      }

      // The initial write position is the beginning of the vector.
      if(m_mode & std::ios_base::out){
         if(m_vect.empty()){
            this->setp(0, 0);
         }
         else{
            this->setp(&m_vect[0], &m_vect[0] + m_vect.size());
         }

         if (m_mode & (std::ios_base::app | std::ios_base::ate))
            base_t::pbump((int)m_vect.size());
      }
      mp_high_water = m_vect.empty() ? 0 : (&m_vect[0] + m_vect.size());
   }

   protected:
   virtual int_type underflow()
   {
      if (base_t::gptr() == 0)
         return CharTraits::eof();
      if (mp_high_water < base_t::pptr())
         mp_high_water = base_t::pptr();
      if (base_t::egptr() < mp_high_water)
         base_t::setg(base_t::eback(), base_t::gptr(), mp_high_water);
      if (base_t::gptr() < base_t::egptr())
         return CharTraits::to_int_type(*base_t::gptr());
      return CharTraits::eof();
   }

   virtual int_type pbackfail(int_type c = CharTraits::eof())
   {
      if(this->gptr() != this->eback()) {
         if(!CharTraits::eq_int_type(c, CharTraits::eof())) {
            if(CharTraits::eq(CharTraits::to_char_type(c), this->gptr()[-1])) {
               this->gbump(-1);
               return c;
            }
            else if(m_mode & std::ios_base::out) {
               this->gbump(-1);
               *this->gptr() = c;
               return c;
            }
            else
               return CharTraits::eof();
         }
         else {
            this->gbump(-1);
            return CharTraits::not_eof(c);
         }
      }
      else
         return CharTraits::eof();
   }

   virtual int_type overflow(int_type c = CharTraits::eof())
   {
      if(m_mode & std::ios_base::out) {
         if(!CharTraits::eq_int_type(c, CharTraits::eof())) {
            if(!(m_mode & std::ios_base::in)) {
               if(this->pptr() < this->epptr()) {
                  *this->pptr() = CharTraits::to_char_type(c);
                           this->pbump(1);
                  if (mp_high_water < base_t::pptr())
                     mp_high_water = base_t::pptr();
                  if ((m_mode & std::ios_base::in) && base_t::egptr() < mp_high_water)
                     base_t::setg(base_t::eback(), base_t::gptr(), mp_high_water);
                  return c;
               }
               else
                  return CharTraits::eof();
            }
            else {
               try{
                  typedef typename vector_type::difference_type dif_t;
                  dif_t inpos  = base_t::gptr() - base_t::eback();
                  dif_t outpos = base_t::pptr() - base_t::pbase() + 1;
                  dif_t hipos = mp_high_water - base_t::pbase();
                  if (hipos < outpos)
                     hipos = outpos;
                  m_vect.push_back(CharTraits::to_char_type(c));
                  m_vect.resize(m_vect.capacity());
                  char_type* p = const_cast<char_type*>(&m_vect[0]);
                  if (m_mode & std::ios_base::in)
                     base_t::setg(p, p + inpos, p + hipos);
                  base_t::setp(p, p + (dif_t)m_vect.size());
                  base_t::pbump((int)outpos);
                  mp_high_water = base_t::pbase() + hipos;
                  return c;
               }
               catch(...){
                  return CharTraits::eof();
               }
            }
         }
         else  // c is EOF, so we don't have to do anything
            return CharTraits::not_eof(c);
      }
      else     // Overflow always fails if it's read-only.
         return CharTraits::eof();
   }

   virtual pos_type seekoff(off_type off, std::ios_base::seekdir dir,
                              std::ios_base::openmode mode 
                                 = std::ios_base::in | std::ios_base::out)
   {
      bool in  = false;
      bool out = false;
         
      const std::ios_base::openmode inout = 
         std::ios_base::in | std::ios_base::out;

      if((mode & inout) == inout) {
         if(dir == std::ios_base::beg || dir == std::ios_base::end)
            in = out = true;
      }
      else if(mode & std::ios_base::in)
         in = true;
      else if(mode & std::ios_base::out)
         out = true;

      if(!in && !out)
         return pos_type(off_type(-1));
      else if((in  && (!(m_mode & std::ios_base::in) || this->gptr() == 0)) ||
               (out && (!(m_mode & std::ios_base::out) || this->pptr() == 0)))
         return pos_type(off_type(-1));

      off_type newoff;
      off_type limit = static_cast<off_type>
                     (mode & std::ios_base::out ?
                      mp_high_water - base_t::pbase() :
                      mp_high_water - base_t::eback()
                     );

      switch(dir) {
         case std::ios_base::beg:
            newoff = 0;
         break;
         case std::ios_base::end:
            newoff = limit;
         break;
         case std::ios_base::cur:
            newoff = in ? static_cast<std::streamoff>(this->gptr() - this->eback()) 
                        : static_cast<std::streamoff>(this->pptr() - this->pbase());
         break;
         default:
            return pos_type(off_type(-1));
      }

      newoff += off;

      if (newoff < 0 || newoff > limit)
         return pos_type(-1);
      if (m_mode & std::ios_base::app && mode & std::ios_base::out && newoff != limit)
         return pos_type(-1);
      if (in)
         base_t::setg(base_t::eback(), base_t::eback() + newoff, base_t::egptr());
      if (out){
         base_t::setp(base_t::pbase(), base_t::epptr());
         base_t::pbump((int)newoff);
      }
      return pos_type(newoff);
   }

   virtual pos_type seekpos(pos_type pos, std::ios_base::openmode mode 
                                 = std::ios_base::in | std::ios_base::out)
   {  return seekoff(pos - pos_type(off_type(0)), std::ios_base::beg, mode);  }

   private:
   std::ios_base::openmode m_mode;
   mutable vector_type     m_vect;
   mutable char_type*      mp_high_water;
   /// @endcond
};

/*!A basic_istream class that holds a character vector specified by CharVector
   template parameter as its formatting buffer. The vector must have
   contiguous storage, like std::vector, boost::interprocess::vector or
   boost::interprocess::basic_string*/
template <class CharVector, class CharTraits>
class basic_ivectorstream
: public std::basic_istream<typename CharVector::value_type, CharTraits>
{
   public:
   typedef CharVector                                                   vector_type;
   typedef typename std::basic_ios
      <typename CharVector::value_type, CharTraits>::char_type          char_type;
   typedef typename std::basic_ios<char_type, CharTraits>::int_type     int_type;
   typedef typename std::basic_ios<char_type, CharTraits>::pos_type     pos_type;
   typedef typename std::basic_ios<char_type, CharTraits>::off_type     off_type;
   typedef typename std::basic_ios<char_type, CharTraits>::traits_type  traits_type;

   /// @cond
   private:
   typedef std::basic_ios<char_type, CharTraits>                basic_ios_t;
   typedef std::basic_istream<char_type, CharTraits>            base_t;
   /// @endcond

   public:
   /*!Constructor. Throws if vector_type default constructor throws.*/
   basic_ivectorstream(std::ios_base::openmode mode = std::ios_base::in)
      :  basic_ios_t(), base_t(0), m_buf(mode | std::ios_base::in)
   {  basic_ios_t::init(&m_buf); }

   /*!Constructor. Throws if vector_type(const VectorParameter &param) throws.*/
   template<class VectorParameter>
   basic_ivectorstream(const VectorParameter &param,
                       std::ios_base::openmode mode = std::ios_base::in)
      :  basic_ios_t(), base_t(0), 
         m_buf(param, mode | std::ios_base::in)
   {  basic_ios_t::init(&m_buf); }

   ~basic_ivectorstream(){};

   public:
   /*!Returns the address of the stored stream buffer.*/
   basic_vectorbuf<CharVector, CharTraits>* rdbuf() const
   { return const_cast<basic_vectorbuf<CharVector, CharTraits>*>(&m_buf); }

   /*!Swaps the underlying vector with the passed vector. 
      This function resets the read position in the stream.
      Does not throw.*/
   void swap_vector(vector_type &vect)
   {  m_buf.swap_vector(vect);   }

   /*!Returns a const reference to the internal vector.
      Does not throw.*/
   const vector_type &vector() const 
   {  return m_buf.vector();   }

   /*!Calls reserve() method of the internal vector.
      Resets the stream to the first position.
      Throws if the internals vector's reserve throws.*/
   void reserve(typename vector_type::size_type size) 
   {  m_buf.reserve(size);   }

   /*!Calls clear() method of the internal vector.
      Resets the stream to the first position.*/
   void clear() 
   {  m_buf.clear();   }

   /// @cond
   private:
   basic_vectorbuf<CharVector, CharTraits> m_buf;
   /// @endcond
};

/*!A basic_ostream class that holds a character vector specified by CharVector
   template parameter as its formatting buffer. The vector must have
   contiguous storage, like std::vector, boost::interprocess::vector or
   boost::interprocess::basic_string*/
template <class CharVector, class CharTraits>
class basic_ovectorstream
   : public std::basic_ostream<typename CharVector::value_type, CharTraits>
{
   public:
   typedef CharVector                                                   vector_type;
   typedef typename std::basic_ios
      <typename CharVector::value_type, CharTraits>::char_type          char_type;
   typedef typename std::basic_ios<char_type, CharTraits>::int_type     int_type;
   typedef typename std::basic_ios<char_type, CharTraits>::pos_type     pos_type;
   typedef typename std::basic_ios<char_type, CharTraits>::off_type     off_type;
   typedef typename std::basic_ios<char_type, CharTraits>::traits_type  traits_type;

   /// @cond
   private:
   typedef std::basic_ios<char_type, CharTraits>      basic_ios_t;
   typedef std::basic_ostream<char_type, CharTraits>  base_t;
   /// @endcond

   public:
   /*!Constructor. Throws if vector_type default constructor throws.*/
   basic_ovectorstream(std::ios_base::openmode mode = std::ios_base::out)
      :  basic_ios_t(), base_t(0), m_buf(mode | std::ios_base::out)
   {  basic_ios_t::init(&m_buf); }

   /*!Constructor. Throws if vector_type(const VectorParameter &param) throws.*/
   template<class VectorParameter>
   basic_ovectorstream(const VectorParameter &param,
                        std::ios_base::openmode mode = std::ios_base::out)
      :  basic_ios_t(), base_t(0), m_buf(param, mode | std::ios_base::out)
   {  basic_ios_t::init(&m_buf); }

   ~basic_ovectorstream(){}

   public:
   /*!Returns the address of the stored stream buffer.*/
   basic_vectorbuf<CharVector, CharTraits>* rdbuf() const
   { return const_cast<basic_vectorbuf<CharVector, CharTraits>*>(&m_buf); }

   /*!Swaps the underlying vector with the passed vector. 
      This function resets the write position in the stream.
      Does not throw.*/
   void swap_vector(vector_type &vect)
   {  m_buf.swap_vector(vect);   }

   /*!Returns a const reference to the internal vector.
      Does not throw.*/
   const vector_type &vector() const 
   {  return m_buf.vector();   }

   /*!Calls reserve() method of the internal vector.
      Resets the stream to the first position.
      Throws if the internals vector's reserve throws.*/
   void reserve(typename vector_type::size_type size) 
   {  m_buf.reserve(size);   }

   /// @cond
   private:
   basic_vectorbuf<CharVector, CharTraits> m_buf;
   /// @endcond
};


/*!A basic_iostream class that holds a character vector specified by CharVector
   template parameter as its formatting buffer. The vector must have
   contiguous storage, like std::vector, boost::interprocess::vector or
   boost::interprocess::basic_string*/
template <class CharVector, class CharTraits>
class basic_vectorstream
: public std::basic_iostream<typename CharVector::value_type, CharTraits>

{
   public:
   typedef CharVector                                                   vector_type;
   typedef typename std::basic_ios
      <typename CharVector::value_type, CharTraits>::char_type          char_type;
   typedef typename std::basic_ios<char_type, CharTraits>::int_type     int_type;
   typedef typename std::basic_ios<char_type, CharTraits>::pos_type     pos_type;
   typedef typename std::basic_ios<char_type, CharTraits>::off_type     off_type;
   typedef typename std::basic_ios<char_type, CharTraits>::traits_type  traits_type;

   /// @cond
   private:
   typedef std::basic_ios<char_type, CharTraits>                 basic_ios_t;
   typedef std::basic_iostream<char_type, CharTraits>            base_t;
   /// @endcond

   public:
   /*!Constructor. Throws if vector_type default constructor throws.*/
   basic_vectorstream(std::ios_base::openmode mode 
                      = std::ios_base::in | std::ios_base::out)
      :  basic_ios_t(), base_t(0), m_buf(mode)
   {  basic_ios_t::init(&m_buf); }

   /*!Constructor. Throws if vector_type(const VectorParameter &param) throws.*/
   template<class VectorParameter>
   basic_vectorstream(const VectorParameter &param, std::ios_base::openmode mode
                      = std::ios_base::in | std::ios_base::out)
      :  basic_ios_t(), base_t(0), m_buf(param, mode)
   {  basic_ios_t::init(&m_buf); }

   ~basic_vectorstream(){}

   public:
   //Returns the address of the stored stream buffer.
   basic_vectorbuf<CharVector, CharTraits>* rdbuf() const
   { return const_cast<basic_vectorbuf<CharVector, CharTraits>*>(&m_buf); }

   /*!Swaps the underlying vector with the passed vector. 
      This function resets the read/write position in the stream.
      Does not throw.*/
   void swap_vector(vector_type &vect)
   {  m_buf.swap_vector(vect);   }

   /*!Returns a const reference to the internal vector.
      Does not throw.*/
   const vector_type &vector() const 
   {  return m_buf.vector();   }

   /*!Calls reserve() method of the internal vector.
      Resets the stream to the first position.
      Throws if the internals vector's reserve throws.*/
   void reserve(typename vector_type::size_type size) 
   {  m_buf.reserve(size);   }

   /*!Calls clear() method of the internal vector.
      Resets the stream to the first position.*/
   void clear() 
   {  m_buf.clear();   }

   /// @cond
   private:
   basic_vectorbuf<CharVector, CharTraits> m_buf;
   /// @endcond
};

//Some typedefs to simplify usage
/*
typedef basic_vectorbuf<std::vector<char> >        vectorbuf;
typedef basic_vectorstream<std::vector<char> >     vectorstream;
typedef basic_ivectorstream<std::vector<char> >    ivectorstream;
typedef basic_ovectorstream<std::vector<char> >    ovectorstream;

typedef basic_vectorbuf<std::vector<wchar_t> >     wvectorbuf;
typedef basic_vectorstream<std::vector<wchar_t> >  wvectorstream;
typedef basic_ivectorstream<std::vector<wchar_t> > wivectorstream;
typedef basic_ovectorstream<std::vector<wchar_t> > wovectorstream;
*/
}} //namespace boost {  namespace interprocess {

#include <boost/interprocess/detail/config_end.hpp>

#endif /* BOOST_INTERPROCESS_VECTORSTREAM_HPP */
