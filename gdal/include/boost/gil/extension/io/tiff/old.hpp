//
// Copyright 2007-2008 Christian Henning
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_EXTENSION_IO_TIFF_OLD_HPP
#define BOOST_GIL_EXTENSION_IO_TIFF_OLD_HPP

#include <boost/gil/extension/io/tiff.hpp>

namespace boost { namespace gil {

/// \ingroup TIFF_IO
/// \brief Returns the width and height of the TIFF file at the specified location.
/// Throws std::ios_base::failure if the location does not correspond to a valid TIFF file
template<typename String>
inline point_t tiff_read_dimensions(String const& filename)
{
    using backend_t = typename get_reader_backend<String, tiff_tag>::type;
    backend_t backend = read_image_info(filename, tiff_tag());
    return { backend._info._width, backend._info._height };
}

/// \ingroup TIFF_IO
/// \brief Loads the image specified by the given tiff image file name into the given view.
/// Triggers a compile assert if the view color space and channel depth are not supported by the TIFF library or by the I/O extension.
/// Throws std::ios_base::failure if the file is not a valid TIFF file, or if its color space or channel depth are not
/// compatible with the ones specified by View, or if its dimensions don't match the ones of the view.
template< typename String
        , typename View
        >
inline
void tiff_read_view( const String& filename
                   , const View&   view
                   )
{
    read_view( filename
             , view
             , tiff_tag()
             );
}

/// \ingroup TIFF_IO
/// \brief Allocates a new image whose dimensions are determined by the given tiff image file, and loads the pixels into it.
/// Triggers a compile assert if the image color space or channel depth are not supported by the TIFF library or by the I/O extension.
/// Throws std::ios_base::failure if the file is not a valid TIFF file, or if its color space or channel depth are not
/// compatible with the ones specified by Image
template< typename String
        , typename Image
        >
inline
void tiff_read_image( const String& filename
                    , Image&        img
                    )
{
    read_image( filename
              , img
              , tiff_tag()
              );
}

/// \ingroup TIFF_IO
/// \brief Loads and color-converts the image specified by the given tiff image file name into the given view.
/// Throws std::ios_base::failure if the file is not a valid TIFF file, or if its dimensions don't match the ones of the view.
template< typename String
        , typename View
        , typename CC
        >
inline
void tiff_read_and_convert_view( const String& filename
                               , const View&   view
                               , CC            cc
                               )
{
    read_and_convert_view( filename
                         , view
                         , cc
                         , tiff_tag()
                         );
}

/// \ingroup TIFF_IO
/// \brief Loads and color-converts the image specified by the given tiff image file name into the given view.
/// Throws std::ios_base::failure if the file is not a valid TIFF file, or if its dimensions don't match the ones of the view.
template< typename String
        , typename View
        >
inline
void tiff_read_and_convert_view( const String& filename
                               , const View&   view
                               )
{
    read_and_convert_view( filename
                         , view
                         , tiff_tag()
                         );
}

/// \ingroup TIFF_IO
/// \brief Allocates a new image whose dimensions are determined by the given tiff image file, loads and color-converts the pixels into it.
/// Throws std::ios_base::failure if the file is not a valid TIFF file
template< typename String
        , typename Image
        , typename CC
        >
inline
void tiff_read_and_convert_image( const String& filename
                                , Image& img
                                , CC     cc
                                )
{
    read_and_convert_image( filename
                          , img
                          , cc
                          , tiff_tag()
                          );
}

/// \ingroup TIFF_IO
/// \brief Allocates a new image whose dimensions are determined by the given tiff image file, loads and color-converts the pixels into it.
/// Throws std::ios_base::failure if the file is not a valid TIFF file
template< typename String
        , typename Image
        >
inline
void tiff_read_and_convert_image( const String filename
                                , Image&       img
                                )
{
    read_and_convert_image( filename
                          , img
                          , tiff_tag()
                          );
}


/// \ingroup TIFF_IO
/// \brief Saves the view to a tiff file specified by the given tiff image file name.
/// Triggers a compile assert if the view color space and channel depth are not supported by the TIFF library or by the I/O extension.
/// Throws std::ios_base::failure if it fails to create the file.
template< typename String
        , typename View
        >
inline
void tiff_write_view( const String& filename
                    , const View&   view
                    )
{
    write_view( filename
              , view
              , tiff_tag()
              );
}

}  // namespace gil
}  // namespace boost

#endif
