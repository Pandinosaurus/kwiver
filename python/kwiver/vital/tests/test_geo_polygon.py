# """
# ckwg +31
# Copyright 2015-2016 by Kitware, Inc.
# All rights reserved.

# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:

#  * Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.

#  * Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.

#  * Neither name of Kitware, Inc. nor the names of any contributors may be used
#    to endorse or promote products derived from this software without specific
#    prior written permission.

# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# ==============================================================================

# Tests for Python interface to vital::geo_polygon

# """

import nose.tools as nt
import numpy as np

from kwiver.vital.types import geo_polygon as gp, geodesy, Polygon
from kwiver.vital.config import Config
from kwiver.vital.modules import modules

# TODO: where to put these? Are they fine here?
loc_ll = np.array([-149.484444, -17.619482])
loc_utm = np.array([236363.98, 8050181.74])

loc2_ll = np.array([-73.759291, 42.849631])


crs_ll = geodesy.SRID.lat_lon_WGS84
crs_utm_6s = geodesy.SRID.UTM_WGS84_south + 6


class TestVitalGeoPolygon(object):
    # Creates a regular (not Geo) polygon for a few tests
    def _create_polygon(self):
        return Polygon(
            [
                np.array([10, 10]),
                np.array([10, 50]),
                np.array([50, 50]),
                np.array([30, 30]),
            ]
        )

    # Use above method to construct and test some geo_polygon instances
    def _create_geo_polygons(self):
        return (gp.GeoPolygon(), gp.GeoPolygon(self._create_polygon(), crs_ll))

    def test_create(self):
        gp.GeoPolygon()
        gp.GeoPolygon(self._create_polygon(), crs_ll)

    def test_inital_is_empty(self):
        g_poly1, g_poly2 = self._create_geo_polygons()
        nt.ok_(g_poly1.is_empty())
        nt.assert_false(g_poly2.is_empty())

    def test_initial_crs(self):
        g_poly1, g_poly2 = self._create_geo_polygons()
        nt.assert_equals(g_poly1.crs(), -1)
        nt.assert_equals(g_poly2.crs(), crs_ll)

    def test_initial_polygon(self):
        _, g_poly2 = self._create_geo_polygons()
        poly = self._create_polygon()

        np.testing.assert_array_equal(g_poly2.polygon().get_vertices(), poly.get_vertices())
        np.testing.assert_array_equal(
            g_poly2.polygon(crs_ll).get_vertices(), poly.get_vertices()
        )

    def test_no_polygon_for_key(self):
        g_poly1 = gp.GeoPolygon()
        nt.assert_raises(IndexError, g_poly1.polygon)
        nt.assert_raises(IndexError, g_poly1.polygon, crs_ll)

    # Try setting the polygon member by using the above locations.
    def test_set_polygon(self):
        for instance in list(self._create_geo_polygons()):

            instance.set_polygon(Polygon([loc2_ll]), crs_ll)

            nt.assert_equals(instance.polygon().num_vertices(), 1)
            nt.assert_equals(instance.crs(), crs_ll)
            np.testing.assert_array_almost_equal(instance.polygon().at(0), loc2_ll)
            np.testing.assert_array_almost_equal(
                instance.polygon(crs_ll).at(0), loc2_ll
            )
            nt.assert_false(instance.is_empty())

            instance.set_polygon(Polygon([loc_utm]), crs_utm_6s)

            nt.assert_equals(instance.polygon().num_vertices(), 1)
            nt.assert_equals(instance.crs(), crs_utm_6s)
            np.testing.assert_array_almost_equal(instance.polygon().at(0), loc_utm)
            np.testing.assert_array_almost_equal(
                instance.polygon(crs_utm_6s).at(0), loc_utm
            )
            nt.assert_false(instance.is_empty())

    def test_conversion(self):
        modules.load_known_modules()

        p_ll = gp.GeoPolygon(Polygon([loc_ll]), crs_ll)
        p_utm = gp.GeoPolygon(Polygon([loc_utm]), crs_utm_6s)

        conv_loc_utm = p_ll.polygon( p_utm.crs() ).at(0)
        conv_loc_ll =  p_utm.polygon( p_ll.crs() ).at(0)

        np.testing.assert_array_almost_equal( p_ll.polygon().at(0), conv_loc_ll, decimal = 7)
        np.testing.assert_array_almost_equal(p_utm.polygon().at(0), conv_loc_utm, decimal = 2)

        np.testing.assert_array_almost_equal(loc_ll,  conv_loc_ll, decimal = 7)
        np.testing.assert_array_almost_equal(loc_utm, conv_loc_utm, decimal = 2)

    def test_to_str_empty(self):
        g_poly1 = gp.GeoPolygon()
        nt.assert_equals(str(g_poly1), "{ empty }")
        print(str(g_poly1))


    # Can't compare these exactly because of rounding error
    def test_to_str(self):
        g_poly = gp.GeoPolygon(Polygon([loc_ll, loc2_ll]), crs_ll)
        print(str(g_poly))

    def test_config_block_set_value_cast_empty_pgon(self):
        val = gp.config_block_set_value_cast(gp.GeoPolygon())
        nt.assert_equals(val, "")