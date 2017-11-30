
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "log.h"
#include "gpul.h"
#include "smsd.h"
#include "router.h"
#include "sms_router.h"
#include "update.h"
#include "verb.h"
#include "sms_php.h"
#include "php_sdinfo.h"
#include "php_misc.h"
#include "error_code.h"
#include "generic_router.h"

int genericStaging(client_state_t *csp, sd_info_t *SDinfo)
{
  int ret;
  SMS_PHP_STARTUP_THREAD();

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_staging.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_STAGING, " Script Failed\n");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericStagingMap(client_state_t *csp, sd_info_t *SDinfo)
{
  int ret;
  SMS_PHP_STARTUP_THREAD();

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_stagingmap.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_STAGING, " Script Failed\n");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericInitialProvisioning(client_state_t *csp, sd_info_t *SDinfo, char *ipaddr, char *login, char *passwd, char *adminpasswd)
{
  int ret;
  SMS_PHP_STARTUP_THREAD();

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  sms_mod_php_set_global_str("ipaddr", ipaddr);
  sms_mod_php_set_global_str("login", login);
  sms_mod_php_set_global_str("passwd", passwd);
  sms_mod_php_set_global_str("adminpasswd", adminpasswd);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_provisioning.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_PROVISIONING, " Script Failed\n");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    unlockSD(csp, SDinfo->SD.sd_cli_prefix, SDinfo->SD.sd_seqnum, "genericInitialProvisioning");
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericCheckProvisioning(client_state_t *csp, sd_info_t *SDinfo)
{
  int ret;
  SMS_PHP_STARTUP_THREAD();

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_checkprovisioning.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_CHECKPROVISIONING, " Script Failed\n");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericSnmpTest(client_state_t *csp, char *cmd, char *address, char *community, char *oid, char *event)
{
  int ret;
  SMS_PHP_STARTUP_THREAD();

  if (!*cmd || !*address || !*community || !*oid || (strcmp(cmd, "get") && strcmp(cmd, "walk")))
  {
    LogWriteExt(LOG_ERROR, "", event, " Bad parameters\n");
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    ret = -1;
    goto end;
  }

  sms_mod_php_set_global_str("snmp_cmd", cmd);
  sms_mod_php_set_global_str("ip_address", address);
  sms_mod_php_set_global_str("snmp_community", community);
  sms_mod_php_set_global_str("oid", oid);
  sms_mod_php_set_global_str("sdid", "");
  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);

  ret = sms_mod_php_execute_script("", "mon_generic", "snmp_test.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, "", event, " Script Failed\n");
    sms_send_user_error((long) csp, "", "", ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericGetConfiguration(sd_info_t *SDinfo, char **config, char **running_config)
{
  int ret;
  char *str;
  SMS_PHP_STARTUP_THREAD();

  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_get_sd_conf.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_GETSDCONF, " Script Failed\n");
    goto end;
  }
  str = sms_mod_php_get_global_str(PHP_GLOBAL_SMS_RETURN_BUF);
  if (!str)
  {
    str = "";
  }
  *running_config = *config = strdup(str);

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericGetRunningConf(client_state_t *csp, sd_info_t *SDinfo)
{
  int ret;
  SMS_PHP_STARTUP_THREAD();

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_get_running_conf.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_GETRUNNINGCONF, " Script Failed\n");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericGetConfig(client_state_t *csp, sd_info_t *SDinfo)
{
  int ret;
  SMS_PHP_STARTUP_THREAD();

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_get_config.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_GETCONFIG, " Script Failed\n");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericGetActivityReport(client_state_t *csp, sd_info_t *SDinfo)
{
  int ret;
  SMS_PHP_STARTUP_THREAD();

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_get_activity_report.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_GETACTIVITYREPORT, " Script Failed\n");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericGetBotnetReport(client_state_t *csp, sd_info_t *SDinfo)
{
  int ret;
  SMS_PHP_STARTUP_THREAD();

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_get_botnet_report.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_GETBOTNETREPORT, " Script Failed\n");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericRestoreConf(client_state_t *csp, sd_info_t *SDinfo, char *revision_id, char *msg)
{
  int ret;
  char *config_type;
  SMS_PHP_STARTUP_THREAD();

  config_type = get_router_config_type(SDinfo->SD.man_id, SDinfo->SD.mod_id);

  sms_mod_php_set_global_str(PHP_GLOBAL_SMS_REVISION_ID, revision_id);
  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  sms_mod_php_set_global_str("config_type", config_type);
  sms_mod_php_set_global_str("sms_msg", msg);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_restore_conf.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_RESTORECONF, " Script Failed\n");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    unlockSD(csp, SDinfo->SD.sd_cli_prefix, SDinfo->SD.sd_seqnum, "genericRestoreConf");
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericBackupConf(client_state_t *csp, sd_info_t *SDinfo, char *backup_module, char *msg)
{
  char *config_type;
  int ret;
  SMS_PHP_STARTUP_THREAD();

  config_type = get_router_config_type(SDinfo->SD.man_id, SDinfo->SD.mod_id);

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  sms_mod_php_set_global_str("backup_module", backup_module);
  sms_mod_php_set_global_str("config_type", config_type);
  sms_mod_php_set_global_str("sms_msg", msg);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_backup_conf.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_BACKUPCONF, " Script Failed\n");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericPushConfig(client_state_t *csp, sd_info_t *SDinfo,char *addon, char *ts_ip,char *ts_port, char *configuration)
{
  char *config_type;
  int ret;
  void *log_handle;
  SMS_PHP_STARTUP_THREAD();

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  config_type = get_router_config_type(SDinfo->SD.man_id, SDinfo->SD.mod_id);

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  sms_mod_php_set_global_str("addon", addon);
  sms_mod_php_set_global_str("ts_ip", ts_ip);
  sms_mod_php_set_global_str("ts_port", ts_port);
  sms_mod_php_set_global_str("configuration", configuration);
  sms_mod_php_set_global_str("config_type", config_type);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, "smsd", "do_push_config.php");
  if (ret)
  {
    GLogERROR(log_handle, " Script Failed\n");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericUpdateConf(client_state_t *csp, sd_info_t *SDinfo)
{
  int ret;
  SMS_PHP_STARTUP_THREAD();

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_update_conf.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_UPDATECONF, " Script Failed\n");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    unlockSD(csp, SDinfo->SD.sd_cli_prefix, SDinfo->SD.sd_seqnum, "genericUpdateConf");
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericSmsexecCmd(client_state_t *csp, sd_info_t *SDinfo, struct router_cmd_head *router_cmd_list)
{
  char *csv = NULL;
  router_cmd_t *router_cmd_elt;
  int first = 1;
  int ret;
  SMS_PHP_STARTUP_THREAD();

  // build the comma separated list of device commands
  router_cmd_elt = GPUL_TAILQ_FIRST (router_cmd_list);
  while (router_cmd_elt)
  {
    if (first)
    {
      first = 0;
    }
    else
    {
      csv = gpul_str_append(csv, ",");
    }
    csv = gpul_str_append(csv, router_cmd_elt->cmd);
    router_cmd_elt = GPUL_TAILQ_NEXT(router_cmd_elt, link);
  }

  sms_mod_php_set_global_str("smsexec_list", csv);
  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_smsexec.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_SMSEXEC, " Script Failed\n");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  freez(csv);

  return ret;
}

int genericSmsAddonExecCmd(client_state_t *csp, sd_info_t *SDinfo, char *addon, struct router_cmd_head *router_cmd_list)
{
  char *csv = NULL;
  router_cmd_t *router_cmd_elt;
  int first = 1;
  int ret;
  SMS_PHP_STARTUP_THREAD();

  // build the comma separated list of device commands
  router_cmd_elt = GPUL_TAILQ_FIRST (router_cmd_list);
  while (router_cmd_elt)
  {
    if (first)
    {
      first = 0;
    }
    else
    {
      csv = gpul_str_append(csv, ",");
    }
    csv = gpul_str_append(csv, router_cmd_elt->cmd);
    router_cmd_elt = GPUL_TAILQ_NEXT(router_cmd_elt, link);
  }

  sms_mod_php_set_global_str("addon", addon);
  sms_mod_php_set_global_str("smsexec_list", csv);
  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_sms_addon_exec.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_SMSEXEC, " Script Failed\n");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  freez(csv);

  return ret;
}

