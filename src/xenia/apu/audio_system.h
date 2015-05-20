/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2013 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#ifndef XENIA_APU_AUDIO_SYSTEM_H_
#define XENIA_APU_AUDIO_SYSTEM_H_

#include <atomic>
#include <mutex>
#include <queue>
#include <thread>

#include "xenia/emulator.h"
#include "xenia/xbox.h"

namespace xe {

namespace kernel { class XHostThread; }

namespace apu {

class AudioDriver;

// This is stored in guest space in big-endian order.
// We load and swap the whole thing to splat here so that we can
// use bitfields.
struct XMAContextData {
  static const uint32_t kSize = 64;
  static const uint32_t kBytesPerBlock = 2048;
  static const uint32_t kSamplesPerFrame = 512;
  static const uint32_t kSamplesPerSubframe = 128;

  // DWORD 0
  uint32_t input_buffer_0_block_count : 12;  // XMASetInputBuffer0, number of
                                             // 2KB blocks.
  uint32_t loop_count : 8;                   // +12bit, XMASetLoopData
  uint32_t input_buffer_0_valid : 1;         // +20bit, XMAIsInputBuffer0Valid
  uint32_t input_buffer_1_valid : 1;         // +21bit, XMAIsInputBuffer1Valid
  uint32_t output_buffer_block_count : 5;    // +22bit
  uint32_t
    output_buffer_write_offset : 5;  // +27bit, XMAGetOutputBufferWriteOffset

                                     // DWORD 1
  uint32_t input_buffer_1_block_count : 12;  // XMASetInputBuffer1, number of
                                             // 2KB blocks.
  uint32_t loop_subframe_end : 2;            // +12bit, XMASetLoopData
  uint32_t unk_dword_1_a : 3;                // ?
  uint32_t loop_subframe_skip : 3;           // +17bit, XMASetLoopData
  uint32_t subframe_decode_count : 4;        // +20bit
  uint32_t unk_dword_1_b : 3;                // ?
  uint32_t sample_rate : 2;                  // +27bit
  uint32_t is_stereo : 1;                    // +29bit
  uint32_t unk_dword_1_c : 1;                // ?
  uint32_t output_buffer_valid : 1;          // +31bit, XMAIsOutputBufferValid

                                             // DWORD 2
  uint32_t input_buffer_read_offset : 30;  // XMAGetInputBufferReadOffset
  uint32_t unk_dword_2 : 2;                // ?

                                           // DWORD 3
  uint32_t loop_start : 26;  // XMASetLoopData
  uint32_t unk_dword_3 : 6;  // ?

                             // DWORD 4
  uint32_t loop_end : 26;        // XMASetLoopData
  uint32_t packet_metadata : 5;  // XMAGetPacketMetadata
  uint32_t current_buffer : 1;   // ?

                                // DWORD 5
  uint32_t input_buffer_0_ptr;  // physical address
                                // DWORD 6
  uint32_t input_buffer_1_ptr;  // physical address
                                // DWORD 7
  uint32_t output_buffer_ptr;   // physical address
                                // DWORD 8
  uint32_t unk_dword_8;         // Some kind of pointer like output_buffer_ptr

                                // DWORD 9
  uint32_t
    output_buffer_read_offset : 5;  // +0bit, XMAGetOutputBufferReadOffset
  uint32_t unk_dword_9 : 27;

  XMAContextData(const void* ptr) {
    xe::copy_and_swap_32_aligned(reinterpret_cast<uint32_t*>(this),
      reinterpret_cast<const uint32_t*>(ptr),
      sizeof(XMAContextData) / 4);
  }

  void Store(void* ptr) {
    xe::copy_and_swap_32_aligned(reinterpret_cast<uint32_t*>(ptr),
      reinterpret_cast<const uint32_t*>(this),
      sizeof(XMAContextData) / 4);
  }
};
static_assert(sizeof(XMAContextData) == 4 * 10, "Must be packed");

class AudioSystem {
 public:
  virtual ~AudioSystem();

  Emulator* emulator() const { return emulator_; }
  Memory* memory() const { return memory_; }
  cpu::Processor* processor() const { return processor_; }

  virtual X_STATUS Setup();
  virtual void Shutdown();

  uint32_t xma_context_array_ptr() const {
    return registers_.xma_context_array_ptr;
  }
  uint32_t AllocateXmaContext();
  void ReleaseXmaContext(uint32_t guest_ptr);

  X_STATUS RegisterClient(uint32_t callback, uint32_t callback_arg,
                          size_t* out_index);
  void UnregisterClient(size_t index);
  void SubmitFrame(size_t index, uint32_t samples_ptr);

  virtual X_STATUS CreateDriver(size_t index, HANDLE wait_handle,
                                AudioDriver** out_driver) = 0;
  virtual void DestroyDriver(AudioDriver* driver) = 0;

  virtual uint64_t ReadRegister(uint32_t addr);
  virtual void WriteRegister(uint32_t addr, uint64_t value);

 protected:
  virtual void Initialize();

 private:
  void ThreadStart();

  static uint64_t MMIOReadRegisterThunk(AudioSystem* as, uint32_t addr) {
    return as->ReadRegister(addr);
  }
  static void MMIOWriteRegisterThunk(AudioSystem* as, uint32_t addr,
                                     uint64_t value) {
    as->WriteRegister(addr, value);
  }

 protected:
  AudioSystem(Emulator* emulator);

  Emulator* emulator_;
  Memory* memory_;
  cpu::Processor* processor_;

  std::unique_ptr<kernel::XHostThread> thread_;
  std::atomic<bool> running_;

  std::mutex lock_;

  // Stored little endian, accessed through 0x7FEA....
  union {
    struct {
      union {
        struct {
          uint8_t ignored0[0x1800];
          // 1800h; points to guest-space physical block of 320 contexts.
          uint32_t xma_context_array_ptr;
        };
        struct {
          uint8_t ignored1[0x1818];
          // 1818h; current context ID.
          uint32_t current_context;
          // 181Ch; next context ID to process.
          uint32_t next_context;
        };
      };
    } registers_;
    uint32_t register_file_[0xFFFF / 4];
  };
  std::vector<uint32_t> xma_context_free_list_;

  static const size_t maximum_client_count_ = 8;

  struct {
    AudioDriver* driver;
    uint32_t callback;
    uint32_t callback_arg;
    uint32_t wrapped_callback_arg;
  } clients_[maximum_client_count_];
  // Last handle is always there in case we have no clients.
  HANDLE client_wait_handles_[maximum_client_count_ + 1];
  std::queue<size_t> unused_clients_;
};

}  // namespace apu
}  // namespace xe

#endif  // XENIA_APU_AUDIO_SYSTEM_H_
