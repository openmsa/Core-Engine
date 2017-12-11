/**
 * Copyright UBIqube Solutions 2008
 * @file php_db.c
 * Creation Date: Jun 16, 2008
 */

#include "sms_php.h"
#include "sms_php_functions_misc.h"
#include "sms_php_wrappers.h"

#include "log.h"
#include "smsd.h"
#include "sms_db.h"
#include "router.h"
#include "update.h"
#include "php_db.h"

/**
 * @addtogroup db
 * @{
 */

/* gestion du lock provisioning
 * a la creation du sd, sd_prov_lock = -1
 * provisioning en cours ou fini KO, sd_prov_lock = 1
 * provisioning fini OK, sd_prov_lock = 0
 */

/** Lock the SD for provisioning
 * @param lcsp  pointer to csp context
 * @param lsd_info  pointer to sd_info context
 * @return 0 if OK<br>
 *         != 0 if error
 */
int sms_sd_prov_lock(long lcsp, long lsd_info)
{
  sd_info_t *sd_info = (sd_info_t *) lsd_info;
  sd_like_t *SD;
  client_state_t *csp = (client_state_t *) lcsp;
  int ret;
  char *event = EVT_LOCKPROVISIONING;

  if (!sd_info || !csp)
  {
    return ERR_SD_FAILED;
  }

  SD = &(sd_info->SD);
  ret = lockProv(csp, SD->sd_cli_prefix, SD->sd_seqnum, event);

  return ret;
}

/** Unlock the SD for provisioning
 * @param lcsp  pointer to csp context
 * @param lsd_info  pointer to sd_info context
 * @return 0 if OK<br>
 *         != 0 if error
 */
int sms_sd_prov_unlock(long lcsp, long lsd_info)
{
  sd_info_t *sd_info = (sd_info_t *) lsd_info;
  sd_like_t *SD;
  client_state_t *csp = (client_state_t *) lcsp;
  int ret;
  char *event = EVT_UNLOCKPROVISIONING;

  if (!sd_info || !csp)
  {
    return ERR_SD_FAILED;
  }

  SD = &(sd_info->SD);
  ret = unlockProv(csp, SD->sd_cli_prefix, SD->sd_seqnum, event);

  return ret;
}

/** Lock the SD
 * @param lcsp  pointer to csp context
 * @param lsd_info  pointer to sd_info context
 * @return 0 if OK<br>
 *         != 0 if error
 */
int sms_sd_lock(long lcsp, long lsd_info)
{
  sd_info_t *sd_info = (sd_info_t *) lsd_info;
  sd_like_t *SD;
  client_state_t *csp = (client_state_t *) lcsp;
  int ret;
  char *event = EVT_LOCKUPDATE;

  if (!sd_info || !csp)
  {
    return ERR_SD_FAILED;
  }

  SD = &(sd_info->SD);
  ret = lockSD(csp, SD->sd_cli_prefix, SD->sd_seqnum, event);

  return ret;
}

/** Unlock the SD
 * @param lcsp  pointer to csp context
 * @param lsd_info  pointer to sd_info context
 * @return 0 if OK<br>
 *         != 0 if error
 */
int sms_sd_unlock(long lcsp, long lsd_info)
{
  sd_info_t *sd_info = (sd_info_t *) lsd_info;
  sd_like_t *SD;
  client_state_t *csp = (client_state_t *) lcsp;
  int ret;
  char *event = EVT_UNLOCKUPDATE;

  if (!sd_info || !csp)
  {
    return ERR_SD_FAILED;
  }

  SD = &(sd_info->SD);
  ret = unlockSD(csp, SD->sd_cli_prefix, SD->sd_seqnum, event);

  return ret;
}

/** Force the asset on the SD
 * @param lcsp  pointer to csp context
 * @param lsd_info  pointer to sd_info context
 * @return 0 if OK<br>
 *         != 0 if error
 */
int sms_sd_forceasset(long lcsp, long lsd_info)
{
  sd_info_t *sd_info = (sd_info_t *) lsd_info;
  client_state_t *csp = (client_state_t *) lcsp;
  int ret;

  if (!sd_info || !csp)
  {
    return ERR_SD_FAILED;
  }

  ret = sdForceAsset(csp, sd_info->sdid);

  return ret;
}

/** Initialize provisioning status for a site
 * @param lcsp  pointer to csp context
 * @param lsd_info  pointer to sd_info context
 * @param num_stages    number of stages for the provisioning of that site
 * @return 0 if OK<br>
 *         sms error if failure
 */
