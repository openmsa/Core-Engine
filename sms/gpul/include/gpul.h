/**
    @file
    @version  $Id: gpul.h 128332 2017-11-28 15:18:51Z ydu $
 */

#ifndef _GPUL_H_
#define _GPUL_H_

#ifdef __cplusplus
extern "C"{
#endif

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500     /* Or: #define _BSD_SOURCE */
#endif

#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define LINE_SZ     4096


/* ******************/
/*     HDF5         */
/* ******************/
#define LOG_MESG_SZ 4096
#define LOG_MESG_OFFSET  0

#define LOG_TIME_SZ 19    // 2008/08/26 16:17:39
#define LOG_TIME_OFFSET (LOG_MESG_OFFSET + LOG_MESG_SZ + 1)

#define LOG_PFLID_SZ 8
#define LOG_PFLID_OFFSET (LOG_TIME_OFFSET + LOG_TIME_SZ + 1)

#define LOG_LEVEL_SZ 1
#define LOG_LEVEL_OFFSET (LOG_PFLID_OFFSET + LOG_PFLID_SZ + 1)

#define LOG_REFERENCE_SZ 100
#define LOG_REFERENCE_OFFSET (LOG_LEVEL_OFFSET + LOG_LEVEL_SZ + 1)

#define LOG_TYPE_SZ 2
#define LOG_TYPE_OFFSET (LOG_REFERENCE_OFFSET + LOG_REFERENCE_SZ + 1)

#define LOG_SIGNATURE_SZ 100
#define LOG_SIGNATURE_OFFSET (LOG_TYPE_OFFSET + LOG_TYPE_SZ + 1)

#define LOG_SEVERITY_SZ 8
#define LOG_SEVERITY_OFFSET (LOG_SIGNATURE_OFFSET + LOG_SIGNATURE_SZ + 1)

#define LINESIZE (LOG_SEVERITY_OFFSET + LOG_SEVERITY_SZ + 1)

/* *********************/
/*    Initialization   */
/* *********************/

/* gpul initialization by thread. */
extern int gpul_init_thread(const char *conf_file, int conf_mode, void **conf_handle, char *log_file, void **log_handle);

/* gpul shutdown by thread. */
extern void gpul_shutdown_thread (void *log_handle, void *conf_handle);

/* gpul initialize signals handling. */
extern int gpul_init_signals (void *log_handle, void (*gpul_signal_handler)(int, siginfo_t *, void *));


/* ******************/
/*    Hash Tables   */
/* ******************/
#define GPUL_HASH_TYPE_STR  0  /**< String key based hash table */
#define GPUL_HASH_TYPE_BUF  1  /**< Memory buffer key based hash table */

typedef void (*tf_hash_free)(void*);  /**< Function to call for releasing the user data associated with hash table */

/* Create a new hash table. */
extern void *gpul_hash_create    (void *log_handle, int type);

/* Add an element with a string key to a hash table. */
extern int   gpul_hash_add_str   (void *log_handle, void *hash, const char *key, void *data);

/* Remove an element with a string key from a hash table. */
extern void *gpul_hash_rem_str   (void *log_handle, void *hash, char *key);

/* Remove an element with a string key from a hash table. */
extern void *gpul_hash_rem_str_elem (void *log_handle, void *hash, char *key, void *data);

/* Get the user data associated with a string key. */
extern void *gpul_hash_get_str   (void *log_handle, void *hash, char *key);

/*Add an element with a memory buffer key to a hash table. */
extern int   gpul_hash_add_buf   (void *log_handle, void *hash, char *key, size_t sz, void *data);

/* Remove an element with a memory buffer key from a hash table. */
extern void *gpul_hash_rem_buf   (void *log_handle, void *hash, char *key, size_t sz);

/* Get the user data associated with a memory buffer key. */
extern void *gpul_hash_get_buf   (void *log_handle, void *hash, char *key, size_t sz);

/* Release a hash table. */
extern void  gpul_hash_release   (void *log_handle, void *hash, tf_hash_free hash_free);

/* Clean a hash table. */
extern void gpul_hash_clean(void *log_handle, void *hash, tf_hash_free hash_free);

/* Get the first user data of a hash table. */
extern void *gpul_hash_get_first (void *log_handle, void *hash);

/* Get the next user data of a hash table. */
extern void *gpul_hash_get_next  (void *log_handle, void *hash);

/* Get the first key of the hash table */
extern void *gpul_hash_get_first_key (void *log_handle, void *hash);

/* Get the next key of the hash table */
extern void *gpul_hash_get_next_key (void *log_handle, void *hash);

/* Get the number of elements in a hash table */
extern int gpul_hash_get_count (void *log_handle, void *hash);



/* *******************************/
/*    Configuration Management   */
/* *******************************/
/**
 * @addtogroup conf
 * @{
 */

/* parameter file read */
#define GPUL_CONF_RMODE_STD    0x01  /**< if handle already openned return it */
#define GPUL_CONF_RMODE_REREAD    0x02  /**< reread the parameters from file and release the previous ones */
#define GPUL_CONF_RMODE_CREAT    0x04  /**< Create the file if doesn't exist */
#define GPUL_CONF_RMODE_PRIVATE    0x08  /**< Create a private instance of the parameters (won't be shared)
  overwrite #GPUL_CONF_RMODE_STD and #GPUL_CONF_RMODE_REREAD modes */

/* parameter file type */
#define GPUL_CONF_TYPE_SYNTAX    0xFF  /**< parameters syntax mask */
#define GPUL_CONF_TYPE_INI    0x01  /**< blank separated ini like (default UBIqube) */
#define GPUL_CONF_TYPE_INI2    0x02  /**< '=' separated win32 ini like (no space around '=') */

/* configuration initialization */
extern void gpul_conf_init (void);

/* configuration shutdown */
extern void gpul_conf_shutdown (void *log_handle);

/* read a configuration file */
extern void *gpul_conf_read_file (void *log_handle, char *file, int mode, int type);

/* read a configuration buffer */
extern void *gpul_conf_read_buf (void *log_handle, char *buffer, int type);

/* get string parameter value */
extern char *gpul_conf_get_str (void *log_handle, void *conf_handle, char *section_name, char *param_name, char *deflt);

/* set string parameter value */
extern int gpul_conf_set_str (void *log_handle, void *conf_handle, char *section_name, char *param_name, char *val);

/* get num parameter value */
extern int gpul_conf_get_num (void *log_handle, void *conf_handle, char *section_name, char *param_name, int deflt);

/* set num parameter value */
extern int gpul_conf_set_num (void *log_handle, void *conf_handle, char *section_name, char *param_name, int val);

/* get address parameter value */
extern struct sockaddr *gpul_conf_get_addr (void *log_handle, void *conf_handle, char *section_name, char *param_addr_name, char *param_port_name);

/* set address parameter value */
extern int gpul_conf_set_addr (void *log_handle, void *conf_handle, char *section_name, char *param_addr_name, char *param_port_name, struct sockaddr *val);

/* Remove a parameter */
extern void gpul_conf_remove (void *log_handle, void *conf_handle, char *section_name, char *param_name);

/* clean all the parameters of a set */
extern void gpul_conf_clean (void *log_handle, void *conf_handle);

/* clean the parms and release the handle without updating the file */
extern void gpul_conf_release_handle (void *log_handle, void *conf_handle);

/* Count the number of section elements of a list. */
extern int gpul_conf_get_elt_nb (void *log_handle, void *conf_handle, char *section_name);

/* Seek the first section element of a list. */
extern int gpul_conf_list_seek_first (void *log_handle, void *conf_handle, char *section_name);

/* Go to the next section element in the current list. */
extern int gpul_conf_list_seek_next (void *log_handle, void *conf_handle, char *section_name);

/* Remove all the section elements of a list. */
extern int gpul_conf_list_clean (void *log_handle, void *conf_handle, char *section_name);

/* Get The current element name. */
extern char *gpul_conf_list_get_cur_elt_name (void *log_handle, void *conf_handle, char *section_name);

/* Get string parameter value. */
extern char *gpul_conf_get_cur_str (void *log_handle, void *conf_handle, char *section_name, char *param_name, char *deflt);

/* Set string parameter value. */
extern int gpul_conf_set_cur_str (void *log_handle, void *conf_handle, char *section_name, char *param_name, char *val);

/* Get num parameter value. */
extern int gpul_conf_get_cur_num (void *log_handle, void *conf_handle, char *section_name, char *param_name, int deflt);

/* Set num parameter value. */
extern int gpul_conf_set_cur_num (void *log_handle, void *conf_handle, char *section_name, char *param_name, int val);

/**
 * @}
 */


/* *************************/
/*    Address Management   */
/* *************************/

/* Check if addr is a valid address. */
extern int gpul_is_valid_addr (char *address);

/* Convert a text to sockaddr. */
extern int gpul_text2sockaddr (void *log_handle, char *address, char *port, struct sockaddr *sa);

/* Convert a sockaddr to text. */
extern int gpul_sockaddr2text (void *log_handle, struct sockaddr *sa, char **address, char **port, int zflag);

/* Get address length. */
extern int gpul_sockaddr_len (void *log_handle, struct sockaddr *sa);

/* Get address pointer. */
extern unsigned char *gpul_sockaddr_data (void *log_handle, struct sockaddr *sa);

/* Get port. */
extern unsigned short gpul_sockaddr_port (void *log_handle, struct sockaddr *sa);

/* Convert network address to text. */
extern void gpul_util_ntoa (void *log_handle, char **buf, int af, unsigned char *addr);

/* compute addr1 - addr2 to get the number of addresses in a range. */
extern int gpul_addr_sub (void *log_handle, struct sockaddr *addr1, struct sockaddr *addr2);

/* check if address is in network */
extern int gpul_is_in_network(void *log_handle, char *addr, char *net, char *mask);

/* check if networks are equals */
extern int gpul_equal_networks(void *log_handle, char *net1, char *mask1, char *net2, char *mask2);

/* compute the network mask prefix (i.e. 255.255.255.0 -> 24) */
extern int gpul_netmask_prefix(char *netmask);

/* compute network from address/mask */
extern void gpul_get_network(char *addr, char *mask, char *network);

/* compute network from range ip addresses */
extern void gpul_find_network_from_range(char * ipbegin , char * ipend, char * dstip, char * dstmask);


/* *********************/
/*    Log Management   */
/* *********************/

/* Log initialization. */
extern void *gpul_log_init (const char *module, const char *filename);

extern int gpul_log_reinit(void *dummy);

extern void *gpul_log_init_task(void *vref_log_handle);

/* Clean the parms and release the handle. */
extern void gpul_log_release_handle (void *log_handle);

/* Set the module name. */
extern int gpul_log_set_module (void *log_handle, char *module);

/* Set the SD identifier. */
extern int gpul_log_set_sd_id (void *log_handle, char *sd_id);

/* Set the current event. */
extern int gpul_log_set_event (void *log_handle, char *event);

/* Set the debug log level. */
extern int gpul_log_set_level (void *log_handle, int level);

/* Get the debug log level. */
extern int gpul_log_get_level (void *log_handle);

/* Log informational message. */
extern void GLogINFO (void *log_handle, const char *format, ...);

/* Log error message. */
extern void GLogERROR (void *log_handle, const char *format, ...);

extern void GLogERRNO(void *log_handle, const char *format, ...);

/* Check whether the level can log a debug message. */
extern int gpul_can_log_debug (void *log_handle, int level);

/* Log a debug message. */
extern void GLogDEBUG (void *log_handle, int level, const char *format, ...);

/* Dump in hexa of a buffer. */
extern void gpul_log_debug_buf (void *log_handle, int level, char *buf, int len, const char *header, ...);

/* Dump in hexa and ascii of a buffer. */
extern void gpul_log_debug_buf_ascii (void *log_handle, int level, char *buf, int len, const char *header, ...);


/* ***********/
/*    Misc   */
/* ***********/
/**
 * @addtogroup util
 * @{
 */

/** Free if not zero. */
#define freez(p) do {        \
  if (p) {          \
    free ((void *)p);          \
    p = 0;          \
  }            \
} while (0)


