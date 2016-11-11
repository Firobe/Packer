// Copyright Oleg Maximenko 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://github.com/svgpp/svgpp for library home page.

#pragma once

#include <boost/optional.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/range/iterator_range.hpp>
#include <svgpp/definitions.hpp>

namespace svgpp { namespace detail 
{

enum paint_option
{
  paint_option_not_set, 
  paint_option_none,
  paint_option_currentColor,
  paint_option_inherit,
  paint_option_color,
  paint_option_funciri
};

template<class ColorFactory, class IccColorFactory, class Iterator, class PropertySource>
bool parse_paint(
  IccColorFactory const & icc_color_factory,
  Iterator & it, Iterator end, PropertySource property_source, 
  paint_option & out_main_option, paint_option & out_funciri_suboption, 
  boost::tuple<
    typename ColorFactory::color_type, boost::optional<typename IccColorFactory::icc_color_type> 
  > & out_color,
  typename boost::iterator_range<Iterator> & out_iri);

}}