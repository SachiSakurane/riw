#pragma once

#include <type_traits>

#include <riw/notify/concepts/observable.hpp>
#include <riw/notify/concepts/subscribable.hpp>

namespace riw {

namespace detail {
  template <class F, class O>
  concept connectable_observable_to_function =
      observable<O> && std::is_invocable_v<F, typename O::value_type> &&
      !std::is_same_v<std::invoke_result_t<F, typename O::value_type>, void>;

  template <observable O, connectable_observable_to_function<O> F>
  struct connection_observable_traits {
    using value_type = typename O::value_type;
    using invoke_result_type = std::invoke_result_t<F, value_type>;

    using observer_type = observer<invoke_result_type>;
    using subscription_type = subscription<value_type>;
  };

  template <observable O, connectable_observable_to_function<O> F>
  class observable_connection : public connection_observable_traits<O, F>::observer_type,
                                public connection_observable_traits<O, F>::subscription_type {
  public:
    using value_type = connection_observable_traits<O, F>::invoke_result_type;

    constexpr observable_connection(O &o, F &&f) : obs{o}, func{std::forward<F>(f)} {
      obs.add_subscription(*this);
    }

    ~observable_connection() { obs.remove_subscription(*this); }

    operator value_type() const { return func(obs.observe()); }
    value_type observe() const override { return func(obs.observe()); }

    void receive(typename connection_observable_traits<O, F>::value_type v) override {
      connection_observable_traits<O, F>::observer_type::notify(func(v));
    }

  private:
    O &obs;
    const F func;
  };

  template <observable O, connectable_observable_to_function<O> F>
  class observable_connection_r : public connection_observable_traits<O, F>::observer_type,
                                  public connection_observable_traits<O, F>::subscription_type {
  public:
    using value_type = connection_observable_traits<O, F>::invoke_result_type;

    constexpr observable_connection_r(O &&o, F &&f)
        : obs{std::forward<O>(o)}, func{std::forward<F>(f)} {
      obs.add_subscription(*this);
    }

    ~observable_connection_r() { obs.remove_subscription(*this); }

    operator value_type() const { return func(obs.observe()); }
    value_type observe() const override { return func(obs.observe()); }

    void receive(typename connection_observable_traits<O, F>::value_type v) override {
      connection_observable_traits<O, F>::observer_type::notify(func(v));
    }

  private:
    O obs;
    const F func;
  };

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
  class subscription_connection : public connection_subscription_traits<O, F>::subscription_type {
  public:
    constexpr subscription_connection(O &o, F &&f) : obs{o}, func{std::forward<F>(f)} {
      obs.add_subscription(*this);
    }

    ~subscription_connection() { obs.remove_subscription(*this); }

    void receive(typename connection_subscription_traits<O, F>::value_type v) override { func(v); }

  private:
    O &obs;
    const F func;
  };

  template <observable O, connectable_observable_to_void_function<O> F>
  class subscription_connection_r : public connection_subscription_traits<O, F>::subscription_type {
  public:
    constexpr subscription_connection_r(O &o, F &&f)
        : obs{std::forward<O>(o)}, func{std::forward<F>(f)} {
      obs.add_subscription(*this);
    }

    ~subscription_connection_r() { obs.remove_subscription(*this); }

    void receive(typename connection_subscription_traits<O, F>::value_type v) override { func(v); }

  private:
    O obs;
    const F func;
  };
} // namespace detail

// to observable
template <observable O, detail::connectable_observable_to_function<O> F>
[[nodiscard]] inline auto operator|(O &o, F &&f) {
  return detail::observable_connection<O, F>{o, std::forward<F>(f)};
}

template <observable O, detail::connectable_observable_to_function<O> F>
[[nodiscard]] inline auto operator|(O &&o, F &&f) {
  return detail::observable_connection_r<O, F>{std::forward<O>(o), std::forward<F>(f)};
}

// to subscription
template <observable O, detail::connectable_observable_to_void_function<O> F>
[[nodiscard]] inline auto operator|(O &o, F &&f) {
  return detail::subscription_connection<O, F>{o, std::forward<F>(f)};
}

template <observable O, detail::connectable_observable_to_void_function<O> F>
[[nodiscard]] inline auto operator|(O &&o, F &&f) {
  return detail::subscription_connection_r<O, F>{std::forward<O>(o), std::forward<F>(f)};
}

} // namespace riw
