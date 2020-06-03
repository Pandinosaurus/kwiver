"""
ckwg +31
Copyright 2020 by Kitware, Inc.
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

Tests for the vital classes metadata_item, typed_metadata (templated),
unknown_metadata_item, and metadata

"""

# TODO:
# 1.) std::exception subclassing worked
# 2.) metadata_item::type?
# 3.) metadata::typeid_for_tag
# 4.) metadata::add
# 5.) unknown_metadata_item CLASS, NOT TAG. TEST THE WHOLE CLASS
# 6.) CHECK THAT TYPED_METADATA<UNKNOWN> STILL NEEDS TO BE TESTED/BOUND
# 7.) did i forget something
# 8.) stuff with std::string name mangling, testing .type!

import nose.tools as nt
import numpy as np

from kwiver.vital.types.metadata import *
from kwiver.vital.types import (
    geodesy as gd,
    GeoPoint,
    GeoPolygon,
    Polygon,
    metadata_tags as mt,
)

# Helper class to store information about whether a typed_metadata item
# can be converted to a double, uint64, and string, and if it can, what the expected value is
class ConvInfo(object):
    def __init__(
        self,
        has_double=False,
        has_uint64=False,
        has_string=False,
        as_double=None,
        as_uint64=None,
        as_string=None,
    ):
        # Boolean values
        self.has_double = has_double
        self.has_uint64 = has_uint64
        self.has_string = has_string

        # Actual data, if exists
        self.as_double = as_double
        self.as_uint64 = as_uint64
        self.as_string = as_string


# Helper class that stores a typed_metadata object's
# name, tag, and data properties
class PropInfo(object):
    def __init__(self, name, tag, data):
        self.name = name
        self.tag = tag
        self.data = data


class TestVitalMetadataItem(object):
    def test_no_construct_base(self):
        err_msg = "kwiver.vital.types.metadata.MetadataItem: No constructor defined!"
        with nt.assert_raises_regexp(TypeError, err_msg):
            MetadataItem()

    # TODO
    def test_no_call_virtual_methods(self):
        pass


