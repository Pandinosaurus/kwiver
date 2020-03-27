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


#include <Eigen/Core>
#include <pybind11/pybind11.h>

#include <pybind11/eigen.h>
#include <pybind11/stl.h>


namespace py = pybind11;

void eigen_array(py::module &m);

namespace kwiver {
namespace vital {
namespace python {
class EigenArray
{
  // We're assuming always dynamic, to make things simpler for first pass
  // TODO We can edit this later to use subclasses instead of two parallel
  Eigen::MatrixXd double_mat;
  Eigen::MatrixXf float_mat;
  char type;

  public:

    EigenArray(int rows = 2,
               int cols = 1,
               bool dynamic_rows = false,
               bool dynamic_cols = false,
               char ctype = 'd');

    void fromVectorF(std::vector< std::vector<float> >);
    void fromVectorD(std::vector< std::vector<double> >);
    static EigenArray fromArray(py::object, char);

    void setType(char ctype) { this->type = ctype; };
    char getType() { return type; };

    py::object getMatrix();
    Eigen::MatrixXd getMatrixD() { return double_mat; };
    Eigen::MatrixXf getMatrixF() { return float_mat; };
    void setMatrixF();
    void setMatrixD();

};
} } }