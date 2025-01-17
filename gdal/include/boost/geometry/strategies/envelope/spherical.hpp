// Boost.Geometry

// Copyright (c) 2020, Oracle and/or its affiliates.

// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle

// Licensed under the Boost Software License version 1.0.
// http://www.boost.org/users/license.html

#ifndef BOOST_GEOMETRY_STRATEGIES_ENVELOPE_SPHERICAL_HPP
#define BOOST_GEOMETRY_STRATEGIES_ENVELOPE_SPHERICAL_HPP


#include <type_traits>

#include <boost/geometry/strategy/spherical/envelope.hpp>
#include <boost/geometry/strategy/spherical/envelope_box.hpp>
#include <boost/geometry/strategy/spherical/envelope_point.hpp>
#include <boost/geometry/strategy/spherical/envelope_multipoint.hpp>
#include <boost/geometry/strategy/spherical/envelope_segment.hpp>

#include <boost/geometry/strategy/spherical/expand_box.hpp> // TEMP
#include <boost/geometry/strategy/spherical/expand_point.hpp>
#include <boost/geometry/strategy/spherical/expand_segment.hpp> // TEMP

#include <boost/geometry/strategies/detail.hpp>
#include <boost/geometry/strategies/envelope/services.hpp>


namespace boost { namespace geometry
{

namespace strategies { namespace envelope
{

template
<
    typename CalculationType = void
>
class spherical : strategies::detail::spherical_base<void>
{
    using base_t = strategies::detail::spherical_base<void>;

public:
    template <typename Geometry, typename Box>
    static auto envelope(Geometry const&, Box const&,
                         typename util::enable_if_point_t<Geometry> * = nullptr)
    {
        return strategy::envelope::spherical_point();
    }

    template <typename Geometry, typename Box>
    static auto envelope(Geometry const&, Box const&,
                         typename util::enable_if_multi_point_t<Geometry> * = nullptr)
    {
        return strategy::envelope::spherical_multipoint();
    }

    template <typename Geometry, typename Box>
    static auto envelope(Geometry const&, Box const&,
                         typename util::enable_if_box_t<Geometry> * = nullptr)
    {
        return strategy::envelope::spherical_box();
    }

    template <typename Geometry, typename Box>
    static auto envelope(Geometry const&, Box const&,
                         typename util::enable_if_segment_t<Geometry> * = nullptr)
    {
        return strategy::envelope::spherical_segment<CalculationType>();
    }

    template <typename Geometry, typename Box>
    static auto envelope(Geometry const&, Box const&,
                         typename util::enable_if_polysegmental_t<Geometry> * = nullptr)
    {
        return strategy::envelope::spherical<CalculationType>();
    }

    template <typename Box, typename Geometry>
    static auto expand(Box const&, Geometry const&,
                       typename util::enable_if_point_t<Geometry> * = nullptr)
    {
        return strategy::expand::spherical_point();
    }

    // TEMP
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

template <typename Geometry, typename Box>
struct default_strategy<Geometry, Box, spherical_tag>
{
    using type = strategies::envelope::spherical<>;
};

template <typename Geometry, typename Box>
struct default_strategy<Geometry, Box, spherical_equatorial_tag>
{
    using type = strategies::envelope::spherical<>;
};

template <typename Geometry, typename Box>
struct default_strategy<Geometry, Box, spherical_polar_tag>
{
    using type = strategies::envelope::spherical<>;
};


template <>
struct strategy_converter<strategy::envelope::spherical_point>
{
    static auto get(strategy::envelope::spherical_point const& )
    {
        return strategies::envelope::spherical<>();
    }
};

template <>
struct strategy_converter<strategy::envelope::spherical_multipoint>
{
    static auto get(strategy::envelope::spherical_multipoint const&)
    {
        return strategies::envelope::spherical<>();
    }
};

template <>
struct strategy_converter<strategy::envelope::spherical_box>
{
    static auto get(strategy::envelope::spherical_box const& )
    {
        return strategies::envelope::spherical<>();
    }
};

template <typename CT>
struct strategy_converter<strategy::envelope::spherical_segment<CT> >
{
    static auto get(strategy::envelope::spherical_segment<CT> const&)
    {
        return strategies::envelope::spherical<CT>();
    }
};

template <typename CT>
struct strategy_converter<strategy::envelope::spherical<CT> >
{
    static auto get(strategy::envelope::spherical<CT> const&)
    {
        return strategies::envelope::spherical<CT>();
    }
};


} // namespace services

}} // namespace strategies::envelope

}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_STRATEGIES_ENVELOPE_SPHERICAL_HPP
