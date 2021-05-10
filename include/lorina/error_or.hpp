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
  \file error_or.hpp
  \brief Implements error_or container similar to llvm::error_or.

  \author Heinz Riener
*/

#pragma once

namespace lorina
{

template<class T>
class error_or
{
  static constexpr bool is_ref = std::is_reference<T>::value;

  using wrap = std::reference_wrapper<std::remove_reference_t<T>>;

public:
  using storage_type = std::conditional_t<is_ref, wrap, T>;

private:
  using reference = std::remove_reference_t<T>&;
  using const_reference = std::remove_reference_t<T> const&;
  using pointer = std::remove_reference_t<T>*;
  using const_pointer = std::remove_reference_t<T> const *;

public:
  template<class E>
  error_or( E ec,
            std::enable_if_t<std::is_error_code_enum<E>::value ||
            std::is_error_condition_enum<E>::value,
            void *> = nullptr )
    : is_error( true )
  {
    new ( get_error_storage() ) std::error_code( make_error_code( ec ) );
  }

  error_or( std::error_code ec )
    : is_error( true )
  {
    new ( get_error_storage() ) std::error_code( ec );
  }

  template<class OtherT>
  error_or( OtherT &&value,
            std::enable_if_t<std::is_convertible<OtherT, T>::value>* = nullptr )
    : is_error( false )
  {
    new ( get_storage() ) storage_type( std::forward<OtherT>( value ) );
  }

  ~error_or()
  {
    if ( !is_error )
    {
      get_storage()->~storage_type();
    }
  }

  explicit operator bool() const
  {
    return !is_error;
  }

  reference operator*()
  {
    assert( !is_error );
    return *get_storage();
  }

  const_reference operator*() const
  {
    assert( !is_error );
    return *get_storage();
  }

  pointer operator->()
  {
    assert( !is_error );
    return get_storage();
  }

  const_pointer operator->() const
  {
    assert( !is_error );
    return get_storage();
  }

  std::error_code get_error() const
  {
    return is_error ? *get_error_storage() : std::error_code();
  }

private:
  storage_type* get_storage()
  {
    return reinterpret_cast<storage_type*>( &storage );
  }

  storage_type const* get_storage() const
  {
    return reinterpret_cast<const storage_type*>( &storage );
  }

  std::error_code* get_error_storage()
  {
    return reinterpret_cast<std::error_code*>( &storage );
  }

  std::error_code const* get_error_storage() const
  {
    return const_cast<std::error_code*>( this->get_error_storage() );
  }

private:
  typename std::aligned_union<0, std::error_code, T>::type storage;
  bool is_error;
}; /* error_or */

} /* lorina */
