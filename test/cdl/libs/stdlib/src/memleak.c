/**************************************************************************
   memleak.c: A simple module for debbuging memory leaks. ANSI C.
   Copyright (c) 2005, 2008 Stanislav Maslovski.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
   *************************************************************************

   ===================
   Theory of operation
   ===================
   The program simply allocates a header together with a piece of data.
   The headers contain information about the calls and are organized in a
   bidirectional list. The history of malloc() - free() calls is collected
   in a separate place in a circular buffer.
   
   ============
   Limitations:
   ============
   I use unsigned long to store the block allocation sizes. The overall allocated
   memory count is stored as a long. In ANSI C there is no type modifier
   for printing unsigned long values, so they are printed by casting to unsigned long.
   Pointers are validated by simple linear search in the lists, do not expect it
   to be the same fast as the library routines.

   ==============
   How to use it:
   ==============
   include memleak.h into all modules of your program you want to debug.
   Place a call to dbg_init() somewhere in the beginning of execution.
   Add calls to dbg_mem_stat() or dbg_heap_dump() or others (see below)
   to any suspicious place of your code. You may use "keyword" argument
   to the dumping functions to reduce the amount of the output.

   ===============================
   void dbg_init(unsigned history_size)
   ===============================
   This initializes the debugger. history_size defines the maximum number of
   stored headers of the freed data.

   ========================
   void dbg_zero_stat(void)
   ========================
   clears statistics.

   =======================
   dbg_catch_sigsegv(void)
   =======================
   triggers on SIGSEGV signal handler.

   ==================================================
   int dbg_check_addr(char *msg, void *addr, int opt)
   ==================================================
   checks for addr in the tables given by opt. opt can be CHK_FREED or
   CHK_ALLOC or CHK_ALL; returns CHK_FREED, CHK_ALLOC, or 0 if not found.
   Also prints debugging message to stderr, prefixed with msg.

   =======================
   void dbg_mem_stat(void) 
   =======================
   This function prints call statistics to stderr. The location of the
   call is printed at the beginning of the status line. The status line
   looks like this:
   
   file:line m:<num> c: <num> r:<num> f:<num> mem:<num>
   
   where m, c, r, f count malloc(), calloc(), realloc(), and free() calls.
   mem equals the total amount of requested and not yet freed memory (not
   exactly the same as the amount of actually allocated physical memory). 
   
   =================================
   void dbg_heap_dump(char *keyword)
   =================================
   This function prints a dump of the current heap state to stderr. Every
   line of the dump corresponds to an allocated and not yet freed piece of
   data. Use "keyword" to narrower the dump: keyword = "" => entire heap
   is dumped, if not "", strstr() is used to select strings with keyword
   in them.

   ====================================
   void dbg_history_dump(char *keyword)
   ====================================
   dumps history of malloc() - free() calls. keyword is the same as above.
*/
#if 0
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#endif
#include <linux/types.h>	/* __u8 etc */

#include "linux/string.h"
#include "stdlib_config.h"
#include "std_common.h"
#include "malloc.h"

#define die(msg) (puts(msg))
#define length(type) ((sizeof(type)*5)/2 + 1)

char *dbg_file_name;
unsigned long dbg_line_number;

static long memory_cnt = 0;
static int malloc_cnt = 0;
static int memalign_cnt = 0;

static int calloc_cnt = 0;
static int realloc_cnt = 0;
static int free_cnt = 0;

static unsigned history_length = 0;

struct head
{
  void *addr;
  unsigned long size;
  char *file;
  unsigned long line;
  /* two addresses took the same space as an address and an integer on many archs => usable */
  union {
    struct { struct head *prev, *next; } list;
    struct { char *file; unsigned long line; } free;
  } in;
};

#define CHK_FREED 1
#define CHK_ALLOC 2
#define CHK_ALL (CHK_FREED | CHK_ALLOC)

int dbg_check_addr(char *msg, void *ptr, int opt);
void dbg_abort(char *msg);

static struct head *first = NULL, *last = NULL;
static struct head *hist_base = NULL, *histp = NULL;

#define HLEN sizeof(struct head)

static struct head *add(void *buf, unsigned long s)
{
  struct head *p;
  p = malloc(HLEN);
  if(p)
    {
      p->addr = buf;
      p->size = s;
      p->file = dbg_file_name;
      p->line = dbg_line_number;
      p->in.list.prev = last;
      p->in.list.next = NULL;
      if(last)
	last->in.list.next = p;
      else
	first = p;
      last = p;
      memory_cnt += s;
    }
  return p;
}

