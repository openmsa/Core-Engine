/** @file router.c
 this file contains the array of configuration functions for routers
 @ingroup  smsd
 */

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <openssl/sha.h>

#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <sys/socketvar.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <strings.h>

#include "log.h"
#include "net.h"
#include "gpul.h"
#include "smsd.h"
#include "sms_db.h"
#include "router.h"
#include "verb.h"
#include "update.h"
#include "error_code.h"
#include "sms_php.h"
#include "sms_router.h"
#include "php_sdinfo.h"
#include "php_misc.h"
#include "generic_router.h"

static int save_conf_task(void *vSD);

int routerGetIPaddress(sd_like_t *SD, sd_interface_t *sd_ext_interface, char *ip, char *event)
{

  if (*SD->sd_ip_config != '\0')
  {
    /* there is a dedicated address to use to connect this SD */
    sprintf(ip, "%s", SD->sd_ip_config);
  }
  else if (sd_ext_interface)
  {
    sprintf(ip, "%s", sd_ext_interface->interface.int_ip_addr);
  }
  else
  {
    *ip = '\0';
  }

  if ((*ip == '\0') || (strcmp(ip, "0.0.0.0") == 0))
  {
    LogWriteExt2(LOG_WARNING, SD->sd_cli_prefix, SD->sd_seqnum, event, " SD has no IP address.\n");
    return ERR_SD_NOIP;
  }

  return 0;
}

int sdGetAsset(client_state_t *csp, char *SDid, char *result, char *event)
{
  sd_asset_like_t sd_asset;
  int ret;

  memset(&sd_asset, 0, sizeof(sd_asset_like_t));
  strncpy(sd_asset.sd_cli_prefix, SDid, sizeof(sd_asset.sd_cli_prefix) - 1);
  sd_asset.sd_seqnum = atol(SDid + sizeof(sd_asset.sd_cli_prefix) - 1);

  ret = HA_SMSSQL_GetAsset(&(csp->db_ctx->sql_ctx), &sd_asset);
  if (ret)
  {
    if (ret == ERR_DB_FAILED)
    {
      LogWriteExt(LOG_ERROR, SDid, event, " getAsset : Cannot get SD FAILED\n");
    }
    else if (ret == ERR_DB_NOTFOUND)
    {
      LogWriteExt(LOG_SECURITY, SDid, event, " getAsset : SD NOT FOUND in the database\n");
    }
    return ret;
  }

  sprintf(
      result,
      "%s%lu\n\
      model          [%s]\n\
      firmware       [%s]\n\
      serial number  [%s]\n\
      license        [%s]\n\
      cpu            [%s]\n\
      memory         [%s]\n\
      ips version    [%s]\n\
      ips_expiration [%s]\n\
      av_version     [%s]\n\
      av_expiration  [%s]\n\
      as_version     [%s]\n\
      as_expiration  [%s]\n\
      url_version    [%s]\n\
      url_expiration [%s]\n",
      sd_asset.sd_cli_prefix, sd_asset.sd_seqnum, sd_asset.sd_hardware_model, sd_asset.sd_firmware, sd_asset.sd_serial_number, sd_asset.sd_hardware_license, sd_asset.sd_cpu, sd_asset.sd_memory, sd_asset.sd_ips_version,
      sd_asset.sd_ips_expiration, sd_asset.sd_av_version, sd_asset.sd_av_expiration, sd_asset.sd_as_version, sd_asset.sd_as_expiration, sd_asset.sd_url_version, sd_asset.sd_url_expiration);

  return 0;
}

/**
 * force la relecture sur le routeur de l'asset
 */
int sdForceAsset(client_state_t *csp, char *SDid)
{
  char sd_cli_prefix[CLI_PREFIX_LEN + 1];
  long sd_seqnum;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  strncpy(sd_cli_prefix, SDid, sizeof(sd_cli_prefix) - 1);
  sd_cli_prefix[CLI_PREFIX_LEN] = '\0';
  sd_seqnum = atol(SDid + sizeof(sd_cli_prefix) - 1);

  /* get a new BD ctx */
  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
  }

  ret = HA_SMSSQL_SetSDAssetUpdate(&(csp->db_ctx->sql_ctx), sd_cli_prefix, sd_seqnum, 1);
  if (ret)
  {
    if (ret == ERR_DB_FAILED)
    {
      GLogERROR(log_handle, " forceAsset : Cannot update SD FAILED");
    }
    else if (ret == ERR_DB_NOTFOUND)
    {
      GLogERROR(log_handle, " forceAsset : SD NOT FOUND in the database");
    }
    else
    {
      GLogERROR(log_handle, " forceAsset : FAILED");
    }
  }

  return ret;
}

