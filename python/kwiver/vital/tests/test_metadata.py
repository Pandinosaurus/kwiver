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
# 1.) Test constructor?
# 3.) metadata::typeid_for_tag
# 4.) metadata::add
# 8.) stuff with std::string name mangling, testing .type!
# 9.) VIRTUAL AND OVERRIDES IN DERIVED CLASSES, AS_UINT WRONG RETURN TYPE

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

# Helper class that stores a typed_metadata object's
# name, tag, and data properties
class PropInfo(object):
    def __init__(self, name, tag, data):
        self.name = name
        self.tag = tag
        self.data = data


# Helper class that stores which types a typed_metadata
# can be converted to
class TypeInfo(object):
    def __init__(
        self,
        has_double=False,
        has_uint64=False,
        has_string=False,
        as_double=None,
        as_uint64=None,
        as_string=None,
    ):
        self.has_double = has_double
        self.has_uint64 = has_uint64
        self.has_string = has_string

        self.as_double = as_double
        self.as_uint64 = as_uint64
        self.as_string = as_string


class TestVitalMetadataItem(object):
    def test_no_construct_base(self):
        err_msg = "kwiver.vital.types.metadata.MetadataItem: No constructor defined!"
        with nt.assert_raises_regexp(TypeError, err_msg):
            MetadataItem()


