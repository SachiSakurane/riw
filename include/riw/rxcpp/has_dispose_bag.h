#pragma once

#include <riw/rxcpp/dispose_bag.h>
#include <riw/utility/noncopyable.h>

namespace riw
{
  template <class Type>
  class has_dispose_bag : private riw::noncopyable<Type>
  {
  protected:
    riw::dispose_bag bag;
  };
}
