//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztañaga 2005-2007. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTERPROCESS_NULL_MUTEX_HPP
#define BOOST_INTERPROCESS_NULL_MUTEX_HPP

#if (defined _MSC_VER) && (_MSC_VER >= 1200)
#  pragma once
#endif

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>


/*!\file
   Describes null_mutex classes
*/

namespace boost {

namespace posix_time
{  class ptime;   }

namespace interprocess {

/*!Implements a mutex that simulates a mutex without doing any operation and
   simulates a successful operation.*/
class null_mutex
{
   /// @cond
   null_mutex(const null_mutex&);
   null_mutex &operator= (const null_mutex&);
   /// @endcond
   public:

   /*!Constructor. Empty.*/
   null_mutex(){}

   /*!Destructor. Empty.*/
   ~null_mutex(){}

   /*!Simulates a mutex lock() operation. Empty function. Does not throw.*/
   void lock(){}

   /*!Simulates a mutex try_lock() operation. Returns always true. Does not throw.*/
   bool try_lock()
   {  return true;   }

   /*!Simulates a mutex timed_lock() operation. Returns always true. Does not throw.*/
   bool timed_lock(const boost::posix_time::ptime &abs_time)
   {  return true;   }

   /*!Simulates a mutex unlock() operation. Empty function. Does not throw.*/
   void unlock(){}

   /*!Simulates a mutex lock_sharable() operation. Empty function.
      Does not throw.*/
   void lock_sharable(){}

   /*!Simulates a mutex try_lock_sharable() operation. Returns always true.
      Does not throw.*/
   bool try_lock_sharable()
   {  return true;   }

   /*!Simulates a mutex timed_lock_sharable() operation. Returns always true.
      Does not throw.*/
   bool timed_lock_sharable(const boost::posix_time::ptime &abs_time)
   {  return true;   }

   /*!Simulates a mutex unlock_sharable() operation. Empty function.
      Does not throw.*/
   void unlock_sharable(){}

   /*!Simulates a mutex lock_upgradable() operation. Empty function.
      Does not throw.*/
   void lock_upgradable(){}

   /*!Simulates a mutex try_lock_upgradable() operation. Returns always true.
      Does not throw.*/
   bool try_lock_upgradable()
   {  return true;   }

   /*!Simulates a mutex timed_lock_upgradable() operation. Returns always true.
      Does not throw.*/
   bool timed_lock_upgradable(const boost::posix_time::ptime &abs_time)
   {  return true;   }

   /*!Simulates a mutex unlock_upgradable() operation. Empty function.
      Does not throw.*/
   void unlock_upgradable(){}

   /*!Simulates unlock_and_lock_upgradable(). Empty function.
      Does not throw.*/
   void unlock_and_lock_upgradable(){}

   /*!Simulates unlock_and_lock_sharable(). Empty function.
      Does not throw.*/
   void unlock_and_lock_sharable(){}

   /*!Simulates unlock_upgradable_and_lock_sharable(). Empty function.
      Does not throw.*/
   void unlock_upgradable_and_lock_sharable(){}

   //Promotions

   /*!Simulates unlock_upgradable_and_lock(). Empty function.
      Does not throw.*/
   void unlock_upgradable_and_lock(){}

   /*!Simulates try_unlock_upgradable_and_lock(). Returns always true.
      Does not throw.*/
   bool try_unlock_upgradable_and_lock()
   {  return true;   }

   /*!Simulates timed_unlock_upgradable_and_lock(). Returns always true.
      Does not throw.*/
   bool timed_unlock_upgradable_and_lock(const boost::posix_time::ptime &abs_time)
   {  return true;   }

   /*!Simulates try_unlock_sharable_and_lock(). Returns always true.
      Does not throw.*/
   bool try_unlock_sharable_and_lock()
   {  return true;   }

   /*!Simulates try_unlock_sharable_and_lock_upgradable(). Returns always true.
      Does not throw.*/
   bool try_unlock_sharable_and_lock_upgradable()
   {  return true;   }
   /// @cond   
   #if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564))
   private:
   char dummy; // BCB would by default use 8 B for empty structure
   #endif
   /// @endcond
};

}  //namespace interprocess {
}  //namespace boost {

#include <boost/interprocess/detail/config_end.hpp>

#endif   //BOOST_INTERPROCESS_NULL_MUTEX_HPP
