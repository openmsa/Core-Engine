/** @file update.c
 This is the main file of the verb module of the smsd daemon.
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
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "log.h"
#include "net.h"
#include "smsd.h"
#include "verb.h"
#include "router.h"
#include "bd.h"
#include "php_misc.h"
#include "sms_php.h"
#include "error_code.h"
#include "update.h"
#include "generic_router.h"

/** buffer size for general purpose */
#define BUF_SZ  80

#define UNLOCK_SD(_EVT_, _LOCK_FLAG_) \
    if ((_LOCK_FLAG_) == UPDATE_LOCK_CHECK_REQUIRED) \
    { \
      int _ret2; \
      if (csp->db_ctx==NULL) csp->db_ctx=sms_db_find_ctx(log_handle, DB_SMS); \
      _ret2=HA_SMSSQL_UnLock(&(csp->db_ctx->sql_ctx), sd_info.SD.sd_cli_prefix, sd_info.SD.sd_seqnum); \
      if (_ret2 == ERR_DB_FAILED || _ret2 == ERR_DB_NOTFOUND) \
      { \
        LogWriteExt2(LOG_ERROR, sd_info.SD.sd_cli_prefix, sd_info.SD.sd_seqnum, _EVT_, " Cannot UNLOCK the  SD (SDid=%s%lu) FAILED\n", sd_info.SD.sd_cli_prefix, sd_info.SD.sd_seqnum); \
      } \
    }

int set_sd_updated(char *SDid, client_state_t *csp, unsigned int sd_updated_flag)
{
  sd_info_t sd_info;
  sd_like_t *SD = &(sd_info.SD);
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  strncpy(SD->sd_cli_prefix, SDid, PREFIXSIZE);
  SD->sd_cli_prefix[PREFIXSIZE] = 0;
  SD->sd_seqnum = atol(SDid + PREFIXSIZE);

  if (csp->db_ctx == NULL)
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);

  return HA_SMSSQL_SetSDUpdated(&(csp->db_ctx->sql_ctx), SD->sd_cli_prefix, SD->sd_seqnum, sd_updated_flag);
}

int getRouterFullConfiguration(char * SDid, client_state_t *csp, char ** configuration, char *event)
{
  int result = 0;
  int ret;
  sd_info_t sd_info;
  sd_like_t *SD = &(sd_info.SD);
  char *config = NULL;
  char *running_config = NULL;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = getSDinfo(csp->db_ctx, SDid, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, " Cannot get sd info");
    return ret;
  }

  /* release its db ctx for a while */
  RELEASE_BD_CTX(csp);

  if (SD->sd_prov_lock)
  {
    freeSDinfo(&sd_info);
    GLogERROR(log_handle, " SD Loked for provisioning");
    return ERR_SD_PROVLOCK;
  }

  if (!SD->sd_connected)
  {
    freeSDinfo(&sd_info);
    return ERR_SD_TIMEOUTCONNECT;
  }

  /* get the configuration from the router */
  result = genericGetConfiguration(&sd_info, &config, &running_config);
  if (result)
  {
    GLogERROR(log_handle, " Cannot get the current VPN configuration from the SD SDid=%s, result:%d", SDid, result);
    freeSDinfo(&sd_info);
    freez(config);
    return (result);
  }

  *configuration = config;

  freeSDinfo(&sd_info);
  return (result);
}

int checkUpdate(char *SDid, client_state_t *csp, char **configuration, char *event)
{
  char *check = NULL;
  int ret;

  ret = getUpdateStatus(SDid, csp, "UPDATE", &check);
  if (ret)
  {
    freez(check);
    return ret;
  }

  *configuration = check;

  return SMS_OK;
}

// I the following array is changed, update the file smsbd/sms_db.c accordingly
char *events[][2] =
{
{ "UPDATE", "CHECKUPDATE" },
{ "FIRMWARE", "CHECKUPDATEFIRMWARE" },
{ "RESTORE", "CHECKRESTORE" },
{ "SENDDATAFILES", "CHECKSENDDATAFILES" },
{ "BACKUP", "CHECKBACKUP" },
{ "SCRIPT", "CHECKEXECSCRIPT" },
{ "PUSHCONFIG", "CHECKPUSHCONFIG" },
{ "PUSHCONFIGIBA", "CHECKPUSHCONFIGIBA" },
{ "PUSHCONFIGSTARTUP", "CHECKPUSHCONFIGSTARTUP" },
{ "LICENSE", "CHECKUPDATELICENSE" },
{ "NODEMIGRATION", "CHECKNODEMIGRATION" },
{ "OBMFAPPLYCONF", "CHECKOBMFAPPLYCONF" },
};

int events_size = sizeof(events) / sizeof(char *[2]);

