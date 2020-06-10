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

# Tests for Python interface to vital geodesy function/classes

# """
from kwiver.vital.types import geodesy as g
from kwiver.vital.tests.helpers import no_call_pure_virtual_method

from multimethod import multimethod
import nose.tools as nt
import numpy as np

class SimpleGeoConversion(g.GeoConversion):
    def __init__(self):
        g.GeoConversion.__init__(self)

    def id(self):
        return "5"

    def describe(self, crs):
        return {"first_key": "first_val"}

    # TODO
    def __call__(self, point, from_crs, to_crs):
        return np.array([from_crs, to_crs])


class TestVitalGeoConversion(object):

    # TODO: when geo_conversion bindings are finished, uncomment below
    def test_no_call_pure_virtual_methods(self):
        gc = g.GeoConversion()
        crs1 = g.SRID.lat_lon_NAD83
        crs2 = g.SRID.UPS_WGS84_north
        point_2d = np.array([1, 2])
        point_3d = np.array([3, 2, 1])
        no_call_pure_virtual_method(gc.id)
        no_call_pure_virtual_method(gc.describe, crs1)
        no_call_pure_virtual_method(gc.__call__, point_2d, crs1, crs2)
        no_call_pure_virtual_method(gc.__call__, point_3d, crs1, crs2)

    def test_init_concrete(self):
        SimpleGeoConversion()


    """ TODO: Things that should be tested:
            char* in id()
            inheriting without overriding?
            no seg faults
    """

class TestVitalGeodesy(object):
    def test_srid(self):
        nt.assert_equals(g.SRID.lat_lon_NAD83, 4269)
        nt.assert_equals(g.SRID.lat_lon_WGS84, 4326)

        nt.assert_equals(g.SRID.UPS_WGS84_north, 32661)
        nt.assert_equals(g.SRID.UPS_WGS84_south, 32761)

        nt.assert_equals(g.SRID.UTM_WGS84_north, 32600)
        nt.assert_equals(g.SRID.UTM_WGS84_south, 32700)

        nt.assert_equals(g.SRID.UTM_NAD83_northeast, 3313)
        nt.assert_equals(g.SRID.UTM_NAD83_northwest, 26900)

    def _create_utm_ups_zone_structs(self):
        return [g.UTMUPSZone(), g.UTMUPSZone(5, True)]

    def test_utm_ups_zone_struct_create(self):
        s1 = g.UTMUPSZone()
        s2 = g.UTMUPSZone(5, True)
        nt.assert_equals(s2.number, 5)
        nt.assert_equals(s2.north, True)

    def test_set_utm_ups_zone_struct_values(self):
        struct_list = self._create_utm_ups_zone_structs()

        # Test getting and setting a few values
        values = [(10, False), (31, True), (12, True), (0, False)]
        for struct in struct_list:
            for num, north_bool in values:
                struct.number = num
                struct.north = north_bool

                nt.assert_equals(struct.number, num)
                nt.assert_equals(struct.north, north_bool)

    def test_bad_set_utm_ups_zone_struct_values(self):
        struct_list = self._create_utm_ups_zone_structs()

        for struct in struct_list:
            # Set to a valid value first
            struct.number = 10
            struct.north = True

            with nt.assert_raises(TypeError):
                struct.number = "string, not int"

            with nt.assert_raises(TypeError):
                struct.north = "string, not bool"

            nt.assert_equals(struct.number, 10)
            nt.assert_equals(struct.north, True)



