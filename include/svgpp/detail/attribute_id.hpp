#pragma once

namespace svgpp { namespace detail
{

  enum attribute_id
  {
#define SVGPP_ON(attribute_name, attribute_string) 
#define SVGPP_ON_NS(ns, attribute_name, attribute_string) 
#define SVGPP_ON_STYLE(attribute_name, attribute_string) attribute_id_## attribute_name,
#include <svgpp/detail/dict/enumerate_all_attributes.inc>
#undef SVGPP_ON
#undef SVGPP_ON_NS
#undef SVGPP_ON_STYLE
    styling_attribute_count,
    last_styling_attribute = styling_attribute_count - 1,
#define SVGPP_ON(attribute_name, attribute_string) attribute_id_## attribute_name,
#define SVGPP_ON_NS(ns, attribute_name, attribute_string) SVGPP_ON(ns ## _ ## attribute_name, attribute_string) 
#define SVGPP_ON_STYLE(attribute_name, attribute_string)
#include <svgpp/detail/dict/enumerate_all_attributes.inc>
#undef SVGPP_ON
#undef SVGPP_ON_NS
#undef SVGPP_ON_STYLE

    attribute_count,
    unknown_attribute_id = attribute_count
  };

}}