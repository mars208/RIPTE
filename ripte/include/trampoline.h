#ifndef TRAMPOLINE
#define TRAMPOLINE

int hook_ret0(void);
void hook_b(void);
void hook_bl(void);
void hook_bx_lr(void);
void hook_pop_fp_pc(void);
void hook_pop_fp_lr(void);
void hook_blx_r3(void);

#endif