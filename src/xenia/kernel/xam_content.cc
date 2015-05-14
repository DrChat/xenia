/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2013 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include "xenia/base/logging.h"
#include "xenia/kernel/kernel_state.h"
#include "xenia/kernel/objects/xenumerator.h"
#include "xenia/kernel/util/shim_utils.h"
#include "xenia/kernel/xam_private.h"
#include "xenia/xbox.h"

namespace xe {
namespace kernel {

struct DeviceInfo {
  uint32_t device_id;
  uint32_t device_type;
  uint64_t total_bytes;
  uint64_t free_bytes;
  std::wstring name;
};
static const DeviceInfo dummy_device_info_ = {
    0xF00D0000, 1, 1024 * 1024 * 1024, 1024 * 1024 * 1024, L"Dummy HDD",
};

SHIM_CALL XamContentGetLicenseMask_shim(PPCContext* ppc_state,
                                        KernelState* state) {
  uint32_t mask_ptr = SHIM_GET_ARG_32(0);
  uint32_t overlapped_ptr = SHIM_GET_ARG_32(1);

  XELOGD("XamContentGetLicenseMask(%.8X, %.8X)", mask_ptr, overlapped_ptr);

  // Arcade games seem to call this and check the result mask for random bits.
  // If we fail, the games seem to use a hardcoded mask, which is likely trial.
  // To be clever, let's just try setting all the bits.
  SHIM_SET_MEM_32(mask_ptr, 0xFFFFFFFF);

  if (overlapped_ptr) {
    state->CompleteOverlappedImmediate(overlapped_ptr, X_ERROR_SUCCESS);
    SHIM_SET_RETURN_32(X_ERROR_IO_PENDING);
  } else {
    SHIM_SET_RETURN_32(X_ERROR_SUCCESS);
  }
}

SHIM_CALL XamShowDeviceSelectorUI_shim(PPCContext* ppc_state,
                                       KernelState* state) {
  uint32_t user_index = SHIM_GET_ARG_32(0);
  uint32_t content_type = SHIM_GET_ARG_32(1);
  uint32_t content_flags = SHIM_GET_ARG_32(2);
  uint64_t total_requested = SHIM_GET_ARG_64(3);
  uint32_t device_id_ptr = SHIM_GET_ARG_32(4);
  uint32_t overlapped_ptr = SHIM_GET_ARG_32(5);

  XELOGD("XamShowDeviceSelectorUI(%d, %.8X, %.8X, %.8X, %.8X, %.8X)",
         user_index, content_type, content_flags, total_requested,
         device_id_ptr, overlapped_ptr);

  switch (content_type) {
    case 1:  // save game
      SHIM_SET_MEM_32(device_id_ptr, dummy_device_info_.device_id | 0x0001);
      break;
    case 2:  // marketplace
      SHIM_SET_MEM_32(device_id_ptr, dummy_device_info_.device_id | 0x0002);
      break;
    case 3:  // title/publisher update?
      SHIM_SET_MEM_32(device_id_ptr, dummy_device_info_.device_id | 0x0003);
      break;
  }

  X_RESULT result = X_ERROR_SUCCESS;
  if (overlapped_ptr) {
    state->CompleteOverlappedImmediate(overlapped_ptr, result);
    SHIM_SET_RETURN_32(X_ERROR_IO_PENDING);
  } else {
    SHIM_SET_RETURN_32(result);
  }
}

SHIM_CALL XamContentGetDeviceName_shim(PPCContext* ppc_state,
                                       KernelState* state) {
  uint32_t device_id = SHIM_GET_ARG_32(0);
  uint32_t name_ptr = SHIM_GET_ARG_32(1);
  uint32_t name_capacity = SHIM_GET_ARG_32(2);

  XELOGD("XamContentGetDeviceName(%.8X, %.8X, %d)", device_id, name_ptr,
         name_capacity);

  if ((device_id & 0xFFFF0000) != dummy_device_info_.device_id) {
    SHIM_SET_RETURN_32(X_ERROR_DEVICE_NOT_CONNECTED);
    return;
  }

  if (name_capacity < dummy_device_info_.name.size() + 1) {
    SHIM_SET_RETURN_32(X_ERROR_INSUFFICIENT_BUFFER);
    return;
  }

  xe::store_and_swap<std::wstring>(SHIM_MEM_ADDR(name_ptr),
                                   dummy_device_info_.name);

  SHIM_SET_RETURN_32(X_ERROR_SUCCESS);
}

SHIM_CALL XamContentGetDeviceState_shim(PPCContext* ppc_state,
                                        KernelState* state) {
  uint32_t device_id = SHIM_GET_ARG_32(0);
  uint32_t overlapped_ptr = SHIM_GET_ARG_32(1);

  XELOGD("XamContentGetDeviceState(%.8X, %.8X)", device_id, overlapped_ptr);

  if ((device_id & 0xFFFF0000) != dummy_device_info_.device_id) {
    if (overlapped_ptr) {
      state->CompleteOverlappedImmediate(overlapped_ptr,
                                         X_ERROR_FUNCTION_FAILED);
      XOverlappedSetExtendedError(SHIM_MEM_BASE + overlapped_ptr,
                                  X_ERROR_DEVICE_NOT_CONNECTED);
      SHIM_SET_RETURN_32(X_ERROR_IO_PENDING);
    } else {
      SHIM_SET_RETURN_32(X_ERROR_DEVICE_NOT_CONNECTED);
    }
    return;
  }

  if (overlapped_ptr) {
    state->CompleteOverlappedImmediate(overlapped_ptr, X_ERROR_SUCCESS);
    SHIM_SET_RETURN_32(X_ERROR_IO_PENDING);
  } else {
    SHIM_SET_RETURN_32(X_ERROR_SUCCESS);
  }
}

SHIM_CALL XamContentGetDeviceData_shim(PPCContext* ppc_state,
                                       KernelState* state) {
  uint32_t device_id = SHIM_GET_ARG_32(0);
  uint32_t device_data_ptr = SHIM_GET_ARG_32(1);

  XELOGD("XamContentGetDeviceData(%.8X, %.8X)", device_id, device_data_ptr);

  if ((device_id & 0xFFFF0000) != dummy_device_info_.device_id) {
    // TODO(benvanik): memset 0 the data?
    SHIM_SET_RETURN_32(X_ERROR_DEVICE_NOT_CONNECTED);
    return;
  }

  const auto& device_info = dummy_device_info_;
  SHIM_SET_MEM_32(device_data_ptr + 0, device_info.device_id);
  SHIM_SET_MEM_32(device_data_ptr + 4, device_info.device_type);
  SHIM_SET_MEM_64(device_data_ptr + 8, device_info.total_bytes);
  SHIM_SET_MEM_64(device_data_ptr + 16, device_info.free_bytes);
  xe::store_and_swap<std::wstring>(SHIM_MEM_ADDR(device_data_ptr + 24),
                                   device_info.name);

  SHIM_SET_RETURN_32(X_ERROR_SUCCESS);
}

SHIM_CALL XamContentResolve_shim(PPCContext* ppc_state, KernelState* state) {
  uint32_t user_index = SHIM_GET_ARG_32(0);
  uint32_t content_data_ptr = SHIM_GET_ARG_32(1);
  uint32_t buffer_ptr = SHIM_GET_ARG_32(2);
  uint32_t buffer_size = SHIM_GET_ARG_32(3);
  uint32_t unk1 = SHIM_GET_ARG_32(4);  // 1
  uint32_t unk2 = SHIM_GET_ARG_32(5);  // 0
  uint32_t unk3 = SHIM_GET_ARG_32(6);  // 0

  auto content_data = XCONTENT_DATA(SHIM_MEM_ADDR(content_data_ptr));

  XELOGD("XamContentResolve(%d, %.8X, %.8X, %d, %.8X, %.8X, %.8X)", user_index,
         content_data_ptr, buffer_ptr, buffer_size, unk1, unk2, unk3);

  // Result of buffer_ptr is sent to RtlInitAnsiString.
  // buffer_size is usually 260 (max path).
  // Games expect zero if resolve was successful.
  assert_always();
  XELOGW("XamContentResolve unimplemented!");

  SHIM_SET_RETURN_32(X_ERROR_NOT_FOUND);
}

// http://gameservice.googlecode.com/svn-history/r14/trunk/ContentManager.cpp
SHIM_CALL XamContentCreateEnumerator_shim(PPCContext* ppc_state,
                                          KernelState* state) {
  uint32_t user_index = SHIM_GET_ARG_32(0);
  uint32_t device_id = SHIM_GET_ARG_32(1);
  uint32_t content_type = SHIM_GET_ARG_32(2);
  uint32_t content_flags = SHIM_GET_ARG_32(3);
  uint32_t item_count = SHIM_GET_ARG_32(4);
  uint32_t buffer_size_ptr = SHIM_GET_ARG_32(5);
  uint32_t handle_ptr = SHIM_GET_ARG_32(6);

  XELOGD("XamContentCreateEnumerator(%d, %.8X, %.8X, %.8X, %.8X, %.8X, %.8X)",
         user_index, device_id, content_type, content_flags, item_count,
         buffer_size_ptr, handle_ptr);

  if (device_id && (device_id & 0xFFFF0000) != dummy_device_info_.device_id) {
    // TODO(benvanik): memset 0 the data?
    SHIM_SET_RETURN_32(X_E_INVALIDARG);
    return;
  }
  if (!device_id) {
    // 0 == whatever
    device_id = dummy_device_info_.device_id;
  }

  if (buffer_size_ptr) {
    SHIM_SET_MEM_32(buffer_size_ptr, item_count * XCONTENT_DATA::kSize);
  }

  auto e = new XStaticEnumerator(state, item_count, XCONTENT_DATA::kSize);
  e->Initialize();

  // Get all content data.
  auto content_datas =
      state->content_manager()->ListContent(device_id, content_type);
  for (auto& content_data : content_datas) {
    auto ptr = e->AppendItem();
    if (!ptr) {
      // Too many items.
      break;
    }
    content_data.Write(ptr);
  }

  SHIM_SET_MEM_32(handle_ptr, e->handle());

  SHIM_SET_RETURN_32(X_ERROR_SUCCESS);
}

void XamContentCreateCore(PPCContext* ppc_state, KernelState* state,
                          uint32_t user_index, std::string root_name,
                          XCONTENT_DATA content_data, uint32_t flags,
                          uint32_t disposition_ptr, uint32_t license_mask_ptr,
                          uint32_t cache_size, uint64_t content_size,
                          uint32_t overlapped_ptr) {
  assert_zero(license_mask_ptr);

  X_RESULT result = X_ERROR_INVALID_PARAMETER;

  auto content_manager = state->content_manager();
  bool create = false;
  bool open = false;
  switch (flags & 0xF) {
    case 1:  // CREATE_NEW
             // Fail if exists.
      if (content_manager->ContentExists(content_data)) {
        result = X_ERROR_ALREADY_EXISTS;
      } else {
        create = true;
      }
      break;
    case 2:  // CREATE_ALWAYS
             // Overwrite existing, if any.
      if (content_manager->ContentExists(content_data)) {
        content_manager->DeleteContent(content_data);
        create = true;
      } else {
        create = true;
      }
      break;
    case 3:  // OPEN_EXISTING
             // Open only if exists.
      if (!content_manager->ContentExists(content_data)) {
        result = X_ERROR_PATH_NOT_FOUND;
      } else {
        open = true;
      }
      break;
    case 4:  // OPEN_ALWAYS
             // Create if needed.
      if (!content_manager->ContentExists(content_data)) {
        create = true;
      } else {
        open = true;
      }
      break;
    case 5:  // TRUNCATE_EXISTING
             // Fail if doesn't exist, if does exist delete and recreate.
      if (!content_manager->ContentExists(content_data)) {
        result = X_ERROR_PATH_NOT_FOUND;
      } else {
        content_manager->DeleteContent(content_data);
        create = true;
      }
      break;
    default:
      assert_unhandled_case(flags & 0xF);
      break;
  }

  if (disposition_ptr) {
    if (overlapped_ptr) {
      // If async always set to zero, but don't set to a real value.
      SHIM_SET_MEM_32(disposition_ptr, 0);
    } else {
      SHIM_SET_MEM_32(disposition_ptr, create ? 1 : 2);
    }
  }

  if (create) {
    result = content_manager->CreateContent(root_name, content_data);
  } else if (open) {
    result = content_manager->OpenContent(root_name, content_data);
  }

  if (overlapped_ptr) {
    state->CompleteOverlappedImmediate(overlapped_ptr, result);
    SHIM_SET_RETURN_32(X_ERROR_IO_PENDING);
  } else {
    SHIM_SET_RETURN_32(result);
  }
}

SHIM_CALL XamContentCreate_shim(PPCContext* ppc_state, KernelState* state) {
  uint32_t user_index = SHIM_GET_ARG_32(0);
  uint32_t root_name_ptr = SHIM_GET_ARG_32(1);
  uint32_t content_data_ptr = SHIM_GET_ARG_32(2);
  uint32_t flags = SHIM_GET_ARG_32(3);
  uint32_t disposition_ptr = SHIM_GET_ARG_32(4);
  uint32_t license_mask_ptr = SHIM_GET_ARG_32(5);
  uint32_t overlapped_ptr = SHIM_GET_ARG_32(6);

  auto root_name = xe::load_and_swap<std::string>(SHIM_MEM_ADDR(root_name_ptr));
  auto content_data = XCONTENT_DATA(SHIM_MEM_ADDR(content_data_ptr));

  XELOGD("XamContentCreate(%d, %.8X(%s), %.8X, %.8X, %.8X, %.8X, %.8X)",
         user_index, root_name_ptr, root_name.c_str(), content_data_ptr, flags,
         disposition_ptr, license_mask_ptr, overlapped_ptr);

  XamContentCreateCore(ppc_state, state, user_index, root_name, content_data,
                       flags, disposition_ptr, license_mask_ptr, 0, 0,
                       overlapped_ptr);
}

SHIM_CALL XamContentCreateEx_shim(PPCContext* ppc_state, KernelState* state) {
  uint32_t user_index = SHIM_GET_ARG_32(0);
  uint32_t root_name_ptr = SHIM_GET_ARG_32(1);
  uint32_t content_data_ptr = SHIM_GET_ARG_32(2);
  uint32_t flags = SHIM_GET_ARG_32(3);
  uint32_t disposition_ptr = SHIM_GET_ARG_32(4);
  uint32_t license_mask_ptr = SHIM_GET_ARG_32(5);
  uint32_t cache_size = SHIM_GET_ARG_32(6);
  uint64_t content_size = SHIM_GET_ARG_64(7);
  uint32_t sp = (uint32_t)ppc_state->r[1];
  uint32_t overlapped_ptr = SHIM_MEM_32(sp + 0x54);

  auto root_name = xe::load_and_swap<std::string>(SHIM_MEM_ADDR(root_name_ptr));
  auto content_data = XCONTENT_DATA(SHIM_MEM_ADDR(content_data_ptr));

  XELOGD(
      "XamContentCreateEx(%d, %.8X(%s), %.8X, %.8X, %.8X, %.8X, %.8X, %.8llX, "
      "%.8X)",
      user_index, root_name_ptr, root_name.c_str(), content_data_ptr, flags,
      disposition_ptr, license_mask_ptr, cache_size, content_size,
      overlapped_ptr);

  XamContentCreateCore(ppc_state, state, user_index, root_name, content_data,
                       flags, disposition_ptr, license_mask_ptr, cache_size,
                       content_size, overlapped_ptr);
}

SHIM_CALL XamContentFlush_shim(PPCContext* ppc_state, KernelState* state) {
  uint32_t root_name_ptr = SHIM_GET_ARG_32(0);
  uint32_t overlapped_ptr = SHIM_GET_ARG_32(1);

  auto root_name = xe::load_and_swap<std::string>(SHIM_MEM_ADDR(root_name_ptr));

  XELOGD("XamContentFlush(%.8X(%s), %.8X)", root_name_ptr, root_name.c_str(),
         overlapped_ptr);

  X_RESULT result = X_ERROR_SUCCESS;
  if (overlapped_ptr) {
    state->CompleteOverlappedImmediate(overlapped_ptr, result);
    SHIM_SET_RETURN_32(X_ERROR_IO_PENDING);
  } else {
    SHIM_SET_RETURN_32(result);
  }
}

SHIM_CALL XamContentClose_shim(PPCContext* ppc_state, KernelState* state) {
  uint32_t root_name_ptr = SHIM_GET_ARG_32(0);
  uint32_t overlapped_ptr = SHIM_GET_ARG_32(1);

  auto root_name = xe::load_and_swap<std::string>(SHIM_MEM_ADDR(root_name_ptr));

  XELOGD("XamContentClose(%.8X(%s), %.8X)", root_name_ptr, root_name.c_str(),
         overlapped_ptr);

  // Closes a previously opened root from XamContentCreate*.
  auto result = state->content_manager()->CloseContent(root_name);

  if (overlapped_ptr) {
    state->CompleteOverlappedImmediate(overlapped_ptr, result);
    SHIM_SET_RETURN_32(X_ERROR_IO_PENDING);
  } else {
    SHIM_SET_RETURN_32(result);
  }
}

SHIM_CALL XamContentGetCreator_shim(PPCContext* ppc_state, KernelState* state) {
  uint32_t user_index = SHIM_GET_ARG_32(0);
  uint32_t content_data_ptr = SHIM_GET_ARG_32(1);
  uint32_t is_creator_ptr = SHIM_GET_ARG_32(2);
  uint32_t creator_xuid_ptr = SHIM_GET_ARG_32(3);
  uint32_t overlapped_ptr = SHIM_GET_ARG_32(4);

  auto content_data = XCONTENT_DATA(SHIM_MEM_ADDR(content_data_ptr));

  XELOGD("XamContentGetCreator(%d, %.8X, %.8X, %.8X, %.8X)", user_index,
         content_data_ptr, is_creator_ptr, creator_xuid_ptr, overlapped_ptr);

  auto result = X_ERROR_SUCCESS;

  if (content_data.content_type == 1) {
    // User always creates saves.
    SHIM_SET_MEM_32(is_creator_ptr, 1);
    if (creator_xuid_ptr) {
      SHIM_SET_MEM_64(creator_xuid_ptr, state->user_profile()->xuid());
    }
  } else {
    SHIM_SET_MEM_32(is_creator_ptr, 0);
    if (creator_xuid_ptr) {
      SHIM_SET_MEM_64(creator_xuid_ptr, 0);
    }
  }

  if (overlapped_ptr) {
    state->CompleteOverlappedImmediate(overlapped_ptr, result);
    SHIM_SET_RETURN_32(X_ERROR_IO_PENDING);
  } else {
    SHIM_SET_RETURN_32(result);
  }
}

SHIM_CALL XamContentGetThumbnail_shim(PPCContext* ppc_state,
                                      KernelState* state) {
  uint32_t user_index = SHIM_GET_ARG_32(0);
  uint32_t content_data_ptr = SHIM_GET_ARG_32(1);
  uint32_t buffer_ptr = SHIM_GET_ARG_32(2);
  uint32_t buffer_size_ptr = SHIM_GET_ARG_32(3);
  uint32_t overlapped_ptr = SHIM_GET_ARG_32(4);

  assert_not_zero(buffer_size_ptr);
  uint32_t buffer_size = SHIM_MEM_32(buffer_size_ptr);
  auto content_data = XCONTENT_DATA(SHIM_MEM_ADDR(content_data_ptr));

  XELOGD("XamContentGetThumbnail(%d, %.8X, %.8X, %.8X(%d), %.8X)", user_index,
         content_data_ptr, buffer_ptr, buffer_size_ptr, buffer_size,
         overlapped_ptr);

  // Get thumbnail (if it exists).
  std::vector<uint8_t> buffer;
  auto result =
      state->content_manager()->GetContentThumbnail(content_data, &buffer);

  SHIM_SET_MEM_32(buffer_size_ptr, uint32_t(buffer.size()));

  if (XSUCCEEDED(result)) {
    // Write data, if we were given a pointer.
    // This may have just been a size query.
    if (buffer_ptr) {
      if (buffer_size < buffer.size()) {
        // Dest buffer too small.
        result = X_ERROR_INSUFFICIENT_BUFFER;
      } else {
        // Copy data.
        std::memcpy(SHIM_MEM_ADDR(buffer_ptr), buffer.data(), buffer.size());
      }
    }
  }

  if (overlapped_ptr) {
    state->CompleteOverlappedImmediate(overlapped_ptr, result);
    SHIM_SET_RETURN_32(X_ERROR_IO_PENDING);
  } else {
    SHIM_SET_RETURN_32(result);
  }
}

SHIM_CALL XamContentSetThumbnail_shim(PPCContext* ppc_state,
                                      KernelState* state) {
  uint32_t user_index = SHIM_GET_ARG_32(0);
  uint32_t content_data_ptr = SHIM_GET_ARG_32(1);
  uint32_t buffer_ptr = SHIM_GET_ARG_32(2);
  uint32_t buffer_size = SHIM_GET_ARG_32(3);
  uint32_t overlapped_ptr = SHIM_GET_ARG_32(4);

  auto content_data = XCONTENT_DATA(SHIM_MEM_ADDR(content_data_ptr));

  XELOGD("XamContentSetThumbnail(%d, %.8X, %.8X, %d, %.8X)", user_index,
         content_data_ptr, buffer_ptr, buffer_size, overlapped_ptr);

  // Buffer is PNG data.
  auto buffer = std::vector<uint8_t>(SHIM_MEM_ADDR(buffer_ptr),
                                     SHIM_MEM_ADDR(buffer_ptr) + buffer_size);
  auto result = state->content_manager()->SetContentThumbnail(
      content_data, std::move(buffer));

  if (overlapped_ptr) {
    state->CompleteOverlappedImmediate(overlapped_ptr, result);
    SHIM_SET_RETURN_32(X_ERROR_IO_PENDING);
  } else {
    SHIM_SET_RETURN_32(result);
  }
}

SHIM_CALL XamContentDelete_shim(PPCContext* ppc_state, KernelState* state) {
  uint32_t user_index = SHIM_GET_ARG_32(0);
  uint32_t content_data_ptr = SHIM_GET_ARG_32(1);
  uint32_t overlapped_ptr = SHIM_GET_ARG_32(2);

  auto content_data = XCONTENT_DATA(SHIM_MEM_ADDR(content_data_ptr));

  XELOGD("XamContentDelete(%d, %.8X, %.8X)", user_index, content_data_ptr,
         overlapped_ptr);

  auto result = state->content_manager()->DeleteContent(content_data);

  if (overlapped_ptr) {
    state->CompleteOverlappedImmediate(overlapped_ptr, result);
    SHIM_SET_RETURN_32(X_ERROR_IO_PENDING);
  } else {
    SHIM_SET_RETURN_32(result);
  }
}

}  // namespace kernel
}  // namespace xe