int checkAllUpdate(char *SDid, client_state_t *csp, char **configuration, char *event)
{
  int result;
  char buf[BUF_SZ];
  char *updateString = NULL;
  char *check = NULL;
  int i;

  for (i = 0; i < events_size; i++)
  {
    result = getUpdateStatus(SDid, csp, events[i][0], &check);
    if (result)
    {
      freez(check);
      freez(updateString);
      return (result);
    }
    sprintf(buf, "\r\n#begin %s\r\n", events[i][1]);
    updateString = gpul_str_append(updateString, buf);
    updateString = gpul_str_append(updateString, check);
    sprintf(buf, "\r\n#end %s\r\n", events[i][1]);
    updateString = gpul_str_append(updateString, buf);
    freez(check);
  }

  *configuration = updateString;

  return SMS_OK;
}

int dnsUpdate(char *SDid, char *ip)
{
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  SMS_PHP_STARTUP_THREAD();

  sms_mod_php_set_global_str("sdid", SDid TSRMLS_CC);
  sms_mod_php_set_global_str("ip", ip TSRMLS_CC);

  ret = sms_mod_php_execute_script(SDid, "smsd", "dns_update.php" TSRMLS_CC);
  if (ret)
  {
    GLogERROR(log_handle, " Script Failed\n");
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}


/* gestion du lock provisioning
 * a la creation du sd, sd_prov_lock = -1
 * provisioning en cours ou fini KO, sd_prov_lock = 1
 * provisioning fini OK, sd_prov_lock = 0
 */
int lockProv(client_state_t *csp, char *sd_cli_prefix, long sd_seqnum, char *event)
{
  int ret;
  int ret2;
  int update_lock_state = 0;
  int provlock;
  int release_db_ctx = 0;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
    release_db_ctx = 1;
  }

  ret = HA_SMSSQL_GetSDprovlock(&(csp->db_ctx->sql_ctx), sd_cli_prefix, sd_seqnum, &provlock);
  if (ret == ERR_DB_NOTFOUND)
  {
    LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " SD NOT FOUND in the database\n");
    goto err;
  }
  else if (ret)
  {
    LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " Cannot READ LOCK PROVISIONING\n");
    goto err;
  }

  // The SD is set DOWN during the provisioning, polld/syslogd will set it UP
  ret2 = HA_SMSSQL_SetSdDown(&(csp->db_ctx->sql_ctx), sd_cli_prefix, sd_seqnum);
  if (ret2)
  {
    LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " Cannot Set the SD DOWN\n");
  }

  if (provlock == 1)
  {
    /* Already locked at high level, nothing to do */
    goto err;
  }

  /* Check the low level lock, we cannot lock at high level if low level is locked. */
  ret = HA_SMSSQL_SetLock(&(csp->db_ctx->sql_ctx), sd_cli_prefix, sd_seqnum, &update_lock_state);
  if (ret)
  {
    LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " Cannot LOCK the SD\n");
    goto err;
  }

  if (update_lock_state == ALREADY_LOCKED)
  {
    ret = ERR_SD_LOCK;
    goto err;
  }

  /* now we have locked at low level, get the high level lock */
  ret = HA_SMSSQL_SetSDprovlock(&(csp->db_ctx->sql_ctx), sd_cli_prefix, sd_seqnum, 1);
  if (ret)
  {
    LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " Cannot LOCK PROVISIONING SD FAILED with err=%d\n", ret );
    goto err;
  }

  ret2 = HA_SMSSQL_UnLock(&(csp->db_ctx->sql_ctx), sd_cli_prefix, sd_seqnum);
  if (ret2)
  {
    LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " Cannot UNLOCK the SD\n");
  }

  err:

  if (release_db_ctx)
  {
    sms_db_release_ctx(csp->db_ctx);
  }

  return ret;
}

int lockProvisioning(char * SDid, client_state_t *csp)
{
  sd_info_t sd_info;
  sd_like_t *SD = &(sd_info.SD);
  int ret;
  char *event = EVT_LOCKPROVISIONING;

  strncpy(SD->sd_cli_prefix, SDid, PREFIXSIZE);
  SD->sd_cli_prefix[PREFIXSIZE] = 0;
  SD->sd_seqnum = atol(SDid + PREFIXSIZE);

  ret = lockProv(csp, SD->sd_cli_prefix, SD->sd_seqnum, event);

  return ret;
}

int unlockProv(client_state_t *csp, char *sd_cli_prefix, long sd_seqnum, char *event)
{
  database_context_t *db_cust;
  int ret;
  int release_db_ctx = 0;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
    release_db_ctx = 1;
  }

  db_cust = sms_db_find_ctx(log_handle, DB_CUST);

  ret = HA_SMSSQL_SetSDprovlock(&(csp->db_ctx->sql_ctx), sd_cli_prefix, sd_seqnum, 0);
  if (ret == ERR_DB_NOTFOUND)
  {
    LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " SD NOT FOUND in the database\n");
  }
  else if (ret)
  {
    LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " Cannot UNLOCK PROVISIONING\n");
  }

  ret = HA_SMSSQL_SetProvDate(&(db_cust->sql_ctx), sd_cli_prefix, sd_seqnum);

  if (release_db_ctx)
  {
    sms_db_release_ctx(csp->db_ctx);
  }

  if (ret == ERR_DB_NOTFOUND)
  {
    LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " SD NOT FOUND in the database\n");
  }
  else if (ret)
  {
    LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " Cannot set prov date\n");
  }

  sms_db_release_ctx(db_cust);

  return ret;
}

