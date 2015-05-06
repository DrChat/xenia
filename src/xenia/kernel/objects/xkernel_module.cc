/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2013 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/kernel/objects/xkernel_module.h"

#include "xenia/base/logging.h"
#include "xenia/cpu/cpu.h"
#include "xenia/emulator.h"
#include "xenia/kernel/objects/xthread.h"

namespace xe {
namespace kernel {

XKernelModule::XKernelModule(KernelState* kernel_state, const char* path)
    : XModule(kernel_state, path) {
  emulator_ = kernel_state->emulator();
  memory_ = emulator_->memory();
  export_resolver_ = kernel_state->emulator()->export_resolver();

  OnLoad();
}

XKernelModule::~XKernelModule() {}

uint32_t XKernelModule::GetProcAddressByOrdinal(uint16_t ordinal) {
  // TODO(benvanik): check export tables.
  XELOGE("GetProcAddressByOrdinal not implemented");
  return 0;
}

uint32_t XKernelModule::GetProcAddressByName(const char* name) {
  XELOGE("GetProcAddressByName not implemented");
  return 0;
}

}  // namespace kernel
}  // namespace xe
