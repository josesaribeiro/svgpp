#pragma once

// RapidXML NS headers must be already included by user

#include <svgpp/definitions.hpp>
#include <svgpp/xml_policy_fwd.hpp>
#include <svgpp/detail/namespace.hpp>
#include <boost/range/iterator_range.hpp>

namespace svgpp
{

template<class Ch>
struct xml_attribute_iterator_policy<rapidxml_ns::xml_attribute<Ch> const *>
{
  typedef rapidxml_ns::xml_attribute<Ch> const * iterator_type;
  typedef boost::iterator_range<Ch const *> attribute_name_type;
  typedef boost::iterator_range<Ch const *> attribute_value_type;

  static const bool store_value = true; // Preferred way of storing attribute: value

  static void advance(iterator_type & xml_attribute)
  {
    xml_attribute = xml_attribute->next_attribute();
  }

  static bool is_end(iterator_type xml_attribute)
  {
    return xml_attribute == NULL;
  }

  static detail::namespace_id get_namespace(iterator_type xml_attribute)
  {
    if (xml_attribute->namespace_uri_size() == 0)
      return detail::namespace_id::svg;
    boost::iterator_range<Ch const *> ns_uri(xml_attribute->namespace_uri(), 
      xml_attribute->namespace_uri() + xml_attribute->namespace_uri_size());
    if (boost::range::equal(detail::xml_namespace_uri<Ch>(), ns_uri))
      return detail::namespace_id::xml;
    else if (boost::range::equal(detail::xlink_namespace_uri<Ch>(), ns_uri))
      return detail::namespace_id::xlink;
    return detail::namespace_id::other;
  }

  static attribute_name_type get_local_name(iterator_type xml_attribute)
  {
    return attribute_name_type(xml_attribute->local_name(), xml_attribute->local_name() + xml_attribute->local_name_size());
  }

  static attribute_value_type get_value(iterator_type xml_attribute)
  {
    return attribute_value_type(xml_attribute->value(), xml_attribute->value() + xml_attribute->value_size());
  }
};

template<class Ch>
struct xml_attribute_iterator_policy<rapidxml_ns::xml_attribute<Ch> *>
  : xml_attribute_iterator_policy<rapidxml_ns::xml_attribute<Ch> const *>
{
  static void advance(typename rapidxml_ns::xml_attribute<Ch> *& xml_attribute)
  {
    xml_attribute = xml_attribute->next_attribute();
  }
};

template<class Ch>
struct xml_element_iterator_policy<rapidxml_ns::xml_node<Ch> const *>
{
  typedef rapidxml_ns::xml_node<Ch> const * iterator_type;
  typedef boost::iterator_range<Ch const *> element_name_type;
  typedef Ch const * element_text_type;
  typedef rapidxml_ns::xml_attribute<Ch> const * attribute_enumerator_type;

  static void advance_element(iterator_type & xml_element)
  {
    xml_element = xml_element->next_sibling(); 
    find_next(xml_element, false);
  }

  static void advance_element_or_text(iterator_type xml_element)
  {
    xml_element = xml_element->next_sibling(); 
    find_next(xml_element, true);
  }

  static bool is_end(iterator_type xml_element)
  {
    return xml_element == NULL;
  }

  static bool is_text(iterator_type xml_element)
  {
    return xml_element->types() != rapidxml_ns::node_element;
  }

  static element_name_type get_local_name(iterator_type xml_element)
  {
    return element_name_type(xml_element->name(), xml_element->name() + xml_element->name_size());
  }

  static element_text_type get_text(iterator_type xml_element)
  {
    return element_text_type(xml_element->value(), xml_element->value() + xml_element->value_size());
  }

  static attribute_enumerator_type get_attributes(iterator_type xml_element)
  {
    return xml_element->first_attribute();
  }

  static iterator_type get_child_elements(iterator_type xml_element)
  {
    iterator_type child_element = xml_element->first_node();
    find_next(child_element, false);
    return child_element;
  }

  static iterator_type get_child_elements_and_texts(iterator_type xml_element)
  {
    iterator_type child_element = xml_element->first_node();
    find_next(child_element, true);
    return child_element;
  }

private:
  static void find_next(iterator_type & xml_element, bool texts_also)
  {
    // TODO: optimize namespace checking by saving pointer to last namespace_uri() known to be SVG
    for(; xml_element; xml_element = xml_element->next_sibling())
    {
      switch(xml_element->type())
      {
      case rapidxml_ns::node_element:
      {
        boost::iterator_range<Ch const *> ns_uri(xml_element->namespace_uri(), 
          xml_element->namespace_uri() + xml_element->namespace_uri_size());
        if (boost::range::equal(detail::svg_namespace_uri<Ch>(), ns_uri))
          return;
        break;
      }
      case rapidxml_ns::node_data:
      case rapidxml_ns::node_cdata:
        if (texts_also)
          return;
        break;
      }
    }
  }
};

template<class Ch>
struct xml_element_iterator_policy<rapidxml_ns::xml_node<Ch> *>
  : xml_element_iterator_policy<rapidxml_ns::xml_node<Ch> const *>
{
};

}