int unlockProvisioning(char * SDid, client_state_t *csp)
{
  sd_info_t sd_info;
  sd_like_t *SD = &(sd_info.SD);
  int ret;
  char *event = EVT_UNLOCKPROVISIONING;

  strncpy(SD->sd_cli_prefix, SDid, PREFIXSIZE);
  SD->sd_cli_prefix[PREFIXSIZE] = 0;
  SD->sd_seqnum = atol(SDid + PREFIXSIZE);

  ret = unlockProv(csp, SD->sd_cli_prefix, SD->sd_seqnum, event);

  return ret;
}

int lockSD(client_state_t *csp, char *sd_cli_prefix, long sd_seqnum, char *event)
{
  int ret;
  int update_lock_state = 0;
  int release_db_ctx = 0;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
    release_db_ctx = 1;
  }

  ret = HA_SMSSQL_SetLock(&(csp->db_ctx->sql_ctx), sd_cli_prefix, sd_seqnum, &update_lock_state);

  if (release_db_ctx)
  {
    sms_db_release_ctx(csp->db_ctx);
  }

  if (ret == ERR_DB_NOTFOUND)
  {
    LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " SD NOT FOUND in the database\n");
    return ret;
  }
  else if (ret)
  {
    LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " Cannot LOCK SD\n");
    return ret;
  }

  switch (update_lock_state)
  {
    case ALREADY_LOCKED:
      LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " SD already LOCKED\n");
      return ERR_SD_LOCK;

    case ALREADY_PROVLOCKED:
      LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " SD already PROVLOCKED\n");
      return ERR_SD_PROVLOCK;

    default:
      return 0;
  }
}

int unlockSD(client_state_t *csp, char *sd_cli_prefix, long sd_seqnum, char *event)
{
  int ret;
  int release_db_ctx = 0;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
    release_db_ctx = 1;
  }

  ret = HA_SMSSQL_UnLock(&(csp->db_ctx->sql_ctx), sd_cli_prefix, sd_seqnum);

  if (release_db_ctx)
  {
    sms_db_release_ctx(csp->db_ctx);
  }

  if (ret == ERR_DB_NOTFOUND)
  {
    LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " SD NOT FOUND in the database\n");
  }
  else if (ret)
  {
    LogWriteExt2(LOG_ERROR, sd_cli_prefix, sd_seqnum, event, " Cannot UNLOCK SD\n");
  }

  return ret;
}

int unlockUpdate(char * SDid, client_state_t *csp)
{
  sd_info_t sd_info;
  sd_like_t *SD = &(sd_info.SD);
  int ret;
  char *event = EVT_UNLOCKUPDATE;

  strncpy(SD->sd_cli_prefix, SDid, PREFIXSIZE);
  SD->sd_cli_prefix[PREFIXSIZE] = 0;
  SD->sd_seqnum = atol(SDid + PREFIXSIZE);

  ret = unlockSD(csp, SD->sd_cli_prefix, SD->sd_seqnum, event);

  return ret;
}

/* format de SDid: '<sd_id> <commande>'  */
/* NB: seuls l'etat des reboots programes sont inscrits en base */
int smsexec(char * SDid, client_state_t *csp, char **result, char *addon)
{
  int ret;
  char * p;
  int len;
  sd_info_t sd_info;
  struct router_cmd_head router_cmd_list;
  router_cmd_t router_cmd_elt;
  char *event = EVT_SMSEXEC;

  len = strlen(SDid);
  p = strchr(SDid, ' ');
  if (!p)
  {
    LogWriteExt(LOG_ERROR, SDid, event, " Bad argument format %s\n", SDid);
    ret = ERR_VERB_BAD_PARAM;
    goto end;
  }

  *p = '\0';
  p++;
  while (*p == ' ')
    p++;

  if ((p - SDid) >= len)
  {
    LogWriteExt(LOG_ERROR, SDid, event, " Bad argument format %s\n", SDid);
    ret = ERR_VERB_BAD_PARAM;
    goto end;
  }

  ret = getSDinfo(csp->db_ctx, SDid, &sd_info);
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDid, event, " Cannot get sd info\n");
    goto end;
  }

  /* release its db ctx for a while */
  RELEASE_BD_CTX(csp);

  /* 1 seul element */
  GPUL_TAILQ_INIT(&router_cmd_list);
  router_cmd_elt.cmd = p;
  GPUL_TAILQ_INSERT_TAIL(&router_cmd_list, &router_cmd_elt, link);

  if (addon)
  {
    ret = genericSmsAddonExecCmd(csp, &sd_info, addon, &router_cmd_list);
  }
  else
  {
    ret = genericSmsexecCmd(csp, &sd_info, &router_cmd_list);
  }

  GPUL_TAILQ_REMOVE(&router_cmd_list, &router_cmd_elt, link);

  freeSDinfo(&sd_info);

  end:

  if (ret)
  {
    sms_send_user_error((long) csp, SDid, "", ret);
  }

  return ret;
}

