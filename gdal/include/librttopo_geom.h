/**********************************************************************
 *
 * rttopo - topology library
 * http://git.osgeo.org/gitea/rttopo/librttopo
 *
 * rttopo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * rttopo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rttopo.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright 2011-2016 Sandro Santilli <strk@kbt.io>
 * Copyright 2011 Paul Ramsey <pramsey@cleverelephant.ca>
 * Copyright 2007-2008 Mark Cave-Ayland
 * Copyright 2001-2006 Refractions Research Inc.
 *
 **********************************************************************/



#ifndef _LIBRTGEOM_H
#define _LIBRTGEOM_H 1

#define GEOS_USE_ONLY_R_API 1
#include <geos_c.h>

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

/**
* @file librttopo_geom.h
*
* This library is the generic geometry handling section of librttopo.
* The geometry objects, constructors, destructors, and a set of spatial
* processing functions, are implemented here.
*
* Programs using this library can install their custom memory managers
* passing them to the rtgeom_init function, and their custom message
* handlers using the appropriate rtgeom_set_*_logger functions.
*/

/**
* Return types for functions with status returns.
*/
#define RT_TRUE 1
#define RT_FALSE 0
#define RT_UNKNOWN 2
#define RT_FAILURE 0
#define RT_SUCCESS 1

/**
* RTRTTYPE numbers
*/
#define  RTPOINTTYPE                1
#define  RTLINETYPE                 2
#define  RTPOLYGONTYPE              3
#define  RTMULTIPOINTTYPE           4
#define  RTMULTILINETYPE            5
#define  RTMULTIPOLYGONTYPE         6
#define  RTCOLLECTIONTYPE           7
#define RTCIRCSTRINGTYPE           8
#define RTCOMPOUNDTYPE             9
#define RTCURVEPOLYTYPE           10
#define RTMULTICURVETYPE          11
#define RTMULTISURFACETYPE        12
#define RTPOLYHEDRALSURFACETYPE   13
#define RTTRIANGLETYPE            14
#define RTTINTYPE                 15

#define RTNUMTYPES                16

/**
* Flags applied in EWKB to indicate Z/M dimensions and
* presence/absence of SRID and bounding boxes
*/
#define RTWKBZOFFSET  0x80000000
#define RTWKBMOFFSET  0x40000000
#define RTWKBSRIDFLAG 0x20000000
#define RTWKBBBOXFLAG 0x10000000

/** Ordinate names */
typedef enum RTORD_T {
  RTORD_X = 0,
  RTORD_Y = 1,
  RTORD_Z = 2,
  RTORD_M = 3
} RTORD;

/**
* Macros for manipulating the 'flags' byte. A uint8_t used as follows:
* ---RGBMZ
* Three unused bits, followed by ReadOnly, Geodetic, HasBBox, HasM and HasZ flags.
*/
#define RTFLAGS_GET_Z(flags) ((flags) & 0x01)
#define RTFLAGS_GET_M(flags) (((flags) & 0x02)>>1)
#define RTFLAGS_GET_BBOX(flags) (((flags) & 0x04)>>2)
#define RTFLAGS_GET_GEODETIC(flags) (((flags) & 0x08)>>3)
#define RTFLAGS_GET_READONLY(flags) (((flags) & 0x10)>>4)
#define RTFLAGS_GET_SOLID(flags) (((flags) & 0x20)>>5)
#define RTFLAGS_SET_Z(flags, value) ((flags) = (value) ? ((flags) | 0x01) : ((flags) & 0xFE))
#define RTFLAGS_SET_M(flags, value) ((flags) = (value) ? ((flags) | 0x02) : ((flags) & 0xFD))
#define RTFLAGS_SET_BBOX(flags, value) ((flags) = (value) ? ((flags) | 0x04) : ((flags) & 0xFB))
#define RTFLAGS_SET_GEODETIC(flags, value) ((flags) = (value) ? ((flags) | 0x08) : ((flags) & 0xF7))
#define RTFLAGS_SET_READONLY(flags, value) ((flags) = (value) ? ((flags) | 0x10) : ((flags) & 0xEF))
#define RTFLAGS_SET_SOLID(flags, value) ((flags) = (value) ? ((flags) | 0x20) : ((flags) & 0xDF))
#define RTFLAGS_NDIMS(flags) (2 + RTFLAGS_GET_Z(flags) + RTFLAGS_GET_M(flags))
#define RTFLAGS_GET_ZM(flags) (RTFLAGS_GET_M(flags) + RTFLAGS_GET_Z(flags) * 2)
#define RTFLAGS_NDIMS_BOX(flags) (RTFLAGS_GET_GEODETIC(flags) ? 3 : RTFLAGS_NDIMS(flags))

/**
* Macros for manipulating the 'typemod' int. An int32_t used as follows:
* Plus/minus = Top bit.
* Spare bits = Next 2 bits.
* SRID = Next 21 bits.
* RTTYPE = Next 6 bits.
* ZM Flags = Bottom 2 bits.
*/

#define RTTYPMOD_GET_SRID(typmod) ((((typmod) & 0x1FFFFF00)<<3)>>11)
#define RTTYPMOD_SET_SRID(typmod, srid) ((typmod) = (((typmod) & 0xE00000FF) | ((srid & 0x001FFFFF)<<8)))
#define RTTYPMOD_GET_TYPE(typmod) ((typmod & 0x000000FC)>>2)
#define RTTYPMOD_SET_TYPE(typmod, type) ((typmod) = (typmod & 0xFFFFFF03) | ((type & 0x0000003F)<<2))
#define RTTYPMOD_GET_Z(typmod) ((typmod & 0x00000002)>>1)
#define RTTYPMOD_SET_Z(typmod) ((typmod) = typmod | 0x00000002)
#define RTTYPMOD_GET_M(typmod) (typmod & 0x00000001)
#define RTTYPMOD_SET_M(typmod) ((typmod) = typmod | 0x00000001)
#define RTTYPMOD_GET_NDIMS(typmod) (2+RTTYPMOD_GET_Z(typmod)+RTTYPMOD_GET_M(typmod))

/**
* Maximum allowed SRID value in serialized geometry.
* Currently we are using 21 bits (2097152) of storage for SRID.
*/
#define SRID_MAXIMUM 999999

/**
 * Maximum valid SRID value for the user
 * We reserve 1000 values for internal use
 */
#define SRID_USER_MAXIMUM 998999

/** Unknown SRID value */
#define SRID_UNKNOWN 0
#define SRID_IS_UNKNOWN(x) ((int)x<=0)

/*
** EPSG WGS84 geographics, OGC standard default SRS, better be in
** the SPATIAL_REF_SYS table!
*/
#define SRID_DEFAULT 4326

#ifndef __GNUC__
# define __attribute__(x)
#endif

/**
 * RT library context
 */
typedef struct RTCTX_T RTCTX;

/**
 * Global functions for memory/logging handlers.
 */
typedef void* (*rtallocator)(size_t size);
typedef void* (*rtreallocator)(void *mem, size_t size);
typedef void (*rtfreeor)(void* mem);
typedef void (*rtreporter)(const char* fmt, va_list ap, void *arg)
  __attribute__ (( format(printf, 1, 0) ));
typedef void (*rtdebuglogger)(int level, const char* fmt, va_list ap, void *arg)
  __attribute__ (( format(printf, 2,0) ));


/**
 * Initialize the library with custom memory management functions
 * you want your application to use.
 * @param allocator function for allocating memory,
 *                  or NULL to use the default
 * @param reallocator function for reallocating memory,
 *                    or NULL to use the default
 * @param freeor function for release memory,
 *               or NULL to use the default
 * @return a context object to use in subsequent calls
 *         to the library
 * @see rtgeom_finish to destroy the created context
 * @ingroup system
 */
RTCTX *rtgeom_init(rtallocator allocator,
                   rtreallocator reallocator,
                   rtfreeor freeor);

/**
 * Deinitialize the library, releasing all context memory
 *
 * @param ctx a context returned by rtgeom_init
 *
 */
void rtgeom_finish(RTCTX *ctx);

/** Return rtgeom version string (not to be freed) */
const char* rtgeom_version(void);

/**
 * This functions are called by programs which want to set up
 * custom handling for error reporting
 */
extern void rtgeom_set_error_logger(RTCTX *ctx,
          rtreporter logger, void *arg);
extern void rtgeom_set_notice_logger(RTCTX *ctx,
          rtreporter logger, void *arg);
extern void rtgeom_set_debug_logger(RTCTX *ctx,
          rtdebuglogger logger, void *arg);

/**
 * Request interruption of any running code
 *
 * Safe for use from signal handlers
 *
 * Interrupted code will (as soon as it finds out
 * to be interrupted) cleanup and return as soon as possible.
 *
 * The return value from interrupted code is undefined,
 * it is the caller responsibility to not take it in consideration.
 *
 */
extern void rtgeom_request_interrupt(const RTCTX *ctx);

/**
 * Cancel any interruption request
 */
extern void rtgeom_cancel_interrupt(const RTCTX *ctx);

/**
 * Install a callback to be called periodically during
 * algorithm execution. Mostly only needed on WIN32 to
 * dispatch queued signals.
 *
 * The callback is invoked before checking for interrupt
 * being requested, so you can request interruption from
 * the callback, if you want (see rtgeom_request_interrupt).
 *
 */
typedef void (rtinterrupt_callback)();
extern rtinterrupt_callback *rtgeom_register_interrupt_callback(const RTCTX *ctx, rtinterrupt_callback *);

/******************************************************************/

typedef struct {
  double afac, bfac, cfac, dfac, efac, ffac, gfac, hfac, ifac, xoff, yoff, zoff;
} RTAFFINE;


/******************************************************************
* RTGBOX structure.
* We include the flags (information about dimensinality),
* so we don't have to constantly pass them
* into functions that use the RTGBOX.
*/
typedef struct
{
  uint8_t flags;
  double xmin;
  double xmax;
  double ymin;
  double ymax;
  double zmin;
  double zmax;
  double mmin;
  double mmax;
} RTGBOX;


/******************************************************************
* SPHEROID
*
*  Standard definition of an ellipsoid (what wkt calls a spheroid)
*    f = (a-b)/a
*    e_sq = (a*a - b*b)/(a*a)
*    b = a - fa
*/
typedef struct
{
  double  a;  /* semimajor axis */
  double  b;   /* semiminor axis b = (a - fa) */
  double  f;  /* flattening f = (a-b)/a */
  double  e;  /* eccentricity (first) */
  double  e_sq;  /* eccentricity squared (first) e_sq = (a*a-b*b)/(a*a) */
  double  radius;  /* spherical average radius = (2*a+b)/3 */
  char  name[20];  /* name of ellipse */
}
SPHEROID;

/******************************************************************
* RTPOINT2D, POINT3D, RTPOINT3DM, RTPOINT4D
*/
typedef struct
{
  double x, y;
}
RTPOINT2D;

typedef struct
{
  double x, y, z;
}
RTPOINT3DZ;

typedef struct
{
  double x, y, z;
}
POINT3D;

typedef struct
{
  double x, y, m;
}
RTPOINT3DM;

typedef struct
{
  double x, y, z, m;
}
RTPOINT4D;

/******************************************************************
*  RTPOINTARRAY
*  Point array abstracts a lot of the complexity of points and point lists.
*  It handles 2d/3d translation
*    (2d points converted to 3d will have z=0 or NaN)
*  DO NOT MIX 2D and 3D POINTS! EVERYTHING* is either one or the other
*/
typedef struct
{
  /* Array of POINT 2D, 3D or 4D, possibly missaligned. */
  uint8_t *serialized_pointlist;

  /* Use RTFLAGS_* macros to handle */
  uint8_t  flags;

  int npoints;   /* how many points we are currently storing */
  int maxpoints; /* how many points we have space for in serialized_pointlist */
}
RTPOINTARRAY;

