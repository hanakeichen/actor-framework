// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/master/LICENSE.

#pragma once

#include <cstddef>

#include "caf/detail/core_export.hpp"
#include "caf/intrusive_ptr.hpp"
#include "caf/ref_counted.hpp"

namespace caf::flow {

/// Controls the flow of items from publishers to subscribers.
class CAF_CORE_EXPORT subscription : public ref_counted {
public:
  /// Internal impl of a `disposable`.
  class impl : public virtual ref_counted {
  public:
    ~impl() override;

    /// Causes the publisher to stop producing items for the subscriber. Any
    /// in-flight items may still get dispatched.
    virtual void cancel() = 0;

    /// Signals demand for `n` more items.
    virtual void request(size_t n) = 0;
  };

  explicit subscription(intrusive_ptr<impl> pimpl) noexcept
    : pimpl_(std::move(pimpl)) {
    // nop
  }

  subscription& operator=(std::nullptr_t) noexcept {
    pimpl_.reset();
    return *this;
  }

  subscription() noexcept = default;
  subscription(subscription&&) noexcept = default;
  subscription(const subscription&) noexcept = default;
  subscription& operator=(subscription&&) noexcept = default;
  subscription& operator=(const subscription&) noexcept = default;

  /// @copydoc impl::cancel
  void cancel() {
    if (pimpl_) {
      pimpl_->cancel();
      pimpl_ = nullptr;
    }
  }

  /// @copydoc impl::request
  /// @pre `valid()`
  void request(size_t n) {
    pimpl_->request(n);
  }

  bool valid() const noexcept {
    return pimpl_ != nullptr;
  }

  explicit operator bool() const noexcept {
    return valid();
  }

  bool operator!() const noexcept {
    return !valid();
  }

  impl* ptr() {
    return pimpl_.get();
  }

  const impl* ptr() const {
    return pimpl_.get();
  }

  intrusive_ptr<impl>&& as_intrusive_ptr() && noexcept {
    return std::move(pimpl_);
  }

  void swap(subscription& other) {
    pimpl_.swap(other.pimpl_);
  }

private:
  intrusive_ptr<impl> pimpl_;
};

} // namespace caf::flow