/* les commandes sont separees par '\r\n' */
int smsexecn(char *SDid, client_state_t *csp, char **result, char *addon)
{
  char buf[BUFSIZ + 1];
  sd_info_t sd_info;
  struct router_cmd_head router_cmd_list;
  router_cmd_t *router_cmd_elt;
  char *p;
  char *q;
  char *event = EVT_SMSEXECN;
  int ret;
  char *commands;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  /* Read commands */
  commands = NULL;
  ret = socketReadLine(csp->cfd, buf, sizeof(buf), 3);
  while (ret > 0)
  {
    GLogDEBUG(log_handle, 15, "  arg: %s", buf);
    commands = gpul_str_append(commands, buf);
    if (ret != (sizeof(buf) - 1))
    {
      commands = gpul_str_append(commands, "\r\n");
    }
    ret = socketReadLine(csp->cfd, buf, sizeof(buf), 3);
  }

  ret = getSDinfo(csp->db_ctx, SDid, &sd_info);
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDid, event, " Cannot get sd info\n");
    goto end;
  }

  /* release its db ctx for a while */
  RELEASE_BD_CTX(csp);

  GPUL_TAILQ_INIT(&router_cmd_list);

  p = commands;
  while (*p != '\0')
  {
    q = strstr(p, "\r\n");
    if (!q)
    {
      break;
    }

    *q = '\0';
    router_cmd_elt = (router_cmd_t *) malloc(sizeof(router_cmd_t));
    router_cmd_elt->cmd = p;
    GPUL_TAILQ_INSERT_TAIL(&router_cmd_list, router_cmd_elt, link);
    p = q + sizeof("\r\n") - 1;
  }

  if (addon)
  {
    ret = genericSmsAddonExecCmd(csp, &sd_info, addon, &router_cmd_list);
  }
  else
  {
    ret = genericSmsexecCmd(csp, &sd_info, &router_cmd_list);
  }

  while ((router_cmd_elt = GPUL_TAILQ_FIRST (&router_cmd_list)) != NULL)
  {
    GPUL_TAILQ_REMOVE(&router_cmd_list, router_cmd_elt, link);
    free(router_cmd_elt);
  }

  freeSDinfo(&sd_info);

  end:

  if (ret)
  {
    sms_send_user_error((long) csp, SDid, "", ret);
  }

  return ret;
}

/* format de params: '<sd_id> <script_file_path>'  */
int exec_script(char *params, client_state_t *csp, int synchronous)
{
  int ret;
  int len;
  int send_user = 1;
  int free_sdinfo = 0;
  char *p;
  char *sdid;
  char *script;
  sd_info_t sd_info;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  len = strlen(params);
  p = strchr(params, ' ');
  if (!p)
  {
    GLogERROR(log_handle, " Bad argument format");
    ret = ERR_VERB_BAD_PARAM;
    goto end;
  }

  sdid = params;
  *p = '\0';
  p++;
  while (*p == ' ')
    p++;
  script = p;

  if ((p - sdid) >= len)
  {
    GLogERROR(log_handle, " Bad argument format");
    ret = ERR_VERB_BAD_PARAM;
    goto end;
  }

  ret = getSDinfo(csp->db_ctx, sdid, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, " Cannot get sd info");
    goto end;
  }

  free_sdinfo = 1;

  /* release its db ctx for a while */
  RELEASE_BD_CTX(csp);

  // From now the response to the user is done by genericExecScript()
  send_user = 0;

  ret = genericExecScript(csp, &sd_info, script, synchronous);

  end:

  if (send_user)
  {
    if (ret)
    {
      sms_send_user_error((long) csp, params, "", ret);
    }
    else
    {
      sms_send_user_ok((long) csp, params, "");
    }
  }

  if (free_sdinfo)
  {
    freeSDinfo(&sd_info);
  }

  return ret;
}