/******************************************************************
* GSERIALIZED
*/
typedef struct
{
  uint32_t size; /* For PgSQL use only, use VAR* macros to manipulate. */
  uint8_t srid[3]; /* 24 bits of SRID */
  uint8_t flags; /* HasZ, HasM, HasBBox, IsGeodetic, IsReadOnly */
  uint8_t data[1]; /* See gserialized.txt */
} GSERIALIZED;


/******************************************************************
* RTGEOM (any geometry type)
*
* Abstract type, note that 'type', 'bbox' and 'srid' are available in
* all geometry variants.
*/
typedef struct
{
  uint8_t type;
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  void *data;
}
RTGEOM;

/* RTPOINTYPE */
typedef struct
{
  uint8_t type; /* RTPOINTTYPE */
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  RTPOINTARRAY *point;  /* hide 2d/3d (this will be an array of 1 point) */
}
RTPOINT; /* "light-weight point" */

/* RTLINETYPE */
typedef struct
{
  uint8_t type; /* RTLINETYPE */
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  RTPOINTARRAY *points; /* array of POINT3D */
}
RTLINE; /* "light-weight line" */

/* TRIANGLE */
typedef struct
{
  uint8_t type;
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  RTPOINTARRAY *points;
}
RTTRIANGLE;

/* RTCIRCSTRINGTYPE */
typedef struct
{
  uint8_t type; /* RTCIRCSTRINGTYPE */
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  RTPOINTARRAY *points; /* array of POINT(3D/3DM) */
}
RTCIRCSTRING; /* "light-weight circularstring" */

/* RTPOLYGONTYPE */
typedef struct
{
  uint8_t type; /* RTPOLYGONTYPE */
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  int nrings;   /* how many rings we are currently storing */
  int maxrings; /* how many rings we have space for in **rings */
  RTPOINTARRAY **rings; /* list of rings (list of points) */
}
RTPOLY; /* "light-weight polygon" */

/* RTMULTIPOINTTYPE */
typedef struct
{
  uint8_t type;
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  int ngeoms;   /* how many geometries we are currently storing */
  int maxgeoms; /* how many geometries we have space for in **geoms */
  RTPOINT **geoms;
}
RTMPOINT;

/* RTMULTILINETYPE */
typedef struct
{
  uint8_t type;
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  int ngeoms;   /* how many geometries we are currently storing */
  int maxgeoms; /* how many geometries we have space for in **geoms */
  RTLINE **geoms;
}
RTMLINE;

/* RTMULTIPOLYGONTYPE */
typedef struct
{
  uint8_t type;
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  int ngeoms;   /* how many geometries we are currently storing */
  int maxgeoms; /* how many geometries we have space for in **geoms */
  RTPOLY **geoms;
}
RTMPOLY;

/* RTCOLLECTIONTYPE */
typedef struct
{
  uint8_t type;
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  int ngeoms;   /* how many geometries we are currently storing */
  int maxgeoms; /* how many geometries we have space for in **geoms */
  RTGEOM **geoms;
}
RTCOLLECTION;

/* RTCOMPOUNDTYPE */
typedef struct
{
  uint8_t type; /* RTCOMPOUNDTYPE */
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  int ngeoms;   /* how many geometries we are currently storing */
  int maxgeoms; /* how many geometries we have space for in **geoms */
  RTGEOM **geoms;
}
RTCOMPOUND; /* "light-weight compound line" */

/* RTCURVEPOLYTYPE */
typedef struct
{
  uint8_t type; /* RTCURVEPOLYTYPE */
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  int nrings;    /* how many rings we are currently storing */
  int maxrings;  /* how many rings we have space for in **rings */
  RTGEOM **rings; /* list of rings (list of points) */
}
RTCURVEPOLY; /* "light-weight polygon" */

/* MULTICURVE */
typedef struct
{
  uint8_t type;
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  int ngeoms;   /* how many geometries we are currently storing */
  int maxgeoms; /* how many geometries we have space for in **geoms */
  RTGEOM **geoms;
}
RTMCURVE;

/* RTMULTISURFACETYPE */
typedef struct
{
  uint8_t type;
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  int ngeoms;   /* how many geometries we are currently storing */
  int maxgeoms; /* how many geometries we have space for in **geoms */
  RTGEOM **geoms;
}
RTMSURFACE;

/* RTPOLYHEDRALSURFACETYPE */
typedef struct
{
  uint8_t type;
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  int ngeoms;   /* how many geometries we are currently storing */
  int maxgeoms; /* how many geometries we have space for in **geoms */
  RTPOLY **geoms;
}
RTPSURFACE;

/* RTTINTYPE */
typedef struct
{
  uint8_t type;
  uint8_t flags;
  RTGBOX *bbox;
  int32_t srid;
  int ngeoms;   /* how many geometries we are currently storing */
  int maxgeoms; /* how many geometries we have space for in **geoms */
  RTTRIANGLE **geoms;
}
RTTIN;

/* Casts RTGEOM->RT* (return NULL if cast is illegal) */
extern RTMPOLY *rtgeom_as_rtmpoly(const RTCTX *ctx, const RTGEOM *rtgeom);
extern RTMLINE *rtgeom_as_rtmline(const RTCTX *ctx, const RTGEOM *rtgeom);
extern RTMPOINT *rtgeom_as_rtmpoint(const RTCTX *ctx, const RTGEOM *rtgeom);
extern RTCOLLECTION *rtgeom_as_rtcollection(const RTCTX *ctx, const RTGEOM *rtgeom);
extern RTPOLY *rtgeom_as_rtpoly(const RTCTX *ctx, const RTGEOM *rtgeom);
extern RTLINE *rtgeom_as_rtline(const RTCTX *ctx, const RTGEOM *rtgeom);
extern RTPOINT *rtgeom_as_rtpoint(const RTCTX *ctx, const RTGEOM *rtgeom);
extern RTCIRCSTRING *rtgeom_as_rtcircstring(const RTCTX *ctx, const RTGEOM *rtgeom);
extern RTCURVEPOLY *rtgeom_as_rtcurvepoly(const RTCTX *ctx, const RTGEOM *rtgeom);
extern RTCOMPOUND *rtgeom_as_rtcompound(const RTCTX *ctx, const RTGEOM *rtgeom);
extern RTPSURFACE *rtgeom_as_rtpsurface(const RTCTX *ctx, const RTGEOM *rtgeom);
extern RTTRIANGLE *rtgeom_as_rttriangle(const RTCTX *ctx, const RTGEOM *rtgeom);
extern RTTIN *rtgeom_as_rttin(const RTCTX *ctx, const RTGEOM *rtgeom);
extern RTGEOM *rtgeom_as_multi(const RTCTX *ctx, const RTGEOM *rtgeom);
extern RTGEOM *rtgeom_as_curve(const RTCTX *ctx, const RTGEOM *rtgeom);

/* Casts RT*->RTGEOM (artays cast) */
extern RTGEOM *rttin_as_rtgeom(const RTCTX *ctx, const RTTIN *obj);
extern RTGEOM *rttriangle_as_rtgeom(const RTCTX *ctx, const RTTRIANGLE *obj);
extern RTGEOM *rtpsurface_as_rtgeom(const RTCTX *ctx, const RTPSURFACE *obj);
extern RTGEOM *rtmpoly_as_rtgeom(const RTCTX *ctx, const RTMPOLY *obj);
extern RTGEOM *rtmline_as_rtgeom(const RTCTX *ctx, const RTMLINE *obj);
extern RTGEOM *rtmpoint_as_rtgeom(const RTCTX *ctx, const RTMPOINT *obj);
extern RTGEOM *rtcollection_as_rtgeom(const RTCTX *ctx, const RTCOLLECTION *obj);
extern RTGEOM *rtcircstring_as_rtgeom(const RTCTX *ctx, const RTCIRCSTRING *obj);
extern RTGEOM *rtcompound_as_rtgeom(const RTCTX *ctx, const RTCOMPOUND *obj);
extern RTGEOM *rtcurvepoly_as_rtgeom(const RTCTX *ctx, const RTCURVEPOLY *obj);
extern RTGEOM *rtpoly_as_rtgeom(const RTCTX *ctx, const RTPOLY *obj);
extern RTGEOM *rtline_as_rtgeom(const RTCTX *ctx, const RTLINE *obj);
extern RTGEOM *rtpoint_as_rtgeom(const RTCTX *ctx, const RTPOINT *obj);


extern RTCOLLECTION* rtcollection_add_rtgeom(const RTCTX *ctx, RTCOLLECTION *col, const RTGEOM *geom);
extern RTMPOINT* rtmpoint_add_rtpoint(const RTCTX *ctx, RTMPOINT *mobj, const RTPOINT *obj);
extern RTMLINE* rtmline_add_rtline(const RTCTX *ctx, RTMLINE *mobj, const RTLINE *obj);
extern RTMPOLY* rtmpoly_add_rtpoly(const RTCTX *ctx, RTMPOLY *mobj, const RTPOLY *obj);
extern RTPSURFACE* rtpsurface_add_rtpoly(const RTCTX *ctx, RTPSURFACE *mobj, const RTPOLY *obj);
extern RTTIN* rttin_add_rttriangle(const RTCTX *ctx, RTTIN *mobj, const RTTRIANGLE *obj);



/***********************************************************************
** Utility functions for flag byte and srid_flag integer.
*/

/**
* Construct a new flags char.
*/
extern uint8_t gflags(const RTCTX *ctx, int hasz, int hasm, int geodetic);

/**
* Extract the geometry type from the serialized form (it hides in
* the anonymous data area, so this is a handy function).
*/
extern uint32_t gserialized_get_type(const RTCTX *ctx, const GSERIALIZED *g);

/**
* Returns the size in bytes to read from toast to get the basic
* information from a geometry: GSERIALIZED struct, bbox and type
*/
extern uint32_t gserialized_max_header_size(const RTCTX *ctx);

/**
* Extract the SRID from the serialized form (it is packed into
* three bytes so this is a handy function).
*/
extern int32_t gserialized_get_srid(const RTCTX *ctx, const GSERIALIZED *g);

/**
* Write the SRID into the serialized form (it is packed into
* three bytes so this is a handy function).
*/
extern void gserialized_set_srid(const RTCTX *ctx, GSERIALIZED *g, int32_t srid);

/**
* Check if a #GSERIALIZED is empty without deserializing first.
* Only checks if the number of elements of the parent geometry
* is zero, will not catch collections of empty, eg:
* GEOMETRYCOLLECTION(POINT EMPTY)
*/
extern int gserialized_is_empty(const RTCTX *ctx, const GSERIALIZED *g);

/**
* Check if a #GSERIALIZED has a bounding box without deserializing first.
*/
extern int gserialized_has_bbox(const RTCTX *ctx, const GSERIALIZED *gser);

/**
* Check if a #GSERIALIZED has a Z ordinate.
*/
extern int gserialized_has_z(const RTCTX *ctx, const GSERIALIZED *gser);

/**
* Check if a #GSERIALIZED has an M ordinate.
*/
extern int gserialized_has_m(const RTCTX *ctx, const GSERIALIZED *gser);

/**
* Check if a #GSERIALIZED is a geography.
*/
extern int gserialized_is_geodetic(const RTCTX *ctx, const GSERIALIZED *gser);

/**
* Return a number indicating presence of Z and M coordinates.
* 0 = None, 1 = M, 2 = Z, 3 = ZM
*/
extern int gserialized_get_zm(const RTCTX *ctx, const GSERIALIZED *gser);

