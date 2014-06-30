#pragma once

#include <svgpp/config.hpp>
#include <svgpp/definitions.hpp>
#include <svgpp/adapter/path.hpp>
#include <svgpp/parser/value_parser_fwd.hpp>
#include <svgpp/parser/detail/value_parser_parameters.hpp>
#include <svgpp/parser/grammar/path_data.hpp>

namespace svgpp
{

template<SVGPP_TEMPLATE_ARGS>
struct value_parser<tag::type::path_data, SVGPP_TEMPLATE_ARGS_PASS>
{
  template<class AttributeTag, class Context, class AttributeValue>
  static bool parse(AttributeTag tag, Context & context, AttributeValue const & attribute_value, 
                                    tag::source::attribute)
  {
    namespace qi = boost::spirit::qi;

    typedef detail::value_parser_parameters<SVGPP_TEMPLATE_ARGS_PASS> args_t;
    typedef typename args_t::get_number_type::template apply<Context>::type coordinate_t;
    typedef typename boost::range_const_iterator<AttributeValue>::type iterator_t;

    typedef typename boost::parameter::parameters<
      boost::parameter::optional<tag::path_policy>
    >::template bind<SVGPP_TEMPLATE_ARGS_PASS>::type args2_t;
    typedef typename boost::parameter::value_type<args2_t, tag::path_policy, 
      context_policy<tag::path_policy, Context> >::type path_policy;
    typedef detail::path_adapter_if_needed<Context, path_policy, coordinate_t> adapted_context_type; 
    typedef path_data_grammar<iterator_t, typename adapted_context_type::type, coordinate_t> path_data_grammar;

    typename adapted_context_type::holder_type adapted_context(context);
    SVGPP_STATIC_IF_SAFE const path_data_grammar grammar;
    iterator_t it = boost::begin(attribute_value), end = boost::end(attribute_value);
    if (qi::phrase_parse(it, end, grammar(boost::phoenix::ref(adapted_context)), typename path_data_grammar::skipper_type()) 
      && it == end)
    {
      return true;
    }
    else
    {
      return args_t::get_error_policy::template apply<Context>::type::parse_failed(context, tag, attribute_value);
    }
  }
};

}