int sdSetConfigMode(client_state_t *csp, char *SDid, int cfg_mode, char *result, char *event)
{
  char sd_cli_prefix[CLI_PREFIX_LEN + 1];
  long sd_seqnum;
  int ret;

  *result = '\0';

  strncpy(sd_cli_prefix, SDid, sizeof(sd_cli_prefix) - 1);
  sd_cli_prefix[CLI_PREFIX_LEN] = '\0';
  sd_seqnum = atol(SDid + sizeof(sd_cli_prefix) - 1);

  ret = HA_SMSSQL_SetSDConfigStep(&(csp->db_ctx->sql_ctx), sd_cli_prefix, sd_seqnum, cfg_mode);
  if (ret)
  {
    if (ret == ERR_DB_FAILED)
    {
      LogWriteExt(LOG_ERROR, SDid, event, " SetSDConfigStep : Cannot get SD FAILED\n");
    }
    else if (ret == ERR_DB_NOTFOUND)
    {
      LogWriteExt(LOG_SECURITY, SDid, event, " SetSDConfigStep : SD %s%lu NOT FOUND in the database\n", sd_cli_prefix, sd_seqnum);
    }
    return ret;
  }

  return 0;
}

int getSDinfo(database_context_t *sms_db_ctx, char *SDid, sd_info_t *sd_info)
{
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = get_sd_info(sms_db_ctx, SDid, sd_info, prog, 1);

  if (ret)
  {
    GLogERROR(log_handle, " Cannot get sdinfo for SD");
    return ret;
  }

  return 0;
}

void freeSDinfo(sd_info_t *sd_info)
{
  free_sd_info(sd_info);
}

int getRunningConf(client_state_t *csp, char *SDid, char *event)
{
  sd_info_t sd_info;
  int ret;

  ret = getSDinfo(csp->db_ctx, SDid, &sd_info);
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDid, event, " Cannot get sd info\n");
    sms_send_user_error((long) csp, SDid, "", ret);
    return ret;
  }

  ret = genericGetRunningConf(csp, &sd_info);
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDid, event, " getRunningConf FAILED\n");
    freeSDinfo(&sd_info);
    return ret;
  }

  freeSDinfo(&sd_info);
  return ret;
}

int getConfig(client_state_t *csp, char *SDid, char *event)
{
  sd_info_t sd_info;
  int ret;

  ret = getSDinfo(csp->db_ctx, SDid, &sd_info);
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDid, event, " Cannot get sd info\n");
    sms_send_user_error((long) csp, SDid, "", ret);
    return ret;
  }

  ret = genericGetConfig(csp, &sd_info);
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDid, event, " getConfig FAILED\n");
    freeSDinfo(&sd_info);
    return ret;
  }

  freeSDinfo(&sd_info);
  return ret;
}

int getActivityReport(client_state_t *csp, char *SDid, char *event)
{
  sd_info_t sd_info;
  int ret;

  ret = getSDinfo(csp->db_ctx, SDid, &sd_info);
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDid, event, " Cannot get sd info\n");
    sms_send_user_error((long) csp, SDid, "", ret);
    return ret;
  }

  ret = genericGetActivityReport(csp, &sd_info);
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDid, event, " getActivityReport FAILED\n");
    freeSDinfo(&sd_info);
    return ret;
  }

  freeSDinfo(&sd_info);
  return ret;
}

