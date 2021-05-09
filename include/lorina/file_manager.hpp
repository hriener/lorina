/* lorina: C++ parsing library
 * Copyright (C) 2018-2021  EPFL
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

/*!
  \file file_manager.hpp
  \brief File manager

  \author Heinz Riener
*/

#pragma once

#include <fmt/format.h>
#include <map>
#include <memory>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace lorina
{

namespace detail
{

struct placement_deleter {
  template <typename T>
  void operator()( T* ptr ) const
  {
    ptr->~T();
  }
};

template <typename T, typename...Args>
void* make_in_place( void* place, Args&&...args )
{
  return ::new ( place ) T( std::forward<Args>(args)... );
}

} /* detail */

template<typename T>
class error_or
{
public:
  error_or( std::error_code const& ec )
    : has_error( true )
  {
    ptr = detail::make_in_place<std::error_code>( &storage, ec );
  }

  error_or( T const& value )
    : has_error( false )
  {
    ptr = detail::make_in_place<T>( &storage, value );
  }

  operator bool() const
  {
    return !has_error;
  }

  T const& operator*() const
  {
    assert( !has_error );
    return *get_storage();
  }

  T const* operator->() const
  {
    assert( !has_error );
    return get_storage();
  }

  std::error_code get_error() const
  {
    return has_error ? *get_error_storage() : std::error_code();
  }

private:
  T const* get_storage() const
  {
    return static_cast<const T*>( ptr );
  }

  std::error_code const* get_error_storage() const
  {
    return static_cast<std::error_code const*>( ptr );
  }

private:
  typename std::aligned_union<0, std::error_code, T>::type storage;
  void *ptr;
  bool has_error;
}; /* error_or */

class file_entry
{
  friend class file_manager;

public:
  file_entry() = default;

  uint32_t get_unique_id() const
  {
    return unique_id;
  }

  std::string get_name() const
  {
    return name;
  }

  std::string get_path() const
  {
    return path;
  }

  bool operator<( file_entry const& other ) const
  {
    return unique_id < other.unique_id;
  }

private:
  uint32_t unique_id{0}; // unique ID
  std::string name; // name of the file
  std::string path; // path to the file
};

class file_entry_ref
{
public:
  file_entry_ref( file_entry const* fe )
    : fe( fe )
  {
  }

  uint32_t get_unique_id() const
  {
    return fe->get_unique_id();
  }

  std::string get_name() const
  {
    return fe->get_name();
  }

  std::string get_path() const
  {
    return fe->get_path();
  }

protected:
  file_entry const* fe;
}; /* file_entry_ref */

class file_manager
{
public:
  file_manager() {}

  ~file_manager()
  {
    release_file_entries();
  }

  error_or<file_entry_ref> get_file( std::string const& filename )
  {
    file_entry*& fe = seen_file_entries[filename];
    if ( !fe )
    {
      /* try to open the file */
      int pfd = ::open( filename.c_str(), O_RDONLY );
      if ( pfd == -1 )
      {
        return std::error_code( errno, std::system_category() );
      }
      ::close( pfd );

      fe = new file_entry();
      fmt::print( "[i] file_manager::alloc 0x{:x}\n", ( uint64_t( fe ) & 0xffff ) );
      fe->unique_id = next_unique_id++;
    }
    return file_entry_ref( fe );
  }

private:
  void release_file_entries()
  {
    /* release memory */
    for ( auto& fe : seen_file_entries )
    {
      fmt::print( "[i] file_manager::release 0x{:x}\n", ( uint64_t( fe.second ) & 0xffff ) );
      delete fe.second;
    }
  }

private:
  uint32_t next_unique_id{0};
  std::map<std::string, file_entry*> seen_file_entries;
}; /* file_manager */

} // namespace lorina
