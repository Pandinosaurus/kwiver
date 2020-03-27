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

#include "eigen_class.h"

namespace py = pybind11;
namespace kvp = kwiver::vital::python;

void eigen_array(py::module &m)
{
  py::class_< kvp::EigenArray, std::shared_ptr<kvp::EigenArray> >(m, "EigenArray")
  .def(py::init< int, int, bool, bool, char>(),
         py::arg("rows")=2, py::arg("cols")=1,
         py::arg("dynamic_rows")=false, py::arg("dynamic_cols")=false,
         py::arg("type")='d')
  .def("get_matrix", &kvp::EigenArray::getMatrix, "Access the C++ matrix by reference", py::return_value_policy::reference_internal)
  .def("copy_matrix", &kvp::EigenArray::getMatrix, "Creates a python copy of the matrix")
  .def_static("from_array", &kvp::EigenArray::fromArray,
         py::arg("data"), py::arg("type")='d')
  ;
}


kvp::EigenArray::
EigenArray(int rows,
           int cols,
           bool dynamic_rows, // we're ignoring these, but keeping them in for API reasons
           bool dynamic_cols,
           char ctype)
{
  type = ctype;
  if(type == 'd') double_mat = Eigen::MatrixXd(rows, cols);
  else if(type == 'f') float_mat = Eigen::MatrixXf(rows, cols);
  else throw std::invalid_argument("Invalid matrix type. Must be 'd' or 'f'");
}

py::object
kvp::EigenArray::
getMatrix()
{
  if(this->getType() == 'd') return py::cast(&double_mat);
  else if(this->getType() == 'f') return py::cast(&float_mat);
  return py::none();
}

void
kvp::EigenArray::
fromVectorF(std::vector< std::vector<float> > data_vec)
{
  unsigned int rows = data_vec.size();
  unsigned int cols = data_vec[0].size();
  this->float_mat = Eigen::MatrixXf(rows, cols);
  for(unsigned int i = 0; i < rows; i++)
  {
    if (data_vec[i].size() != cols)
    {
       throw std::invalid_argument("Input is not an mxn matrix!");
    }
    for(unsigned int j = 0; j < cols; j++)
    {
      float_mat(i,j) = data_vec[i][j];
    }
  }
}

void
kvp::EigenArray::
fromVectorD(std::vector< std::vector<double> > data_vec)
{
  unsigned int rows = data_vec.size();
  unsigned int cols = data_vec[0].size();
  this->double_mat = Eigen::MatrixXd(rows, cols);
  for(unsigned int i = 0; i < rows; i++)
  {
    if (data_vec[i].size() != cols)
    {
       throw std::invalid_argument("Input is not an mxn matrix!");
    }
    for(unsigned int j = 0; j < cols; j++)
    {
      double_mat(i,j) = data_vec[i][j];
    }
  }
}

kvp::EigenArray
kvp::EigenArray::
fromArray(py::object data, char ctype = 'd')
{
  kvp::EigenArray retMat;

  retMat.setType(ctype);

  if(ctype == 'd')
  {
    std::vector< std::vector<double> > data_vec = data.cast<std::vector< std::vector<double> > >();
    retMat.fromVectorD(data_vec);
  }
  else if(ctype == 'f')
  {
    std::vector< std::vector<float> > data_vec = data.cast<std::vector< std::vector<float> > >();
    retMat.fromVectorF(data_vec);
  }
  else
  {
    throw std::invalid_argument("Invalid matrix type. Must be 'd' or 'f'");
  }

  return retMat;
}
