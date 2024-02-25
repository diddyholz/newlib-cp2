#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include "cas_file.h"
#include "crt0.h"

/* This is used by _sbrk.  */
register char *stack_ptr asm ("r15");

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

  _set_errno(error);

  return -1;
}

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
  extern char end;		/* Defined by the linker */
  errno = ENOSYS;
  return -1;
}

void
_exit (int status)
{
  exit_address();
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
  _set_errno(ENOSYS);
  return -1;
}
