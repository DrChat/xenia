/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2013 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/cpu/function.h"

#include "xenia/base/logging.h"
#include "xenia/cpu/symbol_info.h"
#include "xenia/cpu/thread_state.h"

namespace xe {
namespace cpu {

using xe::debug::Breakpoint;

Function::Function(FunctionInfo* symbol_info)
    : address_(symbol_info->address()), symbol_info_(symbol_info) {}

Function::~Function() = default;

bool Function::AddBreakpoint(Breakpoint* breakpoint) {
  std::lock_guard<std::mutex> guard(lock_);
  bool found = false;
  for (auto other : breakpoints_) {
    if (other == breakpoint) {
      found = true;
      break;
    }
  }
  if (found) {
    return true;
  } else {
    breakpoints_.push_back(breakpoint);
    return AddBreakpointImpl(breakpoint);
  }
}

bool Function::RemoveBreakpoint(Breakpoint* breakpoint) {
  std::lock_guard<std::mutex> guard(lock_);
  for (auto it = breakpoints_.begin(); it != breakpoints_.end(); ++it) {
    if (*it == breakpoint) {
      if (!RemoveBreakpointImpl(breakpoint)) {
        return false;
      }
      breakpoints_.erase(it);
    }
  }
  return false;
}

Breakpoint* Function::FindBreakpoint(uint32_t address) {
  std::lock_guard<std::mutex> guard(lock_);
  Breakpoint* result = nullptr;
  for (auto breakpoint : breakpoints_) {
    if (breakpoint->address() == address) {
      result = breakpoint;
      break;
    }
  }
  return result;
}

bool Function::Call(ThreadState* thread_state, uint32_t return_address) {
  // SCOPE_profile_cpu_f("cpu");

  ThreadState* original_thread_state = ThreadState::Get();
  if (original_thread_state != thread_state) {
    ThreadState::Bind(thread_state);
  }

  bool result = true;

  if (symbol_info_->behavior() == FunctionInfo::BEHAVIOR_EXTERN) {
    auto handler = symbol_info_->extern_handler();

    if (handler) {
      handler(thread_state->context(), symbol_info_->extern_arg0(),
              symbol_info_->extern_arg1());
    } else {
      XELOGW("undefined extern call to %.8llX %s", symbol_info_->address(),
             symbol_info_->name().c_str());
      result = false;
    }
  } else {
    CallImpl(thread_state, return_address);
  }

  if (original_thread_state != thread_state) {
    ThreadState::Bind(original_thread_state);
  }
  return result;
}

}  // namespace cpu
}  // namespace xe
