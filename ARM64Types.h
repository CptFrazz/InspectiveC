#ifndef ARM64_TYPES_H
#define ARM64_TYPES_H

// ARM64 defines.
#define alignof(t) __alignof__(t)

// TODO(DavidGoldman): Treat the regs as a pointer directly to avoid casting? i.e.:
// (*(t *)(&args.regs->general.arr[args.ngrn++]))
#define pa_arg(args, t) \
  ( (args.ngrn < 8) ? ((t)(args.regs->general.arr[args.ngrn++])) : \
        pa_stack_arg(args, t) \
    )

#define pa_float(args) \
  ( (args.nsrn < 8) ? args.regs->floating.arr[args.nsrn++].f.f1 : \
        pa_stack_arg(args, float) \
    )

#define pa_double(args) \
  ( (args.nsrn < 8) ? args.regs->floating.arr[args.nsrn++].d.d1 : \
        pa_stack_arg(args, double) \
    )

// We need to align the sp - we do so via sp = ((sp + alignment - 1) & -alignment).
// Then we increment the sp by the size of the argument and return the argument.
#define pa_stack_arg(args, t) \
  (*(t *)( (args.stack = (unsigned char *)( ((uintptr_t)args.stack + (alignof(t) - 1)) & -alignof(t)) + sizeof(t)) - sizeof(t) ))

typedef union FPReg_ {
  __int128_t q;
  struct {
    double d1; // Holds the double (LSB).
    double d2;
  } d;
  struct {
    float f1; // Holds the float (LSB).
    float f2;
    float f3;
    float f4;
  } f;
} FPReg;

struct RegState_ {
  union {
    uint64_t arr[10];
    struct {
      uint64_t x0;
      uint64_t x1;
      uint64_t x2;
      uint64_t x3;
      uint64_t x4;
      uint64_t x5;
      uint64_t x6;
      uint64_t x7;
      uint64_t x8;
      uint64_t lr;
    } regs;
  } general;

  union {
    FPReg arr[8];
    struct {
      FPReg q0;
      FPReg q1;
      FPReg q2;
      FPReg q3;
      FPReg q4;
      FPReg q5;
      FPReg q6;
      FPReg q7;
    } regs;
  } floating;
};

typedef struct pa_list_ {
  struct RegState_ *regs; // Registers saved when function is called.
  unsigned char *stack; // Address of current argument.
  int ngrn; // The Next General-purpose Register Number.
  int nsrn; // The Next SIMD and Floating-point Register Number.
} pa_list;

#endif