/**
* Return the number of dimensions (2, 3, 4) in a geometry
*/
extern int gserialized_ndims(const RTCTX *ctx, const GSERIALIZED *gser);


/**
* Call this function to drop BBOX and SRID
* from RTGEOM. If RTGEOM type is *not* flagged
* with the HASBBOX flag and has a bbox, it
* will be released.
*/
extern void rtgeom_drop_bbox(const RTCTX *ctx, RTGEOM *rtgeom);
extern void rtgeom_drop_srid(const RTCTX *ctx, RTGEOM *rtgeom);

/**
 * Compute a bbox if not already computed
 *
 * After calling this function rtgeom->bbox is only
 * NULL if the geometry is empty.
 */
extern void rtgeom_add_bbox(const RTCTX *ctx, RTGEOM *rtgeom);
/**
* Compute a box for geom and all sub-geometries, if not already computed
*/
extern void rtgeom_add_bbox_deep(const RTCTX *ctx, RTGEOM *rtgeom, RTGBOX *gbox);

/**
 * Get a non-empty geometry bounding box, computing and
 * caching it if not already there
 *
 * NOTE: empty geometries don't have a bounding box so
 *       you'd still get a NULL for them.
 */
extern const RTGBOX *rtgeom_get_bbox(const RTCTX *ctx, const RTGEOM *rtgeom);

/**
* Determine whether a RTGEOM can contain sub-geometries or not
*/
extern int rtgeom_is_collection(const RTCTX *ctx, const RTGEOM *rtgeom);

/******************************************************************/
/* Functions that work on type numbers */

/**
* Determine whether a type number is a collection or not
*/
extern int rttype_is_collection(const RTCTX *ctx, uint8_t type);

/**
* Given an rttype number, what homogeneous collection can hold it?
*/
extern int rttype_get_collectiontype(const RTCTX *ctx, uint8_t type);

/**
* Return the type name string associated with a type number
* (e.g. Point, LineString, Polygon)
*/
extern const char *rttype_name(const RTCTX *ctx, uint8_t type);


/******************************************************************/

/*
 * copies a point from the point array into the parameter point
 * will set point's z=0 (or NaN) if pa is 2d
 * will set point's m=0 (or NaN) if pa is 3d or 2d
 * NOTE: point is a real POINT3D *not* a pointer
 */
extern RTPOINT4D rt_getPoint4d(const RTCTX *ctx, const RTPOINTARRAY *pa, int n);

/*
 * copies a point from the point array into the parameter point
 * will set point's z=0 (or NaN) if pa is 2d
 * will set point's m=0 (or NaN) if pa is 3d or 2d
 * NOTE: this will modify the point4d pointed to by 'point'.
 */
extern int rt_getPoint4d_p(const RTCTX *ctx, const RTPOINTARRAY *pa, int n, RTPOINT4D *point);

/*
 * copies a point from the point array into the parameter point
 * will set point's z=0 (or NaN) if pa is 2d
 * NOTE: point is a real POINT3D *not* a pointer
 */
extern RTPOINT3DZ rt_getPoint3dz(const RTCTX *ctx, const RTPOINTARRAY *pa, int n);
extern RTPOINT3DM rt_getPoint3dm(const RTCTX *ctx, const RTPOINTARRAY *pa, int n);

/*
 * copies a point from the point array into the parameter point
 * will set point's z=0 (or NaN) if pa is 2d
 * NOTE: this will modify the point3d pointed to by 'point'.
 */
extern int rt_getPoint3dz_p(const RTCTX *ctx, const RTPOINTARRAY *pa, int n, RTPOINT3DZ *point);
extern int rt_getPoint3dm_p(const RTCTX *ctx, const RTPOINTARRAY *pa, int n, RTPOINT3DM *point);


/*
 * copies a point from the point array into the parameter point
 * z value (if present is not returned)
 * NOTE: point is a real POINT3D *not* a pointer
 */
extern RTPOINT2D rt_getPoint2d(const RTCTX *ctx, const RTPOINTARRAY *pa, int n);

/*
 * copies a point from the point array into the parameter point
 * z value (if present is not returned)
 * NOTE: this will modify the point2d pointed to by 'point'.
 */
extern int rt_getPoint2d_p(const RTCTX *ctx, const RTPOINTARRAY *pa, int n, RTPOINT2D *point);

/**
* Returns a RTPOINT2D pointer into the RTPOINTARRAY serialized_ptlist,
* suitable for reading from. This is very high performance
* and declared const because you aren't allowed to muck with the
* values, only read them.
*/
extern const RTPOINT2D* rt_getPoint2d_cp(const RTCTX *ctx, const RTPOINTARRAY *pa, int n);

/**
* Returns a RTPOINT3DZ pointer into the RTPOINTARRAY serialized_ptlist,
* suitable for reading from. This is very high performance
* and declared const because you aren't allowed to muck with the
* values, only read them.
*/
extern const RTPOINT3DZ* rt_getPoint3dz_cp(const RTCTX *ctx, const RTPOINTARRAY *pa, int n);

/**
* Returns a RTPOINT4D pointer into the RTPOINTARRAY serialized_ptlist,
* suitable for reading from. This is very high performance
* and declared const because you aren't allowed to muck with the
* values, only read them.
*/
extern const RTPOINT4D* rt_getPoint4d_cp(const RTCTX *ctx, const RTPOINTARRAY *pa, int n);

/*
 * set point N to the given value
 * NOTE that the pointarray can be of any
 * dimension, the appropriate ordinate values
 * will be extracted from it
 *
 * N must be a valid point index
 */
extern void ptarray_set_point4d(const RTCTX *ctx, RTPOINTARRAY *pa, int n, const RTPOINT4D *p4d);

/*
 * get a pointer to nth point of a RTPOINTARRAY
 * You'll need to cast it to appropriate dimensioned point.
 * Note that if you cast to a higher dimensional point you'll
 * possibly corrupt the RTPOINTARRAY.
 *
 * WARNING: Don't cast this to a POINT !
 * it would not be reliable due to memory alignment constraints
 */
extern uint8_t *rt_getPoint_internal(const RTCTX *ctx, const RTPOINTARRAY *pa, int n);

/*
 * size of point represeneted in the RTPOINTARRAY
 * 16 for 2d, 24 for 3d, 32 for 4d
 */
extern int ptarray_point_size(const RTCTX *ctx, const RTPOINTARRAY *pa);


/**
* Construct an empty pointarray, allocating storage and setting
* the npoints, but not filling in any information. Should be used in conjunction
* with ptarray_set_point4d to fill in the information in the array.
*/
extern RTPOINTARRAY* ptarray_construct(const RTCTX *ctx, char hasz, char hasm, uint32_t npoints);

/**
* Construct a new #RTPOINTARRAY, <em>copying</em> in the data from ptlist
*/
extern RTPOINTARRAY* ptarray_construct_copy_data(const RTCTX *ctx, char hasz, char hasm, uint32_t npoints, const uint8_t *ptlist);

/**
* Construct a new #RTPOINTARRAY, <em>referencing</em> to the data from ptlist
*/
extern RTPOINTARRAY* ptarray_construct_reference_data(const RTCTX *ctx, char hasz, char hasm, uint32_t npoints, uint8_t *ptlist);

/**
* Create a new #RTPOINTARRAY with no points. Allocate enough storage
* to hold maxpoints vertices before having to reallocate the storage
* area.
*/
extern RTPOINTARRAY* ptarray_construct_empty(const RTCTX *ctx, char hasz, char hasm, uint32_t maxpoints);

/**
* Append a point to the end of an existing #RTPOINTARRAY
* If allow_duplicate is RT_TRUE, then a duplicate point will
* not be added.
*/
extern int ptarray_append_point(const RTCTX *ctx, RTPOINTARRAY *pa, const RTPOINT4D *pt, int allow_duplicates);

/**
 * Append a #RTPOINTARRAY, pa2 to the end of an existing #RTPOINTARRAY, pa1.
 *
 * If gap_tolerance is >= 0 then the end point of pa1 will be checked for
 * being within gap_tolerance 2d distance from start point of pa2 or an
 * error will be raised and RT_FAILURE returned.
 * A gap_tolerance < 0 disables the check.
 *
 * If end point of pa1 and start point of pa2 are 2d-equal, then pa2 first
 * point will not be appended.
 */
extern int ptarray_append_ptarray(const RTCTX *ctx, RTPOINTARRAY *pa1, RTPOINTARRAY *pa2, double gap_tolerance);

/**
* Insert a point into an existing #RTPOINTARRAY. Zero
* is the index of the start of the array.
*/
extern int ptarray_insert_point(const RTCTX *ctx, RTPOINTARRAY *pa, const RTPOINT4D *p, int where);

/**
* Remove a point from an existing #RTPOINTARRAY. Zero
* is the index of the start of the array.
*/
extern int ptarray_remove_point(const RTCTX *ctx, RTPOINTARRAY *pa, int where);

/**
 * @brief Add a point in a pointarray.
 *
 * @param pa the source RTPOINTARRAY
 * @param p the point to add
 * @param pdims number of ordinates in p (2..4)
 * @param where to insert the point. 0 prepends, pa->npoints appends
 *
 * @returns a newly constructed RTPOINTARRAY using a newly allocated buffer
 *          for the actual points, or NULL on error.
 */
extern RTPOINTARRAY *ptarray_addPoint(const RTCTX *ctx, const RTPOINTARRAY *pa, uint8_t *p, size_t pdims, uint32_t where);

/**
 * @brief Remove a point from a pointarray.
 *   @param which -  is the offset (starting at 0)
 * @return #RTPOINTARRAY is newly allocated
 */
extern RTPOINTARRAY *ptarray_removePoint(const RTCTX *ctx, RTPOINTARRAY *pa, uint32_t where);

/**
 * @brief Merge two given RTPOINTARRAY and returns a pointer
 * on the new aggregate one.
 * Warning: this function free the two inputs RTPOINTARRAY
 * @return #RTPOINTARRAY is newly allocated
 */
extern RTPOINTARRAY *ptarray_merge(const RTCTX *ctx, RTPOINTARRAY *pa1, RTPOINTARRAY *pa2);

extern int ptarray_is_closed(const RTCTX *ctx, const RTPOINTARRAY *pa);
extern int ptarray_is_closed_2d(const RTCTX *ctx, const RTPOINTARRAY *pa);
extern int ptarray_is_closed_3d(const RTCTX *ctx, const RTPOINTARRAY *pa);
extern int ptarray_is_closed_z(const RTCTX *ctx, const RTPOINTARRAY *pa);
extern void ptarray_longitude_shift(const RTCTX *ctx, RTPOINTARRAY *pa);
extern int ptarray_isccw(const RTCTX *ctx, const RTPOINTARRAY *pa);
extern void ptarray_reverse(const RTCTX *ctx, RTPOINTARRAY *pa);
extern RTPOINTARRAY* ptarray_flip_coordinates(const RTCTX *ctx, RTPOINTARRAY *pa);

/**
 * @d1 start location (distance from start / total distance)
 * @d2   end location (distance from start / total distance)
 * @param tolerance snap to vertices at locations < tolerance away from given ones
 */
extern RTPOINTARRAY *ptarray_substring(const RTCTX *ctx, RTPOINTARRAY *pa, double d1, double d2,
                                                          double tolerance);


/**
* Strip out the Z/M components of an #RTGEOM
*/
extern RTGEOM* rtgeom_force_2d(const RTCTX *ctx, const RTGEOM *geom);
extern RTGEOM* rtgeom_force_3dz(const RTCTX *ctx, const RTGEOM *geom);
extern RTGEOM* rtgeom_force_3dm(const RTCTX *ctx, const RTGEOM *geom);
extern RTGEOM* rtgeom_force_4d(const RTCTX *ctx, const RTGEOM *geom);