/** Number of microseconds in one second */
#define GPUL_MICRO_PER_SEC 1000000

/** Number of milliseconds in one second */
#define GPUL_MILLI_PER_SEC 1000

/** Number of microseconds in one millisecond */
#define GPUL_MICRO_PER_MILLI 1000

/** Number of seconds in one minute */
#define GPUL_SEC_PER_MIN  60

/** Number of seconds in one hour */
#define GPUL_SEC_PER_HOUR 3600

/** Number of seconds in one day */
#define GPUL_SEC_PER_DAY  86400

#define GPUL_KILO 1024
#define GPUL_MEGA 1024*GPUL_KILO
#define GPUL_GIGA 1024*GPUL_MEGA


/** Timers operations on struct timeval (sys/time.h)
    Used with the gettimeofday() routine
    Warning, no control if:
         tv_usec or tv_sec < 0
         tv_usec > MICRO_PER_SEC
         t1 > t2 in substraction
 */

/** tr = t1 + t2 */
#define GPUL_TIMERADD(t1, t2, tr)    \
  {            \
    (tr).tv_sec = (t1).tv_sec + (t2).tv_sec;  \
    (tr).tv_usec = (t1).tv_usec + (t2).tv_usec;  \
    if((tr).tv_usec >= GPUL_MICRO_PER_SEC)  \
    {            \
      (tr).tv_usec -= GPUL_MICRO_PER_SEC;  \
      (tr).tv_sec++;        \
    }            \
  }