int save_running_conf(sd_info_t *SDinfo, char *str_flag_update, int *version_changed)
{
  int ret;
  char *config_type;
  char *ret_buf = NULL;
  SMS_PHP_STARTUP_THREAD();

  config_type = get_router_config_type(SDinfo->SD.man_id, SDinfo->SD.mod_id);

  sms_mod_php_set_global_str("flag_update", str_flag_update TSRMLS_CC);
  sms_mod_php_set_global_str("config_type", config_type TSRMLS_CC);
  php_store_sdinfo(SDinfo TSRMLS_CC);

  ret = sms_mod_php_execute_script(SDinfo->sdid, "smsd", "save_router_conf.php" TSRMLS_CC);
  if (ret)
  {
    LogWrite(LOG_ERROR, " Script Failed\n");
    goto end;
  }

  if (version_changed)
  {
    ret_buf = sms_mod_php_get_global_str(PHP_GLOBAL_SMS_RETURN_BUF TSRMLS_CC);
    if (!ret_buf || !strlen(ret_buf))
    {
      *version_changed = 0;
    }
    else
    {
      *version_changed = 1;
    }
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int unarchive_conf(char *params, client_state_t *csp)
{
  int ret;
  char sdid[BUFSIZ];
  char archive[BUFSIZ];
  char folder[BUFSIZ];
  sd_info_t sd_info;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sdid[0] = '\0';
  archive[0] = '\0';
  folder[0] = '\0';

  if (strlen(params) >= BUFSIZ)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = sscanf(params, "%s %s %s", sdid, archive, folder);
  if (ret != 3)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = getSDinfo(csp->db_ctx, sdid, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, "Cannot get sd info");
    sms_send_user_error((long) csp, params, "", ret);
    return ret;
  }

  ret = genericUnarchiveConfiguration(csp, &sd_info, archive, folder);

  freeSDinfo(&sd_info);

  if (ret)
  {
    sms_send_user_error((long) csp, "", "", ret);
    return ret;
  }

  sms_send_user_ok((long) csp, "", "");

  return SMS_OK;
}

int get_archive_conf(char *params, client_state_t *csp)
{
  int ret;
  char sdid[BUFSIZ];
  char folder[BUFSIZ];
  sd_info_t sd_info;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sdid[0] = '\0';
  folder[0] = '\0';

  if (strlen(params) >= BUFSIZ)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = sscanf(params, "%s %s", sdid, folder);
  if (ret != 2)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = getSDinfo(csp->db_ctx, sdid, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, "Cannot get sd info");
    sms_send_user_error((long) csp, params, "", ret);
    return ret;
  }

  ret = genericGetArchiveConfiguration(csp, &sd_info, folder);

  freeSDinfo(&sd_info);

  if (ret)
  {
    sms_send_user_error((long) csp, params, "", ret);
    return ret;
  }

  sms_send_user_ok((long) csp, "", "");

  return SMS_OK;
}

int call_command(char *params, client_state_t *csp)
{
  int ret;
  char sdid[BUFSIZ];
  char command[BUFSIZ];
  char apply_conf[BUFSIZ];
  char *buf = NULL;
  char *result = NULL;
  char *output = NULL;
  char tmp[BUFSIZ * 8];
  int  apply_conf_num;
  sd_info_t sd_info;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sdid[0] = '\0';
  command[0] = '\0';

  if (strlen(params) >= BUFSIZ)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = sscanf(params, "%s %s %s", sdid, command, apply_conf);
  if (ret != 3)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  // Read the additional prameters given in JSON format
  buf = NULL;
  do
  {
    // Read the additional prameters given in JSON format
    ret = socketReadLine(csp->cfd, tmp, sizeof(tmp), SELECTTIMEOUT);
    if (ret < 0)
    {
      GLogERROR(log_handle, "Cannot read command parameters ret %d\n", ret);
      sms_send_user_error((long) csp, params, "", ret);
      freez(buf);
      return ret;
    }
    buf = gpul_str_append(buf, tmp);
    if (ret != (sizeof(tmp) - 1))
    {
      buf = gpul_str_append(buf, "\n");
    }
  }
  while (strlen(tmp) > 0);

  GLogDEBUG(log_handle, 15, "Command %s Parameters %s", command, buf);

  ret = getSDinfo(csp->db_ctx, sdid, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, "Cannot get sd info");
    sms_send_user_error((long) csp, params, "", ret);
    freez(buf);
    return ret;
  }

  apply_conf_num = strtol(apply_conf, NULL, 0);
  ret = genericCallCommand(csp, &sd_info, command, apply_conf_num, buf, &result, &output);

  freeSDinfo(&sd_info);
  freez(buf);

  if (ret)
  {
    sms_send_user_error((long) csp, params, output, ret);
    freez(result);
    freez(output);
    return ret;
  }

  sms_send_user_ok_ex((long) csp, sdid, result, output);
  freez(result);
  freez(output);
  return ret;
}