extern RTGEOM* rtgeom_simplify(const RTCTX *ctx, const RTGEOM *igeom, double dist, int preserve_collapsed);
extern RTGEOM* rtgeom_set_effective_area(const RTCTX *ctx, const RTGEOM *igeom, int set_area, double area);

/*
 * Force to use SFS 1.1 geometry type
 * (rather than SFS 1.2 and/or SQL/MM)
 */
extern RTGEOM* rtgeom_force_sfs(const RTCTX *ctx, RTGEOM *geom, int version);


/*--------------------------------------------------------
 * all the base types (point/line/polygon) will have a
 * basic constructor, basic de-serializer, basic serializer,
 * bounding box finder and (TODO) serialized form size finder.
 *--------------------------------------------------------*/

/*
 * convenience functions to hide the RTPOINTARRAY
 */
extern int rtpoint_getPoint2d_p(const RTCTX *ctx, const RTPOINT *point, RTPOINT2D *out);
extern int rtpoint_getPoint3dz_p(const RTCTX *ctx, const RTPOINT *point, RTPOINT3DZ *out);
extern int rtpoint_getPoint3dm_p(const RTCTX *ctx, const RTPOINT *point, RTPOINT3DM *out);
extern int rtpoint_getPoint4d_p(const RTCTX *ctx, const RTPOINT *point, RTPOINT4D *out);

/******************************************************************
 * RTLINE functions
 ******************************************************************/

/**
 * Add a RTPOINT to an RTLINE
 */
extern int rtline_add_rtpoint(const RTCTX *ctx, RTLINE *line, RTPOINT *point, int where);

/******************************************************************
 * RTPOLY functions
 ******************************************************************/

/**
* Add a ring, allocating extra space if necessary. The polygon takes
* ownership of the passed point array.
*/
extern int rtpoly_add_ring(const RTCTX *ctx, RTPOLY *poly, RTPOINTARRAY *pa);

/**
* Add a ring, allocating extra space if necessary. The curvepolygon takes
* ownership of the passed point array.
*/
extern int rtcurvepoly_add_ring(const RTCTX *ctx, RTCURVEPOLY *poly, RTGEOM *ring);

/**
* Add a component, allocating extra space if necessary. The compoundcurve
* takes owership of the passed geometry.
*/
extern int rtcompound_add_rtgeom(const RTCTX *ctx, RTCOMPOUND *comp, RTGEOM *geom);

/**
* Construct an equivalent compound curve from a linestring.
* Compound curves can have linear components, so this works fine
*/
extern RTCOMPOUND* rtcompound_construct_from_rtline(const RTCTX *ctx, const RTLINE *rtpoly);

/**
* Construct an equivalent curve polygon from a polygon. Curve polygons
* can have linear rings as their rings, so this works fine (in theory?)
*/
extern RTCURVEPOLY* rtcurvepoly_construct_from_rtpoly(const RTCTX *ctx, RTPOLY *rtpoly);


/******************************************************************
 * RTGEOM functions
 ******************************************************************/

extern int rtcollection_ngeoms(const RTCTX *ctx, const RTCOLLECTION *col);

/* Given a generic geometry/collection, return the "simplest" form. */
extern RTGEOM *rtgeom_homogenize(const RTCTX *ctx, const RTGEOM *geom);


/******************************************************************
 * RTMULTIx and RTCOLLECTION functions
 ******************************************************************/

RTGEOM *rtcollection_getsubgeom(const RTCTX *ctx, RTCOLLECTION *col, int gnum);
RTCOLLECTION* rtcollection_extract(const RTCTX *ctx, RTCOLLECTION *col, int type);


/******************************************************************
 * SERIALIZED FORM functions
 ******************************************************************/

/**
* Set the SRID on an RTGEOM
* For collections, only the parent gets an SRID, all
* the children get SRID_UNKNOWN.
*/
extern void rtgeom_set_srid(const RTCTX *ctx, RTGEOM *geom, int srid);

/****************************************************************
 * MEMORY MANAGEMENT
 ****************************************************************/

/*
* The *_free family of functions frees *all* memory associated
* with the pointer. When the recursion gets to the level of the
* RTPOINTARRAY, the RTPOINTARRAY is only freed if it is not flagged
* as "read only". RTGEOMs constructed on top of GSERIALIZED
* from PgSQL use read only point arrays.
*/

extern void ptarray_free(const RTCTX *ctx, RTPOINTARRAY *pa);
extern void rtpoint_free(const RTCTX *ctx, RTPOINT *pt);
extern void rtline_free(const RTCTX *ctx, RTLINE *line);
extern void rtpoly_free(const RTCTX *ctx, RTPOLY *poly);
extern void rttriangle_free(const RTCTX *ctx, RTTRIANGLE *triangle);
extern void rtmpoint_free(const RTCTX *ctx, RTMPOINT *mpt);
extern void rtmline_free(const RTCTX *ctx, RTMLINE *mline);
extern void rtmpoly_free(const RTCTX *ctx, RTMPOLY *mpoly);
extern void rtpsurface_free(const RTCTX *ctx, RTPSURFACE *psurf);
extern void rttin_free(const RTCTX *ctx, RTTIN *tin);
extern void rtcollection_free(const RTCTX *ctx, RTCOLLECTION *col);
extern void rtcircstring_free(const RTCTX *ctx, RTCIRCSTRING *curve);
extern void rtgeom_free(const RTCTX *ctx, RTGEOM *geom);

/*
* The *_release family of functions frees the RTGEOM structures
* surrounding the RTPOINTARRAYs but leaves the RTPOINTARRAYs
* intact. Useful when re-shaping geometries between types,
* or splicing geometries together.
*/

extern void rtpoint_release(const RTCTX *ctx, RTPOINT *rtpoint);
extern void rtline_release(const RTCTX *ctx, RTLINE *rtline);
extern void rtpoly_release(const RTCTX *ctx, RTPOLY *rtpoly);
extern void rttriangle_release(const RTCTX *ctx, RTTRIANGLE *rttriangle);
extern void rtcircstring_release(const RTCTX *ctx, RTCIRCSTRING *rtcirc);
extern void rtmpoint_release(const RTCTX *ctx, RTMPOINT *rtpoint);
extern void rtmline_release(const RTCTX *ctx, RTMLINE *rtline);
extern void rtmpoly_release(const RTCTX *ctx, RTMPOLY *rtpoly);
extern void rtpsurface_release(RTPSURFACE *rtpsurface);
extern void rttin_release(RTTIN *rttin);
extern void rtcollection_release(const RTCTX *ctx, RTCOLLECTION *rtcollection);
extern void rtgeom_release(const RTCTX *ctx, RTGEOM *rtgeom);


/****************************************************************
* Utility
****************************************************************/

extern void printPA(const RTCTX *ctx, RTPOINTARRAY *pa);
extern void printRTPOINT(const RTCTX *ctx, RTPOINT *point);
extern void printRTLINE(const RTCTX *ctx, RTLINE *line);
extern void printRTPOLY(const RTCTX *ctx, RTPOLY *poly);
extern void printRTTRIANGLE(const RTCTX *ctx, RTTRIANGLE *triangle);
extern void printRTPSURFACE(const RTCTX *ctx, RTPSURFACE *psurf);
extern void printRTTIN(const RTCTX *ctx, RTTIN *tin);

extern float  next_float_down(const RTCTX *ctx, double d);
extern float  next_float_up(const RTCTX *ctx, double d);
extern double next_double_down(const RTCTX *ctx, float d);
extern double next_double_up(const RTCTX *ctx, float d);

/* general utilities 2D */
extern double  distance2d_pt_pt(const RTCTX *ctx, const RTPOINT2D *p1, const RTPOINT2D *p2);
extern double  distance2d_sqr_pt_pt(const RTCTX *ctx, const RTPOINT2D *p1, const RTPOINT2D *p2);
extern double  distance2d_pt_seg(const RTCTX *ctx, const RTPOINT2D *p, const RTPOINT2D *A, const RTPOINT2D *B);
extern double  distance2d_sqr_pt_seg(const RTCTX *ctx, const RTPOINT2D *p, const RTPOINT2D *A, const RTPOINT2D *B);
extern RTGEOM* rtgeom_closest_line(const RTCTX *ctx, const RTGEOM *rt1, const RTGEOM *rt2);
extern RTGEOM* rtgeom_furthest_line(const RTCTX *ctx, const RTGEOM *rt1, const RTGEOM *rt2);
extern RTGEOM* rtgeom_closest_point(const RTCTX *ctx, const RTGEOM *rt1, const RTGEOM *rt2);
extern RTGEOM* rtgeom_furthest_point(const RTCTX *ctx, const RTGEOM *rt1, const RTGEOM *rt2);
extern double  rtgeom_mindistance2d(const RTCTX *ctx, const RTGEOM *rt1, const RTGEOM *rt2);
extern double  rtgeom_mindistance2d_tolerance(const RTCTX *ctx, const RTGEOM *rt1, const RTGEOM *rt2, double tolerance);
extern double  rtgeom_maxdistance2d(const RTCTX *ctx, const RTGEOM *rt1, const RTGEOM *rt2);
extern double  rtgeom_maxdistance2d_tolerance(const RTCTX *ctx, const RTGEOM *rt1, const RTGEOM *rt2, double tolerance);

/* 3D */
extern double distance3d_pt_pt(const RTCTX *ctx, const POINT3D *p1, const POINT3D *p2);
extern double distance3d_pt_seg(const POINT3D *p, const POINT3D *A, const POINT3D *B);

extern RTGEOM* rtgeom_furthest_line_3d(const RTCTX *ctx, RTGEOM *rt1, RTGEOM *rt2);
extern RTGEOM* rtgeom_closest_line_3d(const RTCTX *ctx, const RTGEOM *rt1, const RTGEOM *rt2);
extern RTGEOM* rtgeom_closest_point_3d(const RTCTX *ctx, const RTGEOM *rt1, const RTGEOM *rt2);


extern double rtgeom_mindistance3d(const RTCTX *ctx, const RTGEOM *rt1, const RTGEOM *rt2);
extern double rtgeom_mindistance3d_tolerance(const RTCTX *ctx, const RTGEOM *rt1, const RTGEOM *rt2, double tolerance);
extern double rtgeom_maxdistance3d(const RTCTX *ctx, const RTGEOM *rt1, const RTGEOM *rt2);
extern double rtgeom_maxdistance3d_tolerance(const RTCTX *ctx, const RTGEOM *rt1, const RTGEOM *rt2, double tolerance);

extern double rtgeom_area(const RTCTX *ctx, const RTGEOM *geom);
extern double rtgeom_length(const RTCTX *ctx, const RTGEOM *geom);
extern double rtgeom_length_2d(const RTCTX *ctx, const RTGEOM *geom);
extern double rtgeom_perimeter(const RTCTX *ctx, const RTGEOM *geom);
extern double rtgeom_perimeter_2d(const RTCTX *ctx, const RTGEOM *geom);
extern void rtgeom_affine(const RTCTX *ctx, RTGEOM *geom, const RTAFFINE *affine);
extern void rtgeom_scale(const RTCTX *ctx, RTGEOM *geom, const RTPOINT4D *factors);
extern int rtgeom_dimension(const RTCTX *ctx, const RTGEOM *geom);

extern RTPOINT* rtline_get_rtpoint(const RTCTX *ctx, const RTLINE *line, int where);
extern RTPOINT* rtcircstring_get_rtpoint(const RTCTX *ctx, const RTCIRCSTRING *circ, int where);

