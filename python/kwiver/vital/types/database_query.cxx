/*ckwg +29
 * Copyright 2017 by Kitware, Inc.
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


#include <vital/types/database_query.h>

#include <pybind11/pybind11.h>

namespace py=pybind11;



PYBIND11_MODULE(database_query, m)
{
  py::class_<kwiver::vital::database_query, std::shared_ptr<kwiver::vital::database_query>>(m, "DatabaseQuery")
  .def(py::init<>())
  .def_property("id", &kwiver::vital::database_query::id, &kwiver::vital::database_query::set_id)
  .def_property("type", &kwiver::vital::database_query::type, &kwiver::vital::database_query::set_type)
  .def_property("temporal_filter", &kwiver::vital::database_query::temporal_filter, &kwiver::vital::database_query::set_temporal_filter)
  .def_property("spatial_filter", &kwiver::vital::database_query::spatial_filter, &kwiver::vital::database_query::set_spatial_filter)
  .def_property("spatial_region", &kwiver::vital::database_query::spatial_region, &kwiver::vital::database_query::set_spatial_region)
  .def_property("stream_filter", &kwiver::vital::database_query::stream_filter, &kwiver::vital::database_query::set_stream_filter)
  .def_property("descriptors", &kwiver::vital::database_query::descriptors, &kwiver::vital::database_query::set_descriptors)
  .def_property("threshold", &kwiver::vital::database_query::threshold, &kwiver::vital::database_query::set_threshold)
  .def("temporal_lower_bound", &kwiver::vital::database_query::temporal_lower_bound)
  .def("temporal_upper_bound", &kwiver::vital::database_query::temporal_upper_bound)
  .def("set_temporal_bounds", &kwiver::vital::database_query::set_temporal_bounds)
  ;

  py::enum_<kwiver::vital::query_filter>(m, "query_filter")
  .value("IGNORE_FILTER", kwiver::vital::query_filter::IGNORE_FILTER)
  .value("CONTAINS_WHOLLY", kwiver::vital::query_filter::CONTAINS_WHOLLY)
  .value("CONTAINS_PARTLY", kwiver::vital::query_filter::CONTAINS_PARTLY)
  .value("INTERSECTS", kwiver::vital::query_filter::INTERSECTS)
  .value("INTERSECTS_INBOUND", kwiver::vital::query_filter::INTERSECTS_INBOUND)
  .value("INTERSECTS_OUTBOUND", kwiver::vital::query_filter::INTERSECTS_OUTBOUND)
  .value("DOES_NOT_CONTAIN", kwiver::vital::query_filter::DOES_NOT_CONTAIN)
  ;



  py::enum_<kwiver::vital::database_query::query_type>(m, "query_type")
  .value("SIMILARITY", kwiver::vital::database_query::query_type::SIMILARITY)
  .value("RETRIEVAL", kwiver::vital::database_query::query_type::RETRIEVAL)
  ;
}