/** tr = t1 - t2 */
#define GPUL_TIMERSUB(t1, t2, tr)          \
  {                  \
    if((t2).tv_usec > (t1).tv_usec)          \
    {                  \
      (tr).tv_sec = ((t1).tv_sec - 1) - (t2).tv_sec;      \
      (tr).tv_usec = (t1).tv_usec + GPUL_MICRO_PER_SEC - (t2).tv_usec;  \
    }                  \
    else                  \
    {                  \
      (tr).tv_sec = (t1).tv_sec - (t2).tv_sec;        \
      (tr).tv_usec = (t1).tv_usec - (t2).tv_usec;      \
    }                  \
  }

/** t1 > t2 ret = 1
 * t1 = t2 ret = 0
 * t1 < t2 ret = -1
 */
#define GPUL_TIMERCMP(t1, t2, ret) \
{ \
  if ((t1).tv_sec > (t2).tv_sec) \
    { \
      ret = 1; \
    } \
  else if ((t1).tv_sec < (t2).tv_sec) \
    { \
      ret = -1; \
    } \
  else \
    { \
      if ((t1).tv_usec > (t2).tv_usec) \
        { \
          ret = 1; \
        } \
      else if ((t1).tv_usec < (t2).tv_usec) \
        { \
          ret = -1; \
        } \
      else \
        { \
          ret = 0; \
        } \
    } \
}
extern char *gpul_format_time(char *buf, long sec);
extern char *gpul_format_num(char *buf, long num, int isDecimal);