int push_config(client_state_t *csp, char *params)
{
  sd_info_t sd_info;
  char buf[BUFSIZ];
  int ret;
  char sdid[BUFSIZ];
  char addon[BUFSIZ];
  char ts_ip[BUFSIZ];
  char ts_port[BUFSIZ];
  char *configuration;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sdid[0] = '\0';
  addon[0] = '\0';
  ts_ip[0] = '\0';
  ts_port[0] = '\0';

  if (strlen(params) >= BUFSIZ)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = sscanf(params, "%s %s %s %s", sdid, addon, ts_ip, ts_port);
  if (ret < 1)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  /* Read configuration */
  configuration = NULL;
  ret = socketReadLine(csp->cfd, buf, sizeof(buf), 2);
  while (ret >= 0)
  {
    configuration = gpul_str_append(configuration, buf);
    if (ret != (sizeof(buf) -1))
    {
      configuration = gpul_str_append(configuration, "\r\n");
    }
    ret = socketReadLine(csp->cfd, buf, sizeof(buf), 2);
  }

  if ((configuration == NULL) || strlen(configuration) == 0)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = getSDinfo(csp->db_ctx, sdid, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, " Cannot get sd info\n");
    sms_send_user_error((long) csp, sdid, "", ret);
    return ret;
  }

  ret = genericPushConfig(csp, &sd_info, addon, ts_ip, ts_port, configuration);
  if (ret)
  {
    GLogERROR(log_handle, " push config (%s) FAILED", addon);
    goto end;
  }

  end:

  freeSDinfo(&sd_info);
  free(configuration);

  return ret;
}

int provisioning(client_state_t *csp, char *SDid, char *event)
{
  sd_info_t sd_info;
  sd_like_t *SD = &(sd_info.SD);
  char buf[BUFSIZ];
  char ip[256];
  char login[256];
  char passwd[256];
  char adminpasswd[256];
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ip[0] = '\0';
  login[0] = '\0';
  passwd[0] = '\0';
  adminpasswd[0] = '\0';

  /* lire la liste des arguments */
  ret = socketReadLine(csp->cfd, buf, sizeof(buf), 2);
  if (ret > 0)
  {
    sscanf(buf, "%s %s %s %s", ip, login, passwd, adminpasswd);
    GLogDEBUG(log_handle, 15, "  arg: %s", buf);
  }

  ret = getSDinfo(csp->db_ctx, SDid, &sd_info);
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDid, event, " Cannot get sd info\n");
    sms_send_user_error((long) csp, SDid, "", ret);
    return ret;
  }

  if (!*ip)
  {
    strncpy(ip, SD->sd_ip_config, 256);
  }
  if (!*login)
  {
    strncpy(login, SD->sd_login_entry, 256);
  }
  if (!*passwd)
  {
    strncpy(passwd, SD->sd_passwd_entry, 256);
  }
  if (!*adminpasswd)
  {
    strncpy(adminpasswd, SD->sd_passwd_adm, 256);
  }

  // the php is in charge to close the socket at the begining because of the asynchronous mode
  // the php is also in charge to set the status in DB
  ret = genericInitialProvisioning(csp, &sd_info, ip, login, passwd, adminpasswd);
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDid, event, " initial provisioning FAILED\n");
    freeSDinfo(&sd_info);
    return ret;
  }

  freeSDinfo(&sd_info);
  return 0;
}

int checkProvisioning(client_state_t *csp, char *SDid, char *event)
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

  ret = genericCheckProvisioning(csp, &sd_info);
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDid, event, " Check provisioning FAILED\n");
    freeSDinfo(&sd_info);
    return ret;
  }

  freeSDinfo(&sd_info);
  return 0;

}

int backupRouterFullConfiguration(char *params, client_state_t *csp)
{
  sd_info_t sd_info;
  int ret = 0;
  char sdid[BUFSIZ];
  char addon[BUFSIZ];
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sdid[0] = '\0';
  addon[0] = '\0';

  if (strlen(params) >= BUFSIZ)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = sscanf(params, "%s %s", sdid, addon);
  if (ret < 1)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = getSDinfo(csp->db_ctx, sdid, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, " Cannot get sd info");
    sms_send_user_error((long) csp, sdid, "Cannot get sd info", ret);
    return ret;
  }

  ret = genericBackupConf(csp, &sd_info, addon, "update BACKUP");
  if (ret)
  {
    GLogERROR(log_handle, " backup (%s) FAILED", addon);
    goto end;
  }

  end:

  freeSDinfo(&sd_info);

  return ret;
}

int restoreConf(char *params, client_state_t *csp)
{
  sd_info_t sd_info;
  int ret = 0;
  char msg[BUFSIZ];
  char sdid[BUFSIZ];
  char revisionId[BUFSIZ];
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if (strlen(params) >= BUFSIZ)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  sdid[0] = '\0';
  revisionId[0] = '\0';

  ret = sscanf(params, "%s %s", sdid, revisionId);
  if (ret != 2)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = getSDinfo(csp->db_ctx, sdid, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, " Cannot get sd info");
    sms_send_user_error((long) csp, sdid, "Cannot get sd info", ret);
    return ret;
  }

  sprintf(msg, "RESTORE (revision: %s)", revisionId);
  ret = genericRestoreConf(csp, &sd_info, revisionId, msg);
  if (ret)
  {
    GLogERROR(log_handle, " restore configuration (%s) FAILED", msg);
    goto end;
  }

  end:

  freeSDinfo(&sd_info);

  return ret;
}

