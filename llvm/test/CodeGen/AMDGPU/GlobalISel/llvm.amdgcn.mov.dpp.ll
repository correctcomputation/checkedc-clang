; NOTE: Assertions have been autogenerated by utils/update_llc_test_checks.py
; RUN: llc -global-isel -march=amdgcn -mcpu=tonga -verify-machineinstrs < %s | FileCheck -check-prefix=GFX8 %s
; RUN: llc -global-isel -march=amdgcn -mcpu=gfx1010 -verify-machineinstrs -show-mc-encoding < %s | FileCheck -check-prefix=GFX10 %s

; FIXME: Merge with DAG test

define amdgpu_kernel void @dpp_test(i32 addrspace(1)* %out, i32 %in) {
; GFX8-LABEL: dpp_test:
; GFX8:       ; %bb.0:
; GFX8-NEXT:    s_load_dwordx2 s[2:3], s[0:1], 0x24
; GFX8-NEXT:    s_load_dword s0, s[0:1], 0x2c
; GFX8-NEXT:    s_waitcnt lgkmcnt(0)
; GFX8-NEXT:    v_mov_b32_e32 v0, s2
; GFX8-NEXT:    v_mov_b32_e32 v2, s0
; GFX8-NEXT:    v_mov_b32_e32 v1, s3
; GFX8-NEXT:    s_nop 0
; GFX8-NEXT:    v_mov_b32_dpp v2, v2 quad_perm:[1,0,0,0] row_mask:0x1 bank_mask:0x1 bound_ctrl:0
; GFX8-NEXT:    flat_store_dword v[0:1], v2
; GFX8-NEXT:    s_endpgm
;
; GFX10-LABEL: dpp_test:
; GFX10:       ; %bb.0:
; GFX10-NEXT:    s_clause 0x1 ; encoding: [0x01,0x00,0xa1,0xbf]
; GFX10-NEXT:    s_load_dword s2, s[0:1], 0x2c ; encoding: [0x80,0x00,0x00,0xf4,0x2c,0x00,0x00,0xfa]
; GFX10-NEXT:    s_load_dwordx2 s[0:1], s[0:1], 0x24 ; encoding: [0x00,0x00,0x04,0xf4,0x24,0x00,0x00,0xfa]
; GFX10-NEXT:    ; implicit-def: $vcc_hi
; GFX10-NEXT:    s_waitcnt lgkmcnt(0) ; encoding: [0x7f,0xc0,0x8c,0xbf]
; GFX10-NEXT:    v_mov_b32_e32 v2, s2 ; encoding: [0x02,0x02,0x04,0x7e]
; GFX10-NEXT:    v_mov_b32_e32 v0, s0 ; encoding: [0x00,0x02,0x00,0x7e]
; GFX10-NEXT:    v_mov_b32_e32 v1, s1 ; encoding: [0x01,0x02,0x02,0x7e]
; GFX10-NEXT:    v_mov_b32_dpp v2, v2 quad_perm:[1,0,0,0] row_mask:0x1 bank_mask:0x1 bound_ctrl:0 ; encoding: [0xfa,0x02,0x04,0x7e,0x02,0x01,0x08,0x11]
; GFX10-NEXT:    global_store_dword v[0:1], v2, off ; encoding: [0x00,0x80,0x70,0xdc,0x00,0x02,0x7d,0x00]
; GFX10-NEXT:    s_endpgm ; encoding: [0x00,0x00,0x81,0xbf]
  %tmp0 = call i32 @llvm.amdgcn.mov.dpp.i32(i32 %in, i32 1, i32 1, i32 1, i1 true) #0
  store i32 %tmp0, i32 addrspace(1)* %out
  ret void
}
define amdgpu_kernel void @mov_dpp64_test(i64 addrspace(1)* %out, i64 %in1) {
; GFX8-LABEL: mov_dpp64_test:
; GFX8:       ; %bb.0:
; GFX8-NEXT:    s_load_dwordx4 s[0:3], s[0:1], 0x24
; GFX8-NEXT:    s_waitcnt lgkmcnt(0)
; GFX8-NEXT:    v_mov_b32_e32 v0, s2
; GFX8-NEXT:    v_mov_b32_e32 v1, s3
; GFX8-NEXT:    v_mov_b32_e32 v3, s1
; GFX8-NEXT:    v_mov_b32_dpp v0, v0 quad_perm:[1,0,0,0] row_mask:0x1 bank_mask:0x1
; GFX8-NEXT:    v_mov_b32_dpp v1, v1 quad_perm:[1,0,0,0] row_mask:0x1 bank_mask:0x1
; GFX8-NEXT:    v_mov_b32_e32 v2, s0
; GFX8-NEXT:    flat_store_dwordx2 v[2:3], v[0:1]
; GFX8-NEXT:    s_endpgm
;
; GFX10-LABEL: mov_dpp64_test:
; GFX10:       ; %bb.0:
; GFX10-NEXT:    s_load_dwordx4 s[0:3], s[0:1], 0x24 ; encoding: [0x00,0x00,0x08,0xf4,0x24,0x00,0x00,0xfa]
; GFX10-NEXT:    ; implicit-def: $vcc_hi
; GFX10-NEXT:    s_waitcnt lgkmcnt(0) ; encoding: [0x7f,0xc0,0x8c,0xbf]
; GFX10-NEXT:    v_mov_b32_e32 v0, s2 ; encoding: [0x02,0x02,0x00,0x7e]
; GFX10-NEXT:    v_mov_b32_e32 v1, s3 ; encoding: [0x03,0x02,0x02,0x7e]
; GFX10-NEXT:    v_mov_b32_e32 v3, s1 ; encoding: [0x01,0x02,0x06,0x7e]
; GFX10-NEXT:    v_mov_b32_e32 v2, s0 ; encoding: [0x00,0x02,0x04,0x7e]
; GFX10-NEXT:    v_mov_b32_dpp v0, v0 quad_perm:[1,0,0,0] row_mask:0x1 bank_mask:0x1 ; encoding: [0xfa,0x02,0x00,0x7e,0x00,0x01,0x00,0x11]
; GFX10-NEXT:    v_mov_b32_dpp v1, v1 quad_perm:[1,0,0,0] row_mask:0x1 bank_mask:0x1 ; encoding: [0xfa,0x02,0x02,0x7e,0x01,0x01,0x00,0x11]
; GFX10-NEXT:    global_store_dwordx2 v[2:3], v[0:1], off ; encoding: [0x00,0x80,0x74,0xdc,0x02,0x00,0x7d,0x00]
; GFX10-NEXT:    s_endpgm ; encoding: [0x00,0x00,0x81,0xbf]
  %tmp0 = call i64 @llvm.amdgcn.mov.dpp.i64(i64 %in1, i32 1, i32 1, i32 1, i1 false) #0
  store i64 %tmp0, i64 addrspace(1)* %out
  ret void
}

declare i32 @llvm.amdgcn.mov.dpp.i32(i32, i32 immarg, i32 immarg, i32 immarg, i1 immarg) #0
declare i64 @llvm.amdgcn.mov.dpp.i64(i64, i32 immarg, i32 immarg, i32 immarg, i1 immarg) #0

attributes #0 = { convergent nounwind readnone }
