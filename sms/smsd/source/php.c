/*
 * Copyright UBIqube Solutions 2008
 * @file php.c
 * Creation Date: Jun 16, 2008
 */

#include <stdio.h>
#include <pthread.h>

#include "sms_php.h"
#include "sms_php_functions_db.h"
#include "sms_php_functions_misc.h"
#include "sms_php_wrappers.h"

#include "log.h"
#include "smsd.h"
#include "sms_db.h"
#include "router.h"
#include "gpul.h"
#include "php_sdinfo.h"
#include "php_misc.h"
#include "php_db.h"
#include "pattern.h"


/*
 * SMS functions to call from scripts
 */

/* MISC */
PHP_v_s(sms_log_error);
PHP_v_s(sms_log_info);
PHP_v_is(sms_log_debug);

/* Specific smsd */
/* SDINFO */
PHP_i_i(sms_sd_get_config_mode_from_sdinfo);

/* DATABASE */
PHP_i_ii(sms_sd_prov_lock);
PHP_i_ii(sms_sd_prov_unlock);
PHP_i_ii(sms_sd_lock);
PHP_i_ii(sms_sd_unlock);
PHP_i_ii(sms_sd_forceasset);
PHP_i_iiih(sms_bd_init_provstatus);
PHP_i_iiisiss(sms_bd_set_provstatus);
PHP_i_iii(sms_bd_get_provstatus);
PHP_sf_is(sms_bd_get_provstatus_field_str);
PHP_v_i(sms_bd_free_provstatus);
PHP_i_iii(sms_bd_set_poll_mode);
PHP_i_iis(sms_bd_set_ipconfig);
PHP_i_ii(sms_bd_reset_conf_objects);
PHP_i_iiss(sms_bd_set_conf_objects);
PHP_i_iis(sms_bd_delete_conf_objects);
PHP_i_ih(sms_bd_set_log);

/* GENERIC DATABASE */
PHP_i_ishh(sms_store_in_database);
PHP_i_ish(sms_delete_in_database);
PHP_i_s(sms_find_db_context);
PHP_v_i(sms_release_db_context);

/* MISC */
PHP_v_iss(sms_reply_to_user);
PHP_v_iss(sms_send_user_message);
PHP_sf_sss(sms_user_message_add);
PHP_sf_sss(sms_user_message_add_json);
PHP_sf_sss(sms_user_message_add_array);
PHP_sf_ss(sms_user_message_array_add);
PHP_v_iss(sms_send_user_ok);
PHP_v_isss(sms_send_user_ok_ex);
PHP_v_issi(sms_send_user_error_ex);
PHP_v_issi(sms_send_user_error);
PHP_v_i(sms_close_user_socket);
PHP_v_isisss(sms_set_update_status);

static zend_function_entry php_sms_functions[] = {

    /* SDINFO */
    PHP_FE(sms_sd_get_config_mode_from_sdinfo, NULL)

    /* DATABASE */
    PHP_FE(sms_sd_prov_unlock, NULL)
    PHP_FE(sms_sd_prov_lock, NULL)
    PHP_FE(sms_sd_lock, NULL)
    PHP_FE(sms_sd_unlock, NULL)
    PHP_FE(sms_sd_forceasset, NULL)
    PHP_FE(sms_bd_init_provstatus, NULL)
    PHP_FE(sms_bd_set_provstatus, NULL)
    PHP_FE(sms_bd_get_provstatus, NULL)
    PHP_FE(sms_bd_get_provstatus_field_str, NULL)
    PHP_FE(sms_bd_free_provstatus, NULL)
    PHP_FE(sms_bd_set_poll_mode, NULL)
    PHP_FE(sms_bd_set_ipconfig, NULL)
    PHP_FE(sms_bd_reset_conf_objects, NULL)
    PHP_FE(sms_bd_set_conf_objects, NULL)
    PHP_FE(sms_bd_delete_conf_objects, NULL)
    PHP_FE(sms_bd_set_log, NULL)

    /* MISC */
    PHP_FE(sms_log_error, NULL)
    PHP_FE(sms_log_info, NULL)
    PHP_FE(sms_log_debug, NULL)
    PHP_FE(sms_reply_to_user, NULL)
    PHP_FE(sms_send_user_message, NULL)
    PHP_FE(sms_user_message_add, NULL)
    PHP_FE(sms_user_message_add_json, NULL)
    PHP_FE(sms_user_message_add_array, NULL)
    PHP_FE(sms_user_message_array_add, NULL)
    PHP_FE(sms_send_user_ok, NULL)
    PHP_FE(sms_send_user_ok_ex, NULL)
    PHP_FE(sms_send_user_error_ex, NULL)
    PHP_FE(sms_send_user_error, NULL)
    PHP_FE(sms_close_user_socket, NULL)
    PHP_FE(sms_set_update_status, NULL)

    /* GENERIC DATABASE */
    PHP_FE(sms_store_in_database, NULL)
    PHP_FE(sms_delete_in_database, NULL)
    PHP_FE(sms_find_db_context, NULL)
    PHP_FE(sms_release_db_context, NULL)

    {NULL, NULL, NULL}
};


