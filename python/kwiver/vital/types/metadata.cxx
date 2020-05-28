/*ckwg +29
 * Copyright 2020 by Kitware, Inc.
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

#include <vital/types/metadata.h>
#include <vital/types/metadata_tags.h>
#include <vital/types/geo_point.h>
#include <vital/types/geo_polygon.h>
#include <vital/util/demangle.h>

#include <pybind11/pybind11.h>

#include <memory>
#include <string>


namespace py = pybind11;
using namespace kwiver::vital;

#define REGISTER_TYPED_METADATA( TAG, NAME, T, ... ) \
  py::class_< typed_metadata< VITAL_META_ ## TAG, T >, \
              std::shared_ptr< typed_metadata< VITAL_META_ ## TAG, T > >>( m, "typed_metadata_" #TAG ) \
  .def( py::init( [] ( std::string name, T const& data ) \
  { \
    return typed_metadata< VITAL_META_ ## TAG, T >( name, any( data ) ); \
  })) \
  .def( "type", [] ( typed_metadata< VITAL_META_ ## TAG, T > const& self ) \
  { \
    return demangle( self.type().name() ); \
  }) \
  .def( "as_string", &typed_metadata< VITAL_META_ ## TAG, T >::as_string ) \
  .def_property_readonly( "data", [] ( typed_metadata< VITAL_META_ ## TAG, T > const& self ){
    any //TODO: start here
  })
  ;


PYBIND11_MODULE( metadata, m )
{
  py::class_< metadata_item, std::shared_ptr< metadata_item >>( m, "metadata_item" )
  .def( "is_valid",    &metadata_item::is_valid )
  .def( "__nonzero__", [] ( metadata_item const& self )
  {
    return bool(self);
  })
  .def( "__bool__",    [] ( metadata_item const& self )
  {
    return bool(self);
  })
  .def_property_readonly( "name", &metadata_item::name )
  .def_property_readonly( "tag",  &metadata_item::tag )
  // NOTE: data() is put into the derived class.
  // Otherwise, we'd have to create a separate data() function for each type.

  // TODO test this
  .def( "type", &metadata_item::type )
  .def( "as_double",  &metadata_item::as_double )
  .def( "has_double", &metadata_item::has_double )
  .def( "as_uint64",  &metadata_item::as_uint64 )
  .def( "has_uint64", &metadata_item::has_uint64 )
  .def( "__str__",    &metadata_item::as_string )
  .def( "has_string", &metadata_item::has_string )

  // No print_value() since it is almost the same as as_string,
  // except it accepts a stream as argument, which can be pre-configured
  // with a certain precision. Python users obviously won't be able to use this,
  // so we'll just bind as_string.
  ;

  // Now bind all of the typed_metadatas
  KWIVER_VITAL_METADATA_TAGS( REGISTER_TYPED_METADATA )

  // TODO: exceptions??
  // Now bind the actual metadata class
  py::class_< metadata, std::shared_ptr< metadata > >( m, "metadata" )
  // TODO
  // .def( "add" )
  .def( "erase", &metadata::erase )
  .def( "has", &metadata::has )
  .def( "find", &metadata::find )
  // TODO: test
  .def( "__iter__", [] ( metadata& self )
  {
    return py::make_iterator(self.cbegin(), self.cend());
  }, py::keep_alive<0, 1>())
  .def( "size", &metadata::size )
  .def( "empty", &metadata::empty )
  .def( "set_timestamp", &metadata::set_timestamp )
  .def( "timestamp", &metadata::timestamp )

  // TODO: test
  .def_static( "typeid_for_tag", [] ( metadata const& self, vital_metadata_tag tag )
  {
    return demangle(self.typeid_for_tag( tag ).name());
  })
  .def_static( "format_string", &metadata::format_string )
  ;

  m.def( "test_equal_content", &test_equal_content )
  ;
}

#undef REGISTER_TYPED_METADATA