int sms_bd_init_provstatus(long lcsp, long lsd_info, int num_stages, zval *lstage_names)
{
  zend_string *name;
  zval *row;
  zval *value;
  sd_info_t *sd_info = (sd_info_t *) lsd_info;
  sd_like_t *SD;
  client_state_t *csp = (client_state_t *) lcsp;
  int ret = SMS_OK;
  int i;
  char **stage_names;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if (!sd_info || !csp)
  {
    return ERR_SD_FAILED;
  }

  stage_names = (char **)calloc(num_stages, sizeof(char *));

  for (i = 0; i < num_stages; i++)
  {
    row = zend_hash_index_find(Z_ARRVAL_P(lstage_names), i);
    if (row != NULL)
    {
      name = zend_string_init("name", sizeof("name") - 1, 0);
      value = zend_hash_find(Z_ARRVAL_P(row), name);
      zend_string_release(name);
      if (value != NULL)
      {
        stage_names[i] = strdup(Z_STRVAL_P(value));
        GLogDEBUG(log_handle, 15, "Stage %d -> %s", i, stage_names[i]);
      }
    }

    if (row == NULL || value == NULL)
    {
      ret = ERR_LOCAL_PHP;
      break;
    }
  }

  if (ret == SMS_OK)
  {
    SD = &(sd_info->SD);
    if (csp->db_ctx == NULL)
    {
      csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
    }

    ret = HA_SMSSQL_InitProvStatus(&(csp->db_ctx->sql_ctx), SD->sd_cli_prefix, SD->sd_seqnum, num_stages, stage_names);
    if (ret != SMS_OK)
    {
      GLogERROR(log_handle, "Init Prov Status Failed: %s (%d)", get_error_message(ret), ret);
    }
  }

  for (i = 0; i < num_stages; i++)
  {
    freez(stage_names[i]);
  }
  freez(stage_names);

  return ret;
}

/** Set provisioning status for a provisioning stage
 * @param lcsp          pointer to csp context
 * @param lsd_info      pointer to sd_info context
 * @param stage_index   index of the current stage
 * @param status        Status of the stage 'N' 'W' 'E' or 'F'
 * @param error_id      when status = 'F'
 * @param next_status   Status of the next stage (or null)
 * @return 0 if OK<br>
 *         sms error if failure
 */
int sms_bd_set_provstatus(long lcsp, long lsd_info, int stage_index, char *status, int error_id, char *next_status, char *additionalmsg)
{
  sd_info_t *sd_info = (sd_info_t *) lsd_info;
  sd_like_t *SD;
  client_state_t *csp = (client_state_t *) lcsp;
  char *error_message = "";
  int ret;
  unsigned int error_code;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if (!sd_info || !csp || !additionalmsg)
  {
    return ERR_SD_FAILED;
  }

  SD = &(sd_info->SD);
  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
  }

  error_code = get_error_code_num(error_id);
  error_message = get_error_message(error_id);
  if (error_message == NULL)
  {
	error_message = "";
  }

  error_message = strdup(error_message);

  if (strlen(additionalmsg))
  {
	error_message = gpul_str_append(error_message, " [");
	error_message = gpul_str_append(error_message, additionalmsg);
	error_message = gpul_str_append(error_message, "]");
  }

  DbgWrite(DBG_INFO, "%s: sms_bd_set_provstatus: stage = %d  status = '%s'  next_status = '%s'  error = 0x%08x  msg= %s\n", sd_info->sdid, stage_index, status, next_status, error_code, error_message);

  if (next_status && next_status[0] == '\0')
  {
    next_status = NULL;
  }

  ret = HA_SMSSQL_SetProvStatus(&(csp->db_ctx->sql_ctx), SD->sd_cli_prefix, SD->sd_seqnum, stage_index, status, next_status, error_code, error_message);

  return ret;
}

/** Get provisioning status structure pointer for further use
 * @param lcsp          pointer to csp context
 * @param lsd_info      pointer to sd_info context
 * @param stage_index   index of the current stage
 * @return pointer on provisioning status structure
 */
long sms_bd_get_provstatus(long lcsp, long lsd_info, int stage_index)
{
  sd_info_t *sd_info = (sd_info_t *) lsd_info;
  sd_like_t *SD;
  client_state_t *csp = (client_state_t *) lcsp;
  sd_provstatus_like_t *provstatus;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if (!sd_info || !csp)
  {
    return 0;
  }

  SD = &(sd_info->SD);
  provstatus = (sd_provstatus_like_t *) malloc(sizeof(sd_provstatus_like_t));
  strcpy(provstatus->sdps_prefix, SD->sd_cli_prefix);
  provstatus->sdps_seqnum = SD->sd_seqnum;
  provstatus->sdps_stage_index = stage_index;

  /* get a new BD ctx */
  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
  }

  ret = HA_SMSSQL_GetProvStatus(&(csp->db_ctx->sql_ctx), provstatus);
  if (ret)
  {
    free(provstatus);
    return 0;
  }

  return (long) provstatus;
}