int updateConfig(client_state_t *csp, sd_info_t *sd_info, char *event)
{
  sd_like_t *SD = &(sd_info->SD);
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  RELEASE_BD_CTX(csp);

  ret = genericUpdateConf(csp, sd_info);
  if (ret)
  {
    GLogERROR(log_handle, " update configuration FAILED");
  }

  // ask to update asset
  sdForceAsset(csp, SD->sdid);

  RELEASE_BD_CTX(csp);

  // Save the running config
  save_running_conf(&(sd_info->SD), "update", NULL);

  return ret;
}

int staging(client_state_t *csp, char *SDid, int map, char *event)
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

  if (map)
  {
    ret = genericStagingMap(csp, &sd_info);
  }
  else
  {
    ret = genericStaging(csp, &sd_info);
  }

  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDid, event, " staging FAILED\n");
  }

  freeSDinfo(&sd_info);
  return ret;
}

static int update_task(void *vsd_info)
{
  sd_info_t *sd_info = (sd_info_t *) vsd_info;
  sd_like_t *SD = &(sd_info->SD);
  char SDid[SDID_LEN + 1];
  struct client_state csp;
  char *event = "scan_updated";
  int ret;
  void *log_handle;

  strncpy(SDid, SD->sdid, SDID_LEN);
  SDid[SDID_LEN] = '\0';

  log_handle = gpul_sched_get_my_log_handle(sched_handle);
  gpul_log_set_event(log_handle, event);
  gpul_log_set_sd_id(log_handle, SDid);

  ret = SMS_OK;
  memset(&csp, 0, sizeof(struct client_state));
  csp.cfd = -1; // no user
  csp.db_ctx = sms_db_find_ctx(log_handle, DB_SMS);

  ret = getSDinfo(csp.db_ctx, SDid, sd_info);
  if (ret)
  {
    goto end;
  }

  if (SD->sd_type[0] == 'H')
  {
    ret = updateConfig(&csp, sd_info, event);
  }
  else
  {
    ret = ERR_SD_NOT_SUPPORTED;
  }

  if (csp.db_ctx == NULL)
  {
    csp.db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
  }

  end:

  if (ret)
  {
    if ((ret != ERR_SD_NOT_SUPPORTED) && (ret != ERR_SD_LOCK) && (ret != ERR_SD_PROVLOCK))
    {
      sms_set_update_status((long)&csp, SDid, ret, event, "FAILED", "");
      GLogINFO(log_handle, " ends ERROR");
    }
    else if (ret == ERR_SD_NOT_SUPPORTED)
    {
      sms_set_update_status((long)&csp, SDid, SMS_OK, event, "ENDED", "");
      set_sd_updated(SDid, &csp, 0);
      GLogINFO(log_handle, " ends OK");
    }
  }
  else
  {
    GLogINFO(log_handle, " ends OK");
  }

  RELEASE_BD_CTX(&csp);
  freeSDinfo(sd_info);
  freez(sd_info);

  return 0;
}

int scan_updated(void *dummy)
{
  int dataBaseShutdown = TRUE;
  sd_info_t *sd_info;
  sd_like_t *SD;
  int ret;
  char *event = "scan_updated";
  database_context_t *dbctx;
  char task_name[BUFSIZ];
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);
  gpul_log_set_event(log_handle, event);

  dbctx = sms_db_find_ctx(log_handle, DB_SMS);
  sd_info = (sd_info_t *) calloc(1, sizeof(sd_info_t));
  SD = &(sd_info->SD);
  dataBaseShutdown = TRUE;
  while (dataBaseShutdown == TRUE)
  {
    ret = HA_SMSSQL_GetSDToUpdateFirst(&(dbctx->sql_ctx), SD);
    if (ret == ERR_DB_FAILED)
    {
      LogWriteExt(LOG_ERROR, "N/A", "", " Cannot get a SD to update: FAILED\n");
      break;
    }
    else if (ret == ERR_DB_NOTFOUND)
    {
      //DbgWrite(DBG_INFO, " No SD to update from the database\n");
      break;
    }

    dataBaseShutdown = FALSE;
    while (ret != ERR_DB_NOTFOUND)
    {
      sprintf(task_name, "update %s%lu", SD->sd_cli_prefix, SD->sd_seqnum);
      gpul_sched_new_task(sched_handle, update_task, sd_info, NULL, task_name, 0, 0, GPUL_SCHED_ABSOLUTE);

      /* try the next one */
      sd_info = (sd_info_t *) calloc(1, sizeof(sd_info_t));
      SD = &(sd_info->SD);
      ret = HA_SMSSQL_GetSDToUpdateNext(&(dbctx->sql_ctx), SD);
      if (ret == ERR_DB_FAILED)
      {
        LogWriteExt(LOG_ERROR, "N/A", event, "  Cannot get a next SD to update:  FAILED in database read\n");
        break;
      }
      if (ret == ERR_DB_CONNECTION_LOST)
      {
        LogWriteExt(LOG_ERROR, "N/A", event, " Cannot get Next SD to update CONNECTION_LOST, retrying all\n");
        dataBaseShutdown = TRUE;
        break;
      }
    }
  } /* end while databaseshutdown */

  freez(sd_info);

  sms_db_release_ctx(dbctx);

  return 0;
}