/* extract bytes from string s1 between bytes strings s2 and s3 found in s1. */
extern char *gpul_bytextract(char *s1, char *s2, char *s3, int sz1, int sz2, int sz3);
extern char *gpul_string_extract (char *s1, char *s2, char *s3);

/* search bytes string s2 in bytes string s1 */
extern int gpul_bytesearch(char *s1, char *s2, int sz1, int sz2);

/* Remove the blank characters at the beginning and at the end of the string. */
extern char *gpul_str_strip(char *str);
/* idem but work directly in the buffer without copy */
extern char *gpul_str_strip_fast(char *str);

/* Append string at the end of a string. */
extern char *gpul_str_append(char *str, const char *append);
/* Append string at the end of a string and free the source. */
extern char *gpul_str_append_nfree(char *str, char *append);
/* Insert string at the begining of a string. */
extern char *gpul_str_insert(char *insert, char *str);
/* Replace each occurence of serch by replace in subject string. */
extern char *gpul_str_replace(char *search,char *replace, char *subject);

/* Append buffer at the end of a buffer. */
extern char *gpul_mem_append(char *buf, int buf_len, char *append, int append_len);

/* replace " \ \n \r ... by 2 characters \" or \\ or \n ... */
extern char *gpul_backslashify (const char *s1);
extern char *gpul_backslashify_quote (char *result, char *s1);
extern char *gpul_backslashify_doublequote (char *result, char *s1);
extern char *gpul_toupper(char *s);

/* Generate random numer between 0 and max */
extern int gpul_rand(int max);

/* decrypt passwords from config */
extern int gpul_decrypt(const char *instr, char *outstr, int maxlen);

extern void gpul_get_date(struct tm *tm, int utc);

