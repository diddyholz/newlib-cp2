#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include "cas_file.h"
#include "crt0.h"
#include "power.h"
#include "tmu.h"

uint32_t counter_underflows = 0;

/* setup hardware */
void 
__attribute__((section(".bootstrap.text")))
cas_setup()
{
  counter_underflows = 0;

  POWER_MSTPCR0->TMU = 0;

  TMU_TSTR->raw = 0;

  /* Start TMU Channel 2 for time keeping */
  TMU_TCR_2->raw;
  TMU_TCR_2->TPSC = PHI_DIV_16;

  *TMU_TCOR_2 = 0xFFFFFFFF;
  *TMU_TCNT_2 = 0xFFFFFFFF;

  TMU_TSTR->STR2 = 1;
}

/* cleanup hardware */
void 
cas_cleanup()
{
  TMU_TSTR->raw = 0;
  POWER_MSTPCR0->TMU = 1;
}

/* convert cas file errors to errno */
int 
cas_error_to_errno(int cas_result) 
{
  if (cas_result >= 0) 
  {
    return cas_result;
  }

  int error = 0;

  switch (cas_result)
  {
    case CAS_ENOMEM:
        error = ENOMEM;
        break;
    case CAS_EINVAL:
        error = EINVAL;
        break;
    case CAS_EACCES:
        error = EACCES;
        break;
    case CAS_EEXIST:
        error = EEXIST;
        break;
    case CAS_ENAMETOOLONG:
        error = ENAMETOOLONG;
        break;
    case CAS_ENOSPC:
        error = ENOSPC;
        break;
    case CAS_ENOENT:
        error = ENOENT;
        break;
    case CAS_EISDIRECTORY:
        error = EISDIR;
        break;
    case CAS_EMFILE:
        error = EMFILE;
        break;
    case CAS_EBADF:
        error = EBADF;
        break;
    case CAS_ENOTEMPTY:
        error = ENOTEMPTY;
        break;
    
    default:
        error = __ELASTERROR;
        break;
  }  

  errno = error;

  return -1;
}

/* convert open flags to cas flags */
int 
flags_to_cas_flags(int flags) 
{  
  int cas_flags = 0;
  
  cas_flags |= ((flags & 0x03) + 1);

  if (flags & O_CREAT)
  {
    flags |= CAS_O_CREATE;
  }

  if (flags & O_APPEND)
  {
    flags |= CAS_O_APPEND;
  }

  return cas_flags;
}

void cas_stat_to_stat (struct cas_stat *cas_st, 
  struct stat *st)
{
  memset(st, 0, sizeof(*st));
  st->st_size = cas_st->fileSize;
} 

int
access (const char *path,
  int mode)
{
  /* Mode can be ignored as we do not have permissions */
  int fd = _open(path, O_RDONLY);

  if (fd == -1) 
  {
    return -1;
  }

  _close(fd);
  return 0;
}

int 
_read(
  int file, 
  char *ptr, 
  int len)
{
  int ret = cas_read(file, ptr, len);
  return cas_error_to_errno(ret);
}

int 
_lseek(
  int file,
  int ptr,
  int dir)
{
  int ret = cas_lseek(file, ptr, dir);
  return cas_error_to_errno(ret);
}

int 
_write( 
  int file,
  char *ptr,
  int len)
{
  int ret = cas_write(file, ptr, len);
  return cas_error_to_errno(ret);
}

int
_open(const char *path,
  int flags,
  ...)
{
  int ret = cas_open(path, flags_to_cas_flags(flags));
  return cas_error_to_errno(ret);
}

int
_close(int file)
{
  int ret = cas_close(file);
  return cas_error_to_errno(ret);
}

int
_creat(const char *path,
  int mode)
{
  int ret = _open(path, O_WRONLY | O_CREAT | O_TRUNC);
  return cas_error_to_errno(ret);
}

int
_isatty (int fd)
{
  return 0;
}

int
_fstat(int file,
  struct stat *st)
{
  struct cas_stat stat;
  int ret = cas_fstat(file, &stat);

  if (ret < 0) {
    return cas_error_to_errno(ret);
  }

  cas_stat_to_stat(&stat, st);

  return ret;
}

int
_stat (const char *path, 
  struct stat *st)
{
  struct cas_stat stat;
  int ret = cas_stat(path, &stat);

  if (ret < 0) {
    return cas_error_to_errno(ret);
  }

  cas_stat_to_stat(&stat, st);

  return ret;
}

int
_link(char *old, 
  char *new)
{
  errno = ENOSYS;
  return -1;
}

int
_unlink()
{
  errno = ENOSYS;
  return -1;
}

caddr_t 
_sbrk (int incr)
{
  errno = ENOSYS;
  return -1;
}

void
_exit (int status)
{
  _exit_address();
}

int
_kill (pid_t n, int m)
{
  errno = ENOSYS;
  return -1;
}

int
_getpid (void)
{
  return 1;
}

int
_raise ()
{
  return -1;
}

int
_chmod (const char *path, 
  short mode)
{
  errno = ENOSYS;
  return -1;
}

int
_chown (const char *path, 
  short owner, 
  short group)
{
  errno = ENOSYS;
  return -1;
}

int
_utime (const char *path, 
  char *times)
{
  errno = ENOSYS;
  return -1;
}

int
_fork ()
{
  errno = ENOSYS;
  return -1;
}

int
_wait (statusp)
     int *statusp;
{
  errno = ENOSYS;
  return -1;
}

int
_execve (const char *path, char *const argv[], char *const envp[])
{
  errno = ENOSYS;
  return -1;
}

int
_execv (const char *path, char *const argv[])
{
  errno = ENOSYS;
  return -1;
}

int
_pipe (int *fd)
{
  errno = ENOSYS;
  return -1;
}

/* This is only provided because _gettimeofday_r and _times_r are
   defined in the same module, so we avoid a link error.  */
clock_t
_times (struct tms *tp)
{
  errno = ENOSYS;
  return -1;
}

int
_gettimeofday (struct timeval *tv, void *tz)
{
  // Count up global counter if timer underflowed
  if (TMU_TCR_2->UNF)
  {
    TMU_TCR_2->UNF = 0;
    counter_underflows++;
  }

  if (tv)
  {
    uint32_t ticssincestart = 0xFFFFFFFF - *TMU_TCNT_2;
    tv->tv_usec = ticssincestart / (TMU_TICKS_PER_USEC / 4);
    tv->tv_sec = tv->tv_usec / (1000 * 1000);
  }

  return 0;
}

int 
usleep (useconds_t usec)
{
  struct timeval starttime;
  struct timeval curtime;
  _gettimeofday(&starttime, NULL);

  /* FIXME: This will break when it is called while the counter underflows */
  do 
  { 
    _gettimeofday(&curtime, NULL);
  } while ((curtime.tv_usec - starttime.tv_usec) < usec);

  return 0;  
}
