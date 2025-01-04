/* Proposed SG14 status_code
(C) 2018-2020 Niall Douglas <http://www.nedproductions.biz/> (5 commits)
File Created: Feb 2018


Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef BOOST_OUTCOME_SYSTEM_ERROR2_IOSTREAM_SUPPORT_HPP
#define BOOST_OUTCOME_SYSTEM_ERROR2_IOSTREAM_SUPPORT_HPP

#include "error.hpp"

#include <ostream>

BOOST_OUTCOME_SYSTEM_ERROR2_NAMESPACE_BEGIN

/*! Print the status code to a `std::ostream &`.
Requires that `DomainType::value_type` implements an `operator<<` overload for `std::ostream`.
*/
template <class DomainType,  //
          typename std::enable_if<std::is_same<std::ostream, typename std::decay<decltype(std::declval<std::ostream>() << std::declval<typename status_code<DomainType>::value_type>())>::type>::value, bool>::type = true>
inline std::ostream &operator<<(std::ostream &s, const status_code<DomainType> &v)
{
  if(v.empty())
  {
    return s << "(empty)";
  }
  return s << v.domain().name().c_str() << ": " << v.value();
}

/*! Print a status code domain's `string_ref` to a `std::ostream &`.
 */
inline std::ostream &operator<<(std::ostream &s, const status_code_domain::string_ref &v)
{
  return s << v.c_str();
}

/*! Print the erased status code to a `std::ostream &`.
 */
template <class ErasedType> inline std::ostream &operator<<(std::ostream &s, const status_code<erased<ErasedType>> &v)
{
  if(v.empty())
  {
    return s << "(empty)";
  }
  return s << v.domain().name() << ": " << v.message();
}

/*! Print the generic code to a `std::ostream &`.
*/
inline std::ostream &operator<<(std::ostream &s, const generic_code &v)
{
  if(v.empty())
  {
    return s << "(empty)";
  }
  return s << v.domain().name() << ": " << v.message();
}

BOOST_OUTCOME_SYSTEM_ERROR2_NAMESPACE_END

#endif