#define ADVANCE(p) {if(++(p) >= hist_base + history_length) (p) = hist_base;}

static void del(struct head *p)
{
  struct head *prev, *next;
  prev = p->in.list.prev;
  next = p->in.list.next;
  if(prev)
    prev->in.list.next = next;
  else
    first = next;
  if(next)
    next->in.list.prev = prev;
  else
    last = prev;
  memory_cnt -= p->size;
  /* update history */
  if(history_length)
    {
      p->in.free.file = dbg_file_name;
      p->in.free.line = dbg_line_number;
      memcpy(histp, p, HLEN);
      ADVANCE(histp);
    }
  free(p);
}

static void replace(struct head *p, void *buf, unsigned long s)
{
  memory_cnt -= p->size;
  p->addr = buf;
  p->size = s;
  p->file = dbg_file_name;
  p->line = dbg_line_number;
  memory_cnt += s;
}

void *dbg_malloc(unsigned long s)
{
  void *buf;
  malloc_cnt++;
  buf = malloc(s);
  if(buf)
    {
      if(add(buf, s))
	return buf;
      else
	free(buf);
    }
  printf( "%s:%lu: dbg_malloc: not enough memory\n", dbg_file_name, dbg_line_number);
  return NULL;
}

void *dbg_memalign(unsigned long alignment, unsigned long s)
{
  void *buf;
  memalign_cnt++;
  buf = memalign(alignment, s);
  if(buf)
    {
      if(add(buf, s))
	return buf;
      else
	free(buf);
    }
  printf( "%s:%lu: dbg_memalign: not enough memory\n", dbg_file_name, dbg_line_number);
  return NULL;
}

void *dbg_calloc(unsigned long n, unsigned long s)
{
  void *buf;
  s *= n;
  calloc_cnt++;
  buf = malloc(s);
  if(buf)
    {
      if(add(buf, s))
	{
	  /* standard calloc() sets memory to zero */
	  memset(buf, 0, s);
	  return buf;
	}
      else
	free(buf);
    }
  printf( "%s:%lu: dbg_calloc: not enough memory\n", dbg_file_name, dbg_line_number);
  return NULL;
}

static struct head *find_in_heap(void *addr)
{
  struct head *p;
  /* start search from lately allocated blocks */ 
  for(p = last; p; p = p->in.list.prev)
    if(p->addr == addr) return p;
  return NULL;
}

static struct head *find_in_hist(void *addr)
{
  struct head *p;
  int cnt;
  if(history_length)
    {
      if(histp->addr)
	{
	  cnt = history_length;
	  p = histp;
	}
      else
	{
	  cnt = histp - hist_base;
	  p = hist_base;
	}
      while(cnt--)
	{
	  if(p->addr == addr) return p;
	  ADVANCE(p);
	}
    }
  return NULL;
}

void dbg_free(void *buf)
{
  struct head *p;
  free_cnt++;
  if(buf)
    if((p = find_in_heap(buf)))
      {
        del(p);
        free(buf);
      }
    else
	dbg_check_addr("dbg_free", buf, CHK_FREED);
  else
    printf( "%s:%lu: dbg_free: NULL\n", dbg_file_name, dbg_line_number);
}

void *dbg_realloc(void *buf, unsigned long s)
{
  struct head *p;
  realloc_cnt++;
  if(buf) /* when buf is NULL do malloc. counted twice as r and m */
    if(s)  /* when s = 0 realloc() acts like free(). counted twice: as r and f */
      if((p = find_in_heap(buf)))
	{
	  buf = realloc(buf, s);
	  if(buf)
	    {
	      replace(p, buf, s);
	      return buf;
	    }
	  else
	    printf( "%s:%lu: dbg_realloc: not enough memory\n",
		    dbg_file_name, dbg_line_number);
	}
      else
	dbg_check_addr("dbg_realloc", buf, CHK_FREED);
    else
      dbg_free(buf);
  else
    return dbg_malloc(s);
  return NULL;
}

