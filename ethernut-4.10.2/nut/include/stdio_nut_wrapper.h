// prefix all function calls and definitions by NUT_

// rename FILE to NUT_FILE 
#define FILE NUT_FILE

// 
#define NUT_STDIO_PREFIXED

// prefix all stdio calls with NUT_
#define clearerr(...) NUT_clearerr(__VA_ARGS__)
#define fclose(...) NUT_fclose(__VA_ARGS__)
#define feof(...) NUT_feof(__VA_ARGS__)
#define ferror(...) NUT_ferror(__VA_ARGS__)
#define fflush(...) NUT_fflush(__VA_ARGS__)
#define fgetc(...) NUT_fgetc(__VA_ARGS__)
#define fgets(...) NUT_fgets(__VA_ARGS__)
#define fopen(...) NUT_fopen(__VA_ARGS__)
#define fprintf(...) NUT_fprintf(__VA_ARGS__)
#define fputc(...) NUT_fputc(__VA_ARGS__)
#define fputs(...) NUT_fputs(__VA_ARGS__)
#define fpurge(...) NUT_fpurge(__VA_ARGS__)
#define fread(...) NUT_fread(__VA_ARGS__)
#define freopen(...) NUT_freopen(__VA_ARGS__)
#define fscanf(...) NUT_fscanf(__VA_ARGS__)
#define fseek(...) NUT_fseek(__VA_ARGS__)
#define ftell(...) NUT_ftell(__VA_ARGS__)
#define fwrite(...) NUT_fwrite(__VA_ARGS__)
#define getc(...) NUT_getc(__VA_ARGS__)
#define getchar(...) NUT_getchar(__VA_ARGS__)
#define gets(...) NUT_gets(__VA_ARGS__)
#define printf(...) NUT_printf(__VA_ARGS__)
#define putc(...) NUT_putc(__VA_ARGS__)
#define putchar(...) NUT_putchar(__VA_ARGS__)
#define puts(...) NUT_puts(__VA_ARGS__)
#define scanf(...) NUT_scanf(__VA_ARGS__)
#define sprintf(...) NUT_sprintf(__VA_ARGS__)
#define sscanf(...) NUT_sscanf(__VA_ARGS__)
#define ungetc(...) NUT_ungetc(__VA_ARGS__)
#define vfprintf(...) NUT_vfprintf(__VA_ARGS__)
#define vfscanf(...) NUT_vfscanf(__VA_ARGS__)
#define vsprintf(...) NUT_vsprintf(__VA_ARGS__)
#define vsscanf(...) NUT_vsscanf(__VA_ARGS__)
#define fprintf_P(...) NUT_fprintf(__VA_ARGS__)
#define fputs_P(...) NUT_fputs(__VA_ARGS__)
#define fscanf_P(...) NUT_fscanf(__VA_ARGS__)
#define fwrite_P(...) NUT_fwrite(__VA_ARGS__)
#define printf_P(...) NUT_printf(__VA_ARGS__)
#define puts_P(...) NUT_puts(__VA_ARGS__)
#define scanf_P(...) NUT_scanf(__VA_ARGS__)
#define sprintf_P(...) NUT_sprintf(__VA_ARGS__)
#define sscanf_P(...) NUT_sscanf(__VA_ARGS__)
#define vfprintf_P(...) NUT_vfprintf(__VA_ARGS__)
#define vfscanf_P(...) NUT_vfscanf(__VA_ARGS__)
#define vsprintf_P(...) NUT_vsprintf(__VA_ARGS__)
#define vsscanf_P(...) NUT_vsscanf(__VA_ARGS__)

#define fileno(...) NUT__fileno(__VA_ARGS__)
#define fcloseall(...) NUT_fcloseall(__VA_ARGS__)

#define _fileno(...) NUT__fileno(__VA_ARGS__)
#define _fdopen(...) NUT__fdopen(__VA_ARGS__)
#define _flushall(...) NUT__flushall(__VA_ARGS__)
#define _fmode(...) NUT__fmode(__VA_ARGS__)

