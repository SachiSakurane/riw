#pragma once

#include <riw/rxcpp/dispose_bag.hpp>

namespace riw {
class disposed_tag {
public:
  explicit disposed_tag(riw::dispose_bag &bag) : bag_{bag} {}

  void push(rxcpp::composite_subscription &&subscription) { bag_.push(std::move(subscription)); }

private:
  riw::dispose_bag &bag_;
};

inline void operator|(rxcpp::composite_subscription &&subscription, riw::disposed_tag bag) {
  bag.push(std::move(subscription));
}

inline disposed_tag disposed(riw::dispose_bag &by) { return disposed_tag{by}; }
} // namespace riw
