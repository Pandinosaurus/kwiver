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


// TODO: enums


PYBIND11_MODULE(database_query, m)
{
  py::class_<kwiver::vital::database_query, std::shared_ptr<kwiver::vital::database_query>>(m, "DatabaseQuery")
  .def(py::init<>())
  .def_property("m_id", &kwiver::vital::database_query::id, &kwiver::vital::database_query::set_id)
  .def_property("m_type", &kwiver::vital::database_query::type, &kwiver::vital::database_query::set_type)
  .def_property("m_temporal_filter", &kwiver::vital::database_query::temporal_filter, &kwiver::vital::database_query::set_temporal_filter)
  .def_property("m_spatial_filter", &kwiver::vital::database_query::spatial_filter, &kwiver::vital::database_query::set_spatial_filter)
  .def_property("m_spatial_region", &kwiver::vital::database_query::spatial_region, &kwiver::vital::database_query::set_spatial_region)
  .def_property("m_stream_filter", &kwiver::vital::database_query::stream_filter, &kwiver::vital::database_query::set_stream_filter)
  .def_property("m_descriptors", &kwiver::vital::database_query::descriptors, &kwiver::vital::database_query::set_descriptors)
  .def_property("m_threshold", &kwiver::vital::database_query::threshold, &kwiver::vital::database_query::threshold)
  .def("temporal_lower_bound", &kwiver::vital::database_query::temporal_lower_bound)
  .def("temporal_upper_bound", &kwiver::vital::database_query::temporal_upper_bound)
  .def("set_temporal_bounds", &kwiver::vital::database_query::set_temporal_bounds)
  ;
}
