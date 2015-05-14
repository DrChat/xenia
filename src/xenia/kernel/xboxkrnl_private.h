/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2013 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef XENIA_KERNEL_XBOXKRNL_PRIVATE_H_
#define XENIA_KERNEL_XBOXKRNL_PRIVATE_H_

#include "xenia/kernel/xboxkrnl_ordinals.h"

namespace xe {
namespace kernel {

class KernelState;

namespace xboxkrnl {
// Registration functions, one per file.
void RegisterAudioExports(xe::cpu::ExportResolver* export_resolver,
                          KernelState* state);
void RegisterAudioXmaExports(xe::cpu::ExportResolver* export_resolver,
                             KernelState* state);
void RegisterDebugExports(xe::cpu::ExportResolver* export_resolver,
                          KernelState* state);
void RegisterErrorExports(xe::cpu::ExportResolver* export_resolver,
                          KernelState* state);
void RegisterHalExports(xe::cpu::ExportResolver* export_resolver,
                        KernelState* state);
void RegisterIoExports(xe::cpu::ExportResolver* export_resolver,
                       KernelState* state);
void RegisterMemoryExports(xe::cpu::ExportResolver* export_resolver,
                           KernelState* state);
void RegisterMiscExports(xe::cpu::ExportResolver* export_resolver,
                         KernelState* state);
void RegisterModuleExports(xe::cpu::ExportResolver* export_resolver,
                           KernelState* state);
void RegisterObExports(xe::cpu::ExportResolver* export_resolver,
                       KernelState* state);
void RegisterRtlExports(xe::cpu::ExportResolver* export_resolver,
                        KernelState* state);
void RegisterStringExports(xe::cpu::ExportResolver* export_resolver,
                           KernelState* state);
void RegisterThreadingExports(xe::cpu::ExportResolver* export_resolver,
                              KernelState* state);
void RegisterUsbcamExports(xe::cpu::ExportResolver* export_resolver,
                           KernelState* state);
void RegisterVideoExports(xe::cpu::ExportResolver* export_resolver,
                          KernelState* state);
}  // namespace xboxkrnl

}  // namespace kernel
}  // namespace xe

#endif  // XENIA_KERNEL_XBOXKRNL_PRIVATE_H_
