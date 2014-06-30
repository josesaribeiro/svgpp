#pragma once

#include <svgpp/config.hpp>
#include <svgpp/context_policy_load_value.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/parser/detail/common.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace svgpp
{

namespace detail
{
  template<class AttributeTag, class Context, class ValueType>
  inline void call_set_value(Context & context, ValueType const & value)
  {
    context_policy<tag::load_value_policy, Context>::set(context, AttributeTag(), value);
  }

  template<class AttributeTag, class Context, class ValueType>
  inline void call_set_value_def(Context & context)
  {
    context_policy<tag::load_value_policy, Context>::set(context, AttributeTag(), ValueType());
  }
}

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::attribute::viewBox, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class Context, class AttributeValue>
  static bool parse(tag::attribute::viewBox tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::attribute)
  {
    namespace qi = boost::spirit::qi;
    using namespace boost::phoenix;
    using qi::_1;

    typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::get_number_type::template apply<Context>::type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);

    SVGPP_STATIC_IF_SAFE const qi::real_parser<coordinate_t, detail::svg_real_policies<coordinate_t> > number;
    SVGPP_STATIC_IF_SAFE const detail::comma_wsp_rule<iterator_t> comma_wsp;
    coordinate_t x, y, w, h;
    if (qi::parse(it, end, 
      number[ref(x) = _1] >> qi::no_skip[comma_wsp] >>
      number[ref(y) = _1] >> qi::no_skip[comma_wsp] >>
      number[ref(w) = _1] >> qi::no_skip[comma_wsp] >>
      number[ref(h) = _1]) && it == end)
    {
      context_policy<tag::load_value_policy, Context>::set(context, tag, x, y, w, h);
      return true;
    }
    else
    {
      return args_t::get_error_policy::template apply<Context>::type::parse_failed(context, tag, attribute_value);
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::attribute::bbox, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class Context, class AttributeValue>
  static bool parse(tag::attribute::bbox tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::attribute)
  {
    namespace qi = boost::spirit::qi;
    using namespace boost::phoenix;
    using qi::_1;

    typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::get_number_type::template apply<Context>::type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);

    SVGPP_STATIC_IF_SAFE const qi::real_parser<coordinate_t, detail::svg_real_policies<coordinate_t> > number;
    coordinate_t lo_x, lo_y, hi_x, hi_y;
    if (qi::parse(it, end, 
      number[ref(lo_x) = _1] >> qi::lit(',') >>
      number[ref(lo_y) = _1] >> qi::lit(',') >>
      number[ref(hi_x) = _1] >> qi::lit(',') >>
      number[ref(hi_y) = _1]) && it == end)
    {
      context_policy<tag::load_value_policy, Context>::set(context, tag, lo_x, lo_y, hi_x, hi_y);
      return true;
    }
    else
    {
      return args_t::get_error_policy::template apply<Context>::type::parse_failed(context, attribute_value);
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::attribute::preserveAspectRatio, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class Context, class AttributeValue>
  static bool parse(tag::attribute::preserveAspectRatio tag, Context & context, 
    AttributeValue const & attribute_value, tag::source::attribute)
  {
    namespace qi = boost::spirit::qi;
    using namespace boost::phoenix;
    using qi::_1;
    using qi::_a;
    using qi::_b;
    using qi::_c;

    typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    qi::rule<iterator_t, qi::locals<bool, int, bool> > rule 
        =   (  -( qi::lit("defer") [_a = true]
                  >> +qi::ascii::space
                )
            >>  ( qi::lit("none") [_b = 1]
                | qi::lit("xMinYMin") [_b = 2]
                | qi::lit("xMidYMin") [_b = 3]
                | qi::lit("xMaxYMin") [_b = 4]
                | qi::lit("xMinYMid") [_b = 5]
                | qi::lit("xMidYMid") [_b = 6]
                | qi::lit("xMaxYMid") [_b = 7]
                | qi::lit("xMinYMax") [_b = 8]
                | qi::lit("xMidYMax") [_b = 9]
                | qi::lit("xMaxYMax") [_b = 10]
                )
            >> -( +qi::ascii::space
                >> ( qi::lit("meet") [_c = false]
                   | qi::lit("slice") [_c = true] 
                   ) 
                )
            ) [bind(&value_parser::call_set_value<Context>, ref(context), _a, _b, _c)];
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    if (qi::parse(it, end, rule) && it == end)
      return true;
    else
    {
      return args_t::get_error_policy::template apply<Context>::type::parse_failed(context, tag, attribute_value);
    }
  }

  template<class Context>
  static void call_set_value(Context & context, bool defer, int align, bool slice)
  {
    switch (align)
    {
    case 1:
      context_policy<tag::load_value_policy, Context>::set(context, 
        tag::attribute::preserveAspectRatio(), 
        defer, tag::value::none());
      break;
#define SVGPP_CASE(value_tag) \
      if (slice) \
        context_policy<tag::load_value_policy, Context>::set(context, \
          tag::attribute::preserveAspectRatio(), \
          defer, tag::value::value_tag(), tag::value::slice()); \
      else \
        context_policy<tag::load_value_policy, Context>::set(context, \
          tag::attribute::preserveAspectRatio(), \
          defer, tag::value::value_tag(), tag::value::meet()); \
      break;

      case 2: SVGPP_CASE(xMinYMin);
      case 3: SVGPP_CASE(xMidYMin);
      case 4: SVGPP_CASE(xMaxYMin);
      case 5: SVGPP_CASE(xMinYMid);
      case 6: SVGPP_CASE(xMidYMid);
      case 7: SVGPP_CASE(xMaxYMid);
      case 8: SVGPP_CASE(xMinYMax);
      case 9: SVGPP_CASE(xMidYMax);
      case 10: SVGPP_CASE(xMaxYMax);
#undef SVGPP_CASE
    }
  }
};

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<boost::mpl::pair<tag::element::stop, tag::attribute::offset>, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class Context, class AttributeValue>
  static bool parse(tag::attribute::offset tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::attribute)
  {
    namespace qi = boost::spirit::qi;
    using namespace boost::phoenix;
    using qi::_1;

    typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::get_number_type::template apply<Context>::type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);

    SVGPP_STATIC_IF_SAFE const qi::real_parser<coordinate_t, detail::number_policies<coordinate_t, tag::source::attribute> > number;
    coordinate_t value;
    bool percentage = false;
    if (qi::parse(it, end, number[ref(value) = _1] >> -(qi::lit('%')[ref(percentage) = true])) && it == end)
    {
      context_policy<tag::load_value_policy, Context>::set(context, tag, percentage ? value : value / 100);
      return true;
    }
    else
    {
      return args_t::get_error_policy::template apply<Context>::type::parse_failed(context, tag, attribute_value);
    }
  }
};

}