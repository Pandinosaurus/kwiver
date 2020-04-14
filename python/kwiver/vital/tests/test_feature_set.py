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

Tests for Python interface to vital::uid

"""

from kwiver.vital.types import Feature, FeatureSet, SimpleFeatureSet

import nose.tools as nt


class TestVitalFeatureSet(object):
    def test_new(self):
        FeatureSet()

    def test_bad_call_virtual_size(self):
        fs = FeatureSet()
        with nt.assert_raises_regexp(
            RuntimeError, "Tried to call pure virtual function"
        ):
            fs.size()

    def test_bad_call_virtual_size(self):
        fs = FeatureSet()
        with nt.assert_raises_regexp(
            RuntimeError, "Tried to call pure virtual function"
        ):
            fs.features()

    # TODO: inherit from this class and implement methods


class TestVitalSimpleFeatureSet(object):
    def _create_features(self):
        pass

    def test_new(self):
        SimpleFeatureSet()
        # TODO

    def test_size(self):
        sfs = SimpleFeatureSet()

        nt.assert_equals(sfs.size(), 0)

        # TODO

    def test_features(self):
        sfs = SimpleFeatureSet()

        nt.assert_equals(sfs.features(), [])

        # TODO
