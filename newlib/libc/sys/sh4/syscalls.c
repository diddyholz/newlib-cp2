#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include "cas_file.h"
#include "crt0.h"
#include "power.h"
#include "tmu.h"
#include "lcd.h"

#define DEBUG_CHAR_WIDTH 	8
#define DEBUG_CHAR_HEIGHT	12
#define DEBUG_LINE_HEIGHT	14

#define DEBUG_MAX_COLS    40
#define DEBUG_MAX_ROWS    37

#define DEBUG_FONTBASE    0x8062F4C8

#define DEBUG_COLOR_OUT   0xFFFF

uint32_t counter_underflows __attribute__((section(".bootstrap.data")));

uint8_t print_col __attribute__((section(".bootstrap.data")));
uint8_t print_row __attribute__((section(".bootstrap.data")));
uint32_t used_rows __attribute__((section(".bootstrap.data")));

char debug_lines[DEBUG_MAX_ROWS][DEBUG_MAX_COLS];

/* setup hardware */
void 
__attribute__((section(".bootstrap.text")))
cas_setup ()
{
  counter_underflows = 0;
  print_col = DEBUG_MAX_COLS;
  print_row = DEBUG_MAX_ROWS;
  used_rows = 0;

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
cas_cleanup ()
{
  TMU_TSTR->raw = 0;
  POWER_MSTPCR0->TMU = 1;
}

void 
debug_print_char (char character, 
  uint8_t col, 
  uint8_t row, 
  uint16_t foreground, 
  uint16_t background, 
  bool enable_transparency)
{
  uint8_t charIndex = character - ' ';

  // fill background with background color if it is not black
  if(!enable_transparency || background)
  {
    for (uint8_t y = 0; y < DEBUG_LINE_HEIGHT; y++)
    {
      for (uint8_t x = 0; x < DEBUG_CHAR_WIDTH; x++)
      {
        FRAMEBUFFER[((y + row * DEBUG_LINE_HEIGHT) * LCD_WIDTH) 
          + (x + col * DEBUG_CHAR_WIDTH)] = background;
      }
    }
  }

  // now draw the character
  uint16_t *pixel = (uint16_t *)(DEBUG_FONTBASE + (0xC0 * charIndex));

  const uint16_t tempXPos = col * DEBUG_CHAR_WIDTH;
  const uint16_t tempYPos = row * DEBUG_LINE_HEIGHT + 1;

  for (uint8_t iy = 0; iy < DEBUG_CHAR_HEIGHT; iy++)
  {
    for (uint8_t ix = 0; ix < DEBUG_CHAR_WIDTH; ix++)
    {
      if (*pixel == 0)
      {
        FRAMEBUFFER[((iy + tempYPos) * LCD_WIDTH) + (ix + tempXPos)] = foreground;
      }
      
      pixel++;   
    }
  }
}

void debug_print_line (const char *line, 
    uint8_t row)
{
  bool end = false;

  for (uint8_t i = 0; i < DEBUG_MAX_COLS; i++)
  {
    if (line[i] == '\0')
    {
      end = true;
    }
    
    const char char_to_print = end? ' ' : line[i];
    debug_print_char(char_to_print, i, row, DEBUG_COLOR_OUT, 0x0000, false);
  }
}

void debug_print_all (void)
{
  for (uint8_t i = 0; i < DEBUG_MAX_ROWS && i < used_rows; i++)
  {
    int8_t line = i;

    if (used_rows > DEBUG_MAX_ROWS)
    {
      line = (DEBUG_MAX_ROWS - 1) - print_row + i;
      line %= DEBUG_MAX_ROWS;
    }

    debug_print_line(debug_lines[i], line);
  }
}

void debug_cursor_inc (void)
{
  print_col++;

  if (print_col < DEBUG_MAX_COLS)
  {
    return;
  }

  // Reached end of line
  print_col = 0;
  print_row++;
  used_rows++;

  if (print_row < DEBUG_MAX_ROWS)
  {
    goto clear_row;
  }

  // Reached end of rows
  print_row = 0;

clear_row:
  for (uint8_t i = 0; i < DEBUG_MAX_COLS; i++)
  {
    debug_lines[print_row][i] = '\0';
  }
}

void debug_cursor_dec (void)
{
  // Check if at startup
  if (print_col == 0 && used_rows == 1)
  {
    return;
  }

  print_col--;

  if (print_col < DEBUG_MAX_COLS)
  {
    return;
  }

  print_col = DEBUG_MAX_COLS - 1;
  print_row--;
  used_rows--;

  if (print_row < DEBUG_MAX_ROWS)
  {
    return;
  }

  print_row = DEBUG_MAX_ROWS - 1;
}

void
debug_add_string (const char *str,
  int len)
{
  for (int i = 0; i < len; i++)
  {
    if (str[i] == '\b')
    {
      debug_cursor_dec();
      continue;
    }

    if (str[i] == '\n')
    {
      print_col = DEBUG_MAX_COLS;
      continue;
    }

    debug_cursor_inc();
    debug_lines[print_row][print_col] = str[i];
  }
}

/* convert cas file errors to errno */
int 
cas_error_to_errno (int cas_result) 
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
flags_to_cas_flags (int flags) 
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

int _open(const char *, int, ...);
int _close(int);

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
mkdir (const char *path, 
  mode_t mode)
{
  int ret = cas_mkdir(path);
  return cas_error_to_errno(ret);
}

int 
_read(
  int file, 
  char *ptr, 
  int len)
{
  /* Do not do anything for std streams */
  if (file < 3) 
  {
    return 0;
  }

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
  /* Do not do anything for stdin */
  if (file == 0) 
  {
    return len;
  }

  /* stdout and stderr */
  if (file < 3) 
  {
    debug_add_string(ptr, len);
    debug_print_all();
    LCD_Refresh();
    
    return len;
  }

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
  /* Do not do anything for std streams */
  if (file < 3) 
  {
    return 0;
  }

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
  return (caddr_t)-1;
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