class TestVitalTypedMetadata(object):
    # Creates a geo_point and geo_poly
    def setUp(self):
        self.g_point = GeoPoint(np.array([1, 2, 3]), gd.SRID.lat_lon_WGS84)

        poly = Polygon(
            [
                np.array([10, 10]),
                np.array([10, 50]),
                np.array([50, 50]),
                np.array([30, 30]),
            ]
        )

        self.g_poly = GeoPolygon(poly, gd.SRID.lat_lon_WGS84)

    # We're going to test 1 TypedMetadata of each type, plus
    # TypedMetadata_UNKNOWN
    def test_driver(self):
        # GPS_WEEK - int
        tag = mt.tags.VITAL_META_GPS_WEEK
        inst1 = TypedMetadata_GPS_WEEK("GPS_WEEK_NAME1", 3)
        inst2 = TypedMetadata_GPS_WEEK("GPS_WEEK_NAME2", -3)
        self.check_instance(inst1, PropInfo("GPS_WEEK_NAME1", tag, 3))
        self.check_instance(inst2, PropInfo("GPS_WEEK_NAME2", tag, -3))

        # UNIX_TIMESTAMP - uint64
        tag = mt.tags.VITAL_META_UNIX_TIMESTAMP
        inst1 = TypedMetadata_UNIX_TIMESTAMP("UNIX_TIMESTAMP_NAME1", 5)
        inst2 = TypedMetadata_UNIX_TIMESTAMP("UNIX_TIMESTAMP_NAME2", 0)
        prop_info1 = PropInfo("UNIX_TIMESTAMP_NAME1", tag, 5)
        prop_info2 = PropInfo("UNIX_TIMESTAMP_NAME2", tag, 0)
        conv_info1 = ConvInfo(has_uint64=True, as_uint64=5)
        conv_info2 = ConvInfo(has_uint64=True, as_uint64=0)
        self.check_instance(inst1, prop_info1, conv_info1)
        self.check_instance(inst2, prop_info2, conv_info2)

        # SLANT_RANGE - double
        tag = mt.tags.VITAL_META_SLANT_RANGE
        inst1 = TypedMetadata_SLANT_RANGE("SLANT_RANGE_NAME1", 3.14)
        inst2 = TypedMetadata_SLANT_RANGE("SLANT_RANGE_NAME2", -3.14)
        prop_info1 = PropInfo("SLANT_RANGE_NAME1", tag, 3.14)
        prop_info2 = PropInfo("SLANT_RANGE_NAME2", tag, -3.14)
        conv_info1 = ConvInfo(has_double=True, as_double=3.14)
        conv_info2 = ConvInfo(has_double=True, as_double=-3.14)
        self.check_instance(inst1, prop_info1, conv_info1)
        self.check_instance(inst2, prop_info2, conv_info2)

        # MISSION_ID - string
        tag = mt.tags.VITAL_META_MISSION_ID
        inst1 = TypedMetadata_MISSION_ID("MISSION_ID_NAME1", "")
        inst2 = TypedMetadata_MISSION_ID("MISSION_ID_NAME2", "data123")
        prop_info1 = PropInfo("MISSION_ID_NAME1", tag, "")
        prop_info2 = PropInfo("MISSION_ID_NAME2", tag, "data123")
        conv_info1 = ConvInfo(has_string=True, as_string="")
        conv_info2 = ConvInfo(has_string=True, as_string="data123")

        # VIDEO_KEY_FRAME - bool
        tag = mt.tags.VITAL_META_VIDEO_KEY_FRAME
        inst1 = TypedMetadata_VIDEO_KEY_FRAME("VIDEO_KEY_FRAME_NAME1", True)
        inst2 = TypedMetadata_VIDEO_KEY_FRAME("VIDEO_KEY_FRAME_NAME2", False)
        self.check_instance(inst1, PropInfo("VIDEO_KEY_FRAME_NAME1", tag, True))
        self.check_instance(inst2, PropInfo("VIDEO_KEY_FRAME_NAME2", tag, False))

        # FRAME_CENTER - kwiver::vital::geo_point
        tag = mt.tags.VITAL_META_FRAME_CENTER
        inst1 = TypedMetadata_FRAME_CENTER("FRAME_CENTER_NAME1", GeoPoint())
        inst2 = TypedMetadata_FRAME_CENTER("FRAME_CENTER_NAME2", self.g_point)
        self.check_instance(inst1, PropInfo("FRAME_CENTER_NAME1", tag, GeoPoint()))
        self.check_instance(inst2, PropInfo("FRAME_CENTER_NAME2", tag, self.g_point))

        # CORNER_POINTS - kwiver::vital::geo_polygon
        tag = mt.tags.VITAL_META_CORNER_POINTS
        inst1 = TypedMetadata_CORNER_POINTS("CORNER_POINTS_NAME1", GeoPolygon())
        inst2 = TypedMetadata_CORNER_POINTS("CORNER_POINTS_NAME2", self.g_poly)
        self.check_instance(inst1, PropInfo("CORNER_POINTS_NAME1", tag, GeoPolygon()))
        self.check_instance(inst2, PropInfo("CORNER_POINTS_NAME2", tag, self.g_poly))

        # UNKNOWN - int. There's also a separate class for the unknown tag,
        # which will be tested later
        tag = mt.tags.VITAL_META_UNKNOWN
        inst1 = TypedMetadata_UNKNOWN("UNKNOWN_NAME1", 2)
        inst2 = TypedMetadata_UNKNOWN("UNKNOWN_NAME2", -2)
        self.check_instance(inst1, PropInfo("UNKNOWN_NAME1", tag, 2))
        self.check_instance(inst2, PropInfo("UNKNOWN_NAME2", tag, -2))

    def check_instance(self, inst, prop_info, conv_info=ConvInfo(), is_valid=True):

        self.check_initial_properties(inst, prop_info)
        self.check_as_double(inst, conv_info.has_double, conv_info.as_double)
        self.check_as_uint64(inst, conv_info.has_uint64, conv_info.as_uint64)
        self.check_as_string(inst, conv_info.has_string, conv_info.as_string)
        self.check_is_valid(inst, is_valid)

    def check_initial_properties(self, inst, prop_info):
        nt.assert_equals(inst.name, prop_info.name)
        # TODO: int cast?
        nt.assert_equals(inst.tag, prop_info.tag)

        # A few tests on inst.data
        nt.ok_(isinstance(inst.data, type(prop_info.data)))
        if isinstance(prop_info.data, GeoPoint):
            nt.assert_equals(inst.data.crs(), prop_info.data.crs())
            if prop_info.data.is_empty():
                nt.ok_(inst.data.is_empty())
            else:
                inst_loc = inst.data.location()
                exp_loc = prop_info.data.location()
                np.testing.assert_array_almost_equal(inst_loc, exp_loc)

        elif isinstance(prop_info.data, GeoPolygon):
            nt.assert_equals(inst.data.crs(), prop_info.data.crs())
            if prop_info.data.is_empty():
                nt.ok_(inst.data.is_empty())
            else:
                inst_verts = inst.data.polygon().get_vertices()
                exp_verts = prop_info.data.polygon().get_vertices()
                np.testing.assert_array_almost_equal(inst_verts, exp_verts)

        else:
            nt.assert_equals(inst.data, prop_info.data)

    def check_is_valid(self, inst, is_valid):
        nt.assert_equals(bool(inst), is_valid)
        nt.assert_equals(inst.is_valid(), is_valid)
        nt.assert_equals(inst.__nonzero__(), is_valid)
        nt.assert_equals(inst.__bool__(), is_valid)

    # Following 3 functions test whether the conversion functions work
    # as expected. If the conversion is possible, test that the data matches
    # Otherwise, check that the proper exception is thrown.
    def check_as_double(self, inst, exp_has_double, exp_val):
        nt.assert_equals(inst.has_double(), exp_has_double)
        if inst.has_double():
            # Make sure that the value we're about to is predefined
            nt.ok_(exp_val is not None)
            np.testing.assert_almost_equal(inst.as_double(), exp_val)
        else:
            with nt.assert_raises(RuntimeError):
                inst.as_double()

    def check_as_uint64(self, inst, exp_has_uint64, exp_val):
        nt.assert_equals(inst.has_uint64(), exp_has_uint64)
        if inst.has_uint64():
            # Make sure that the value we're about to is predefined
            nt.ok_(exp_val is not None)
            nt.assert_equals(inst.as_uint64(), exp_val)
        else:
            with nt.assert_raises(RuntimeError):
                inst.as_uint64()

    # Strings are a special case. has_string() == False doesn't mean
    # that as_string() will raise an exception.
    def check_as_string(self, inst, exp_has_string, exp_val):
        nt.assert_equals(inst.has_string(), exp_has_string)
        if inst.has_string():
            # Make sure that the value we're about to is predefined
            nt.ok_(exp_val is not None)
            nt.assert_equals(inst.as_string(), exp_val)
        else:
            # with nt.assert_raises(None):
            if True:
                print(type(inst.as_string()))

    #     expected_types = [
    #         "int",
    #         "unsigned long",
    #         "double",
    #         "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >",
    #         "bool",
    #         "kwiver::vital::geo_point",
    #         "kwiver::vital::geo_polygon",
    #         "int",
    #     ]
