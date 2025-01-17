//
// experimental/impl/as_single.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_ASIO_IMPL_EXPERIMENTAL_AS_SINGLE_HPP
#define BOOST_ASIO_IMPL_EXPERIMENTAL_AS_SINGLE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/detail/config.hpp>

#include <tuple>

#include <boost/asio/associated_executor.hpp>
#include <boost/asio/associated_allocator.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/detail/handler_alloc_helpers.hpp>
#include <boost/asio/detail/handler_cont_helpers.hpp>
#include <boost/asio/detail/handler_invoke_helpers.hpp>
#include <boost/asio/detail/type_traits.hpp>
#include <boost/asio/detail/variadic_templates.hpp>

#include <boost/asio/detail/push_options.hpp>

namespace boost {
namespace asio {
namespace experimental {
namespace detail {

// Class to adapt a as_single_t as a completion handler.
template <typename Handler>
class as_single_handler
{
public:
  typedef void result_type;

  template <typename CompletionToken>
  as_single_handler(as_single_t<CompletionToken> e)
    : handler_(BOOST_ASIO_MOVE_CAST(CompletionToken)(e.token_))
  {
  }

  template <typename RedirectedHandler>
  as_single_handler(BOOST_ASIO_MOVE_ARG(RedirectedHandler) h)
    : handler_(BOOST_ASIO_MOVE_CAST(RedirectedHandler)(h))
  {
  }

  void operator()()
  {
    handler_();
  }

  template <typename Arg>
  void operator()(BOOST_ASIO_MOVE_ARG(Arg) arg)
  {
    handler_(BOOST_ASIO_MOVE_CAST(Arg)(arg));
  }

  template <typename... Args>
  void operator()(BOOST_ASIO_MOVE_ARG(Args)... args)
  {
    handler_(std::make_tuple(BOOST_ASIO_MOVE_CAST(Args)(args)...));
  }

//private:
  Handler handler_;
};

template <typename Handler>
inline void* asio_handler_allocate(std::size_t size,
    as_single_handler<Handler>* this_handler)
{
  return boost_asio_handler_alloc_helpers::allocate(
      size, this_handler->handler_);
}

template <typename Handler>
inline void asio_handler_deallocate(void* pointer, std::size_t size,
    as_single_handler<Handler>* this_handler)
{
  boost_asio_handler_alloc_helpers::deallocate(
      pointer, size, this_handler->handler_);
}

template <typename Handler>
inline bool asio_handler_is_continuation(
    as_single_handler<Handler>* this_handler)
{
  return boost_asio_handler_cont_helpers::is_continuation(
        this_handler->handler_);
}

template <typename Function, typename Handler>
inline void asio_handler_invoke(Function& function,
    as_single_handler<Handler>* this_handler)
{
  boost_asio_handler_invoke_helpers::invoke(
      function, this_handler->handler_);
}

template <typename Function, typename Handler>
inline void asio_handler_invoke(const Function& function,
    as_single_handler<Handler>* this_handler)
{
  boost_asio_handler_invoke_helpers::invoke(
      function, this_handler->handler_);
}

template <typename Signature>
struct as_single_signature
{
  typedef Signature type;
};

template <typename R>
struct as_single_signature<R()>
{
  typedef R type();
};

template <typename R, typename Arg>
struct as_single_signature<R(Arg)>
{
  typedef R type(Arg);
};

template <typename R, typename... Args>
struct as_single_signature<R(Args...)>
{
  typedef R type(std::tuple<typename decay<Args>::type...>);
};

} // namespace detail
} // namespace experimental

#if !defined(GENERATING_DOCUMENTATION)

template <typename CompletionToken, typename Signature>
struct async_result<experimental::as_single_t<CompletionToken>, Signature>
{
  typedef typename async_result<CompletionToken,
    typename experimental::detail::as_single_signature<Signature>::type>
      ::return_type return_type;

  template <typename Initiation>
  struct init_wrapper
  {
    init_wrapper(Initiation init)
      : initiation_(BOOST_ASIO_MOVE_CAST(Initiation)(init))
    {
    }

    template <typename Handler, typename... Args>
    void operator()(
        BOOST_ASIO_MOVE_ARG(Handler) handler,
        BOOST_ASIO_MOVE_ARG(Args)... args)
    {
      BOOST_ASIO_MOVE_CAST(Initiation)(initiation_)(
          experimental::detail::as_single_handler<
            typename decay<Handler>::type>(
              BOOST_ASIO_MOVE_CAST(Handler)(handler)),
          BOOST_ASIO_MOVE_CAST(Args)(args)...);
    }

    Initiation initiation_;
  };

  template <typename Initiation, typename RawCompletionToken, typename... Args>
  static return_type initiate(
      BOOST_ASIO_MOVE_ARG(Initiation) initiation,
      BOOST_ASIO_MOVE_ARG(RawCompletionToken) token,
      BOOST_ASIO_MOVE_ARG(Args)... args)
  {
    return async_initiate<CompletionToken,
      typename experimental::detail::as_single_signature<Signature>::type>(
        init_wrapper<typename decay<Initiation>::type>(
          BOOST_ASIO_MOVE_CAST(Initiation)(initiation)),
        token.token_, BOOST_ASIO_MOVE_CAST(Args)(args)...);
  }
};

template <typename Handler, typename Executor>
struct associated_executor<
    experimental::detail::as_single_handler<Handler>, Executor>
  : detail::associated_executor_forwarding_base<Handler, Executor>
{
  typedef typename associated_executor<Handler, Executor>::type type;

  static type get(
      const experimental::detail::as_single_handler<Handler>& h,
      const Executor& ex = Executor()) BOOST_ASIO_NOEXCEPT
  {
    return associated_executor<Handler, Executor>::get(h.handler_, ex);
  }
};

template <typename Handler, typename Allocator>
struct associated_allocator<
    experimental::detail::as_single_handler<Handler>, Allocator>
{
  typedef typename associated_allocator<Handler, Allocator>::type type;

  static type get(
      const experimental::detail::as_single_handler<Handler>& h,
      const Allocator& a = Allocator()) BOOST_ASIO_NOEXCEPT
  {
    return associated_allocator<Handler, Allocator>::get(h.handler_, a);
  }
};

#endif // !defined(GENERATING_DOCUMENTATION)

} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_IMPL_EXPERIMENTAL_AS_SINGLE_HPP