extern long gpul_hex_to_bin(char *str, unsigned char *hex);

/**
 * @}
 */

/* **************/
/*    Threads   */
/* **************/

#ifdef _THREAD_SAFE

#include <pthread.h>

/**
 * @addtogroup thread
 * @{
 */


/** Declare a new thread mutex.
 *   @note The mutex must be initialized by #GPUL_MUTEX_INIT before any use.
 */
#define GPUL_MUTEX_DECLARE(mutex)    \
  pthread_mutex_t mutex

/** Declare a new thread mutex initialized
 */
#define GPUL_MUTEX_DECLARE_INIT(mutex)    \
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER

/** Initialize a thread mutex in blocking mode
 */
#define GPUL_MUTEX_INIT(mutex)      \
  pthread_mutex_init (&(mutex), NULL)

#if 1
/** Lock a thread mutex
 */
#define GPUL_MUTEX_LOCK(mutex)      \
  pthread_mutex_lock (&(mutex))

/** Lock a thread mutex
 */
#define GPUL_MUTEX_SPINLOCK(mutex)      \
  while (pthread_mutex_trylock(&(mutex)) usleep(100)

#define GPUL_MUTEX_TRYLOCK(mutex)      \
  pthread_mutex_trylock(&(mutex))

/** Unlock a thread mutex
 */
#define GPUL_MUTEX_UNLOCK(mutex)    \
  pthread_mutex_unlock (&(mutex))


#else
// DEBUG

/** Lock a thread mutex
 */
#define GPUL_MUTEX_LOCK(mutex)      { DbgWrite(DBG_INFO, " %s:%d GPUL_MUTEX_LOCK_0 %p owner 0x%x thread 0x%x\n", __FILE__, __LINE__, &mutex, mutex.__m_owner, pthread_self());  pthread_mutex_lock (&(mutex)); DbgWrite(DBG_INFO, " GPUL_MUTEX_LOCK_1 0x%p owner 0x%x\n", &mutex, mutex.__m_owner);}

/** Unlock a thread mutex
 */
#define GPUL_MUTEX_UNLOCK(mutex)    { DbgWrite(DBG_INFO, " %s:%d GPUL_MUTEX_UNLOCK %p owner 0x%x thread 0x%x\n", __FILE__, __LINE__, &mutex, mutex.__m_owner);  pthread_mutex_unlock (&(mutex));}

#endif

/** Declare a new condition
 *   @note The condition must be initialized by #GPUL_COND_INIT before any use.
 */
#define GPUL_COND_DECLARE(cond)    \
  pthread_cond_t cond

/** Declare a new condition initialized
 */
#define GPUL_COND_DECLARE_INIT(cond)    \
  pthread_cond_t cond = PTHREAD_COND_INITIALIZER

/** Initialize a condition
 */
#define GPUL_COND_INIT(cond)    \
  pthread_cond_init(&cond, NULL)

/** Wait on condition, thread go asleep
 */
#define GPUL_COND_WAIT(cond, mutex)    \
  pthread_cond_wait(&(cond), &(mutex))

/** Wake-up threads waiting on condition
 */
#define GPUL_COND_SIGNAL(cond)    \
  pthread_cond_broadcast(&(cond))

#else /* (not) _THREAD_SAFE */

#define GPUL_MUTEX_DECLARE(mutex)
#define GPUL_MUTEX_DECLARE_INIT(mutex)
#define GPUL_MUTEX_INIT(mutex)
#define GPUL_MUTEX_LOCK(mutex)
#define GPUL_MUTEX_UNLOCK(mutex)
#define GPUL_COND_DECLARE(cond)
#define GPUL_COND_DECLARE_INIT(cond)
#define GPUL_COND_WAIT(cond, mutex)
#define GPUL_COND_SIGNAL(cond)

#endif /* (not) _THREAD_SAFE */


/**
 * @}
 */


/* *****************/
/*   SEARCH TREE   */
/* *****************/
/**
 * @addtogroup searchtree
 * @{
 */

extern void *st_create_search_tree(void);
extern void st_release_search_tree(void *search_tree, void (*free_func)(void *));
extern void *st_add_to_dictionary(void *search_tree, char *word, void *user_data);
extern void st_remove_from_dictionary(void *search_tree, char *word, int free_node);
extern void *st_search(void *search_tree, char *message, int full_match);

/**
 * @}
 */

/* **************/
/*   SCHEDULER  */
/* **************/
/**
 * @addtogroup scheduler
 * @{
 */
#define GPUL_SCHED_ABSOLUTE 0
#define GPUL_SCHED_RELATIVE 1

typedef struct gpul_sched_stats {
    int instant_running_tasks;
    int instant_tasks;
    int avg_running_tasks;
    int avg_tasks;
} gpul_sched_stats_t;

typedef int (*task_exec)(void*);  /**< Function to call to execute a scheduler task */
typedef void (*user_data_free)(void*); /**< Function to call to free user_data */

extern void *gpul_sched_init(int heart_beat, int num_lo_prio_threads, int num_hi_prio_threads, int max_tasks, int thread_stack_size, const char *module, const char *log_file);
extern void gpul_sched_shutdown(void *handle);
extern void *gpul_sched_new_task(void *handle, task_exec function, void *user_data, user_data_free user_free, char *task_name, long start_delay, long period, int schedule_mode);
extern void *gpul_sched_new_prio_task(void *handle, task_exec function, void *user_data, user_data_free user_free, char *task_name, long start_delay, long period, int schedule_mode);
extern void gpul_sched_set_task_name(void *handle, void *task_handle, char *name);
extern void gpul_sched_kill_task(void *handle, void *task_handle);
extern void gpul_sched_run_task(void *handle, void *task_handle);
extern void gpul_sched_register_task_cond(void *handle, pthread_cond_t *cond);
extern char *gpul_sched_stats(void *handle);
extern int gpul_sched_upd_stats(void *handle, gpul_sched_stats_t *stats);
extern int gpul_sched_get_nb_tasks(void *handle);
extern void *gpul_sched_get_my_task_handle(void *handle);
extern void *gpul_sched_get_my_log_handle(void *handle);
extern long gpul_sched_milliseconds_to_date(int hour, int min, int sec);
extern pthread_t gpul_sched_get_my_threadid(void *handle);

#define GPUL_SCHED_REAL_START_TIME  0
#define GPUL_SCHED_ASKED_START_TIME 1
extern struct timeval *gpul_sched_get_my_task_start_time(void *handle, int start_time_type);

/**
 * @}
 */


/* *************************/
/*  JSON MESSAGE FORMATING */
/* *************************/

extern char *gpul_json_add_value(char *message, const char *name, const char *value);
extern char *gpul_json_add_array(char *message, const char *name, const char *array);
extern char *gpul_json_array_add_row(char *array, const char *row);
extern char *gpul_json_close_message(char *message);

#define GPUL_JSON_STATUS         "sms_status"
#define GPUL_JSON_STATUS_OK      "OK"
#define GPUL_JSON_STATUS_FAIL    "FAIL"
#define GPUL_JSON_MESSAGE        "sms_message"
#define GPUL_JSON_CODE           "sms_code"
#define GPUL_JSON_RESULT         "sms_result"

/*
 * Copyright (c) 1991, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *  This product includes software developed by the University of
 *  California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *  @(#)queue.h  8.5 (Berkeley) 8/20/94
 */

/*
 * Tail queue definitions.
 */

/** Head type definition
    @ingroup  tailq
 */
#define GPUL_TAILQ_HEAD(name, type)     \
struct name {                           \
    type *tqh_first;                    \
    type **tqh_last;                    \
}

/** Entry in structure
    @ingroup tailq */
#define GPUL_TAILQ_ENTRY(type)          \
struct {                                \
    type *tqe_next;                     \
    type **tqe_prev;                    \
}

/** First element of a TAILQ
    @ingroup tailq */
#define  GPUL_TAILQ_FIRST(head)    ((head)->tqh_first)
/** Next element of a TAILQ
    @ingroup tailq */
#define  GPUL_TAILQ_NEXT(elm, field)  ((elm)->field.tqe_next)
/** End of a TAILQ
    @ingroup tailq */
#define  GPUL_TAILQ_END(head)    NULL
/** Last element of a TAILQ
    @ingroup tailq */
#define GPUL_TAILQ_LAST(head, headname)         \
  (*(((struct headname *)((head)->tqh_last))->tqh_last))
/** Previous element of a TAILQ
    @ingroup tailq */
#define GPUL_TAILQ_PREV(elm, headname, field)         \
  (*(((struct headname *)((elm)->field.tqe_prev))->tqh_last))

/*
 * Tail queue functions.
 */
/** Head initialization (Mandatory)
    @ingroup tailq */
#define  GPUL_TAILQ_INIT(head) do {             \
  (head)->tqh_first = NULL;                     \
  (head)->tqh_last = &(head)->tqh_first;        \
} while (0)