extern RTPOINT* rtcompound_get_startpoint(const RTCTX *ctx, const RTCOMPOUND *rtcmp);
extern RTPOINT* rtcompound_get_endpoint(const RTCTX *ctx, const RTCOMPOUND *rtcmp);
extern RTPOINT* rtcompound_get_rtpoint(const RTCTX *ctx, const RTCOMPOUND *rtcmp, int where);


extern double ptarray_length_2d(const RTCTX *ctx, const RTPOINTARRAY *pts);
extern double ptarray_length(const RTCTX *ctx, const RTPOINTARRAY *pts);
extern double ptarray_arc_length_2d(const RTCTX *ctx, const RTPOINTARRAY *pts);


extern int pt_in_ring_2d(const RTCTX *ctx, const RTPOINT2D *p, const RTPOINTARRAY *ring);
extern int azimuth_pt_pt(const RTCTX *ctx, const RTPOINT2D *p1, const RTPOINT2D *p2, double *ret);
extern int rtpoint_inside_circle(const RTCTX *ctx, const RTPOINT *p, double cx, double cy, double rad);
extern void rtgeom_reverse(const RTCTX *ctx, RTGEOM *rtgeom);
extern void rtline_reverse(const RTCTX *ctx, RTLINE *line);
extern void rtpoly_reverse(const RTCTX *ctx, RTPOLY *poly);
extern void rttriangle_reverse(const RTCTX *ctx, RTTRIANGLE *triangle);
extern char* rtgeom_summary(const RTCTX *ctx, const RTGEOM *rtgeom, int offset);
extern char* rtpoint_to_latlon(const RTCTX *ctx, const RTPOINT *p, const char *format);
extern int rtgeom_startpoint(const RTCTX *ctx, const RTGEOM* rtgeom, RTPOINT4D* pt);

/**
* Ensure the outer ring is clockwise oriented and all inner rings
* are counter-clockwise.
*/
extern void rtgeom_force_clockwise(const RTCTX *ctx, RTGEOM *rtgeom);
extern void rtpoly_force_clockwise(const RTCTX *ctx, RTPOLY *poly);
extern void rttriangle_force_clockwise(const RTCTX *ctx, RTTRIANGLE *triangle);


extern void interpolate_point4d(const RTCTX *ctx, RTPOINT4D *A, RTPOINT4D *B, RTPOINT4D *I, double F);

void rtgeom_longitude_shift(const RTCTX *ctx, RTGEOM *rtgeom);


/**
* @brief Check whether or not a rtgeom is big enough to warrant a bounding box.
*
* Check whether or not a rtgeom is big enough to warrant a bounding box
* when stored in the serialized form on disk. Currently only points are
* considered small enough to not require a bounding box, because the
* index operations can generate a large number of box-retrieval operations
* when scanning keys.
*/
extern int rtgeom_needs_bbox(const RTCTX *ctx, const RTGEOM *geom);

/**
* Count the total number of vertices in any #RTGEOM.
*/
extern int rtgeom_count_vertices(const RTCTX *ctx, const RTGEOM *geom);

/**
* Count the total number of rings in any #RTGEOM. Multipolygons
* and other collections get counted, not the same as OGC st_numrings.
*/
extern int rtgeom_count_rings(const RTCTX *ctx, const RTGEOM *geom);

/**
* Return true or false depending on whether a geometry has
* a valid SRID set.
*/
extern int rtgeom_has_srid(const RTCTX *ctx, const RTGEOM *geom);

/**
* Return true or false depending on whether a geometry is an "empty"
* geometry (no vertices members)
*/
extern int rtgeom_is_empty(const RTCTX *ctx, const RTGEOM *geom);

/**
* Return true or false depending on whether a geometry is a linear
* feature that closes on itself.
*/
extern int rtgeom_is_closed(const RTCTX *ctx, const RTGEOM *geom);

/**
* Return the dimensionality (relating to point/line/poly) of an rtgeom
*/
extern int rtgeom_dimensionality(const RTCTX *ctx, RTGEOM *geom);

/* Is rtgeom1 geometrically equal to rtgeom2 ? */
char rtgeom_same(const RTCTX *ctx, const RTGEOM *rtgeom1, const RTGEOM *rtgeom2);
char ptarray_same(const RTCTX *ctx, const RTPOINTARRAY *pa1, const RTPOINTARRAY *pa2);
char rtpoint_same(const RTCTX *ctx, const RTPOINT *p1, const RTPOINT *p2);
char rtline_same(const RTCTX *ctx, const RTLINE *p1, const RTLINE *p2);
char rtpoly_same(const RTCTX *ctx, const RTPOLY *p1, const RTPOLY *p2);
char rttriangle_same(const RTCTX *ctx, const RTTRIANGLE *p1, const RTTRIANGLE *p2);
char rtcollection_same(const RTCTX *ctx, const RTCOLLECTION *p1, const RTCOLLECTION *p2);
char rtcircstring_same(const RTCTX *ctx, const RTCIRCSTRING *p1, const RTCIRCSTRING *p2);


/**
 * @brief Clone RTGEOM object. Serialized point lists are not copied.
 *
 * #RTGBOX are copied
 *
 * @see ptarray_clone
 */
extern RTGEOM *rtgeom_clone(const RTCTX *ctx, const RTGEOM *rtgeom);

/**
* Deep clone an RTGEOM, everything is copied
*/
extern RTGEOM *rtgeom_clone_deep(const RTCTX *ctx, const RTGEOM *rtgeom);

/* TODO Move to Internal */
RTPOINT *rtpoint_clone(const RTCTX *ctx, const RTPOINT *rtgeom);
RTPOINTARRAY *ptarray_clone_deep(const RTCTX *ctx, const RTPOINTARRAY *ptarray);


/*
* Geometry constructors. These constructors to not copy the point arrays
* passed to them, they just take references, so do not free them out
* from underneath the geometries.
*/
extern RTPOINT* rtpoint_construct(const RTCTX *ctx, int srid, RTGBOX *bbox, RTPOINTARRAY *point);
extern RTMPOINT *rtmpoint_construct(const RTCTX *ctx, int srid, const RTPOINTARRAY *pa);
extern RTLINE* rtline_construct(const RTCTX *ctx, int srid, RTGBOX *bbox, RTPOINTARRAY *points);
extern RTCIRCSTRING* rtcircstring_construct(const RTCTX *ctx, int srid, RTGBOX *bbox, RTPOINTARRAY *points);
extern RTPOLY* rtpoly_construct(const RTCTX *ctx, int srid, RTGBOX *bbox, uint32_t nrings, RTPOINTARRAY **points);
extern RTCURVEPOLY* rtcurvepoly_construct(int srid, RTGBOX *bbox, uint32_t nrings, RTGEOM **geoms);
extern RTTRIANGLE* rttriangle_construct(const RTCTX *ctx, int srid, RTGBOX *bbox, RTPOINTARRAY *points);
extern RTCOLLECTION* rtcollection_construct(const RTCTX *ctx, uint8_t type, int srid, RTGBOX *bbox, uint32_t ngeoms, RTGEOM **geoms);
/*
* Empty geometry constructors.
*/
extern RTGEOM* rtgeom_construct_empty(const RTCTX *ctx, uint8_t type, int srid, char hasz, char hasm);
extern RTPOINT* rtpoint_construct_empty(const RTCTX *ctx, int srid, char hasz, char hasm);
extern RTLINE* rtline_construct_empty(const RTCTX *ctx, int srid, char hasz, char hasm);
extern RTPOLY* rtpoly_construct_empty(const RTCTX *ctx, int srid, char hasz, char hasm);
extern RTCURVEPOLY* rtcurvepoly_construct_empty(const RTCTX *ctx, int srid, char hasz, char hasm);
extern RTCIRCSTRING* rtcircstring_construct_empty(const RTCTX *ctx, int srid, char hasz, char hasm);
extern RTCOMPOUND* rtcompound_construct_empty(const RTCTX *ctx, int srid, char hasz, char hasm);
extern RTTRIANGLE* rttriangle_construct_empty(const RTCTX *ctx, int srid, char hasz, char hasm);
extern RTMPOINT* rtmpoint_construct_empty(const RTCTX *ctx, int srid, char hasz, char hasm);
extern RTMLINE* rtmline_construct_empty(const RTCTX *ctx, int srid, char hasz, char hasm);
extern RTMPOLY* rtmpoly_construct_empty(const RTCTX *ctx, int srid, char hasz, char hasm);
extern RTCOLLECTION* rtcollection_construct_empty(const RTCTX *ctx, uint8_t type, int srid, char hasz, char hasm);


/* Other constructors */
extern RTPOINT *rtpoint_make2d(const RTCTX *ctx, int srid, double x, double y);
extern RTPOINT *rtpoint_make3dz(const RTCTX *ctx, int srid, double x, double y, double z);
extern RTPOINT *rtpoint_make3dm(const RTCTX *ctx, int srid, double x, double y, double m);
extern RTPOINT *rtpoint_make4d(const RTCTX *ctx, int srid, double x, double y, double z, double m);
extern RTPOINT *rtpoint_make(const RTCTX *ctx, int srid, int hasz, int hasm, const RTPOINT4D *p);
extern RTLINE *rtline_from_rtgeom_array(const RTCTX *ctx, int srid, uint32_t ngeoms, RTGEOM **geoms);
extern RTLINE *rtline_from_ptarray(const RTCTX *ctx, int srid, uint32_t npoints, RTPOINT **points); /* TODO: deprecate */
extern RTLINE *rtline_from_rtmpoint(const RTCTX *ctx, int srid, const RTMPOINT *mpoint);
extern RTLINE *rtline_addpoint(RTLINE *line, RTPOINT *point, uint32_t where);
extern RTLINE *rtline_removepoint(const RTCTX *ctx, RTLINE *line, uint32_t which);
extern void rtline_setPoint4d(const RTCTX *ctx, RTLINE *line, uint32_t which, RTPOINT4D *newpoint);
extern RTPOLY *rtpoly_from_rtlines(const RTCTX *ctx, const RTLINE *shell, uint32_t nholes, const RTLINE **holes);
extern RTTRIANGLE *rttriangle_from_rtline(const RTCTX *ctx, const RTLINE *shell);


/* Some point accessors */
extern double rtpoint_get_x(const RTCTX *ctx, const RTPOINT *point);
extern double rtpoint_get_y(const RTCTX *ctx, const RTPOINT *point);
extern double rtpoint_get_z(const RTCTX *ctx, const RTPOINT *point);
extern double rtpoint_get_m(const RTCTX *ctx, const RTPOINT *point);

/**
* Return SRID number
*/
extern int32_t rtgeom_get_srid(const RTCTX *ctx, const RTGEOM *geom);

/**
* Return RTRTTYPE number
*/
extern uint32_t rtgeom_get_type(const RTCTX *ctx, const RTGEOM *geom);

/**
* Return #RT_TRUE if geometry has Z ordinates
*/
extern int rtgeom_has_z(const RTCTX *ctx, const RTGEOM *geom);

/**
* Return #RT_TRUE if geometry has M ordinates.
*/
extern int rtgeom_has_m(const RTCTX *ctx, const RTGEOM *geom);

/**
* Return the number of dimensions (2, 3, 4) in a geometry
*/
extern int rtgeom_ndims(const RTCTX *ctx, const RTGEOM *geom);

/*
 * Given a point, returns the location of closest point on pointarray
 * as a fraction of total length (0: first point -- 1: last point).
 *
 * If not-null, the third argument will be set to the actual distance
 * of the point from the pointarray.
 */
