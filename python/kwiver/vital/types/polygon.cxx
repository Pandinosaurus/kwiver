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


#include <vital/types/polygon.h>
#include "eigen_class.h"

#include <pybind11/pybind11.h>
#include <memory>
#include <pybind11/stl.h>

namespace py=pybind11;
namespace kv=kwiver::vital;



kv::polygon
new_from_point_list(py::list py_pt_list)
{
  std::vector<kv::vector_2d> pt_list;
  for(auto py_pt : py_pt_list)
  {
    pt_list.push_back(py::cast<EigenArray>(py_pt).getMatrixD());
  }
  return kv::polygon(pt_list);
}


PYBIND11_MODULE(polygon, m)
{
  py::class_< kv::polygon, std::shared_ptr< kv::polygon > >(m, "Polygon")
  .def(py::init<>())
  .def(py::init< const std::vector< kv::polygon::point_t >& >())
  .def("push_back",
    static_cast<void (kv::polygon::*) (double, double)>(&kv::polygon::push_back))
  .def("push_back",
    static_cast<void (kv::polygon::*) (const kv::polygon::point_t&)>(&kv::polygon::push_back))
  .def("num_vertices", &kv::polygon::num_vertices)
  .def("get_vertices", &kv::polygon::get_vertices)
  .def("contains", static_cast<bool (kv::polygon::*) (double, double)>(&kv::polygon::contains))
  .def("contains", static_cast<bool (kv::polygon::*) (const kv::polygon::point_t&)>(&kv::polygon::contains))
  .def("at", &kv::polygon::at)
  ;
}