/** Update the license on a router
 @param SDid the id of the router
 @param csp the current context
 */
int updateLicense(char * SDid, client_state_t * csp)
{
  int ret;
  sd_info_t sd_info;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = getSDinfo(csp->db_ctx, SDid, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, " Cannot get sd info");
    return ret;
  }

  ret = genericUpdateLicense(csp, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, " Cannot update license");
  }

  freeSDinfo(&sd_info);
  return ret;
}

/** Update the firmware on a router
 @param SDid the id of the router
 @param csp the current context
 */
int updateFirmware(char *params, client_state_t * csp)
{
  int ret;
  char sdid[BUFSIZ];
  char param[BUFSIZ];
  sd_info_t sd_info;
  char server_addr[BUFSIZ];
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sdid[0] = '\0';
  param[0] = '\0';
  server_addr[0] = '\0';

  if (strlen(params) >= BUFSIZ)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = sscanf(params, "%s %s %s", sdid, param, server_addr);
  if (ret < 1)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = getSDinfo(csp->db_ctx, sdid, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, " Cannot get sd info");
    return ret;
  }

  ret = genericUpdateFirmware(csp, &sd_info, param, server_addr);
  if (ret)
  {
    GLogERROR(log_handle, " Cannot update firmware");
  }

  freeSDinfo(&sd_info);
  return ret;
}

int setUpdateStatus(char *SDid, client_state_t *csp, char *type, char *status, char *failure_mag)
{
  sd_updatestatus_like_t sd_updatestatus;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  memset(&sd_updatestatus, 0, sizeof(sd_updatestatus));

  strncpy(sd_updatestatus.sd_cli_prefix, SDid, PREFIXSIZE);
  sd_updatestatus.sd_seqnum = atol(SDid + PREFIXSIZE);
  strncpy(sd_updatestatus.upd_type, type, UPD_TYPE_LEN);
  strncpy(sd_updatestatus.upd_status, status, UPD_STATUS_LEN);
  strncpy(sd_updatestatus.upd_failure_message, failure_mag, FAILURE_MESSAGE_LEN);

  /* get a new BD ctx */
  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
  }

  ret = HA_SMSSQL_SetUpdateStatus(&(csp->db_ctx->sql_ctx), &sd_updatestatus);
  if (ret == ERR_DB_FAILED || ret == ERR_DB_NOTFOUND)
  {
    if (ret == ERR_DB_FAILED)
    {
      GLogERROR(log_handle, " Cannot set Update Status (%s) FAILED", type);
    }
    else
    {
      GLogERROR(log_handle, " Set Update Status (%s) SD NOT FOUND in the database", type);
    }
    return ret;
  }

  return 0;
}

int getUpdateStatus(char *SDid, client_state_t *csp, char *type, char **result)
{
  sd_updatestatus_like_t sd_updatestatus;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  memset(&sd_updatestatus, 0, sizeof(sd_updatestatus));

  strncpy(sd_updatestatus.sd_cli_prefix, SDid, PREFIXSIZE);
  sd_updatestatus.sd_seqnum = atol(SDid + PREFIXSIZE);
  strncpy(sd_updatestatus.upd_type, type, UPD_TYPE_LEN);

  /* get a new BD ctx */
  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
  }

  ret = HA_SMSSQL_GetUpdateStatus(&(csp->db_ctx->sql_ctx), &sd_updatestatus);
  if (ret)
  {
    if (ret != ERR_DB_NOTFOUND)
    {
      GLogERROR(log_handle, " Cannot get Update Status (%s) FAILED", type);
      return ret;
    }

    //GLogERROR(log_handle, " Get Update Status (%s) SD NOT FOUND in the database", type);
    strcpy(sd_updatestatus.upd_lastdate, "01-01-1970 00:00:00");
    strcpy(sd_updatestatus.upd_status, "NOTPROCESSED");
  }

  // For compatibility keep only the first character of the status : 1 instead of strlen(sd_updatestatus.upd_status)
  *result = malloc(strlen(sd_updatestatus.upd_lastdate) + 2 + 1 + 2 + strlen(sd_updatestatus.upd_failure_message) + 1);
  sprintf(*result, "%s%s%c%s%s", sd_updatestatus.upd_lastdate, SMSD_MSG_EOL, sd_updatestatus.upd_status[0], SMSD_MSG_EOL, sd_updatestatus.upd_failure_message);

  return 0;
}

int ha_swap(client_state_t *csp, char *SDid)
{
  sd_info_t sd_info;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = getSDinfo(csp->db_ctx, SDid, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, " Cannot get sd info");
    sms_send_user_error((long) csp, SDid, "", ret);
    return ret;
  }

  ret = genericHaSwap(csp, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, " HA swap FAILED");
  }

  freeSDinfo(&sd_info);
  return ret;
}