extern double ptarray_locate_point(const RTCTX *ctx, const RTPOINTARRAY *pa, const RTPOINT4D *pt, double *dist, RTPOINT4D *p_located);

/**
* Add a measure dimension to a line, interpolating linearly from the start
* to the end value.
*/
extern RTLINE *rtline_measured_from_rtline(const RTCTX *ctx, const RTLINE *rtline, double m_start, double m_end);
extern RTMLINE* rtmline_measured_from_rtmline(const RTCTX *ctx, const RTMLINE *rtmline, double m_start, double m_end);

/**
* Determine the location(s) along a measured line where m occurs and
* return as a multipoint. Offset to left (positive) or right (negative).
*/
extern RTGEOM* rtgeom_locate_along(const RTCTX *ctx, const RTGEOM *rtin, double m, double offset);

/**
* Determine the segments along a measured line that fall within the m-range
* given. Return as a multiline or geometrycollection.
* Offset to left (positive) or right (negative).
*/
extern RTCOLLECTION* rtgeom_locate_between(const RTCTX *ctx, const RTGEOM *rtin, double from, double to, double offset);

/**
* Find the measure value at the location on the line closest to the point.
*/
extern double rtgeom_interpolate_point(const RTCTX *ctx, const RTGEOM *rtin, const RTPOINT *rtpt);

/**
* Find the time of closest point of approach
*
* @param mindist if not null will be set to the minimum distance between
*                the trajectories at the closest point of approach.
*
* @return the time value in which the minimum distance was reached, -1
*         if inputs are invalid (rterror is called in that case),
*         -2 if the trajectories do not share any point in time.
*/
extern double rtgeom_tcpa(const RTCTX *ctx, const RTGEOM *g1, const RTGEOM *g2, double *mindist);

/**
* Is the closest point of approach within a distance ?
*
* @return RT_TRUE or RT_FALSE
*/
extern int rtgeom_cpa_within(const RTCTX *ctx, const RTGEOM *g1, const RTGEOM *g2, double maxdist);

/**
* Return RT_TRUE or RT_FALSE depending on whether or not a geometry is
* a linestring with measure value growing from start to end vertex
*/
extern int rtgeom_is_trajectory(const RTCTX *ctx, const RTGEOM *geom);
extern int rtline_is_trajectory(const RTCTX *ctx, const RTLINE *geom);

/*
 * Ensure every segment is at most 'dist' long.
 * Returned RTGEOM might is unchanged if a POINT.
 */
extern RTGEOM *rtgeom_segmentize2d(const RTCTX *ctx, RTGEOM *line, double dist);
extern RTPOINTARRAY *ptarray_segmentize2d(const RTCTX *ctx, const RTPOINTARRAY *ipa, double dist);
extern RTLINE *rtline_segmentize2d(const RTCTX *ctx, RTLINE *line, double dist);
extern RTPOLY *rtpoly_segmentize2d(const RTCTX *ctx, RTPOLY *line, double dist);
extern RTCOLLECTION *rtcollection_segmentize2d(const RTCTX *ctx, RTCOLLECTION *coll, double dist);

/**
* Calculate the GeoHash (http://geohash.org) string for a geometry. Caller must free.
*/
char *rtgeom_geohash(const RTCTX *ctx, const RTGEOM *rtgeom, int precision);
unsigned int geohash_point_as_int(const RTCTX *ctx, RTPOINT2D *pt);


/**
* The return values of rtline_crossing_direction(const RTCTX *ctx)
*/
enum RTCG_LINE_CROSS_TYPE {
    LINE_NO_CROSS = 0,
    LINE_CROSS_LEFT = -1,
    LINE_CROSS_RIGHT = 1,
    LINE_MULTICROSS_END_LEFT = -2,
    LINE_MULTICROSS_END_RIGHT = 2,
    LINE_MULTICROSS_END_SAME_FIRST_LEFT = -3,
    LINE_MULTICROSS_END_SAME_FIRST_RIGHT = 3
};

/**
* Given two lines, characterize how (and if) they cross each other
*/
int rtline_crossing_direction(const RTCTX *ctx, const RTLINE *l1, const RTLINE *l2);

/**
* Given a geometry clip  based on the from/to range of one of its ordinates (x, y, z, m). Use for m- and z- clipping.
*/
RTCOLLECTION* rtgeom_clip_to_ordinate_range(const RTCTX *ctx, const RTGEOM *rtin, char ordinate, double from, double to, double offset);

/**
 * Macros for specifying GML options.
 * @{
 */
/** For GML3 only, include srsDimension attribute in output */
#define RT_GML_IS_DIMS     (1<<0)
/** For GML3 only, declare that datas are lat/lon. Swaps axis order */
#define RT_GML_IS_DEGREE   (1<<1)
/** For GML3, use <LineString> rather than <Curve> for lines */
#define RT_GML_SHORTLINE   (1<<2)
/** For GML2 and GML3, output only extent of geometry */
#define RT_GML_EXTENT      (1<<4)


#define IS_DIMS(x) ((x) & RT_GML_IS_DIMS)
#define IS_DEGREE(x) ((x) & RT_GML_IS_DEGREE)
/** @} */

/**
 * Macros for specifying X3D options.
 * @{
 */
/** For flip X/Y coordinates to Y/X */
#define RT_X3D_FLIP_XY     (1<<0)
#define RT_X3D_USE_GEOCOORDS     (1<<1)
#define X3D_USE_GEOCOORDS(x) ((x) & RT_X3D_USE_GEOCOORDS)



extern char* rtgeom_to_gml2(const RTCTX *ctx, const RTGEOM *geom, const char *srs, int precision, const char *prefix);
extern char* rtgeom_extent_to_gml2(const RTCTX *ctx, const RTGEOM *geom, const char *srs, int precision, const char *prefix);
/**
 * @param opts output options bitfield, see RT_GML macros for meaning
 */
extern char* rtgeom_extent_to_gml3(const RTCTX *ctx, const RTGEOM *geom, const char *srs, int precision, int opts, const char *prefix);
extern char* rtgeom_to_gml3(const RTCTX *ctx, const RTGEOM *geom, const char *srs, int precision, int opts, const char *prefix, const char *id);
extern char* rtgeom_to_kml2(const RTCTX *ctx, const RTGEOM *geom, int precision, const char *prefix);
extern char* rtgeom_to_geojson(const RTCTX *ctx, const RTGEOM *geo, char *srs, int precision, int has_bbox);
extern char* rtgeom_to_svg(const RTCTX *ctx, const RTGEOM *geom, int precision, int relative);
extern char* rtgeom_to_x3d3(const RTCTX *ctx, const RTGEOM *geom, char *srs, int precision, int opts, const char *defid);

/**
 * Create an RTGEOM object from a GeoJSON representation
 *
 * @param geojson the GeoJSON input
 * @param srs output parameter. Will be set to a newly allocated
 *            string holding the spatial reference string, or NULL
 *            if no such parameter is found in input.
 *            If not null, the pointer must be freed with rtfree.
 */
extern RTGEOM* rtgeom_from_geojson(const RTCTX *ctx, const char *geojson, char **srs);

/**
* Initialize a spheroid object for use in geodetic functions.
*/
extern void spheroid_init(const RTCTX *ctx, SPHEROID *s, double a, double b);

/**
* Calculate the geodetic distance from rtgeom1 to rtgeom2 on the spheroid.
* A spheroid with major axis == minor axis will be treated as a sphere.
* Pass in a tolerance in spheroid units.
*/
extern double rtgeom_distance_spheroid(const RTCTX *ctx, const RTGEOM *rtgeom1, const RTGEOM *rtgeom2, const SPHEROID *spheroid, double tolerance);

/**
* Calculate the location of a point on a spheroid, give a start point, bearing and distance.
*/
extern RTPOINT* rtgeom_project_spheroid(const RTCTX *ctx, const RTPOINT *r, const SPHEROID *spheroid, double distance, double azimuth);

/**
* Derive a new geometry with vertices added to ensure no vertex is more
* than max_seg_length (in radians) from any other vertex.
*/
extern RTGEOM* rtgeom_segmentize_sphere(const RTCTX *ctx, const RTGEOM *rtg_in, double max_seg_length);

/**
* Calculate the bearing between two points on a spheroid.
*/
extern double rtgeom_azumith_spheroid(const RTCTX *ctx, const RTPOINT *r, const RTPOINT *s, const SPHEROID *spheroid);

/**
* Calculate the geodetic area of a rtgeom on the sphere. The result
* will be multiplied by the average radius of the supplied spheroid.
*/
extern double rtgeom_area_sphere(const RTCTX *ctx, const RTGEOM *rtgeom, const SPHEROID *spheroid);

/**
* Calculate the geodetic area of a rtgeom on the spheroid. The result
* will have the squared units of the spheroid axes.
*/
extern double rtgeom_area_spheroid(const RTCTX *ctx, const RTGEOM *rtgeom, const SPHEROID *spheroid);

/**
* Calculate the geodetic length of a rtgeom on the unit sphere. The result
* will have to by multiplied by the real radius to get the real length.
*/
extern double rtgeom_length_spheroid(const RTCTX *ctx, const RTGEOM *geom, const SPHEROID *s);

/**
* Calculate covers predicate for two rtgeoms on the sphere. Currently
* only handles point-in-polygon.
*/
extern int rtgeom_covers_rtgeom_sphere(const RTCTX *ctx, const RTGEOM *rtgeom1, const RTGEOM *rtgeom2);

/**
* Remove repeated points!
*/
extern RTGEOM* rtgeom_remove_repeated_points(const RTCTX *ctx, const RTGEOM *in, double tolerance);

extern char rttriangle_is_repeated_points(const RTCTX *ctx, RTTRIANGLE *triangle);

/**
 * Swap ordinate values in every vertex of the geometry.
 *
 * Ordinates to swap are specified using an index with meaning:
 * 0=x, 1=y, 2=z, 3=m
 *
 * Swapping an existing ordinate with an unexisting one results
 * in undefined value being written in the existing ordinate.
 * Caller should verify and prevent such calls.
 *
 * Availability: 2.2.0
 */
extern void rtgeom_swap_ordinates(const RTCTX *ctx, RTGEOM *in, RTORD o1, RTORD o2);

/**
* Reverse the X and Y coordinate order. Useful for geometries in lat/lon order
* than need to be converted to lon/lat order.
*
* NOTE: uses rtgeom_swap_ordinates internally,
*       kept for backward compatibility
*/
extern RTGEOM* rtgeom_flip_coordinates(const RTCTX *ctx, RTGEOM *in);

struct RTPOINTITERATOR;
typedef struct RTPOINTITERATOR RTPOINTITERATOR;

/**
 * Create a new RTPOINTITERATOR over supplied RTGEOM*
 */
extern RTPOINTITERATOR* rtpointiterator_create(const RTCTX *ctx, const RTGEOM* g);

/**
 * Create a new RTPOINTITERATOR over supplied RTGEOM*
 * Supports modification of coordinates during iteration.
 */
extern RTPOINTITERATOR* rtpointiterator_create_rw(const RTCTX *ctx, RTGEOM* g);

/**
 * Free all memory associated with the iterator
 */
extern void rtpointiterator_destroy(const RTCTX *ctx, RTPOINTITERATOR* s);

/**
 * Returns RT_TRUE if there is another point available in the iterator.
 */
extern int rtpointiterator_has_next(const RTCTX *ctx, RTPOINTITERATOR* s);

/**
 * Attempts to replace the next point int the iterator with p, and advances
 * the iterator to the next point.
 * Returns RT_SUCCESS if the assignment was successful, RT_FAILURE otherwise.
 * */
extern int rtpointiterator_modify_next(const RTCTX *ctx, RTPOINTITERATOR* s, const RTPOINT4D* p);

