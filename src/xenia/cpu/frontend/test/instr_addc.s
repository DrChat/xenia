# constant tests are commented since add_carry isn't supported

test_addc_1:
  #_ REGISTER_IN r4 1
  #_ REGISTER_IN r5 2
  addc r3, r4, r5
  adde r6, r0, r0
  blr
  #_ REGISTER_OUT r3 3
  #_ REGISTER_OUT r4 1
  #_ REGISTER_OUT r5 2
  #_ REGISTER_OUT r6 0

#test_addc_1_constant:
#  li r4, 1
#  li r5, 2
#  addc r3, r4, r5
#  adde r6, r0, r0
#  blr
#  #_ REGISTER_OUT r3 3
#  #_ REGISTER_OUT r4 1
#  #_ REGISTER_OUT r5 2
#  #_ REGISTER_OUT r6 0

test_addc_2:
  #_ REGISTER_IN r4 0xFFFFFFFFFFFFFFFF
  #_ REGISTER_IN r5 0
  addc r3, r4, r5
  adde r6, r0, r0
  blr
  #_ REGISTER_OUT r3 0xFFFFFFFFFFFFFFFF
  #_ REGISTER_OUT r4 0xFFFFFFFFFFFFFFFF
  #_ REGISTER_OUT r5 0
  #_ REGISTER_OUT r6 0

#test_addc_2_constant:
#  li r4, -1
#  li r5, 0
#  addc r3, r4, r5
#  adde r6, r0, r0
#  blr
#  #_ REGISTER_OUT r3 0xFFFFFFFFFFFFFFFF
#  #_ REGISTER_OUT r4 0xFFFFFFFFFFFFFFFF
#  #_ REGISTER_OUT r5 0
#  #_ REGISTER_OUT r6 0

test_addc_3:
  #_ REGISTER_IN r4 0xFFFFFFFFFFFFFFFF
  #_ REGISTER_IN r5 1
  addc r3, r4, r5
  adde r6, r0, r0
  blr
  #_ REGISTER_OUT r3 0
  #_ REGISTER_OUT r4 0xFFFFFFFFFFFFFFFF
  #_ REGISTER_OUT r5 1
  #_ REGISTER_OUT r6 1

#test_addc_3_constant:
#  li r4, -1
#  li r5, 1
#  addc r3, r4, r5
#  adde r6, r0, r0
#  blr
#  #_ REGISTER_OUT r3 0
#  #_ REGISTER_OUT r4 0xFFFFFFFFFFFFFFFF
#  #_ REGISTER_OUT r5 1
#  #_ REGISTER_OUT r6 1

test_addc_4:
  #_ REGISTER_IN r4 0xFFFFFFFFFFFFFFFF
  #_ REGISTER_IN r5 123
  addc r3, r4, r5
  adde r6, r0, r0
  blr
  #_ REGISTER_OUT r3 0x000000000000007A
  #_ REGISTER_OUT r4 0xFFFFFFFFFFFFFFFF
  #_ REGISTER_OUT r5 123
  #_ REGISTER_OUT r6 1

#test_addc_4_constant:
#  li r4, -1
#  li r5, 123
#  addc r3, r4, r5
#  adde r6, r0, r0
#  blr
#  #_ REGISTER_OUT r3 0x000000000000007A
#  #_ REGISTER_OUT r4 0xFFFFFFFFFFFFFFFF
#  #_ REGISTER_OUT r5 123
#  #_ REGISTER_OUT r6 1

test_addc_5:
  #_ REGISTER_IN r4 0x7FFFFFFFFFFFFFFF
  #_ REGISTER_IN r5 0xFFFFFFFFFFFFFFFF
  addc r3, r4, r5
  adde r6, r0, r0
  blr
  #_ REGISTER_OUT r3 0x7FFFFFFFFFFFFFFE
  #_ REGISTER_OUT r4 0x7FFFFFFFFFFFFFFF
  #_ REGISTER_OUT r5 0xFFFFFFFFFFFFFFFF
  #_ REGISTER_OUT r6 1

#test_addc_5_constant:
#  li r4, -1
#  sldi r5, r4, 1
#  addc r3, r4, r5
#  adde r6, r0, r0
#  blr
#  #_ REGISTER_OUT r3 0x7FFFFFFFFFFFFFFE
#  #_ REGISTER_OUT r4 0x7FFFFFFFFFFFFFFF
#  #_ REGISTER_OUT r5 0xFFFFFFFFFFFFFFFF
#  #_ REGISTER_OUT r6 1
