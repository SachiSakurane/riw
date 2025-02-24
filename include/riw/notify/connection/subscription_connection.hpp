#pragma once

#include <optional>

#include <riw/notify/concepts/observable.hpp>
#include <riw/notify/store.hpp>

namespace riw {

namespace detail {
  template <class F, class O>
  concept connectable_observable_to_void_function =
      observable<O> && std::is_invocable_v<F, typename O::value_type> &&
      std::is_same_v<std::invoke_result_t<F, typename O::value_type>, void>;

  template <observable O, connectable_observable_to_void_function<O> F>
  struct connection_subscription_traits {
    using value_type = typename O::value_type;

    using subscription_type = subscription<value_type>;
  };

  template <observable O, connectable_observable_to_void_function<O> F>
  class subscription_connection : public riw::store {
  public:
    using value_type = connection_subscription_traits<O, F>::value_type;

    class subscription_impl : public connection_subscription_traits<O, F>::subscription_type {
    public:
      constexpr subscription_impl() {}
      constexpr subscription_impl(O &o, F &&f) : obs{o}, func{std::forward<F>(f)} {}

      std::optional<std::reference_wrapper<O>> get_observable() { return obs; }
      void receive(value_type v) override { func(v); }

    private:
      std::optional<std::reference_wrapper<O>> obs;
      const F func;
    };

    constexpr subscription_connection(O &o, F &&f) {
      sub = std::make_shared<subscription_impl>(o, std::forward<F>(f));
      sub->get_observable()->get().add_subscribe(sub);
    }

  private:
    std::shared_ptr<subscription_impl> sub;
  };

  template <observable O, connectable_observable_to_void_function<O> F>
  class subscription_connection_r : public riw::store {
  public:
    using value_type = connection_subscription_traits<O, F>::value_type;

    class subscription_impl : public connection_subscription_traits<O, F>::subscription_type {
    public:
      constexpr subscription_impl(O &&o, F &&f)
          : obs{std::forward<O>(o)}, func{std::forward<F>(f)} {}

      std::optional<O> &get_observable() { return obs; }
      void receive(value_type v) override { func(v); }

    private:
      std::optional<O> obs;
      const F func;
    };

    constexpr subscription_connection_r(O &&o, F &&f)
        : sub{std::make_shared<subscription_impl>(std::forward<O>(o), std::forward<F>(f))} {
      sub->get_observable()->add_subscribe(sub);
    }

  private:
    std::shared_ptr<subscription_impl> sub;
  };

} // namespace detail

template <observable O, detail::connectable_observable_to_void_function<O> F>
[[nodiscard]] inline auto operator|(O &o, F &&f) {
  return detail::subscription_connection<O, F>{o, std::forward<F>(f)};
}

template <observable O, detail::connectable_observable_to_void_function<O> F>
[[nodiscard]] inline auto operator|(O &&o, F &&f) {
  return detail::subscription_connection_r<O, F>{std::forward<O>(o), std::forward<F>(f)};
}

} // namespace riw
