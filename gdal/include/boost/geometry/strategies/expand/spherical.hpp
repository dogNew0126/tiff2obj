// Boost.Geometry

// Copyright (c) 2020, Oracle and/or its affiliates.

// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle

// Licensed under the Boost Software License version 1.0.
// http://www.boost.org/users/license.html

#ifndef BOOST_GEOMETRY_STRATEGIES_EXPAND_SPHERICAL_HPP
#define BOOST_GEOMETRY_STRATEGIES_EXPAND_SPHERICAL_HPP


#include <type_traits>

#include <boost/geometry/strategy/spherical/expand_box.hpp>
#include <boost/geometry/strategy/spherical/expand_point.hpp>
#include <boost/geometry/strategy/spherical/expand_segment.hpp>

#include <boost/geometry/strategies/detail.hpp>
#include <boost/geometry/strategies/expand/services.hpp>


namespace boost { namespace geometry
{


namespace strategies { namespace expand
{

template
<
    typename CalculationType = void
>
class spherical : strategies::detail::spherical_base<void>
{
    using base_t = strategies::detail::spherical_base<void>;

public:
    template <typename Box, typename Geometry>
    static auto expand(Box const&, Geometry const&,
                       typename util::enable_if_point_t<Geometry> * = nullptr)
    {
        return strategy::expand::spherical_point();
    }

    template <typename Box, typename Geometry>
    static auto expand(Box const&, Geometry const&,
                       typename util::enable_if_box_t<Geometry> * = nullptr)
    {
        return strategy::expand::spherical_box();
    }

    template <typename Box, typename Geometry>
    static auto expand(Box const&, Geometry const&,
                       typename util::enable_if_segment_t<Geometry> * = nullptr)
    {
        return strategy::expand::spherical_segment<CalculationType>();
    }
};


namespace services
{

template <typename Box, typename Geometry>
struct default_strategy<Box, Geometry, spherical_tag>
{
    using type = strategies::expand::spherical<>;
};

template <typename Box, typename Geometry>
struct default_strategy<Box, Geometry, spherical_equatorial_tag>
{
    using type = strategies::expand::spherical<>;
};

template <typename Box, typename Geometry>
struct default_strategy<Box, Geometry, spherical_polar_tag>
{
    using type = strategies::expand::spherical<>;
};


template <>
struct strategy_converter<strategy::expand::spherical_point>
{
    static auto get(strategy::expand::spherical_point const& )
    {
        return strategies::expand::spherical<>();
    }
};

template <>
struct strategy_converter<strategy::expand::spherical_box>
{
    static auto get(strategy::expand::spherical_box const& )
    {
        return strategies::expand::spherical<>();
    }
};

template <typename CT>
struct strategy_converter<strategy::expand::spherical_segment<CT> >
{
    static auto get(strategy::expand::spherical_segment<CT> const&)
    {
        return strategies::expand::spherical<CT>();
    }
};


} // namespace services

}} // namespace strategies::envelope

}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_STRATEGIES_EXPAND_SPHERICAL_HPP