# There are 2 subclasses (although one is templated),
# typed_metadata and unknown_metadata. typed_metadata has 2 templated parameters.
# Because of pybind's inability to handle template types, there is a separate class for
# each instantiation.
class TestVitalMetadataItemSubclasses(object):
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
        type_info1 = TypeInfo(as_string="3")
        type_info2 = TypeInfo(as_string="-3")
        self.check_instance(inst1, PropInfo("GPS_WEEK_NAME1", tag, 3), type_info1)
        self.check_instance(inst2, PropInfo("GPS_WEEK_NAME2", tag, -3), type_info2)

        # UNIX_TIMESTAMP - uint64
        tag = mt.tags.VITAL_META_UNIX_TIMESTAMP
        inst1 = TypedMetadata_UNIX_TIMESTAMP("UNIX_TIMESTAMP_NAME1", 5)
        inst2 = TypedMetadata_UNIX_TIMESTAMP("UNIX_TIMESTAMP_NAME2", 0)
        prop_info1 = PropInfo("UNIX_TIMESTAMP_NAME1", tag, 5)
        prop_info2 = PropInfo("UNIX_TIMESTAMP_NAME2", tag, 0)
        type_info1 = TypeInfo(has_uint64=True, as_uint64=5, as_string="5")
        type_info2 = TypeInfo(has_uint64=True, as_uint64=0, as_string="0")
        self.check_instance(inst1, prop_info1, type_info1)
        self.check_instance(inst2, prop_info2, type_info2)

        # SLANT_RANGE - double
        tag = mt.tags.VITAL_META_SLANT_RANGE
        inst1 = TypedMetadata_SLANT_RANGE("SLANT_RANGE_NAME1", 3.14)
        inst2 = TypedMetadata_SLANT_RANGE("SLANT_RANGE_NAME2", -3.14)
        prop_info1 = PropInfo("SLANT_RANGE_NAME1", tag, 3.14)
        prop_info2 = PropInfo("SLANT_RANGE_NAME2", tag, -3.14)
        type_info1 = TypeInfo(has_double=True, as_double=3.14, as_string="3.14")
        type_info2 = TypeInfo(has_double=True, as_double=-3.14, as_string="-3.14")
        self.check_instance(inst1, prop_info1, type_info1)
        self.check_instance(inst2, prop_info2, type_info2)

        # MISSION_ID - string
        tag = mt.tags.VITAL_META_MISSION_ID
        inst1 = TypedMetadata_MISSION_ID("MISSION_ID_NAME1", "")
        inst2 = TypedMetadata_MISSION_ID("MISSION_ID_NAME2", "data123")
        prop_info1 = PropInfo("MISSION_ID_NAME1", tag, "")
        prop_info2 = PropInfo("MISSION_ID_NAME2", tag, "data123")
        type_info1 = TypeInfo(has_string=True, as_string="")
        type_info2 = TypeInfo(has_string=True, as_string="data123")
        self.check_instance(inst1, prop_info1, type_info1)
        self.check_instance(inst2, prop_info2, type_info2)

        # VIDEO_KEY_FRAME - bool
        tag = mt.tags.VITAL_META_VIDEO_KEY_FRAME
        inst1 = TypedMetadata_VIDEO_KEY_FRAME("VIDEO_KEY_FRAME_NAME1", True)
        inst2 = TypedMetadata_VIDEO_KEY_FRAME("VIDEO_KEY_FRAME_NAME2", False)
        type_info1 = TypeInfo(as_string="True")
        type_info2 = TypeInfo(as_string="False")
        self.check_instance(inst1, PropInfo("VIDEO_KEY_FRAME_NAME1", tag, True), type_info1)
        self.check_instance(inst2, PropInfo("VIDEO_KEY_FRAME_NAME2", tag, False), type_info2)

        # FRAME_CENTER - kwiver::vital::geo_point
        tag = mt.tags.VITAL_META_FRAME_CENTER
        inst1 = TypedMetadata_FRAME_CENTER("FRAME_CENTER_NAME1", GeoPoint())
        inst2 = TypedMetadata_FRAME_CENTER("FRAME_CENTER_NAME2", self.g_point)
        type_info1 = TypeInfo(as_string=str(GeoPoint()))
        type_info2 = TypeInfo(as_string=str(self.g_point))
        self.check_instance(inst1, PropInfo("FRAME_CENTER_NAME1", tag, GeoPoint()), type_info1)
        self.check_instance(inst2, PropInfo("FRAME_CENTER_NAME2", tag, self.g_point), type_info2)

        # CORNER_POINTS - kwiver::vital::geo_polygon
        tag = mt.tags.VITAL_META_CORNER_POINTS
        inst1 = TypedMetadata_CORNER_POINTS("CORNER_POINTS_NAME1", GeoPolygon())
        inst2 = TypedMetadata_CORNER_POINTS("CORNER_POINTS_NAME2", self.g_poly)
        type_info1 = TypeInfo(as_string=str(GeoPolygon()))
        type_info2 = TypeInfo(as_string=str(self.g_poly))
        self.check_instance(inst1, PropInfo("CORNER_POINTS_NAME1", tag, GeoPolygon()), type_info1)
        self.check_instance(inst2, PropInfo("CORNER_POINTS_NAME2", tag, self.g_poly), type_info2)

        # UNKNOWN - int. There's also a separate class for the unknown tag,
        # which will be tested later
        tag = mt.tags.VITAL_META_UNKNOWN
        inst1 = TypedMetadata_UNKNOWN("UNKNOWN_NAME1", 2)
        inst2 = TypedMetadata_UNKNOWN("UNKNOWN_NAME2", -2)
        type_info1 = TypeInfo(as_string="2")
        type_info2 = TypeInfo(as_string="-2")
        self.check_instance(inst1, PropInfo("UNKNOWN_NAME1", tag, 2), type_info1)
        self.check_instance(inst2, PropInfo("UNKNOWN_NAME2", tag, -2), type_info2)

        # UnknownMetadataItem - This is a separate subclass of metadata_item
        # Can keep the tag the same
        inst = UnknownMetadataItem()
        exp_name = "Requested metadata item is not in collection"
        exp_string = "--Unknown metadata item--"
        prop_info = PropInfo(exp_name, tag, 0)
        type_info = TypeInfo(as_string=exp_string, as_double=0, as_uint64=0)
        self.check_instance(inst, prop_info, type_info, is_valid=False)

    def check_instance(self, inst, prop_info, type_info, is_valid=True):
        self.check_initial_properties(inst, prop_info)
        self.check_as_double(inst, type_info.has_double, type_info.as_double)
        self.check_as_uint64(inst, type_info.has_uint64, type_info.as_uint64)
        self.check_as_string(inst, type_info.has_string, type_info.as_string)
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
        if inst.has_double() or exp_val is not None:
            # Make sure that the value we're about to compare is correct type
            np.testing.assert_almost_equal(inst.as_double(), exp_val)
        else:
            with nt.assert_raises(RuntimeError):
                inst.as_double()

    def check_as_uint64(self, inst, exp_has_uint64, exp_val):
        nt.assert_equals(inst.has_uint64(), exp_has_uint64)
        if inst.has_uint64() or exp_val is not None:
            # Make sure that the value we're about to compare is correct type
            nt.assert_equals(inst.as_uint64(), exp_val)
        else:
            with nt.assert_raises(RuntimeError):
                inst.as_uint64()

    # as_string is defined for every type
    def check_as_string(self, inst, exp_has_string, exp_val):
        nt.assert_equals(inst.has_string(), exp_has_string)
        # Always able to convert
        nt.assert_equals(inst.as_string(), exp_val)

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


# And finally, the actual metadata class
class TestVitalMetadata(object):
    def test_init(self):
        Metadata()

    def test_add(self):
        m = Metadata()
        nt.assert_equals(m.size(), 0)
        nt.ok_(m.empty())
        # TODO...

    def test_erase(self):
        pass

    # Tests for has and find
    def test_retrieve(self):
        m = Metadata()
        # Make sure there are no initial elements
        nt.assert_false(m.has(mt.tags.VITAL_META_GPS_WEEK))
        nt.assert_false(m.has(mt.tags.VITAL_META_VIDEO_KEY_FRAME))
        nt.assert_false(m.has(mt.tags.VITAL_META_UNKNOWN))
        nt.ok_(isinstance(m.find(mt.tags.VITAL_META_GPS_WEEK)), UnknownMetadataItem)
        nt.ok_(isinstance(m.find(mt.tags.VITAL_META_VIDEO_KEY_FRAME)), UnknownMetadataItem)
        nt.ok_(isinstance(m.find(mt.tags.VITAL_META_UNKNOWN)), UnknownMetadataItem)

    def test_timestamp(self):
        pass
