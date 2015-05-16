
#ifndef SYS_PTRACE_H
#define SYS_PTRACE_H

#if __cplusplus
extern "C" {
#endif

  struct pt_regs;

  void ptrace_dump_regs( struct pt_regs *p );
  void ptrace_stackdump_from( const int *frame_ptr );
  void ptrace_stackdump_regs( struct pt_regs *p );
  void ptrace_stackdump_here( void );

#if __cplusplus
}
#endif

#endif /* SYS_PTRACE_H */
