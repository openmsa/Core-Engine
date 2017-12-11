/*
 * Copyright UBIqube Solutions 2007
 * File: sms_php.h
 * Creation Date: Thu Aug 23 15:02:41 UTC 2007
 */

#ifndef _SMS_PHP_H_
#define _SMS_PHP_H_

#include <main/php.h>
#include <main/SAPI.h>
#include <main/php_main.h>
#include <main/php_variables.h>
#include <main/php_ini.h>
#include <zend_ini.h>

extern PHP_MINIT_FUNCTION(sms);
extern zend_module_entry sms_php_module_entry;


#ifdef ZTS
#include "TSRM.h"
#endif

/*
 * Global variables (by thread context) shared between scripts and SMS
 */
/* sms_mod_php_xxx_global_ptr */
#define PHP_GLOBAL_SMS_SD_CTX       "sms_sd_ctx"
#define PHP_GLOBAL_SMS_SDINFO       "sms_sd_info"
#define PHP_GLOBAL_SMS_CSP          "sms_csp"
#define PHP_GLOBAL_SMS_REVISION_ID  "revision_id"

/* sms_mod_php_xxx_global_str */
#define PHP_GLOBAL_SMS_RETURN_BUF   "SMS_RETURN_BUF"
#define PHP_GLOBAL_SMS_OUTPUT_BUF   "SMS_OUTPUT_BUF"
#define PHP_GLOBAL_UBI_CONFIG       "UBI_CONFIG"

/*
 * SAPI PHP MODULE
 */

extern int sms_sapi_php_init(void);
extern void sms_sapi_php_shutdown(void);

/*
 * SMS PHP MODULE
 */
#include "sms_db.h"

extern void php_store_sdinfo(sd_info_t *sd_info);
extern void php_store_cust_info(cust_info_t *cust_info);

extern int sms_mod_php_execute_script(char *sd_id, char *module, char *script);

extern int sms_mod_php_set_global_ptr(char *var_name, void *ptr);
extern long sms_mod_php_get_global_ptr(char *var_name);
extern int sms_mod_php_set_global_bool(char *var_name, int val);
extern int sms_mod_php_get_global_long(char *var_name);
extern int sms_mod_php_set_global_long(char *var_name, long val);

extern int sms_mod_php_set_global_str(char *var_name, char *data);
extern char *sms_mod_php_get_global_str(char *var_name);
extern char *sms_mod_php_get_global_mem(char *var_name, int *len);

extern void sms_sapi_php_register_variables(zval *track_vars_array);

extern int set_log_by_id(database_context_t *ctx, fildelog_like_t *Plog);
extern int set_sd_alarm(void *log_handle, char *cli_prefix, long seqnum, int alarm_level, char *log_reference, const char *format, ...);

/* Initialization of new thread,
 * MUST be the first instruction due to the declarations
 */
#define SMS_INIT_TSRM()				\
  THREAD_T thr;					    \
  thr = pthread_self();				\
  ts_resource_ex(0, &thr)

#define SMS_PHP_STARTUP_THREAD()	\
       SMS_INIT_TSRM();				\
  php_request_startup()

/* Called at the end of the thread */
#define SMS_PHP_SHUTDOWN_THREAD()	\
{                                   \
  zend_try {					    \
    php_request_shutdown(NULL);		\
  } zend_end_try();                 \
  ts_free_thread();                 \
}

#endif /* _SMS_PHP_H_ */

