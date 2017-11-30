/** @file pattern.c
 this file contains the array of configuration functions for routers

 */

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "log.h"
#include "smsd.h"
#include "router.h"
#include "gpul.h"
#include "sms_php.h"
#include "php_sdinfo.h"
#include "pattern.h"

int resolve_template(client_state_t *csp, char *SDid, char *template_path)
{
  sd_info_t SDinfo;
  int ret = 0;
  void *log_handle;
  SMS_PHP_STARTUP_THREAD();

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = getSDinfo(csp->db_ctx, SDid, &SDinfo);
  if (ret)
  {
    GLogERROR(log_handle, " Cannot get sd info");
    goto end;
  }

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  sms_mod_php_set_global_str("template_path", template_path);
  php_store_sdinfo(&SDinfo);

  ret = sms_mod_php_execute_script(SDinfo.sdid, SDinfo.router_path, "do_resolve_template.php");
  if (ret)
  {
    ret = sms_mod_php_execute_script(SDinfo.sdid, "smsd", "do_resolve_template.php");
    if (ret)
    {
      LogWrite(LOG_ERROR, " Script Failed\n");
    }
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  freeSDinfo(&SDinfo);

  return ret;
}

int resolve_template_buffer(client_state_t *csp, char *SDid, char *template_buffer)
{
  sd_info_t SDinfo;
  int ret = 0;
  void *log_handle;
  SMS_PHP_STARTUP_THREAD();

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = getSDinfo(csp->db_ctx, SDid, &SDinfo);
  if (ret)
  {
    GLogERROR(log_handle, " Cannot get sd info");
    goto end;
  }

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  sms_mod_php_set_global_str("template_buffer", template_buffer);
  php_store_sdinfo(&SDinfo);

  ret = sms_mod_php_execute_script(SDinfo.sdid, SDinfo.router_path, "do_resolve_template.php");
  if (ret)
  {
    ret = sms_mod_php_execute_script(SDinfo.sdid, "smsd", "do_resolve_template.php");
    if (ret)
    {
      LogWrite(LOG_ERROR, " Script Failed\n");
    }
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  freeSDinfo(&SDinfo);

  return ret;
}