int genericUnarchiveConfiguration(client_state_t *csp, sd_info_t *SDinfo, char *archive, char *folder)
{
  int ret;
  void *log_handle;
  SMS_PHP_STARTUP_THREAD();

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  sms_mod_php_set_global_str("archive", archive);
  sms_mod_php_set_global_str("folder", folder);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_unarchive_conf.php");
  if (ret)
  {
    GLogERROR(log_handle, "Script Failed");
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericGetArchiveConfiguration(client_state_t *csp, sd_info_t *SDinfo, char *folder)
{
  int ret;
  void *log_handle;
  SMS_PHP_STARTUP_THREAD();

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  sms_mod_php_set_global_str("folder", folder);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_get_archive_conf.php");
  if (ret)
  {
    GLogERROR(log_handle, "Script Failed");
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericUpdateLicense(client_state_t *csp, sd_info_t * SDinfo)
{
  int ret;
  void *log_handle;
  SMS_PHP_STARTUP_THREAD();

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_update_license.php");
  if (ret)
  {
    GLogERROR(log_handle, "Script Failed");
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericUpdateFirmware(client_state_t *csp, sd_info_t * SDinfo, char *param, char *server_addr)
{
  int ret;
  void *log_handle;
  SMS_PHP_STARTUP_THREAD();

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  sms_mod_php_set_global_str("param1", param);
  sms_mod_php_set_global_str("param2", server_addr);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_update_firmware.php");
  if (ret)
  {
    GLogERROR(log_handle, "Script Failed");
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericExecScript(client_state_t *csp, sd_info_t *SDinfo, char *script, int synchronous)
{
  int ret;
  SMS_PHP_STARTUP_THREAD();

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  sms_mod_php_set_global_str("exec_script_name", script);
  sms_mod_php_set_global_bool("synchronous", synchronous);

  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_exec_script.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_UPDATECONF, " Script Failed\n");
    unlockSD(csp, SDinfo->SD.sd_cli_prefix, SDinfo->SD.sd_seqnum, "genericExecScript");
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

/**
 *
 */
int genericGetReport(sd_info_t * SDinfo, char **reportBuffer, int fullReport, int startDate, int endDate)
{
  int ret;
  char *str;
  SMS_PHP_STARTUP_THREAD();


  sms_mod_php_set_global_bool("fullReport", fullReport);

  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_get_report.php");
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDinfo->sdid, EVT_GETREPORT, " Script Failed\n");
    goto end;
  }
  str = sms_mod_php_get_global_str(PHP_GLOBAL_SMS_RETURN_BUF);
  if (!str)
  {
    str = "";
  }
  *reportBuffer = strdup(str);

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericHaSwap(client_state_t *csp, sd_info_t *SDinfo)
{
  int ret;
  void *log_handle;
  SMS_PHP_STARTUP_THREAD();

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_ha_swap.php");
  if (ret)
  {
    GLogERROR(log_handle, " Script Failed");
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericCheckSerialNumber(sd_info_t *SDinfo, char *ipaddr)
{
  int ret;
  void *log_handle;
  SMS_PHP_STARTUP_THREAD();

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  // Check serial for Netasq only
  if (SDinfo->SD.man_id != NETASQ)
  {
    ret = SMS_OK;
    goto end;
  }

  php_store_sdinfo(SDinfo);
  sms_mod_php_set_global_str("ipaddr", ipaddr);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_check_serial_number.php");
  if (ret)
  {
    GLogERROR(log_handle, " Script Failed");
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int genericCallCommand(client_state_t *csp, sd_info_t *SDinfo, char *command, int apply_conf, char *params, char **result, char **output)
{
  int ret;
  char *str;
  SMS_PHP_STARTUP_THREAD();


  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_sdinfo(SDinfo);
  sms_mod_php_set_global_str("commandName", command);
  sms_mod_php_set_global_str("commandParams", params);
  sms_mod_php_set_global_long("apply_conf", apply_conf);

  ret = sms_mod_php_execute_script(SDinfo->sdid, "smsd", "do_call_command.php");
  str = sms_mod_php_get_global_str(PHP_GLOBAL_SMS_RETURN_BUF);
  if (!str)
  {
    str = "";
  }
  *result = strdup(str);

  str = sms_mod_php_get_global_str(PHP_GLOBAL_SMS_OUTPUT_BUF);
  if (!str)
  {
    str = "";
  }
  *output = strdup(str);

  SMS_PHP_SHUTDOWN_THREAD();

  return ret;

}

int genericSendDataFiles(client_state_t *csp, sd_info_t *SDinfo, char *addon, char *server_addr, char *server_ftp_login, char *server_ftp_passwd)
{
  int ret;
  void *log_handle;
  SMS_PHP_STARTUP_THREAD();

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  sms_mod_php_set_global_str("param1", addon);
  sms_mod_php_set_global_str("param2", server_addr);
  sms_mod_php_set_global_str("param3", server_ftp_login);
  sms_mod_php_set_global_str("param4", server_ftp_passwd);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_send_data_files.php");
  if (ret)
  {
    GLogERROR(log_handle, " Script Failed");
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int generic_sms_cmd(client_state_t *csp, sd_info_t *SDinfo, char *cmd, char *optional_params)
{
  char buf[BUFSIZ];
  char *p;
  int ret;
  void *log_handle;
  SMS_PHP_STARTUP_THREAD();

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sprintf(buf, "do_cmd_%s.php", cmd);
  for (p = buf; *p != '\0'; p++)
  {
    *p = tolower(*p);
  }
  sms_mod_php_set_global_str("cmd", cmd);
  sms_mod_php_set_global_str("optional_params", optional_params);
  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, buf);
  if (ret)
  {
    GLogERROR(log_handle, " Script Failed");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    unlockSD(csp, SDinfo->SD.sd_cli_prefix, SDinfo->SD.sd_seqnum, "generic_sms_cmd");
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int generic_smsd_cmd(client_state_t *csp, char *cmd, char *optional_params)
{
  char buf[BUFSIZ];
  char *p;
  int ret;
  void *log_handle;
  SMS_PHP_STARTUP_THREAD();

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sprintf(buf, "do_cmd_%s.php", cmd);
  for (p = buf; *p != '\0'; p++)
  {
    *p = tolower(*p);
  }
  sms_mod_php_set_global_str("cmd", cmd);
  sms_mod_php_set_global_str("optional_params", optional_params);
  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);

  ret = sms_mod_php_execute_script("debug", "smsd", buf);
  if (ret)
  {
    GLogERROR(log_handle, " Script Failed");
    sms_send_user_error((long) csp, "debug", "", ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int generic_sms_cust_cmd(client_state_t *csp, cust_info_t *cust_info, char *cmd, char *optional_params)
{
  char buf[BUFSIZ];
  char *p;
  int ret;
  void *log_handle;
  SMS_PHP_STARTUP_THREAD();

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sprintf(buf, "do_cust_cmd_%s.php", cmd);
  for (p = buf; *p != '\0'; p++)
  {
    *p = tolower(*p);
  }
  sms_mod_php_set_global_str("cmd", cmd);
  sms_mod_php_set_global_str("optional_params", optional_params);
  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_cust_info(cust_info);

  ret = sms_mod_php_execute_script(cust_info->custid, "smsd", buf);
  if (ret)
  {
    GLogERROR(log_handle, " Script Failed");
    sms_send_user_error((long) csp, cust_info->custid, "", ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int generic_gen_template(client_state_t *csp, sd_info_t *SDinfo, char *revision1, char *revision2, char *addon)
{
  int ret;
  void *log_handle;
  SMS_PHP_STARTUP_THREAD();

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sms_mod_php_set_global_str("revision1", revision1);
  sms_mod_php_set_global_str("revision2", revision2);
  sms_mod_php_set_global_str("addon", addon);
  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp);
  php_store_sdinfo(SDinfo);

  ret = sms_mod_php_execute_script(SDinfo->sdid, SDinfo->router_path, "do_gen_template.php");
  if (ret)
  {
    GLogERROR(log_handle, "Script Failed");
    sms_send_user_error((long) csp, SDinfo->sdid, "", ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}