int dbg_check_addr(char *msg, void *addr, int opt)
{
  struct head *p;
  if(opt & CHK_ALLOC)
    if((p = find_in_heap(addr)))
      {
	printf( "%s:%lu: %s: allocated (alloc: %s:%lu size: %lu)\n",
		dbg_file_name, dbg_line_number, msg, p->file, p->line, (unsigned long)p->size);
	return CHK_ALLOC;
      }
  if(opt & CHK_FREED)
    if((p = find_in_hist(addr)))
      {
	printf( "%s:%lu: %s: freed (alloc: %s:%lu size: %lu free: %s:%lu)\n",
		dbg_file_name, dbg_line_number, msg, p->file, p->line,
		(unsigned long)p->size, p->in.free.file, p->in.free.line);
	return CHK_FREED;
      }
  if(opt) printf( "%s:%lu: %s: unknown\n", dbg_file_name, dbg_line_number, msg);
  return 0;
}

void dbg_mem_stat(void)
{
  printf( "%s:%lu: mem_stat -- malloc: %d, memalign: %d, calloc: %d, realloc: %d, free: %d, mem_leak: %ld\n",
	  dbg_file_name, dbg_line_number,
	  malloc_cnt, memalign_cnt, calloc_cnt, realloc_cnt, free_cnt, memory_cnt);
}

void dbg_heap_dump(char *key)
{
  char *buf;
  struct head *p;
  printf( "***** %s:%lu: heap dump start\n", dbg_file_name, dbg_line_number);
  p = first;
  while(p)
    {
      buf = malloc(strlen(p->file) + 2*length(long) + 20);
      sprintf(buf, "(alloc: %s:%lu size: %lu)\n", p->file, p->line, (unsigned long)p->size);
      p = p->in.list.next;
      if(strstr(buf, key)) puts(buf);
      free(buf);
    }
  printf( "***** %s:%lu: heap dump end\n", dbg_file_name, dbg_line_number);
}

void dbg_history_dump(char *key)
{
  int cnt;
  char *buf;
  struct head *p;
  if(history_length)
    {
      printf( "***** %s:%lu: history dump start\n", dbg_file_name, dbg_line_number);
      if(histp->addr)
	{
	  cnt = history_length;
	  p = histp;
	}
      else
	{
	  cnt = histp - hist_base;
	  p = hist_base;
	}
      while(cnt--)
	{
	  buf = malloc(strlen(p->file) + strlen(p->in.free.file) + 3*length(long) + 30);
	  sprintf(buf, "(alloc: %s:%lu size: %lu free: %s:%lu)\n",
		  p->file, p->line, (unsigned long)p->size, p->in.free.file, p->in.free.line);
	  if(strstr(buf, key)) puts(buf);
	  free(buf);
	  ADVANCE(p);
	}
      printf( "***** %s:%lu: history dump end\n", dbg_file_name, dbg_line_number);
    }
}     

void dbg_abort(char *msg)
{
  printf( "+++++ %s: aborting.\n+++++ last call at %s:%lu\n",
	            msg, dbg_file_name, dbg_line_number);
  dbg_mem_stat();
  dbg_heap_dump("");
  if(history_length) dbg_history_dump("");
  //abort();
}

void dbg_zero_stat(void)
{
  memory_cnt = 0;
  malloc_cnt = 0;
  memalign_cnt = 0;
  calloc_cnt = 0;
  realloc_cnt = 0;
  free_cnt = 0;
}
#if 0
static void sigsegv_handler(int signal)
{
  if(signal == SIGSEGV)
      dbg_abort("catched SIGSEGV");
}

void dbg_catch_sigsegv(void)
{
  signal(SIGSEGV, sigsegv_handler);
}
#endif
void dbg_init(int hist_len)
{
  history_length = hist_len;
  if(history_length)
    {
      histp = hist_base = calloc(history_length, HLEN);
      if(hist_base == NULL) die("cannot allocate history buffer");
    }
}

#ifdef WITH_DBG_STRDUP
/* Quick fix to support strdup() and strndup() calls.
   No checking of the pointers passed to these functions is done.
   Counted as m in the statistic.
*/

char *dbg_strndup(const char *str, unsigned long n)
{
  unsigned long len;
  char *buf;
  len = strlen(str);
  if(len > n) len = n;
  buf = dbg_malloc(len + 1);
  if(buf)
    {
      memcpy(buf, str, len);
      buf[len] = 0;
    }
  return buf;
}

char *dbg_strdup(const char *str)
{
  /* imposes a limit on the string length */
  return dbg_strndup(str, ULONG_MAX);
}
#endif

/*
  end
*/