int getPingStatus(client_state_t *csp, char *params, char *event)
{
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  SMS_PHP_STARTUP_THREAD();

  sms_mod_php_set_global_ptr(PHP_GLOBAL_SMS_CSP, csp TSRMLS_CC);
  sms_mod_php_set_global_str("HOST", params TSRMLS_CC);

  ret = sms_mod_php_execute_script("debug", "smsd", "do_getPingStatus.php" TSRMLS_CC);
  if (ret)
  {
    char *str = sms_mod_php_get_global_str(PHP_GLOBAL_SMS_RETURN_BUF TSRMLS_CC);
    if (!str)
    {
      str = "";
    }
    GLogERROR(log_handle, " Script Failed\n");
    sms_send_user_error((long) csp, params, str, ret);
    goto end;
  }

  end:
  SMS_PHP_SHUTDOWN_THREAD();

  return ret;
}

int smsCmd(char *params, client_state_t *csp)
{
  sd_info_t sd_info;
  char buf[BUFSIZ];
  int ret;
  char sdid[BUFSIZ];
  char cmd[BUFSIZ];
  char *optional_params;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sdid[0] = '\0';
  cmd[0] = '\0';

  if (strlen(params) >= BUFSIZ)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = sscanf(params, "%s %s", sdid, cmd);
  if (ret < 1)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  /* Read configuration */
  optional_params = NULL;
  ret = socketReadLine(csp->cfd, buf, sizeof(buf), 2);
  while (ret > 0)
  {
    GLogDEBUG(log_handle, 15, "  arg: %s", buf);
    optional_params = gpul_str_append(optional_params, buf);
    if (ret != (sizeof(buf) - 1))
    {
      optional_params = gpul_str_append(optional_params, "\r\n");
    }
    ret = socketReadLine(csp->cfd, buf, sizeof(buf), 2);
  }

  if (strcmp(sdid, "NONE"))
  {
    ret = getSDinfo(csp->db_ctx, sdid, &sd_info);
    if (ret)
    {
      GLogERROR(log_handle, " Cannot get sd info");
      sms_send_user_error((long) csp, sdid, "", ret);
      return ret;
    }
    ret = generic_sms_cmd(csp, &sd_info, cmd, optional_params);

    freeSDinfo(&sd_info);
  }
  else
  {
    ret = generic_smsd_cmd(csp, cmd, optional_params);
  }

  return ret;
}

int smsCustCmd(char *params, client_state_t *csp)
{
  cust_info_t cust_info;
  char buf[BUFSIZ];
  int ret = SMS_OK;
  char custid[BUFSIZ];
  char cmd[BUFSIZ];
  char *optional_params;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  custid[0] = '\0';
  cmd[0] = '\0';

  if (strlen(params) >= BUFSIZ)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = sscanf(params, "%s %s", custid, cmd);
  if (ret < 1)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  /* Read configuration */
  optional_params = NULL;
  ret = socketReadLine(csp->cfd, buf, sizeof(buf), 2);
  while (ret > 0)
  {
    GLogDEBUG(log_handle, 15, "  arg: %s", buf);
    optional_params = gpul_str_append(optional_params, buf);
    if (ret != (sizeof(buf) - 1))
    {
      optional_params = gpul_str_append(optional_params, "\r\n");
    }
    ret = socketReadLine(csp->cfd, buf, sizeof(buf), 2);
  }

  ret = get_cust_info(csp->db_ctx, custid, &cust_info, prog);
  if (ret)
  {
    sms_send_user_error((long) csp, custid, "", ret);
    return ret;
  }

  ret = generic_sms_cust_cmd(csp, &cust_info, cmd, optional_params);

  free_all_in_cust(&cust_info);

  return ret;
}

int genTemplate(char *params, client_state_t *csp)
{
  sd_info_t sd_info;
  int ret = 0;
  char msg[BUFSIZ];
  char sdid[BUFSIZ];
  char revisionId[BUFSIZ];
  char revisionId2[BUFSIZ];
  char addon[BUFSIZ];
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if (strlen(params) >= BUFSIZ)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  sdid[0] = '\0';
  revisionId[0] = '\0';
  revisionId2[0] = '\0';
  addon[0] = '\0';

  ret = sscanf(params, "%s %s %s %s", sdid, revisionId, revisionId2, addon);
  if (ret < 3)
  {
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = getSDinfo(csp->db_ctx, sdid, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, " Cannot get sd info");
    sms_send_user_error((long) csp, sdid, "Cannot get sd info", ret);
    return ret;
  }

  ret = generic_gen_template(csp, &sd_info, revisionId, revisionId2, addon);
  if (ret)
  {
    GLogERROR(log_handle, "template generation (%s) FAILED", msg);
    goto end;
  }

  end:

  freeSDinfo(&sd_info);

  return ret;
}