/** Get a field value for a provisioning status structure pointer
 * @param lprovstatus   pointer on provisioning status structure
 * @param field_name    name of the field to retrieve
 * @return field value
 */
char *sms_bd_get_provstatus_field_str(long lprovstatus, char *field_name)
{
  sd_provstatus_like_t *provstatus = (sd_provstatus_like_t *) lprovstatus;
  char *result;

  if (!provstatus)
  {
    return NULL;
  }

  if (!strcmp(field_name, "sdps_status"))
  {
    return strdup(provstatus->sdps_status);
  }
  if (!strcmp(field_name, "sdps_error_code"))
  {
    result = (char *) malloc(11);
    sprintf(result, "0x%08x", (uint) provstatus->sdps_error_code);
    return result;
  }
  if (!strcmp(field_name, "sdps_error_message"))
  {
    return strdup(provstatus->sdps_error_message);
  }
  return NULL;
}

/** Release provisioning status structure previsouly allocated by sms_bd_get_provstatus function
 * @param lprovstatus   pointer on provisioning status structure
 */
void sms_bd_free_provstatus(long lprovstatus)
{
  if (lprovstatus)
  {
    free((void *) lprovstatus);
  }
}

/** Set polling mode
 * @param lcsp          pointer to csp context
 * @param lsd_info      pointer to sd_info context
 * @param poll_mode     bit mask of polling mode
 * @param snmp_community SNMP community
 * @return 0 if OK<br>
 *         sms error if failure
 */
int sms_bd_set_poll_mode(long lcsp, long lsd_info, int poll_mode)
{
  sd_info_t *sd_info = (sd_info_t *) lsd_info;
  sd_like_t *SD;
  client_state_t *csp = (client_state_t *) lcsp;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if (!sd_info || !csp)
  {
    return ERR_SD_FAILED;
  }

  SD = &(sd_info->SD);
  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
  }

  DbgWrite(DBG_INFO, "sms_bd_set_poll_mode: mode = 0x%08x\n", poll_mode);

  ret = HA_SMSSQL_SetPollMode(&(csp->db_ctx->sql_ctx), SD->sd_cli_prefix, SD->sd_seqnum, poll_mode);

  return ret;
}

/** Set sd_ip_config field
 * @param lcsp          pointer to csp context
 * @param lsd_info      pointer to sd_info context
 * @param ip_addr       ip address
 * @return 0 if OK<br>
 *         sms error if failure
 */
int sms_bd_set_ipconfig(long lcsp, long lsd_info, char *ip_addr)
{
  sd_info_t *sd_info = (sd_info_t *) lsd_info;
  sd_like_t *SD;
  client_state_t *csp = (client_state_t *) lcsp;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if (!sd_info || !csp)
  {
    return ERR_SD_FAILED;
  }

  SD = &(sd_info->SD);
  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
  }

  DbgWrite(DBG_INFO, "sms_bd_set_ipconfig: ip = %s\n", ip_addr);

  ret = HA_SMSSQL_SetSDIPConfig(&(csp->db_ctx->sql_ctx), SD->sd_cli_prefix, SD->sd_seqnum, ip_addr, 1);

  return ret;
}

int sms_bd_reset_conf_objects(long lcsp, long lsd_info)
{
  sd_info_t *sd_info = (sd_info_t *) lsd_info;
  sd_like_t *SD;
  client_state_t *csp = (client_state_t *) lcsp;
  sd_crud_object_like_t crud_object;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if (!sd_info || !csp)
  {
    return ERR_SD_FAILED;
  }

  SD = &(sd_info->SD);
  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
  }

  DbgWrite(DBG_INFO, "sms_bd_reset_conf_objects\n");

  memset(&crud_object, 0, sizeof(crud_object));
  strncpy(crud_object.sd_cli_prefix, SD->sd_cli_prefix, CLI_PREFIX_LEN);
  crud_object.sd_seqnum = SD->sd_seqnum;

  ret = HA_SMSSQL_ResetSDCrudObjects(&(csp->db_ctx->sql_ctx), &crud_object);

  return ret;
}