/**
 * Attempts to assign the next point in the iterator to p, and advances
 * the iterator to the next point.  If p is NULL, the iterator will be
 * advanced without reading a point.
 * Returns RT_SUCCESS if the assignment was successful, RT_FAILURE otherwise.
 * */
extern int rtpointiterator_next(const RTCTX *ctx, RTPOINTITERATOR* s, RTPOINT4D* p);

/**
 * Attempts to assigns the next point in the iterator to p.  Does not advance.
 * Returns RT_SUCCESS if the assignment was successful, RT_FAILURE otherwise.
 */
extern int rtpointiterator_peek(const RTCTX *ctx, RTPOINTITERATOR* s, RTPOINT4D* p);


/**
* Convert a single hex digit into the corresponding char
*/
extern uint8_t parse_hex(const RTCTX *ctx, char *str);

/**
* Convert a char into a human readable hex digit
*/
extern void deparse_hex(const RTCTX *ctx, uint8_t str, char *result);



/***********************************************************************
** Functions for managing serialized forms and bounding boxes.
*/

/**
* Calculate the geocentric bounding box directly from the serialized
* form of the geodetic coordinates. Only accepts serialized geographies
* flagged as geodetic. Caller is responsible for disposing of the RTGBOX.
*/
extern RTGBOX* gserialized_calculate_gbox_geocentric(const GSERIALIZED *g);

/**
* Calculate the geocentric bounding box directly from the serialized
* form of the geodetic coordinates. Only accepts serialized geographies
* flagged as geodetic.
*/
int gserialized_calculate_gbox_geocentric_p(const GSERIALIZED *g, RTGBOX *g_box);

/**
* Return a RTWKT representation of the gserialized geometry.
* Caller is responsible for disposing of the char*.
*/
extern char* gserialized_to_string(const RTCTX *ctx, const GSERIALIZED *g);

/**
* Return a copy of the input serialized geometry.
*/
extern GSERIALIZED* gserialized_copy(const RTCTX *ctx, const GSERIALIZED *g);

/**
* Check that coordinates of RTGEOM are all within the geodetic range (-180, -90, 180, 90)
*/
extern int rtgeom_check_geodetic(const RTCTX *ctx, const RTGEOM *geom);

/**
* Gently move coordinates of RTGEOM if they are close enough into geodetic range.
*/
extern int rtgeom_nudge_geodetic(const RTCTX *ctx, RTGEOM *geom);

/**
* Force coordinates of RTGEOM into geodetic range (-180, -90, 180, 90)
*/
extern int rtgeom_force_geodetic(const RTCTX *ctx, RTGEOM *geom);

/**
* Set the RTFLAGS geodetic bit on geometry an all sub-geometries and pointlists
*/
extern void rtgeom_set_geodetic(const RTCTX *ctx, RTGEOM *geom, int value);

/**
* Calculate the geodetic bounding box for an RTGEOM. Z/M coordinates are
* ignored for this calculation. Pass in non-null, geodetic bounding box for function
* to fill out. RTGEOM must have been built from a GSERIALIZED to provide
* double aligned point arrays.
*/
extern int rtgeom_calculate_gbox_geodetic(const RTCTX *ctx, const RTGEOM *geom, RTGBOX *gbox);

/**
* Calculate the 2-4D bounding box of a geometry. Z/M coordinates are honored
* for this calculation, though for curves they are not included in calculations
* of curvature.
*/
extern int rtgeom_calculate_gbox_cartesian(const RTCTX *ctx, const RTGEOM *rtgeom, RTGBOX *gbox);

/**
* Calculate bounding box of a geometry, automatically taking into account
* whether it is cartesian or geodetic.
*/
extern int rtgeom_calculate_gbox(const RTCTX *ctx, const RTGEOM *rtgeom, RTGBOX *gbox);

/**
* New function to read doubles directly from the double* coordinate array
* of an aligned rtgeom #RTPOINTARRAY (built by de-serializing a #GSERIALIZED).
*/
extern int rt_getPoint2d_p_ro(const RTCTX *ctx, const RTPOINTARRAY *pa, int n, RTPOINT2D **point);

/**
* Calculate geodetic (x/y/z) box and add values to gbox. Return #RT_SUCCESS on success.
*/
extern int ptarray_calculate_gbox_geodetic(const RTCTX *ctx, const RTPOINTARRAY *pa, RTGBOX *gbox);

/**
* Calculate box (x/y) and add values to gbox. Return #RT_SUCCESS on success.
*/
extern int ptarray_calculate_gbox_cartesian(const RTCTX *ctx, const RTPOINTARRAY *pa, RTGBOX *gbox );

/**
* Calculate a spherical point that falls outside the geocentric gbox
*/
void gbox_pt_outside(const RTCTX *ctx, const RTGBOX *gbox, RTPOINT2D *pt_outside);

/**
* Create a new gbox with the dimensionality indicated by the flags. Caller
* is responsible for freeing.
*/
extern RTGBOX* gbox_new(const RTCTX *ctx, uint8_t flags);

/**
* Zero out all the entries in the #RTGBOX. Useful for cleaning
* statically allocated gboxes.
*/
extern void gbox_init(const RTCTX *ctx, RTGBOX *gbox);

/**
* Update the merged #RTGBOX to be large enough to include itself and the new box.
*/
extern int gbox_merge(const RTCTX *ctx, const RTGBOX *new_box, RTGBOX *merged_box);

/**
* Update the output #RTGBOX to be large enough to include both inputs.
*/
extern int gbox_union(const RTCTX *ctx, const RTGBOX *g1, const RTGBOX *g2, RTGBOX *gout);

/**
* Move the box minimums down and the maximums up by the distance provided.
*/
extern void gbox_expand(const RTCTX *ctx, RTGBOX *g, double d);

/**
* Initialize a #RTGBOX using the values of the point.
*/
extern int gbox_init_point3d(const RTCTX *ctx, const POINT3D *p, RTGBOX *gbox);

/**
* Update the #RTGBOX to be large enough to include itself and the new point.
*/
extern int gbox_merge_point3d(const RTCTX *ctx, const POINT3D *p, RTGBOX *gbox);

/**
* Return true if the point is inside the gbox
*/
extern int gbox_contains_point3d(const RTCTX *ctx, const RTGBOX *gbox, const POINT3D *pt);

/**
* Allocate a string representation of the #RTGBOX, based on dimensionality of flags.
*/
extern char* gbox_to_string(const RTCTX *ctx, const RTGBOX *gbox);

/**
* Return a copy of the #RTGBOX, based on dimensionality of flags.
*/
extern RTGBOX* gbox_copy(const RTCTX *ctx, const RTGBOX *gbox);

/**
* Warning, do not use this function, it is very particular about inputs.
*/
extern RTGBOX* gbox_from_string(const RTCTX *ctx, const char *str);

/**
* Return #RT_TRUE if the #RTGBOX overlaps, #RT_FALSE otherwise.
*/
extern int gbox_overlaps(const RTCTX *ctx, const RTGBOX *g1, const RTGBOX *g2);

/**
* Return #RT_TRUE if the #RTGBOX overlaps on the 2d plane, #RT_FALSE otherwise.
*/
extern int gbox_overlaps_2d(const RTCTX *ctx, const RTGBOX *g1, const RTGBOX *g2);

/**
* Return #RT_TRUE if the first #RTGBOX contains the second on the 2d plane, #RT_FALSE otherwise.
*/
extern int  gbox_contains_2d(const RTCTX *ctx, const RTGBOX *g1, const RTGBOX *g2);

/**
* Copy the values of original #RTGBOX into duplicate.
*/
extern void gbox_duplicate(const RTCTX *ctx, const RTGBOX *original, RTGBOX *duplicate);

/**
* Return the number of bytes necessary to hold a #RTGBOX of this dimension in
* serialized form.
*/
extern size_t gbox_serialized_size(const RTCTX *ctx, uint8_t flags);

/**
* Check if 2 given Gbox are the same
*/
extern int gbox_same(const RTCTX *ctx, const RTGBOX *g1, const RTGBOX *g2);

/**
* Check if 2 given RTGBOX are the same in x and y
*/
extern int gbox_same_2d(const RTCTX *ctx, const RTGBOX *g1, const RTGBOX *g2);

/**
* Check if two given RTGBOX are the same in x and y, or would round to the same
* RTGBOX in x and if serialized in GSERIALIZED
*/
extern int gbox_same_2d_float(const RTCTX *ctx, const RTGBOX *g1, const RTGBOX *g2);

/**
 * Round given RTGBOX to float boundaries
 *
 * This turns a RTGBOX into the version it would become
 * after a serialize/deserialize round trip.
 */
extern void gbox_float_round(const RTCTX *ctx, RTGBOX *gbox);

/**
* Return false if any of the dimensions is NaN or infinite
*/
extern int gbox_is_valid(const RTCTX *ctx, const RTGBOX *gbox);

/**
* Utility function to get type number from string. For example, a string 'POINTZ'
* would return type of 1 and z of 1 and m of 0. Valid
*/
extern int geometry_type_from_string(const RTCTX *ctx, const char *str, uint8_t *type, int *z, int *m);

/**
* Calculate required memory segment to contain a serialized form of the RTGEOM.
* Primarily used internally by the serialization code. Exposed to allow the cunit
* tests to exercise it.
*/
extern size_t gserialized_from_rtgeom_size(const RTCTX *ctx, const RTGEOM *geom);

/**
* Allocate a new #GSERIALIZED from an #RTGEOM. For all non-point types, a bounding
* box will be calculated and embedded in the serialization. The geodetic flag is used
* to control the box calculation (cartesian or geocentric). If set, the size pointer
* will contain the size of the final output, which is useful for setting the PgSQL
* VARSIZE information.
*/
extern GSERIALIZED* gserialized_from_rtgeom(const RTCTX *ctx, RTGEOM *geom, int is_geodetic, size_t *size);

/**
* Allocate a new #RTGEOM from a #GSERIALIZED. The resulting #RTGEOM will have coordinates
* that are double aligned and suitable for direct reading using rt_getPoint2d_p_ro
*/
extern RTGEOM* rtgeom_from_gserialized(const RTCTX *ctx, const GSERIALIZED *g);

/**
* Pull a #RTGBOX from the header of a #GSERIALIZED, if one is available. If
* it is not, calculate it from the geometry. If that doesn't work (null
* or empty) return RT_FAILURE.
*/
extern int gserialized_get_gbox_p(const RTCTX *ctx, const GSERIALIZED *g, RTGBOX *gbox);


/**
 * Parser check flags
 *
 *  @see rtgeom_from_wkb
 *  @see rtgeom_from_hexwkb
 */
#define RT_PARSER_CHECK_MINPOINTS  1
#define RT_PARSER_CHECK_ODD        2
#define RT_PARSER_CHECK_CLOSURE    4
#define RT_PARSER_CHECK_ZCLOSURE   8

#define RT_PARSER_CHECK_NONE   0
#define RT_PARSER_CHECK_ALL  (RT_PARSER_CHECK_MINPOINTS | RT_PARSER_CHECK_ODD | RT_PARSER_CHECK_CLOSURE)

/**
 * Parser result structure: returns the result of attempting to convert
 * (E)RTWKT/(E)RTWKB to RTGEOM
 */
typedef struct struct_rtgeom_parser_result
{
  const char *wkinput;        /* Copy of pointer to input RTWKT/RTWKB */
  uint8_t *serialized_rtgeom;   /* Pointer to serialized RTGEOM */
  int size;                   /* Size of serialized RTGEOM in bytes */
  RTGEOM *geom;               /* Pointer to RTGEOM struct */
  const char *message;        /* Error/warning message */
  int errcode;                /* Error/warning number */
  int errlocation;            /* Location of error */
  int parser_check_flags;     /* Bitmask of validity checks run during this parse */
}
RTGEOM_PARSER_RESULT;

