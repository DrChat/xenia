/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2013 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef XENIA_KERNEL_XBOXKRNL_OBJECT_TABLE_H_
#define XENIA_KERNEL_XBOXKRNL_OBJECT_TABLE_H_

#include <mutex>
#include <string>
#include <unordered_map>

#include "xenia/xbox.h"

namespace xe {
namespace kernel {

#undef GetObject // windows...

class XObject;

class ObjectTable {
 public:
  ObjectTable();
  ~ObjectTable();

  X_STATUS AddHandle(XObject* object, X_HANDLE* out_handle,
                     bool removable = true);
  X_STATUS RemoveHandle(X_HANDLE handle, bool force = false);
  X_STATUS GetObject(X_HANDLE handle, XObject** out_object,
                     bool already_locked = false);

  X_STATUS AddNameMapping(const std::string& name, X_HANDLE handle);
  void RemoveNameMapping(const std::string& name);
  X_STATUS GetObjectByName(const std::string& name, X_HANDLE* out_handle);

 private:
  X_HANDLE TranslateHandle(X_HANDLE handle);
  X_STATUS FindFreeSlot(uint32_t* out_slot);

  typedef struct {
    XObject* object;
    bool removable; // Can this be removed by the guest?
  } ObjectTableEntry;

  std::mutex table_mutex_;
  uint32_t table_capacity_;
  ObjectTableEntry* table_;
  uint32_t last_free_entry_;
  std::unordered_map<std::string, X_HANDLE> name_table_;
};

}  // namespace kernel
}  // namespace xe

#endif  // XENIA_KERNEL_XBOXKRNL_OBJECT_TABLE_H_
