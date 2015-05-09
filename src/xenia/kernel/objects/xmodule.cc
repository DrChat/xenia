/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2013 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/kernel/objects/xmodule.h"

namespace xe {
namespace kernel {

XModule::XModule(KernelState* kernel_state, const std::string& path, bool kmod)
    : XObject(kernel_state, kTypeModule, !kmod), path_(path) {
  auto last_slash = path.find_last_of('/');
  if (last_slash == path.npos) {
    last_slash = path.find_last_of('\\');
  }
  if (last_slash == path.npos) {
    name_ = path_;
  } else {
    name_ = path_.substr(last_slash + 1);
  }
  auto dot = name_.find_last_of('.');
  if (dot != name_.npos) {
    name_ = name_.substr(0, dot);
  }
}

XModule::~XModule() { kernel_state_->UnregisterModule(this); }

void XModule::OnLoad() { kernel_state_->RegisterModule(this); }

X_STATUS XModule::GetSection(const char* name, uint32_t* out_section_data,
                             uint32_t* out_section_size) {
  return X_STATUS_UNSUCCESSFUL;
}

}  // namespace kernel
}  // namespace xe
