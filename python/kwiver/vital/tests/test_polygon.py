"""
ckwg +31
Copyright 2015-2016 by Kitware, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither name of Kitware, Inc. nor the names of any contributors may be used
   to endorse or promote products derived from this software without specific
   prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

==============================================================================

Tests for Python interface to vital::polygon

"""

from kwiver.vital.types import Polygon, EigenArray
import nose.tools as nt


class TestVitalPolygon(object):
    def _create_points(self):
        return (
            EigenArray.from_array([[10], [10]]),
            EigenArray.from_array([[10], [50]]),
            EigenArray.from_array([[50], [50]]),
            EigenArray.from_array([[30], [30]]),
        )

    def _create_polygons(self):
        pts = list(self._create_points())
        return (Polygon(), Polygon(pts))

    def test_new(self):
        pts = list(self._create_points())
        Polygon()
        Polygon(pts)


    def test_initial_at(self):
        pt1, pt2, pt3, pt4 = self._create_points()
        _, p2 = self._create_polygons()

        nt.assert_equal(pt1, p2.at(0))
        nt.assert_equal(pt1, p2.at(1))
        nt.assert_equal(pt1, p2.at(2))
        nt.assert_equal(pt1, p2.at(3))


    # def test_initial_num_vertices(self):
    #     p1, p2 = self._create_polygons()
    #     nt.assert_equal(p1.num_vertices(), 0)
    #     nt.assert_equal(p2.num_vertices(), 4)

    # def test_add_points(self):
    #     p1, p2 = self._create_polygons()
    #     pt1, pt2, pt3, pt4 = self._create_points()
    #     # Start with p1
    #     p1.push_back(pt1)
    #     nt.assert_equal(p1.num_vertices(), 1)

    #     p1.push_back(pt2)
    #     nt.assert_equal(p1.num_vertices(), 2)

    #     p1.push_back(pt3)
    #     nt.assert_equal(p1.num_vertices(), 3)

    #     p1.push_back(pt4)
    #     nt.assert_equal(p1.num_vertices(), 4)

    #     nt.assert_equal(pt1, p1.at(0))
    #     nt.assert_equal(pt2, p1.at(1))
    #     nt.assert_equal(pt3, p1.at(2))
    #     nt.assert_equal(pt4, p1.at(3))

        # Now p2