/*************************************
    SMS EXTENSION MODULE FOR PHP
 *************************************/

zend_module_entry sms_php_module_entry =
{
    STANDARD_MODULE_HEADER,
    .name                  = "sms",
    .functions             = php_sms_functions,
    .module_startup_func   = PHP_MINIT(sms),
    .module_shutdown_func  = NULL,
    .request_startup_func  = NULL,
    .request_shutdown_func = NULL,
    .info_func             = NULL,
    .version               = "16.2",
    STANDARD_MODULE_PROPERTIES
};


/* register config variables in _SERVER superglobal */
void sms_sapi_php_register_variables(zval *track_vars_array TSRMLS_DC)
{
  char buf[64];
  pthread_t thread_id = pthread_self();

  // php_import_environment_variables(track_vars_array);

  /**
   * @ingroup globalvariables
   * @{
   */

  /*! @var $_SERVER['DOMAIN']   MSA Domain name */
  php_register_variable("DOMAIN", currentConfig->domain, track_vars_array TSRMLS_CC);
  php_register_variable("IP_SMS_SYSLOG", currentConfig->syslogArray[syslog_master].syslog_add_ip, track_vars_array TSRMLS_CC);
  php_register_variable("SMS_NETWORK_IP", currentConfig->smsArray[sms_master].sms_network_ip, track_vars_array TSRMLS_CC);
  php_register_variable("SMS_MASK_IP", currentConfig->smsArray[sms_master].sms_mask_ip, track_vars_array TSRMLS_CC);
  php_register_variable("SMS_NETWORK_IP_DISASTER", currentConfig->smsArray[sms_disaster].sms_network_ip, track_vars_array TSRMLS_CC);
  php_register_variable("SMS_MASK_IP_DISASTER", currentConfig->smsArray[sms_disaster].sms_mask_ip, track_vars_array TSRMLS_CC);
  php_register_variable("TFTP_BASE", currentConfig->tftp_base, track_vars_array TSRMLS_CC);
  php_register_variable("SMS_ADDRESS_IP", currentConfig->smsArray[sms_master].sms_address_ip, track_vars_array TSRMLS_CC);
  php_register_variable("SMS_ADDRESS_IPV6", currentConfig->smsArray[sms_master].sms_address_ipv6, track_vars_array TSRMLS_CC);
  php_register_variable("NTP_SERVER", currentConfig->ntpserver, track_vars_array TSRMLS_CC);
  php_register_variable("GENERATED_CONF_BASE", "/opt/sms/spool/routerconfigs", track_vars_array TSRMLS_CC);
  php_register_variable("DNS_DEFAULT", currentConfig->dns_default, track_vars_array TSRMLS_CC);
  php_register_variable("UBI_VSOC_NAME", currentConfig->ubi_vsoc_name, track_vars_array TSRMLS_CC);
  php_register_variable("UBI_CENTRALIZED_NODE_NAME", currentConfig->ubi_centralized_node_name, track_vars_array TSRMLS_CC);
  php_register_variable("NOTIF_MAIL_ADDRESS", currentConfig->notif_mail_address, track_vars_array TSRMLS_CC);
  php_register_variable("NTP_BACKUP_SERVER", currentConfig->ntpbackupserver, track_vars_array TSRMLS_CC);
  php_register_variable("FMC_REPOSITORY", currentConfig->fmc_repository, track_vars_array TSRMLS_CC);
  php_register_variable("FMC_ENTITIES2FILES", currentConfig->fmc_entities2files, track_vars_array TSRMLS_CC);
  php_register_variable("SCP_USERNAME", currentConfig->scp_user, track_vars_array TSRMLS_CC);
  php_register_variable("SCP_PASSWORD", currentConfig->scp_pass, track_vars_array TSRMLS_CC);

  sprintf(buf, "%d", currentConfig->local_server_synchro_timeout);
  php_register_variable("LOCAL_SERVER_SYNCHRO_TIMEOUT", buf, track_vars_array TSRMLS_CC);
  php_register_variable("NODE_NAME", currentConfig->node_name, track_vars_array TSRMLS_CC);

  snprintf(buf, 16, "%lu", thread_id);
  php_register_variable("THREAD_ID", buf, track_vars_array TSRMLS_CC);
  snprintf(buf, 16, "%d", currentConfig->debug);
  php_register_variable("DEBUG_LEVEL", buf, track_vars_array TSRMLS_CC);

  /**
   * @}
   */
}
