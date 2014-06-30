#pragma once

#include <svgpp/context_policy_load_path.hpp>
#include <svgpp/context_policy_load_value.hpp>
#include <svgpp/definitions.hpp>
#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <boost/mpl/set.hpp>
#include <stdexcept>

namespace svgpp
{

typedef boost::mpl::set4<tag::attribute::x1, tag::attribute::y1, tag::attribute::x2, tag::attribute::y2>
  line_shape_attributes;

template<class Length>
class collect_line_attributes_adapter: boost::noncopyable
{
public:
  template<class Context, class LengthToUserCoordinatesConverter>
  void on_exit_attributes(Context & context, LengthToUserCoordinatesConverter const & converter) const
  {
    on_exit_attributesT<context_policy<tag::load_value_policy, Context> >(context, converter);
  }

  template<class ErrorPolicy, class LoadPolicy, class Context, class LengthToUserCoordinatesConverter>
  bool on_exit_attributesT(Context & context, LengthToUserCoordinatesConverter const & converter) const
  {
    typename LengthToUserCoordinatesConverter::coordinate_type
      x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    if (x1_)
      x1 = converter.length_to_user_coordinate(*x1_, tag::width_length());
    if (x2_)
      x2 = converter.length_to_user_coordinate(*x2_, tag::width_length());
    if (y1_)
      y1 = converter.length_to_user_coordinate(*y1_, tag::height_length());
    if (y2_)
      y2 = converter.length_to_user_coordinate(*y2_, tag::height_length());

    LoadPolicy::set_line(context, x1, y1, x2, y2);
    return true;
  }

  void set(tag::attribute::x1, Length const & val) { x1_ = val; }
  void set(tag::attribute::y1, Length const & val) { y1_ = val; }
  void set(tag::attribute::x2, Length const & val) { x2_ = val; }
  void set(tag::attribute::y2, Length const & val) { y2_ = val; }

private:
  boost::optional<Length> x1_, y1_, x2_, y2_;
};

template<class LoadPathPolicy = void>
struct line_to_path_adapter
{
  template<class Context, class Coordinate>
  static void set_line(Context & context, Coordinate x1, Coordinate y1, Coordinate x2, Coordinate y2)
  {
    typedef typename boost::mpl::if_<
      boost::is_same<LoadPathPolicy, void>,
      context_policy<tag::load_path_policy, Context>,
      LoadPathPolicy
    >::type load_policy;

    load_policy::path_move_to(context, x1, y1, tag::absolute_coordinate());
    load_policy::path_line_to(context, x2, y2, tag::absolute_coordinate());
    load_policy::path_exit(context);
  }

private:
  line_to_path_adapter();
};

}