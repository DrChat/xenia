/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2013 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/cpu/backend/backend.h"

namespace xe {
namespace cpu {
namespace backend {

Backend::Backend(Processor* processor) : processor_(processor) {
  memset(&machine_info_, 0, sizeof(machine_info_));
}

Backend::~Backend() = default;

bool Backend::Initialize() { return true; }

void* Backend::AllocThreadData() { return nullptr; }

void Backend::FreeThreadData(void* thread_data) {}

}  // namespace backend
}  // namespace cpu
}  // namespace xe