int sms_bd_set_conf_objects(long lcsp, long lsd_info, char *name, char *value)
{
  sd_info_t *sd_info = (sd_info_t *) lsd_info;
  sd_like_t *SD;
  client_state_t *csp = (client_state_t *) lcsp;
  sd_crud_object_like_t crud_object;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if (!sd_info || !csp)
  {
    return ERR_SD_FAILED;
  }

  SD = &(sd_info->SD);
  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
  }

  DbgWrite(DBG_INFO, "sms_bd_set_conf_objects: name=%s    value=%s\n", name, value);

  memset(&crud_object, 0, sizeof(crud_object));
  strncpy(crud_object.sd_cli_prefix, SD->sd_cli_prefix, CLI_PREFIX_LEN);
  crud_object.sd_seqnum = SD->sd_seqnum;
  strncpy(crud_object.crud_name, name, CRUD_NAME_LEN);
  strncpy(crud_object.crud_value, value, CRUD_VALUE_LEN);

  ret = HA_SMSSQL_SetSDCrudObjects(&(csp->db_ctx->sql_ctx), &crud_object);

  return ret;

}

int sms_bd_delete_conf_objects(long lcsp, long lsd_info, char *name)
{
  sd_info_t *sd_info = (sd_info_t *) lsd_info;
  sd_like_t *SD;
  client_state_t *csp = (client_state_t *) lcsp;
  sd_crud_object_like_t crud_object;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if (!sd_info || !csp)
  {
    return ERR_SD_FAILED;
  }

  SD = &(sd_info->SD);
  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
  }

  DbgWrite(DBG_INFO, "sms_bd_delete_conf_objects: name=%s\n", name);

  memset(&crud_object, 0, sizeof(crud_object));
  strncpy(crud_object.sd_cli_prefix, SD->sd_cli_prefix, CLI_PREFIX_LEN);
  crud_object.sd_seqnum = SD->sd_seqnum;
  strncpy(crud_object.crud_name, name, CRUD_NAME_LEN);

  ret = HA_SMSSQL_DeleteSDCrudObjects(&(csp->db_ctx->sql_ctx), &crud_object);

  return ret;

}

int sms_bd_set_log(long lsd_info, zval *zlog)
{
  zend_string *name;
  zval *value;
  sd_info_t *sd_info = (sd_info_t *)lsd_info;
  fildelog_like_t fildelog;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  database_context_t *db_sms_ctx;

  memset(&fildelog, 0, sizeof(fildelog));

  strncpy(fildelog.cli_prefix, sd_info->SD.sd_cli_prefix, CLI_PREFIX_LEN);
  fildelog.seqnum = sd_info->SD.sd_seqnum;

  name = zend_string_init("log_msg", sizeof("log_msg") - 1, 0);
  value = zend_hash_find(Z_ARRVAL_P(zlog), name);
  zend_string_release(name);
  if (value != NULL)
  {
    strncpy(fildelog.log_msg, Z_STRVAL_P(value), sizeof(fildelog.log_msg) - 1);
    GLogDEBUG(log_handle, 15, "log_msg [%s]", fildelog.log_msg);
  }

  name = zend_string_init("log_level", sizeof("log_level") - 1, 0);
  value = zend_hash_find(Z_ARRVAL_P(zlog), name);
  zend_string_release(name);
  if (value != NULL)
  {
    fildelog.log_level = strtol(Z_STRVAL_P(value), NULL, 0) % 8;
    GLogDEBUG(log_handle, 15, "log_level [%d]", fildelog.log_level);
  }

  name = zend_string_init("log_reference", sizeof("log_reference") - 1, 0);
  value = zend_hash_find(Z_ARRVAL_P(zlog), name);
  zend_string_release(name);
  if (value != NULL)
  {
    strncpy(fildelog.log_reference, Z_STRVAL_P(value), sizeof(fildelog.log_reference) - 1);
    GLogDEBUG(log_handle, 15, "log_reference [%s]", fildelog.log_reference);
  }

  db_sms_ctx = sms_db_find_ctx(NULL, DB_SMS);

  ret = set_log_by_id(db_sms_ctx, &fildelog);

  sms_db_release_ctx(db_sms_ctx);

  if (ret == ERR_DB_FAILED || ret == ERR_DB_NOTFOUND)
  {
    if (ret == ERR_DB_FAILED)
    {
      GLogERROR(log_handle, " Cannot write log FAILED");
    }
    else
    {
      GLogERROR(log_handle, " SD NOT FOUND in the database");
    }
  }

  return SMS_OK;
}

/**
 * @}
 */
