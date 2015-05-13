test_lvr_1:
  #_ MEMORY_IN 000010B0 090A0A0B 0C0F120A 0B0C0D0E 0F10130C 0D0E1011 121314FF FFFFFFFF
  #_ REGISTER_IN r4 0x10B7
  #_ REGISTER_IN r5 0x10
  lvrx v3, r4, r5
  blr
  #_ REGISTER_OUT r4 0x10B7
  #_ REGISTER_OUT r5 0x10
  #_ REGISTER_OUT v3 [00000000, 00000000, 000D0E10, 11121314]

test_lvr_1_constant:
  #_ MEMORY_IN 000010B0 090A0A0B 0C0F120A 0B0C0D0E 0F10130C 0D0E1011 121314FF FFFFFFFF
  li r4, 0x10B7
  li r5, 0x10
  lvrx v3, r4, r5
  blr
  #_ REGISTER_OUT r4 0x10B7
  #_ REGISTER_OUT r5 0x10
  #_ REGISTER_OUT v3 [00000000, 00000000, 000D0E10, 11121314]