/** Entry initialization (optionnal if inserted)
    @ingroup tailq */
#define GPUL_TAILQ_INIT_ENTRY(elm, field) do {        \
  (elm)->field.tqe_next = 0;                          \
  (elm)->field.tqe_prev = 0;                          \
} while (0)

/** Insert an element at the head
    @ingroup tailq */
#define GPUL_TAILQ_INSERT_HEAD(head, elm, field) do {               \
  if (((elm)->field.tqe_next = (head)->tqh_first) != NULL)          \
    (head)->tqh_first->field.tqe_prev = &(elm)->field.tqe_next;     \
  else                                                              \
    (head)->tqh_last = &(elm)->field.tqe_next;                      \
  (head)->tqh_first = (elm);                                        \
  (elm)->field.tqe_prev = &(head)->tqh_first;                       \
} while (0)

/** Insert an element at the tail
    @ingroup tailq */
#define GPUL_TAILQ_INSERT_TAIL(head, elm, field) do {      \
  (elm)->field.tqe_next = NULL;                            \
  (elm)->field.tqe_prev = (head)->tqh_last;                \
  *(head)->tqh_last = (elm);                               \
  (head)->tqh_last = &(elm)->field.tqe_next;               \
} while (0)

/** Insert an element after another element
    @ingroup tailq */