int send_data_files(char *params, client_state_t *csp)
{
  int ret;
  char sdid[BUFSIZ];
  char addon[BUFSIZ];
  char server_addr[BUFSIZ];
  char server_ftp_login[BUFSIZ];
  char server_ftp_passwd[BUFSIZ];
  sd_info_t sd_info;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sdid[0] = '\0';
  addon[0] = '\0';
  server_addr[0] = '\0';
  server_ftp_login[0] = '\0';
  server_ftp_passwd[0] = '\0';

  if (strlen(params) >= BUFSIZ)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = sscanf(params, "%s %s %s %s %s", sdid, addon, server_addr, server_ftp_login, server_ftp_passwd);
  if (ret < 1)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = getSDinfo(csp->db_ctx, sdid, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, "Cannot get sd info");
    sms_send_user_error((long) csp, params, "", ret);
    return ret;
  }

  ret = genericSendDataFiles(csp, &sd_info, addon, server_addr, server_ftp_login, server_ftp_passwd);

  freeSDinfo(&sd_info);
  return ret;
}

int check_serial(char *params, client_state_t *csp)
{
  int ret;
  char sdid[BUFSIZ];
  char ipaddr[BUFSIZ];
  sd_info_t sd_info;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sdid[0] = '\0';
  ipaddr[0] = '\0';

  if (strlen(params) >= BUFSIZ)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = sscanf(params, "%s %s", sdid, ipaddr);
  if (ret != 2)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = getSDinfo(csp->db_ctx, sdid, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, "Cannot get sd info");
    sms_send_user_error((long) csp, sdid, "", ret);
    return ret;
  }

  ret = genericCheckSerialNumber(&sd_info, ipaddr);

  freeSDinfo(&sd_info);

  if (ret)
  {
    sms_send_user_error((long) csp, sdid, "", ret);
    return ret;
  }

  sms_send_user_ok((long) csp, sdid, "");
  return SMS_OK;
}

static int save_conf_task(void *vSD)
{
  sd_like_t *SD = (sd_like_t*)vSD;
  sd_info_t sd_info;
  int ret = SMS_OK;
  database_context_t *dbctx;
  int version_changed;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  dbctx = sms_db_find_ctx(log_handle, DB_SMS);
  ret = getSDinfo(dbctx, SD->sdid, &sd_info);
  sms_db_release_ctx(dbctx);
  if (ret)
  {
    GLogERROR(log_handle, "Cannot get sd info");
    goto end;
  }

  save_running_conf(&sd_info, "daily config", &version_changed);

  if (version_changed && (currentConfig->alarm_sev_conf_changed > 0))
  {
    set_sd_alarm(log_handle, SD->sd_cli_prefix, SD->sd_seqnum, currentConfig->alarm_sev_conf_changed, "CONF", "Configuration has been changed");
  }

  freeSDinfo(&sd_info);

  end:

  return ret;
}

int save_all_conf(void *dummy)
{
  int dataBaseShutdown = TRUE;
  sd_like_t *SD;
  database_context_t *dbctx;
  char task_name[BUFSIZ];
  int ret = SMS_OK;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  dbctx = sms_db_find_ctx(log_handle, DB_SMS);

  SD = (sd_like_t*) calloc(1, sizeof(sd_like_t));
  dataBaseShutdown = TRUE;
  while (dataBaseShutdown == TRUE)
  {
    ret = HA_SMSSQL_GetSDconnectedFirst(&(dbctx->sql_ctx), SD);
    if (ret == ERR_DB_FAILED)
    {
      GLogERROR(log_handle, "Cannot get a first SD: FAILED");
      goto end;
    }
    else if (ret == ERR_DB_NOTFOUND)
    {
      GLogINFO(log_handle, "No SD from the database");
      break;
    }

    dataBaseShutdown = FALSE;
    while (ret != ERR_DB_NOTFOUND)
    {
      if (SD->sd_connected)
      {
        sprintf(task_name, "save conf %s%lu", SD->sd_cli_prefix, SD->sd_seqnum);
        gpul_sched_new_task(sched_handle, save_conf_task, SD, NULL, task_name, 0, 0, GPUL_SCHED_ABSOLUTE);
      }

      /* try the next one */
      SD = (sd_like_t*) calloc(1, sizeof(sd_like_t));
      ret = HA_SMSSQL_GetSDconnectedNext(&(dbctx->sql_ctx), SD);
      if (ret == ERR_DB_FAILED)
      {
        GLogERROR(log_handle, "Cannot get a next SD:  FAILED in database read");
        goto end;
      }
      if (ret == ERR_DB_CONNECTION_LOST)
      {
        GLogERROR(log_handle, "Cannot get Next SD: CONNECTION_LOST, retrying all");
        dataBaseShutdown = TRUE;
        break;
      }
    }
  }/*end while databaseshutdown */

  end:
  freez(SD);
  sms_db_release_ctx(dbctx);

  return SMS_OK;
}
