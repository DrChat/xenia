# Copyright 2014 Ben Vanik. All Rights Reserved.
{
  'sources': [
    'arena.cc',
    'arena.h',
    'assert.h',
    'atomic.h',
    'byte_order.h',
    'debugging.h',
    'delegate.h',
	'disc_device_win.cc',
    'fs.h',
    'fs.cc',
    'logging.cc',
    'logging.h',
    'main.h',
    'mapped_memory.h',
    'math.cc',
    'math.h',
    'memory_generic.cc',
    'memory.h',
    'platform.h',
    'reset_scope.h',
    'string.cc',
    'string.h',
    'string_buffer.cc',
    'string_buffer.h',
    'threading.cc',
    'threading.h',
    'type_pool.h',
    'vec128.h',
  ],

  'conditions': [
    ['OS == "mac" or OS == "linux"', {
      'sources': [
        'main_posix.cc',
        'mapped_memory_posix.cc',
      ],
    }],
    ['OS == "linux"', {
      'sources': [
        'threading_posix.cc',
      ],
    }],
    ['OS == "mac"', {
      'sources': [
        'debugging_mac.cc',
        'threading_mac.cc',
      ],
    }],
    ['OS == "win"', {
      'sources': [
        'debugging_win.cc',
        'fs_win.cc',
        'main_win.cc',
        'mapped_memory_win.cc',
        'threading_win.cc',
      ],
    }],
  ],

  'includes': [
  ],
}
