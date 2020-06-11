/*ckwg +29
 * Copyright 2020 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <vital/types/geodesy.h>

#include <python/kwiver/vital/util/pybind11.h>

#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>


#include <memory>

namespace py=pybind11;
namespace kv=kwiver::vital;

// TODO: See TODO in PYBIND11_MODULE call
// class deletable_geo_conversion
//   : public kv::geo_conversion
// {
// public:
//   ~deletable_geo_conversion() override = default;
// };


// class geo_conversion_trampoline
//   : public deletable_geo_conversion
// {
// public:
  // Inheriting default constructor
  // using deletable_geo_conversion::deletable_geo_conversion; //TODO: should this be here?

  // Override virtual functions
//   char const* id() const override;
//   kv::geo_crs_description_t describe( int crs ) override;
//   kv::vector_2d operator()( kv::vector_2d const& point, int from, int to ) override;
//   kv::vector_3d operator()( kv::vector_3d const& point, int from, int to ) override;
// };

PYBIND11_MODULE(geodesy, m)
{
  // Define a submodule for the SRID namespace,
  // which contains identifier codes for different spatial reference systems
  auto msri = m.def_submodule("SRID");
  msri.attr("lat_lon_NAD83") = kv::SRID::lat_lon_NAD83;
  msri.attr("lat_lon_WGS84") = kv::SRID::lat_lon_WGS84;
  msri.attr("UPS_WGS84_north") = kv::SRID::UPS_WGS84_north;
  msri.attr("UPS_WGS84_south") = kv::SRID::UPS_WGS84_south;
  msri.attr("UTM_WGS84_north") = kv::SRID::UTM_WGS84_north;
  msri.attr("UTM_WGS84_south") = kv::SRID::UTM_WGS84_south;
  msri.attr("UTM_NAD83_northeast") = kv::SRID::UTM_NAD83_northeast;
  msri.attr("UTM_NAD83_northwest") = kv::SRID::UTM_NAD83_northwest;
  ;

  // TODO: The protected virtual destructor in geo_conversion causes a lot of problems. Without
  // the deleteable_geo_conversion class, compiler errors are thrown because Pybind won't
  // be able to delete any instances. Using py::nodelete forces us to use a unique_ptr, which
  // won't work with the definitions of get_geo_conv and set_geo_conv. Plus, we need a ctor in the
  // base class so that the trampoline will work, and using py::nodelete will give memory leaks.
  // For now, all code related to binding this class has been commented out. When a solution is found,
  // it will be uncommented.

  // py::class_<deletable_geo_conversion, geo_conversion_trampoline, std::shared_ptr<deletable_geo_conversion>>(m, "GeoConversion")
  // .def("id", &deletable_geo_conversion::id)
  // .def("describe", &deletable_geo_conversion::describe)
  // .def("__call__", (kv::vector_2d (deletable_geo_conversion::*) (kv::vector_2d const&, int, int)) &deletable_geo_conversion::operator())
  // .def("__call__", (kv::vector_3d (deletable_geo_conversion::*) (kv::vector_3d const&, int, int)) &deletable_geo_conversion::operator())
  // ;

  py::class_<kv::utm_ups_zone_t, std::shared_ptr<kv::utm_ups_zone_t>>(m, "UTMUPSZone")
  .def(py::init<>())
  .def(py::init<int, bool>())
  .def_readwrite("number", &kv::utm_ups_zone_t::number)
  .def_readwrite("north", &kv::utm_ups_zone_t::north)
  ;
}

// TODO: See TODO in PYBIND11_MODULE call
// char const*
// geo_conversion_trampoline
// ::id() const
// {
//   VITAL_PYBIND11_OVERLOAD_PURE(
//     char const*,
//     kv::geo_conversion,
//     id,
//   );
// }

// kv::geo_crs_description_t
// geo_conversion_trampoline
// ::describe( int crs )
// {
//   VITAL_PYBIND11_OVERLOAD_PURE(
//     kv::geo_crs_description_t,
//     kv::geo_conversion,
//     describe,
//     crs
//   );
// }

// kv::vector_2d
// geo_conversion_trampoline
// ::operator()
// ( kv::vector_2d const& point, int from_crs, int to_crs )
// {
//   VITAL_PYBIND11_OVERLOAD_PURE_NAME(
//     kv::vector_2d,
//     kv::geo_conversion,
//     "__call__",
//     operator(),
//     point,
//     from_crs,
//     to_crs
//   );
// }

// kv::vector_3d
// geo_conversion_trampoline
// ::operator()
// ( kv::vector_3d const& point, int from_crs, int to_crs )
// {
//   VITAL_PYBIND11_OVERLOAD_PURE_NAME(
//     kv::vector_3d,
//     kv::geo_conversion,
//     "__call__",
//     operator(),
//     point,
//     from_crs,
//     to_crs
//   );
// }