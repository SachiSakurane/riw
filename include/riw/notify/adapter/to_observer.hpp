#pragma once

#include <optional>

#include <riw/notify/concepts/observable.hpp>
#include <riw/notify/observer.hpp>

namespace riw {
namespace detail {
  class to_observer_unique_ptr_tag {};

  template <observable O>
  class observer_wrapper_r final : public riw::observer<typename O::value_type> {
  public:
    using value_type = typename O::value_type;
    using subscription_type = riw::subscription<value_type>;

    class observer_wrapper_r_impl {
    public:
      observer_wrapper_r_impl(O &o) : obs{o} {}

      void subscribe(std::weak_ptr<subscription_type> s) { obs->get().subscribe(s); }
      value_type observe() const { return obs->get().observe(); }
      void notify(value_type v) { return obs->get().notify(v); }
      std::size_t subscriptions_count() const { return obs->get().subscriptions_count(); }
      void fetch() { return obs->get().fetch(); }

    private:
      std::optional<std::reference_wrapper<O>> obs;
    };

    observer_wrapper_r(O &o) : impl{std::make_shared<observer_wrapper_r_impl>(o)} {}

    void subscribe(std::weak_ptr<subscription_type> s) override { impl->subscribe(s); }
    value_type observe() const override { return impl->observe(); }
    operator value_type() const { return impl->observe(); }
    void notify(value_type v) override { impl->notify(v); }
    std::size_t subscriptions_count() const override { return impl->subscriptions_count(); }
    void fetch() override { impl->fetch(); }

  private:
    std::shared_ptr<observer_wrapper_r_impl> impl;
  };

  template <observable O>
  class observer_wrapper final : public riw::observer<typename O::value_type> {
  public:
    using value_type = typename O::value_type;
    using subscription_type = riw::subscription<value_type>;

    class observer_wrapper_impl {
    public:
      observer_wrapper_impl(O &&o) : obs{std::forward<O>(o)} {}

      void subscribe(std::weak_ptr<subscription_type> s) { obs->subscribe(s); }
      value_type observe() const { return obs->observe(); }
      void notify(value_type v) { return obs->notify(v); }
      std::size_t subscriptions_count() const { return obs->subscriptions_count(); }
      void fetch() { return obs->fetch(); }

    private:
      std::optional<O> obs;
    };

    observer_wrapper(O &&o) : impl{std::make_shared<observer_wrapper_impl>(std::forward<O>(o))} {}

    void subscribe(std::weak_ptr<subscription_type> s) override { impl->subscribe(s); }
    value_type observe() const override { return impl->observe(); }
    operator value_type() const { return impl->observe(); }
    void notify(value_type v) override { impl->notify(v); }
    std::size_t subscriptions_count() const override { return impl->subscriptions_count(); }
    void fetch() override { impl->fetch(); }

  private:
    std::shared_ptr<observer_wrapper_impl> impl;
  };
} // namespace detail

constexpr static auto to_observer_unique_ptr = detail::to_observer_unique_ptr_tag{};

template <observable O>
[[nodiscard]] inline auto operator|(O &o, detail::to_observer_unique_ptr_tag) {
  return std::make_unique<detail::observer_wrapper_r<O>>(o);
}

template <observable O>
[[nodiscard]] inline auto operator|(O &&o, detail::to_observer_unique_ptr_tag) {
  return std::make_unique<detail::observer_wrapper<O>>(std::forward<O>(o));
}
} // namespace riw