void xe::kernel::xam::RegisterContentExports(
    xe::cpu::ExportResolver* export_resolver, KernelState* state) {
  SHIM_SET_MAPPING("xam.xex", XamContentGetLicenseMask, state);
  SHIM_SET_MAPPING("xam.xex", XamShowDeviceSelectorUI, state);
  SHIM_SET_MAPPING("xam.xex", XamContentGetDeviceName, state);
  SHIM_SET_MAPPING("xam.xex", XamContentGetDeviceState, state);
  SHIM_SET_MAPPING("xam.xex", XamContentGetDeviceData, state);
  SHIM_SET_MAPPING("xam.xex", XamContentResolve, state);
  SHIM_SET_MAPPING("xam.xex", XamContentCreateEnumerator, state);
  SHIM_SET_MAPPING("xam.xex", XamContentCreate, state);
  SHIM_SET_MAPPING("xam.xex", XamContentCreateEx, state);
  SHIM_SET_MAPPING("xam.xex", XamContentFlush, state);
  SHIM_SET_MAPPING("xam.xex", XamContentClose, state);
  SHIM_SET_MAPPING("xam.xex", XamContentGetCreator, state);
  SHIM_SET_MAPPING("xam.xex", XamContentGetThumbnail, state);
  SHIM_SET_MAPPING("xam.xex", XamContentSetThumbnail, state);
  SHIM_SET_MAPPING("xam.xex", XamContentDelete, state);
}