/*
 * Parser error messages (these must match the message array in rtgparse.c)
 */
#define PARSER_ERROR_MOREPOINTS     1
#define PARSER_ERROR_ODDPOINTS      2
#define PARSER_ERROR_UNCLOSED       3
#define PARSER_ERROR_MIXDIMS        4
#define PARSER_ERROR_INVALIDGEOM    5
#define RTPARSER_ERROR_INVALIDWKBTYPE 6
#define PARSER_ERROR_INCONTINUOUS   7
#define PARSER_ERROR_TRIANGLEPOINTS 8
#define PARSER_ERROR_LESSPOINTS     9
#define PARSER_ERROR_OTHER          10



/*
 * Unparser result structure: returns the result of attempting to convert RTGEOM to (E)RTWKT/(E)RTWKB
 */
typedef struct struct_rtgeom_unparser_result
{
  uint8_t *serialized_rtgeom;  /* Copy of pointer to input serialized RTGEOM */
  char *wkoutput;      /* Pointer to RTWKT or RTWKB output */
  int size;      /* Size of serialized RTGEOM in bytes */
  const char *message;    /* Error/warning message */
  int errlocation;    /* Location of error */
}
RTGEOM_UNPARSER_RESULT;

/*
 * Unparser error messages (these must match the message array in rtgunparse.c)
 */
#define UNPARSER_ERROR_MOREPOINTS   1
#define UNPARSER_ERROR_ODDPOINTS  2
#define UNPARSER_ERROR_UNCLOSED    3


/*
** Variants available for RTWKB and RTWKT output types
*/

#define RTWKB_ISO 0x01
#define RTWKB_SFSQL 0x02
#define RTWKB_EXTENDED 0x04
#define RTWKB_NDR 0x08
#define RTWKB_XDR 0x10
#define RTWKB_HEX 0x20
#define RTWKB_NO_NPOINTS 0x40 /* Internal use only */
#define RTWKB_NO_SRID 0x80 /* Internal use only */

#define RTWKT_ISO 0x01
#define RTWKT_SFSQL 0x02
#define RTWKT_EXTENDED 0x04


/*
** Variants available for TWKB
*/
#define TWKB_BBOX 0x01 /* User wants bboxes */
#define TWKB_SIZE 0x02 /* User wants sizes */
#define TWKB_ID 0x04 /* User wants id */
#define RTTWKB_NO_TYPE 0x10 /* No type because it is a sub geoemtry */
#define TWKB_NO_ID 0x20 /* No ID because it is a subgeoemtry */
#define TWKB_DEFAULT_PRECISION 0 /* Aim for 1m (or ft) rounding by default */

/*
** New parsing and unparsing functions.
*/

/**
* @param rtgeom geometry to convert to RTWKT
* @param variant output format to use (RTWKT_ISO, RTWKT_SFSQL, RTWKT_EXTENDED)
*/
extern char*   rtgeom_to_wkt(const RTCTX *ctx, const RTGEOM *geom, uint8_t variant, int precision, size_t *size_out);

/**
* @param rtgeom geometry to convert to RTWKT
* @param variant output format to use
*                (RTWKB_ISO, RTWKB_SFSQL, RTWKB_EXTENDED, RTWKB_NDR, RTWKB_XDR)
*/
extern uint8_t*  rtgeom_to_wkb(const RTCTX *ctx, const RTGEOM *geom, uint8_t variant, size_t *size_out);

/**
* @param rtgeom geometry to convert to HEXWKB
* @param variant output format to use
*                (RTWKB_ISO, RTWKB_SFSQL, RTWKB_EXTENDED, RTWKB_NDR, RTWKB_XDR)
*/
extern char*   rtgeom_to_hexwkb(const RTCTX *ctx, const RTGEOM *geom, uint8_t variant, size_t *size_out);

/**
* @param rtgeom geometry to convert to EWKT
*/
extern char *rtgeom_to_ewkt(const RTCTX *ctx, const RTGEOM *rtgeom);

/**
 * @param check parser check flags, see RT_PARSER_CHECK_* macros
 * @param size length of RTWKB byte buffer
 * @param wkb RTWKB byte buffer
 */
extern RTGEOM* rtgeom_from_wkb(const RTCTX *ctx, const uint8_t *wkb, const size_t wkb_size, const char check);

/**
 * @param check parser check flags, see RT_PARSER_CHECK_* macros
 */
extern RTGEOM* rtgeom_from_hexwkb(const RTCTX *ctx, const char *hexwkb, const char check);

extern uint8_t*  bytes_from_hexbytes(const RTCTX *ctx, const char *hexbuf, size_t hexsize);

extern char*   hexbytes_from_bytes(const RTCTX *ctx, uint8_t *bytes, size_t size);


/* Memory management */
extern void *rtalloc(const RTCTX *ctx, size_t size);
extern void *rtrealloc(const RTCTX *ctx, void *mem, size_t size);
extern void rtfree(const RTCTX *ctx, void *mem);

/* Utilities */
extern char *rtmessage_truncate(const RTCTX *ctx, char *str, int startpos, int endpos, int maxlength, int truncdirection);

/*
* TWKB functions
*/

/**
 * @param check parser check flags, see RT_PARSER_CHECK_* macros
 * @param size parser check flags, see RT_PARSER_CHECK_* macros
 */
extern RTGEOM* rtgeom_from_twkb(const RTCTX *ctx, uint8_t *twkb, size_t twkb_size, char check);

/**
 * @param geom input geometry
 * @param variant what variations on TWKB are requested?
 * @param twkb_size returns the length of the output TWKB in bytes if set
 */
extern uint8_t* rtgeom_to_twkb(const RTCTX *ctx, const RTGEOM *geom, uint8_t variant, int8_t precision_xy, int8_t precision_z, int8_t precision_m, size_t *twkb_size);

extern uint8_t* rtgeom_to_twkb_with_idlist(const RTCTX *ctx, const RTGEOM *geom, int64_t *idlist, uint8_t variant, int8_t precision_xy, int8_t precision_z, int8_t precision_m, size_t *twkb_size);

/*******************************************************************************
 * SQLMM internal functions - TODO: Move into separate header files
 ******************************************************************************/

int rtgeom_has_arc(const RTCTX *ctx, const RTGEOM *geom);
RTGEOM *rtgeom_stroke(const RTCTX *ctx, const RTGEOM *geom, uint32_t perQuad);
RTGEOM *rtgeom_unstroke(const RTCTX *ctx, const RTGEOM *geom);

/*******************************************************************************
 * GEOS proxy functions on RTGEOM
 ******************************************************************************/

/** Return GEOS version string (not to be freed) */
const char* rtgeom_geos_version(void);

/** Convert an RTGEOM to a GEOS Geometry and convert back -- for debug only */
RTGEOM* rtgeom_geos_noop(const RTCTX *ctx, const RTGEOM *geom) ;

RTGEOM *rtgeom_normalize(const RTCTX *ctx, const RTGEOM *geom);
RTGEOM *rtgeom_intersection(const RTCTX *ctx, const RTGEOM *geom1, const RTGEOM *geom2);
RTGEOM *rtgeom_difference(const RTCTX *ctx, const RTGEOM *geom1, const RTGEOM *geom2);
RTGEOM *rtgeom_symdifference(const RTCTX *ctx, const RTGEOM* geom1, const RTGEOM* geom2);
RTGEOM *rtgeom_union(const RTCTX *ctx, const RTGEOM *geom1, const RTGEOM *geom2);
RTGEOM *rtgeom_linemerge(const RTCTX *ctx, const RTGEOM *geom1);
RTGEOM *rtgeom_unaryunion(const RTCTX *ctx, const RTGEOM *geom1);
RTGEOM *rtgeom_clip_by_rect(const RTCTX *ctx, const RTGEOM *geom1, double x0, double y0, double x1, double y1);
RTCOLLECTION *rtgeom_subdivide(const RTCTX *ctx, const RTGEOM *geom, int maxvertices);

/**
 * Snap vertices and segments of a geometry to another using a given tolerance.
 *
 * @param geom1 the geometry to snap
 * @param geom2 the geometry to snap to
 * @param tolerance the distance under which vertices and segments are snapped
 *
 * Requires GEOS-3.3.0+
 */
RTGEOM* rtgeom_snap(const RTCTX *ctx, const RTGEOM* geom1, const RTGEOM* geom2, double tolerance);

/*
 * Return the set of paths shared between two linear geometries,
 * and their direction (same or opposite).
 *
 * @param geom1 a lineal geometry
 * @param geom2 another lineal geometry
 *
 * Requires GEOS-3.3.0+
 */
RTGEOM* rtgeom_sharedpaths(const RTCTX *ctx, const RTGEOM* geom1, const RTGEOM* geom2);

/*
 * An offset curve against the input line.
 *
 * @param rtline a lineal geometry
 * @param size offset distance. Offset left if negative and right if positive
 * @param quadsegs number of quadrature segments in curves (try 8)
 * @param joinStyle (1 = round, 2 = mitre, 3 = bevel)
 * @param mitreLimit (try 5.0)
 * @return derived geometry (linestring or multilinestring)
 *
 * Requires GEOS-3.2.0+
 */
RTGEOM* rtgeom_offsetcurve(const RTCTX *ctx, const RTLINE *rtline, double size, int quadsegs, int joinStyle, double mitreLimit);

/*
 * Return true if the input geometry is "simple" as per OGC defn.
 *
 * @return 1 if simple, 0 if non-simple, -1 on exception (rterror is called
 *         in that case)
 */
int rtgeom_is_simple(const RTCTX *ctx, const RTGEOM *rtgeom);


/*******************************************************************************
 * GEOS-dependent extra functions on RTGEOM
 ******************************************************************************/

/**
 * Take a geometry and return an areal geometry
 * (Polygon or MultiPolygon).
 * Actually a wrapper around GEOSpolygonize,
 * transforming the resulting collection into
 * a valid polygon Geometry.
 */
RTGEOM* rtgeom_buildarea(const RTCTX *ctx, const RTGEOM *geom) ;


/**
 * Attempts to make an invalid geometries valid w/out losing points.
 *
 * NOTE: this is only available when librtgeom is built against
 *       GEOS 3.3.0 or higher
 */
RTGEOM* rtgeom_make_valid(const RTCTX *ctx, RTGEOM* geom);

/*
 * Split (multi)polygon by line; (multi)line by (multi)line,
 * (multi)point or (multi)polygon boundary.
 *
 * Collections are accepted as first argument.
 * Returns all obtained pieces as a collection.
 */
RTGEOM* rtgeom_split(const RTCTX *ctx, const RTGEOM* rtgeom_in, const RTGEOM* blade_in);

/*
 * Fully node a set of linestrings, using the least nodes preserving
 * all the input ones.
 *
 * Requires GEOS-3.3.0 or higher
 */
RTGEOM* rtgeom_node(const RTCTX *ctx, const RTGEOM* rtgeom_in);

/**
 * Take vertices of a geometry and build a delaunay
 * triangulation on them.
 *
 * @param geom the input geometry
 * @param tolerance an optional snapping tolerance for improved tolerance
 * @param edgeOnly if non-zero the result will be a MULTILINESTRING,
 *                 otherwise it'll be a COLLECTION of polygons.
 */
RTGEOM* rtgeom_delaunay_triangulation(const RTCTX *ctx, const RTGEOM *geom, double tolerance, int edgeOnly);

#endif /* !defined _LIBRTGEOM_H  */