#define GPUL_TAILQ_INSERT_AFTER(head, listelm, elm, field) do {     \
  if (((elm)->field.tqe_next = (listelm)->field.tqe_next) != NULL)  \
    (elm)->field.tqe_next->field.tqe_prev = &(elm)->field.tqe_next; \
  else                                                              \
    (head)->tqh_last = &(elm)->field.tqe_next;                      \
  (listelm)->field.tqe_next = (elm);                                \
  (elm)->field.tqe_prev = &(listelm)->field.tqe_next;               \
} while (0)

/** Insert an element before another element
    @ingroup tailq */
#define  GPUL_TAILQ_INSERT_BEFORE(listelm, elm, field) do {    \
  (elm)->field.tqe_prev = (listelm)->field.tqe_prev;           \
  (elm)->field.tqe_next = (listelm);                           \
  *(listelm)->field.tqe_prev = (elm);                          \
  (listelm)->field.tqe_prev = &(elm)->field.tqe_next;          \
} while (0)

/** Remove an element
    @ingroup tailq */
#define GPUL_TAILQ_REMOVE(head, elm, field) do {                    \
  if (((elm)->field.tqe_next) != NULL)                              \
    (elm)->field.tqe_next->field.tqe_prev = (elm)->field.tqe_prev;  \
  else                                                              \
    (head)->tqh_last = (elm)->field.tqe_prev;                       \
  *(elm)->field.tqe_prev = (elm)->field.tqe_next;                   \
} while (0)


/* **********************/
/*    File Management   */
/* **********************/

/* Take a read lock on a file. */
extern int gpul_file_freadlock (FILE *fp);

/* Take a write lock on a file. */
extern int gpul_file_fwritelock (FILE *fp);

/* Release a lock on a file. */
extern int gpul_file_funlock (FILE *fp);

/* Remove recursively a directory and all its content */
extern char *gpul_file_rmdir (char *dirname);

/* Copy file */
extern int gpul_file_copy (char *source, char *dest);

/* find files */

typedef struct gpul_file_entry
{
  char name[GPUL_KILO];
  GPUL_TAILQ_ENTRY (struct gpul_file_entry) link;
} gpul_file_entry_t;

GPUL_TAILQ_HEAD(gpul_file_entry_list_head, gpul_file_entry_t);
extern int gpul_file_find(char *search_path, char *search_name, struct gpul_file_entry_list_head *file_list);

#ifdef __cplusplus
}
#endif

#endif /* _GPUL_H_ */
