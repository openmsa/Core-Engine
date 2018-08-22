/** @file
 Header file of the SMS SQL Library.
 */

#ifndef __SMSSQL_H_
#define __SMSSQL_H_

#include <sql.h>
#include <sqlext.h>
#include "sms_error.h"
#include "log.h"
#include "gpul.h"

typedef struct sql_handles
{
  SQLHDBC   hdbc;
  void *    hash_hstmt;
  char *    schema_owner;
} sql_handles;


#define sql_context sql_handles *


#define LOG_MESG_LEN        1000

#define MAXHOSTSIZE 16    /* au moins (taille de cli_prefix (3) + taille max d'un int (10) + fin de chaine (1)) */

#define PHONE_NUM_LEN           20
#define CLI_PREFIX_LEN          3
#define PUBLIC_KEY_LEN          40
#define SERIAL_NUM_LEN          50
#define LOG_PASSWD_LEN          50
#define ONE_CHAR_ORACLE         1
#define IP_ADD_MASK_LEN         16
#define IPV6_ADDR_LEN           39
#define FIRST_LAST_NAME_LEN     50
#define MANUFACTURER_LEN        50
#define MODEL_LEN               50
#define VERSION_LEN             256
#define REG_TOPO_LEN            1
#define REG_CA_NAME             128
#define ENCRYPTION_LEVEL_LEN    10
#define ENCAP_LEN               256
#define SHARED_KEY_LEN          256
#define FIRMWARE_LEN            255
#define DATE_LEN                21
#define TIMESTAMP_LEN           19
#define SHA1_LEN                40
#define ERROR_MESSAGE_LEN       4000
#define FW_APPLICATION_LEN      256
#define SD_OS_VERSION_LEN       256
#define URL_NAME_LEN            256
#define CAT_NAME_LEN            256
#define ASR_ADDR_LEN            256
#define DOMAIN_LEN              512
#define HOSTNAME_LEN            50
#define SD_SNMP_COMMUNITY_LEN   64
#define SNMP_NAME_LEN           50
#define SNMPV3_SEC_NAME_LEN     255
#define SNMPV3_SEC_LEVEL_LEN    32
#define SNMPV3_AUTH_PHRASE_LEN  255
#define SNMPV3_AUTH_TYPE_LEN    32
#define SNMPV3_PRIV_PHRASE_LEN  255
#define SNMPV3_PRIV_TYPE_LEN    32
#define SNMPV3_ENGINE_ID_LEN    255
#define OID_LEN                 1408
#define DS_LEN                  256
#define FTP_PASSWD_LEN          50
#define SDPS_ERROR_MESSAGE_LEN  256
#define SUB_INTERFACE_LEN       4
#define SDID_LEN                11
#define FWR_DESC_LEN            64
#define FW_CHECKSUM_LEN         64
#define NETZONE_LEN             40
#define SD_EXTERNAL_REFERENCE_LEN 255
#define BEGINTIME_LEN           10
#define ENDINGTIME_LEN          10
#define TIME_LEN                5
#define ACTIONNAME_LEN          50
#define SD_TIMEZONE_LEN         16

#define NODE_NAME_LEN           50

#define DPID_LEN                23

#define RTE_VARRAY_SIZE         128

#define DSCP_LEN                5

#define LOG_REFERENCE_LEN       100
#define LOG_SIGNATURE_LEN       100

#define ALREADY_LOCKED          0
#define OK_LOCKED               1
#define OK_LOCKED_BY_TIMEOUT    2
#define ALREADY_PROVLOCKED      3

#define CONTACT_EMAIL_LEN       300
#define SIT_NOM_LEN             100
#define SIT_EXTERNAL_REFERENCE  25

#define SD_ASSET_ATTRIBUTE_NAME_LEN 	100
#define SD_ASSET_ATTRIBUTE_VALUE_LEN    256

#define SD_EXTENDED_ATTRIBUTE_NAME_LEN     200
#define SD_EXTENDED_ATTRIBUTE_TYPE_LEN     30
#define SD_EXTENDED_ATTRIBUTE_VALUE_LEN    4000

#define DCL_MSG_LEN             1000
#define DCL_DESCRIBE_LEN        4000
#define DCL_RECOMAND_ACTION_LEN 4000
#define DCL_EXTERNAL_REF_LEN    500

#define CONFIGVAR_NAME_LEN      128
#define CONFIGVAR_VALUE_LEN     4000
#define CONFIGVAR_COMMENT_LEN   256
#define CONFIGVAR_TYPE_LEN      30

#define EXT_ATTR_NAME_LEN       128
#define EXT_ATTR_VALUE_LEN      4000
#define EXT_ATTR_COMMENT_LEN    256
#define EXT_ATTR_TYPE_LEN       30

#define CRUD_NAME_LEN           200
#define CRUD_VALUE_LEN          4000
#define CRUD_TYPE_LEN           30

#define FAILURE_MESSAGE_LEN     4000
#define UPD_STATUS_LEN          20
#define UPD_TYPE_LEN            50

#define DOMAIN_NAME_LEN         253
#define ABO_NAME_LEN  100

#define PARAM_NAME_LEN  64
#define PARAM_TYPE_LEN  30
#define PARAM_VALUE_LEN 4000
#define PARAM_COMMENT_LEN   256

#define GES_NOM_LEN     50
#define GES_PRENOM_LEN  50
#define GES_EMAIL_LEN   300
#define PFL_TYPE_LEN  30
#define STAGE_NAME_LEN       100

/*
 * constants for router manufacturer
 */

#define CISCO           1
#define GENERIC         14
#define LINUX           15
#define NETSCREEN       16
#define FORTINET        17
#define JUNIPER         18
#define BLUECOAT        19
#define ISS             20
#define CHECKPOINT      21
#define NETAPP          22
#define NETASQ          23
#define ONEACCESS       25
#define DELL            26
#define SDN             27
#define PALOALTO        28
#define STORMSHIELD     16010401



#define PING_DB_SZ  100
typedef struct ping_db_state {
    char sd_prefix[CLI_PREFIX_LEN + 1];
    long sd_seqnum;
    long sd_pingtime;
    int sd_is_up;
} ping_db_state_t;


/**
 * \struct sd_like_t
 * Structure corresponding to the schema of table SD in the SMS database
 **/
typedef struct sd_like_t
{
  /*! Customer prefix (Part of the key)       */char sd_cli_prefix[CLI_PREFIX_LEN + 1];
  /*! Sequence Number (Part of the key)       */long sd_seqnum;
  /*! SDID                                    */char sdid[SDID_LEN + 1];
  /*! SD Serial Number                        */char sd_serial_number[SERIAL_NUM_LEN + 1];
  /*! Login                                   */char sd_login_entry[LOG_PASSWD_LEN + 1];
  /*! Password                                */char sd_passwd_entry[LOG_PASSWD_LEN + 1];
  /*! SD Type ('S' : soft, 'H' : Hard)        */char sd_type[ONE_CHAR_ORACLE + 1];
  /*! IP Configuration                        */char sd_ip_config[IPV6_ADDR_LEN + 1];
  /*! SD updated status                       */int sd_updated;
  /*! Bool SD connected status                */int sd_connected;
  /*! Firmware Number                         */char sd_firmware[FIRMWARE_LEN + 1];
  /*! Administration login                    */char sd_login_adm[LOG_PASSWD_LEN + 1];
  /*! Administration Password                 */char sd_passwd_adm[LOG_PASSWD_LEN + 1];
  /*! Bool for Permanent connection           */int sd_permanent_connection;
  /*! Provisioning lock                       */int sd_prov_lock;
  /*! SD configuration step                   */int sd_config_step;
  /*! SD Operator IP address                  */char sd_cli_ip_addr[IP_ADD_MASK_LEN + 1];
  /*! SD Operator IP mask                     */char sd_cli_ip_mask[IP_ADD_MASK_LEN + 1];
  /*! SD manufacturer id                      */int man_id;
  /*! SD model id                             */int mod_id;
  /*! SD FW Profile id                        */long pfl_id;
  /*! SD HSRP type
   *  0 : Not an    HSRP SD
   *  1 : Primary   HSRP SD
   *  2 : secondary HSRP SD                   */int sd_hsrp_type;
  /*! SD HSRP partner id                      */long sd_hsrp_partner_id;
  /*! sd Backup connectivity                  */char sd_backup_connectivity[ONE_CHAR_ORACLE + 1];
  /*! Alert possible for SD (not direct)      */int sd_log_alert;
  /*! sd Alert Mail                           */int sd_alert_mail;
  /*! sd Log Flag                             */int sd_log;
  /*! sd Log more Flag                        */int sd_log_more;
  /*! ips_expiration                          */char sd_ips_expiration[DATE_LEN + 1];
  /*! sd_av_expiration                        */char sd_av_expiration[DATE_LEN + 1];
  /*! sd_url_expiration                       */char sd_url_expiration[DATE_LEN + 1];
  /*! sd_as_expiration                        */char sd_as_expiration[DATE_LEN + 1];
  /*! sd_log_report                           */int sd_log_report;
  /*! Type de CPU                             */char sd_cpu[MODEL_LEN + 1];
  /*! Memoire                                 */char sd_memory[MODEL_LEN + 1];
  /*! No version IPS                          */char sd_ips_version[VERSION_LEN + 1];
  /*! No version Antivirus                    */char sd_av_version[VERSION_LEN + 1];
  /*! No version AntiSpam                     */char sd_as_version[VERSION_LEN + 1];
  /*! No version Filtre URL                   */char sd_url_version[VERSION_LEN + 1];
  /*! Modele du boitier                       */char sd_hardware_model[MODEL_LEN + 1];
  /*! Numero de license                       */char sd_hardware_license[MODEL_LEN + 1];
  /*! Interface name                          */char ext_int_name[FIRST_LAST_NAME_LEN + 1];
  /*! int physical name                       */char ext_int_physical_name[FIRST_LAST_NAME_LEN + 1];
  /*! sd_asset_update                         */int sd_asset_update;
  /*! hostname (monitoring only)              */char sd_hostname[HOSTNAME_LEN + 1];
  /*! communaute (monitoring only)            */char sd_snmp_community[SD_SNMP_COMMUNITY_LEN + 1];
  /*! monitor pfl id                          */int sd_monitor_pflid;
  /*! ftp server password                     */char sd_ftp_passwd[FTP_PASSWD_LEN + 1];
  /*! polling mode                            */unsigned long sd_poll_mode;
  /*! ftp activation                          */int sd_ftp_activated;
  /*! subscriber id						      */long sd_abo_id;
  /*! External reference                      */char sd_external_reference[SD_EXTERNAL_REFERENCE_LEN + 1];
  /*! Ping time                               */long sd_pingtime;
  /*! configuration profile ID                */ long sd_configuration_pflid;
  /*! mngt port                               */ long sd_management_port;
  /*! spare mngt port                         */ long sd_management_port_fallback;
  /*! mntr port                               */ long sd_monitoring_port;
  /*! node ip address                         */ char sd_node_ip_addr [IP_ADD_MASK_LEN + 1];
  /*! node name                               */ char sd_node_name [NODE_NAME_LEN + 1];
  /*! configuration use ipv6                  */ int  sd_conf_isipv6;
  /*! dpip for openflow devices               */ char sd_dpid [ DPID_LEN + 1 ];
  /*! abo External reference                  */char sd_abo_external_ref [SD_EXTERNAL_REFERENCE_LEN + 1];
  /*! Timezone from CRUD                      */ char sd_timezone [ SD_TIMEZONE_LEN + 1 ];
} sd_like_t;


/**
 * \struct sd_asset_like_t
 * Structure corresponding to the schema of table SD in the SMS database
 **/
typedef struct sd_asset_like_t
{
  /*! Customer prefix         */char sd_cli_prefix[CLI_PREFIX_LEN + 1];
  /*! Sequence Number         */long sd_seqnum;
  /*! SD Serial Number        */char sd_serial_number[SERIAL_NUM_LEN + 1];
  /*! Modele du boitier       */char sd_hardware_model[MODEL_LEN + 1];
  /*! Type de CPU             */char sd_cpu[MODEL_LEN + 1];
  /*! Memoire                 */char sd_memory[MODEL_LEN + 1];
  /*! Firmware ID             */char sd_firmware[FIRMWARE_LEN + 1];
  /*! Numero de license       */char sd_hardware_license[MODEL_LEN + 1];
  /*! No version IPS          */char sd_ips_version[VERSION_LEN + 1];
  /*! Date expiration IPS     */char sd_ips_expiration[DATE_LEN + 1];
  /*! No version Antivirus    */char sd_av_version[VERSION_LEN + 1];
  /*! Date expiration AV      */char sd_av_expiration[DATE_LEN + 1];
  /*! No version AntiSpam     */char sd_as_version[VERSION_LEN + 1];
  /*! Date expiration AS      */char sd_as_expiration[DATE_LEN + 1];
  /*! No version Filtre URL   */char sd_url_version[VERSION_LEN + 1];
  /*! Date expiration URLF    */char sd_url_expiration[DATE_LEN + 1];
} sd_asset_like_t;

typedef struct sd_licexpiration_like_t
{
  /*! Customer prefix         */char sd_cli_prefix[CLI_PREFIX_LEN + 1];
  /*! Sequence Number         */long sd_seqnum;
  /*! Date expiration AS      */char sd_expiration_date[DATE_LEN + 1];
  /*! Date expiration URLF    */int sd_licisexpired;
} sd_licexpiration_like_t;


/**
 * \struct sd_interface_like_t
 * structure corresponding to the schema of table SD_INTERFACE in the SMS database
 **/
typedef struct sd_interface_like_t
{
  /*! Customer prefix         */char sd_cli_prefix[CLI_PREFIX_LEN + 1];
  /*! Sequence Number         */long sd_seqnum;
  /*! Interface type          */
  /*! I = ISDN                */
  /*! N =                     */
  /*! P = PSTN (RTC)          */
  /*! F = FRAME RELAY         */
  /*! A = ADSL, E=Ethernet    */char int_type[ONE_CHAR_ORACLE + 1];
  /*! Connectivity side       */
  /*! I internal E=external   */char int_connectivity_side[ONE_CHAR_ORACLE + 1];
  /*! Interface name          */char int_name[FIRST_LAST_NAME_LEN + 1];
  /*! IP_TYPE                 */char int_ip_type[ONE_CHAR_ORACLE + 1];
  /*! IP @                    */char int_ip_addr[IP_ADD_MASK_LEN + 1];
  /*! IP Mask                 */char int_ip_mask[IP_ADD_MASK_LEN + 1];
  /*! IP gateway              */char int_ip_gw[IP_ADD_MASK_LEN + 1];
  /*! First DNS               */char int_dns_1[IP_ADD_MASK_LEN + 1];
  /*! Second DNS              */char int_dns_2[IP_ADD_MASK_LEN + 1];
  /*! int physical name       */char int_physical_name[FIRST_LAST_NAME_LEN + 1];
  /*! bandwidth in Kb/s       */unsigned long int_bandwidth;
  /*! Interface Secure level  */int int_security_level;
  /*! Interface VLAN          */int int_vlan;
  /*! Booleen public/prive    */int int_ispublic;
  /*! Sub interface           */char int_sub_interface[SUB_INTERFACE_LEN + 1];
  /*! Order number            */int int_ordernum;
  /*! domain name             */char int_domain_name[ DOMAIN_NAME_LEN + 1 ];
  /*! Interface IpV6 address  */char int_ipv6_addr [ IPV6_ADDR_LEN +1 ];
  /*! Interface use IPv6      */int int_use_ipv6 ;
  /*! Interface IpV6 mask     */int int_ipv6_mask ;
} sd_interface_like_t;


typedef struct fildelog_like_t
{
  char sdid[SDID_LEN + 1];
  char cli_prefix[CLI_PREFIX_LEN + 1];
  long seqnum;
  char log_msg[LOG_MESG_LEN + 1];
  int log_level;
  char log_reference[LOG_REFERENCE_LEN + 1];
} fildelog_like_t;


typedef struct sd_events_like_t
{
  char log_prefix[CLI_PREFIX_LEN + 1];
  long log_seqnum;
  long log_pflid;
  long log_type;
  long nb_log;
} sd_events_like_t;

typedef struct snmp_polling_like
{
  /*! snmp pfl id             */long snmp_pfl_id;
  /*! snmp id                 */long snmp_id;
  /*! snmp counter name       */char snmp_name[SNMP_NAME_LEN + 1];
  /*! snmp counter oid        */char snmp_oid[OID_LEN + 1];
  /*!----------------------------------------------------------------------------------------------*/
  /*! snmp data storage (rrd)          */
  /*! syntax: 'Type:360:MinVal:MaxVal' */
  /*! Type: 'GAUGE' or 'COUNTER'       */
  /*! MinVal: numeric or 'U'           */
  /*! MaxVal: numeric or 'U'           */
  /*!                         */char snmp_ds[DS_LEN + 1];
  /*!----------------------------------------------------------------------------------------------*/
  /*! snmp comparator sign    */
  /*!   '<', '>'              */char snmp_comparator[ONE_CHAR_ORACLE + 1];
  /*!----------------------------------------------------------------------------------------------*/
  /*! snmp threshold          */long snmp_threshold;
  /*! snmp alert period (sec) */long snmp_alert_period;
  /*! snmp polling period (sec) */long snmp_polling_period;
} snmp_polling_like_t;

typedef struct sd_provstatus_like
{
  /*! */char sdps_prefix[CLI_PREFIX_LEN + 1];
  /*! */long sdps_seqnum;
  /*! */unsigned long sdps_stage_index;
  /*! */char sdps_status[ONE_CHAR_ORACLE + 1];
  /*! */unsigned long sdps_error_code;
  /*! */char sdps_error_message[SDPS_ERROR_MESSAGE_LEN + 1];
} sd_provstatus_like_t;

typedef struct sd_abologs_like_t
{
  long abo_id;
  char log_prefix[CLI_PREFIX_LEN + 1];
  long log_type;
  long nb_log;
} sd_abologs_like_t;


/**
 ** \struct alarm_like_t
 ** structure corresponding to the schema of table ALERTMAIL in the SMS database
 ***/
typedef struct alarm_like
{
  /*!                                                   */char sd_cli_prefix[CLI_PREFIX_LEN + 1];
  /*!                                                   */long sd_seqnum;
  /*! SDID                                              */char sdid[SDID_LEN + 1];
  /*! Subscriber id                                     */long sd_abo_id;
  /*! IP Configuration                                  */char sd_ip_config[IPV6_ADDR_LEN + 1];
  /*! Alarm ID                                          */long mal_id;
  /*! Alarm date                                        */char mal_date[DATE_LEN + 1];
  /*! Log level (syslog)                                */int log_level;
  /*! Log message                                       */char log_mesg[LOG_MESG_LEN + 1];
  /*! Log reference                                     */char log_reference[LOG_REFERENCE_LEN + 1];
  /*! Log signature                                     */char log_signature[LOG_SIGNATURE_LEN + 1];
  /*! Manufacturer ID                                   */long man_id;
  /*! Model ID                                          */long mod_id;
  /*! Log type                                          */long log_type;
  /*! profil ID                                         */long mal_pfl_id;
  /*! Manufacturer name                                 */char man_name[MANUFACTURER_LEN + 1];
  /*! Model name                                        */char model_name[MODEL_LEN + 1];
  /*! Site name                                         */char sit_nom[SIT_NOM_LEN + 1];
  /*! Alarm descr message                               */char dcl_msg[DCL_MSG_LEN + 1];
  /*! Alarm descr describe                              */char dcl_describe[DCL_DESCRIBE_LEN + 1];
  /*! Alarm descr recommanded action                    */char dcl_recomand_action[DCL_RECOMAND_ACTION_LEN + 1];
  /*! Alarm descr external reference                    */char dcl_external_ref[DCL_EXTERNAL_REF_LEN + 1];
  /*! true if log is disabled                           */int log_disable;
} alarm_like_t;

/**
 ** \struct alarm_rule_t
 ** structure to get the rules used to search ElasticSearch for alarms
 ***/
typedef struct alarm_rule
{
    long actor_id;
    char alarm_name[SD_EXTENDED_ATTRIBUTE_VALUE_LEN + 1];
    char alarm_rule[SD_EXTENDED_ATTRIBUTE_VALUE_LEN + 1];
    char alarm_criteria[SD_EXTENDED_ATTRIBUTE_VALUE_LEN + 1];
} alarm_rule_t;

typedef struct manager_info
{
    long abo_id;
    char first_name[GES_PRENOM_LEN + 1];
    char last_name[GES_NOM_LEN + 1];
    char email[GES_EMAIL_LEN + 1];
    long role;
} manager_info_t;

/**
 ** \struct sd_asset_attribute_like_t
 ** structure corresponding to the schema of table ALERTMAIL in the SMS database
 ***/
typedef struct sd_asset_attribute_like
{
  char sd_cli_prefix[CLI_PREFIX_LEN + 1];
  long sd_seqnum;
  char atr_name[SD_ASSET_ATTRIBUTE_NAME_LEN + 1];
  char atr_value[SD_ASSET_ATTRIBUTE_VALUE_LEN + 1];
  char atr_lastupdate[DATE_LEN + 1];
  int atr_assettype;
} sd_asset_attribute_like_t;

/**
 ** \struct sd_configvar_like_t
 ** structure corresponding to the schema of table SD_CONFIGVAR in the SMS database
 ***/
typedef struct sd_configvar_like
{
/*! cli prefix                       */ char sd_cli_prefix  [ CLI_PREFIX_LEN +1 ];
/*! SD sequence number               */ long sd_seqnum;
/*! configuration variable name      */ char var_name    [ CONFIGVAR_NAME_LEN +1 ];
/*! configuration variable value     */ char var_value   [ CONFIGVAR_VALUE_LEN +1 ];
/*! configuration variable comment   */ char var_comment [ CONFIGVAR_COMMENT_LEN +1 ];
/*! configuration variable type      */ char var_type    [ CONFIGVAR_TYPE_LEN    +1 ];
} sd_configvar_like_t;

/**
 ** \struct sd_extended_attr_like_t
 ** structure corresponding to the schema of table SD_EXTENDED_ATTRIBUTE in the SMS database
 ***/
typedef struct sd_ext_attr_like
{
/*! cli prefix                       */ char sd_cli_prefix  [ CLI_PREFIX_LEN +1 ];
/*! SD sequence number               */ long sd_seqnum;
/*! extended attribute name          */ char var_name    [ EXT_ATTR_NAME_LEN +1 ];
/*! extended attribute value         */ char var_value   [ EXT_ATTR_VALUE_LEN +1 ];
/*! extended attribute comment       */ char var_comment [ EXT_ATTR_COMMENT_LEN +1 ];
/*! extended attribute type          */ char var_type    [ EXT_ATTR_TYPE_LEN    +1 ];
} sd_ext_attr_like_t;

/**
 ** \struct sd_crud_object_like_t
 ** structure corresponding to the schema of table SD_CRUD_OBJECT in the SMS database
 ***/
typedef struct sd_crud_object_like
{
/*! cli prefix                       */ char sd_cli_prefix  [ CLI_PREFIX_LEN +1 ];
/*! SD sequence number               */ long sd_seqnum;
/*! configuration variable name      */ char crud_name      [ CRUD_NAME_LEN +1 ];
/*! configuration variable value     */ char crud_value     [ CRUD_VALUE_LEN +1 ];
/*! configuration variable type      */ char crud_type      [ CRUD_TYPE_LEN +1 ];
} sd_crud_object_like_t;

typedef struct sd_updatestatus_like
{
/*! cli prefix                       */ char sd_cli_prefix[CLI_PREFIX_LEN + 1];
/*! SD sequence number               */ long sd_seqnum;
/*! update type
 * LICENSE
 * FIRMWARE
                                     */ char upd_type[UPD_TYPE_LEN + 1];
/*! update status
 * WORKING
 * ENDED
 * FAILED
 * NOTPROCESSED
                                     */ char upd_status[UPD_STATUS_LEN + 1];
/*! last update date                 */ char upd_lastdate[DATE_LEN + 1];
/*! update failure message           */ char upd_failure_message[FAILURE_MESSAGE_LEN + 1];
} sd_updatestatus_like_t;

#define DSV_NAME_LEN 50
#define DSV_COMMENT_LEN 4000
#define DSV_TYPE_LEN 30
#define DSV_SSH_LOGIN_LEN 50
#define DSV_SSH_PASSWD_LEN 50
#define DSV_FTP_LOGIN_LEN 50
#define DSV_FTP_PASSWD_LEN 50
#define DSV_FTP_HOMEDIR_LEN 250
#define DSV_TFTP_HOMEDIR_LEN 250
#define DSV_PUBLIC_IP_LEN 39
#define DSV_MAINTENANCE_IP_LEN 39
#define SBN_NETWORK_IP_LEN 39
#define SBN_NETWORK_MASK_LEN 39

typedef struct dedicated_server_like
{
    char dsv_name[DSV_NAME_LEN + 1];
    char dsv_comment[DSV_COMMENT_LEN + 1];
    char dsv_type[DSV_TYPE_LEN + 1];
    char dsv_ssh_login[DSV_SSH_LOGIN_LEN + 1];
    char dsv_ssh_passwd[DSV_SSH_PASSWD_LEN + 1];
    char dsv_ftp_login[DSV_FTP_LOGIN_LEN + 1];
    char dsv_ftp_passwd[DSV_FTP_PASSWD_LEN + 1];
    char dsv_ftp_homedir[DSV_FTP_HOMEDIR_LEN + 1];
    char dsv_tftp_homedir[DSV_TFTP_HOMEDIR_LEN + 1];
    char dsv_public_ip[DSV_PUBLIC_IP_LEN + 1];
    char dsv_maintenance_ip[DSV_MAINTENANCE_IP_LEN + 1];
    char sbn_network_ip[SBN_NETWORK_IP_LEN + 1];
    char sbn_network_mask[SBN_NETWORK_MASK_LEN + 1];
    int  dsv_use_ipv6;
} dedicated_server_like_t;

typedef struct msa_parameters_like
{
  char param_name[PARAM_NAME_LEN + 1];
  char param_type[PARAM_TYPE_LEN + 1];
  char param_value[PARAM_VALUE_LEN + 1];
  char param_comment[PARAM_COMMENT_LEN + 1];
  char param_date_create[DATE_LEN + 1];
  char param_date_lastupdate[DATE_LEN + 1];
} msa_parameters_like_t;

typedef struct alarm_info_by_sd
{
    char sdid[MAXHOSTSIZE];
    char sd_email[CONTACT_EMAIL_LEN + 1];
    char abo_email[CONTACT_EMAIL_LEN + 1];
    char sit_nom[SIT_NOM_LEN + 1];
    char sit_external_reference[SIT_EXTERNAL_REFERENCE + 1];
    char abo_name[ABO_NAME_LEN + 1];
    char sd_ip_config[IP_ADD_MASK_LEN + 1];
    char man_name[MANUFACTURER_LEN + 1];
    char model_name[MODEL_LEN + 1];
} alarm_info_by_sd_t;


#define SERVICE_NAME               512
#define SERVICE_ID                 24
#define SERVICE_EXTERNAL_REFERENCE 255

typedef struct service_instance
{
    char abo_email[CONTACT_EMAIL_LEN + 1];
    char abo_name[ABO_NAME_LEN + 1];
    char service_id[SERVICE_ID + 1];
    char service_name[SERVICE_NAME + 1];
    char service_external_reference[SERVICE_EXTERNAL_REFERENCE + 1];
} service_instance_t;

/** Enable Threads for Oracle session
 @return     OK when Threads Enabled
 @return     FAILED when failed
 */
extern SQLRETURN SMSSQL_EnableThreads(void);

/** Allocate of a runtime context and connect user/password to Oracle instance
 @param  ctx     Adress of sql context to allocate
 @param  Username    User name
 @param  Password    User password
 @param  Service     Oracle service name to connect to
 @return         OK     when allocation performed            \n
 @return         FAILED on allocation failure
 */
extern SQLRETURN SMSSQL_AllocateContext(sql_context *ctx, char *UserName, char *Password, char *Service);

/** Free of a runtime context (close connection to the Oracle instance and free runtime context)
 @param  context Adress of sql context to free, set to NULL after completion of this routine
 @return     OK     when context is free                 \n
 @return     FAILED on failure
 */
extern SQLRETURN SMSSQL_FreeContext(sql_context *context);

/** Get information about a given SD ID
 @param  ctx sql context to use
 @param  sd  Adress of a struct of sd_like_t type
 @return     OK       if the information are loaded into sd struct       \n
 @return     NOTFOUND if the SD_ID doesn't matche any SD ID in the database  \n
 @return     FAILED   if there a failure.
 */
extern SQLRETURN SMSSQL_GetSD(sql_context ctx, sd_like_t *sd, int check_node);

/** Get information about a given SD Serial Number
 @param  ctx sql context to use
 @param  sd  Adress of a struct of sd_like_t type
 @return     OK       if the information are loaded into sd struct       \n
 @return     NOTFOUND if the SD serial number doesn't matche any SD ID in the database   \n
 @return     FAILED   if there a failure.
 */
//extern SQLRETURN SMSSQL_GetSDbySerialNum(sql_context ctx, sd_like_t *sd);

/** Get the value of the public key of a given SD ID
 @param  ctx   sqlcontext to use
 @param  sd_prefix Customer prefix for the SD to retrieve
 @param  sd_seqnum Sequence number for the SD to retrieve
 @param  sd_pk     Public key to set for the SD
 @return       OK       if the update correctly performed            \n
 @return       NOTFOUND if the SD_ID doesn't matche any SD ID in the tables  \n
 @return       FAILED   if ther is a failure
 */
//extern SQLRETURN SMSSQL_GetPublicKey(sql_context ctx, char *sd_prefix, long sd_seqnum, char *sd_pk);

/** Set value of IP Config @ for a given SD ID
 @param  ctx     sqlcontext to use
 @param  sd_prefix   Customer prefix for the SD to retrieve
 @param  sd_seqnum   Sequence number for the SD to retrieve
 @param  sd_ipconf   New IP Conf to set
 @param  sd_connected    Boolean Connected SD flag ( Set to true or False)
 @return OK           if the update correctly performed                    \n
 @return NOTFOUND     if the SD_ID doesn't matche any SD ID in the tables  \n
 @return FAILED       if there is a failure
 */
extern SQLRETURN SMSSQL_SetSDIPConfig(sql_context ctx, char *sd_prefix, long sd_seqnum, char *sd_ipconf, int connected_flag);

/*----------------------------------------  SMSSQL_SetSDConnected -----------------------------------*/
/** Set value of connected flag for a given SD ID
 @param  ctx             sqlcontext to use
 @param  sd_prefix       Customer prefix for the SD to retrieve
 @param  sd_seqnum       Sequence number for the SD to retrieve
 @param  sd_connected    Boolean Connected SD flag ( Set to true or False)
 @return OK               if the update correctly performed                    \n
 @return NOTFOUND         if the SD_ID doesn't matche any SD ID in the tables  \n
 @return FAILED           if there is a failure
 */
extern SQLRETURN SMSSQL_SetSDConnected(sql_context ctx, char *Pprefix, long Pseqnum, int connected_flag);

/*----------------------------------------  SMSSQL_SetSDUpdated -----------------------------------*/
/** Set value of updated flag for a given SD ID
 @param  ctx             sqlcontext to use
 @param  sd_prefix       Customer prefix for the SD to retrieve
 @param  sd_seqnum       Sequence number for the SD to retrieve
 @param  sd_updated    Boolean updated SD flag ( Set to true or False)
 @return OK               if the update correctly performed                    \n
 @return NOTFOUND         if the SD_ID doesn't matche any SD ID in the tables  \n
 @return FAILED           if there is a failure
 */
extern SQLRETURN SMSSQL_SetSDUpdated(sql_context ctx, char *Pprefix, long Pseqnum, int updated_flag);

/** Get information about First connected SD
 * Retreive only connected SD with Dynamic IP address
 @param  ctx     sql context to use
 @param  sd      Adress of a struct of sd_like_t type
 @return         OK       if the information are loaded into sd struct           \n
 @return         NOTFOUND if the SD public key doesn't matche any SD ID in the database  \n
 @return         FAILED   if there a failure.
 */
extern SQLRETURN SMSSQL_GetSDconnectedFirst(sql_context ctx, sd_like_t *sd);

/** Get information about Next connected SD
 * Retreive only connected SD with Dynamic IP address
 @param  ctx     sql context to use
 @param  sd      Adress of a struct of sd_like_t type
 @return         OK       if the information are loaded into sd struct           \n
 @return         NOTFOUND if the SD public key doesn't matche any SD ID in the database  \n
 @return         FAILED   if there a failure.
 */
extern SQLRETURN SMSSQL_GetSDconnectedNext(sql_context ctx, sd_like_t *sd);

/** Find if a specified SD is CONNECTED
 * Set
 @param  ctx     sql context to use
 @param  sd      Adress of a struct of sd_like_t type witha valid PK.
 @param  FlagConnected   Boolean Set to TRUE if SD connected or have Static IP @
 else set TO FALSE
 @return         OK       if the information are loaded into sd struct           \n
 @return         NOTFOUND if the SD public key doesn't matche any SD ID in the database  \n
 @return         FAILED   if there a failure.
 */
//extern SQLRETURN SMSSQL_SDisConnected(sql_context ctx, sd_like_t *sd, int *FlagConnected);

/** Set SD Lock ON if not already locked for the given SD ID.
 @param  ctx             sqlcontext to use
 @param  sd_prefix       Customer prefix for the SD to retrieve
 @param  sd_seqnum       Sequence number for the SD to retrieve
 @param  lock_state      Lock state returned
 @return OK               if the select correctly performed                    \n
 @return NOTFOUND         if the SD_ID doesn't matche any SD ID in the tables  \n
 @return FAILED           if there is a failure
 */
extern SQLRETURN SMSSQL_SetLock(sql_context ctx, char *Pprefix, long Pseqnum, int *lock_state);

/** Set SD Lock OFF for the given SD ID
 @param  ctx             sqlcontext to use
 @param  sd_prefix       Customer prefix for the SD to retrieve
 @param  sd_seqnum       Sequence number for the SD to retrieve
 @return OK               if the select correctly performed                    \n
 @return NOTFOUND         if the SD_ID doesn't matche any SD ID in the tables  \n
 @return FAILED           if there is a failure
 */
extern SQLRETURN SMSSQL_UnLock(sql_context ctx, char *Pprefix, long Pseqnum);

/** Set SD Lock OFF for all SD
 @param  ctx             sqlcontext to use
 @return OK               if the select correctly performed                    \n
 @return NOTFOUND         if the SD_ID doesn't matche any SD ID in the tables  \n
 @return FAILED           if there is a failure
 */
extern SQLRETURN SMSSQL_UnLockAll(sql_context ctx, char *Pmesg);

/*---------------------------------------- SMSSQL_UnLock -----------------------------------*/
/** Set SD Lock OFF for the given SD ID
        @param  ctx             sqlcontext to use
        @param  sd_prefix       Customer prefix for the SD to retrieve
        @param  sd_seqnum       Sequence number for the SD to retrieve
        @return SMS_OK               if the select correctly performed                    \n
        @return NOTFOUND         if the SD_ID doesn't matche any SD ID in the tables  \n
        @return ERR_DB_FAILED           if there is a failure
 */
extern SQLRETURN SMSSQL_SetProvDate ( sql_context ctx, char *Pprefix, long Pseqnum );

/** Get the update status for a given SD ID and type of update
        @param  ctx              sqlcontext to use
        @param  Pupdate_status   Pointer to a sd_updatestatus_like_t structure
        @return                  SMS_OK          if the update correctly performed                    \n
        @return                  ERR_DB_NOTFOUND if the SD_ID doesn't matche any SD ID in the tables  \n
        @return                  ERR_DB_FAILED   if there is a failure
*/
extern SQLRETURN SMSSQL_GetUpdateStatus(sql_context ctx, sd_updatestatus_like_t *sd_updatestatus);

/** Set the update status for a given SD ID and type of update
        @param  ctx              sqlcontext to use
        @param  Pupdate_status   Pointer to a sd_updatestatus_like_t structure
        @return                  SMS_OK          if the update correctly performed                    \n
        @return                  ERR_DB_NOTFOUND if the SD_ID doesn't matche any SD ID in the tables  \n
        @return                  ERR_DB_FAILED   if there is a failure
*/
extern SQLRETURN SMSSQL_SetUpdateStatus(sql_context ctx, sd_updatestatus_like_t *sd_updatestatus);

/** Set the value of the SD provisioning lock
 @param  ctx       sqlcontext to use
 @param  Pprefix   Pointer to a string containing the customer prefix
 @param  Pseqnum   Sequence Number for the SD
 @param  Plock     Prov lock to be set in database
 @return           OK       if the update correctly performed                    \n
 @return           NOTFOUND if the SD_ID doesn't matche any SD ID in the tables  \n
 @return           FAILED   if there is a failure
 */
extern SQLRETURN SMSSQL_SetSDprovlock(sql_context ctx, char *Pprefix, long Pseqnum, int Plock);

/** Get the value of the SD provisioning lock
 @param  ctx       sqlcontext to use
 @param  Pprefix   Pointer to a string containing the customer prefix
 @param  Pseqnum   Sequence Number for the SD
 @param  Plock     Prov lock to be read from the database
 @return           OK       if the update correctly performed                    \n
 @return           NOTFOUND if the SD_ID doesn't matche any SD ID in the tables  \n
 @return           FAILED   if there is a failure
 */
extern SQLRETURN SMSSQL_GetSDprovlock(sql_context ctx, char *Pprefix, long Pseqnum, int *Plock);

/** Update configuration setp of the given SD ID
 @param  ctx       sqlcontext to use
 @param  Pprefix   Pointer to a string containing the customer prefix
 @param  Pseqnum   Sequence Number for the SD
 @param  Pcfg_step Configuration step
 @return OK       if the insert correctly performed                    \n
 @return NOTFOUND if the IP addr doesn't matche any SD ID in the database  \n
 @return FAILED   if there is a failure
 */
extern SQLRETURN SMSSQL_SetSDConfigStep(sql_context ctx, char *Pcli_prefix, long Pseqnum, int Pcfg_step);

/** Update configuration setp of the given SD ID
 @param  ctx             sqlcontext to use
 @param  Pcfg_step_begin Configuration step
 @param  Pcfg_step_end   Configuration step
 @return OK              if the insert correctly performed                    \n
 @return NOTFOUND        if the IP addr doesn't matche any SD ID in the database  \n
 @return FAILED          if there is a failure
 */
//extern SQLRETURN SMSSQL_ResetSDConfigStep(sql_context ctx, int Pcfg_step_begin, int Pcfg_step_end);

/** Read configuration setp of the given SD ID
 **         @param  ctx       sqlcontext to use
 **         @param  Pprefix   Pointer to a string containing the customer prefix
 **         @param  Pseqnum   Sequence Number for the SD
 **         @param  Pcfg_step Configuration step
 **         @return OK       if the insert correctly performed                    \n
 **         @return NOTFOUND if the IP addr doesn't matche any SD ID in the database  \n
 **         @return FAILED   if there is a failure
 **
 **
 **/
//extern SQLRETURN SMSSQL_GetSDConfigStep(sql_context ctx, char *Pcli_prefix, long Pseqnum, int *Pcfg_step);

/** Get First SD_INTERFACE information about a given SD ID
 @param  ctx     sql context to use
 @param  interface   Adress of a struct of sd_interface_like_t type
 @return     OK       if the information are loaded into adsl struct     \n
 @return     NOTFOUND if the SD_ID doesn't matche any adsl SD informationsin the database    \n
 @return     FAILED   if there a failure.
 */
extern SQLRETURN SMSSQL_GetFirstSDinterface(sql_context ctx, char *Pprefix, long Pseqnum, sd_interface_like_t *interface);

/** Get Next SD_INTERFACE information about a given SD ID
 @param  ctx     sql context to use
 @param  interface   Adress of a struct of sd_interface_like_t type
 @return     OK       if the information are loaded into adsl struct     \n
 @return     NOTFOUND if the SD_ID doesn't matche any adsl SD informationsin the database    \n
 @return     FAILED   if there a failure.
 */
extern SQLRETURN SMSSQL_GetNextSDinterface(sql_context ctx, sd_interface_like_t *interface);

extern SQLRETURN SMSSQL_GetAsset(sql_context ctx, sd_asset_like_t *sd_asset);
extern SQLRETURN SMSSQL_SetAsset(sql_context ctx, sd_asset_like_t *sd_asset);

extern SQLRETURN SMSSQL_SetSDPing(sql_context ctx, char *Pprefix, long Pseqnum, long Ppingtime, int Pisup, int Pdoinsert);
extern SQLRETURN SMSSQL_SetSDPingList ( sql_context ctx, ping_db_state_t *Pping_state_array, int Pcount );

extern SQLRETURN SMSSQL_SetSDAssetUpdate(sql_context ctx, char *Pprefix, long Pseqnum, int AssetUpdate_flag);

extern SQLRETURN SMSSQL_GetFirstSnmpPolling(sql_context ctx, snmp_polling_like_t *snmp_polling);
extern SQLRETURN SMSSQL_GetNextSnmpPolling(sql_context ctx, snmp_polling_like_t *snmp_polling);

extern SQLRETURN SMSSQL_GetFirstSdProfile(sql_context ctx, char *Pprefix, long Pseqnum, char *pfl_type, long *Ppfl_id);
extern SQLRETURN SMSSQL_GetNextSdProfile(sql_context ctx, char *Pprefix, long Pseqnum, char *pfl_type, long *Ppfl_id);

/* get the SD to update */
extern SQLRETURN SMSSQL_GetSDToUpdateFirst(sql_context ctx, sd_like_t *sd);
extern SQLRETURN SMSSQL_GetSDToUpdateNext(sql_context ctx, sd_like_t *sd);

extern SQLRETURN SMSSQL_InitProvStatus(sql_context ctx, char *Pprefix, long Pseqnum, int Pnum_stages, char **Pname_stages);
extern SQLRETURN SMSSQL_SetProvStatus(sql_context ctx, char *Pprefix, long Pseqnum, int Pstage_index, char *Pstatus, char *Pnext_status, unsigned int Perror_code, char *Perror_message);
extern SQLRETURN SMSSQL_GetProvStatus(sql_context ctx, sd_provstatus_like_t *Pprovstatus);

extern SQLRETURN SMSSQL_SetPollMode(sql_context ctx, char *Pprefix, long Pseqnum, int sd_poll_mode);

extern SQLRETURN SMSSQL_GetSDHostnameFirst(sql_context ctx, sd_like_t *sd);
extern SQLRETURN SMSSQL_GetSDHostnameNext(sql_context ctx, sd_like_t *sd);

//extern SQLRETURN SMSSQL_GetHostnameByIP(sql_context ctx, char *P_ipaddr, char *Psd_hostname);

extern SQLRETURN SMSSQL_GetSDinCustomerFirst(sql_context ctx, long cust_id, sd_like_t *sd);
extern SQLRETURN SMSSQL_GetSDinCustomerNext(sql_context ctx, sd_like_t *sd);

extern SQLRETURN SMSSQL_GetAlarmInfoBySite(sql_context ctx, long Psd_seqnum, char *Pemail, char *Psit_nom, char *Psit_external_reference, char *Pabo_name);

extern SQLRETURN SMSSQL_GetLogDisableFirst(sql_context *ctx, fildelog_like_t *log);
extern SQLRETURN SMSSQL_GetLogDisableNext(sql_context *ctx, fildelog_like_t *log);
extern SQLRETURN SMSSQL_GetLogAlarmFirst(sql_context *ctx, fildelog_like_t *log);
extern SQLRETURN SMSSQL_GetLogAlarmNext(sql_context *ctx, fildelog_like_t *log);

extern SQLRETURN SMSSQL_SetSdAssetAttribute(sql_context ctx, sd_asset_attribute_like_t *PassetAttribute);
//extern SQLRETURN SMSSQL_UpdateAllSdAssetAttribute(sql_context ctx, char *Pprefix, long Pseqnum, sd_asset_attribute_like_t *assetAttribute, int Pnb);
//extern SQLRETURN SMSSQL_RemoveSdAssetAttribute(sql_context ctx, sd_asset_attribute_like_t *PassetAttribute);
//extern SQLRETURN SMSSQL_GetSdAssetAttributeFirst(sql_context ctx, char *Pprefix, long Pseqnum, sd_asset_attribute_like_t *PassetAttribute);
extern SQLRETURN SMSSQL_GetSdAssetAttributeNext(sql_context ctx, sd_asset_attribute_like_t *PassetAttribute);
extern SQLRETURN SMSSQL_GenericUpateOrInsert(sql_context ctx, char *table_name, void *fields_hashtable, void *keys_hashtable);
extern SQLRETURN SMSSQL_GenericDelete(sql_context ctx, char *table_name, void *keys_hashtable);

//extern SQLRETURN SMSSQL_SetIPPhoneStatus(sql_context ctx, char *Pprefix, long Pseqnum, int Psize, ip_phone_status_like_t *PipPhoneStatus);

extern SQLRETURN SMSSQL_GetFirstSDConfigVar(sql_context ctx, sd_configvar_like_t *sd_configvar, char *Pprefix, long Pseqnum);
extern SQLRETURN SMSSQL_GetNextSDConfigVar(sql_context ctx, sd_configvar_like_t *sd_configvar);

extern SQLRETURN SMSSQL_GetFirstSDExtAttr(sql_context ctx, sd_ext_attr_like_t *sd_ext_attr, char *Pprefix, long Pseqnum);
extern SQLRETURN SMSSQL_GetNextSDExtAttr(sql_context ctx, sd_ext_attr_like_t *sd_ext_attr);

/**
 Display the list of sd and the date of licence expiration date
 *
 */
extern SQLRETURN SMSSQL_GetFirstAvExpirationDate (sql_context ctx, sd_licexpiration_like_t *SDAsset );
extern SQLRETURN SMSSQL_GetNextAvExpirationDate (sql_context ctx, sd_licexpiration_like_t *SDAsset );

extern SQLRETURN SMSSQL_GetFirstIpsExpirationDate (sql_context ctx, sd_licexpiration_like_t *SDAsset );
extern SQLRETURN SMSSQL_GetNextIpsExpirationDate (sql_context ctx, sd_licexpiration_like_t *SDAsset );

extern SQLRETURN SMSSQL_GetFirstUrlExpirationDate (sql_context ctx, sd_licexpiration_like_t *SDAsset );
extern SQLRETURN SMSSQL_GetNextUrlExpirationDate (sql_context ctx, sd_licexpiration_like_t *SDAsset );

extern SQLRETURN SMSSQL_GetFirstAsExpirationDate (sql_context ctx, sd_licexpiration_like_t *SDAsset );
extern SQLRETURN SMSSQL_GetNextAsExpirationDate (sql_context ctx, sd_licexpiration_like_t *SDAsset );


extern SQLRETURN SMSSQL_SetSDHAActiveStatus ( sql_context ctx, char *p_sd_prefix, long p_sd_seqnum, int p_isActive );

extern SQLRETURN SMSSQL_ResetSDCrudObjects(sql_context ctx, sd_crud_object_like_t *Pcrud_object);
extern SQLRETURN SMSSQL_SetSDCrudObjects(sql_context ctx, sd_crud_object_like_t *Pcrud_object);
extern SQLRETURN SMSSQL_DeleteSDCrudObjects(sql_context ctx, sd_crud_object_like_t *Pcrud_object);
extern SQLRETURN SMSSQL_GetFirstSDCrudObject ( sql_context ctx, sd_crud_object_like_t *Pcrud_object );
extern SQLRETURN SMSSQL_GetNextSDCrudObject ( sql_context ctx, sd_crud_object_like_t *Pcrud_object );
extern int SMSSQL_GetFirstSDCrudObjectByName ( sql_context ctx, sd_crud_object_like_t *Pcrud_object, char *crud_name );
extern int SMSSQL_GetNextSDCrudObjectByName( sql_context ctx, sd_crud_object_like_t *Pcrud_object );

extern SQLRETURN SMSSQL_GetFirstDedicatedServerByOperator ( sql_context ctx, dedicated_server_like_t *dedicated_server, char *cli_prefix );
extern SQLRETURN SMSSQL_GetNextDedicatedServerByOperator ( sql_context ctx, dedicated_server_like_t *dedicated_server );

extern SQLRETURN SMSSQL_GetExtAttrAlarmRuleFirst(sql_context ctx, alarm_rule_t *alarm_rule);
extern SQLRETURN SMSSQL_GetExtAttrAlarmRuleNext(sql_context ctx, alarm_rule_t *alarm_rule);

extern SQLRETURN SMSSQL_GetMsaParam (sql_context ctx, msa_parameters_like_t *msa_param );
extern SQLRETURN SMSSQL_SetMsaParam (sql_context ctx, msa_parameters_like_t *msa_param );

extern SQLRETURN SMSSQL_GetFirstNodeSD ( sql_context ctx, alarm_info_by_sd_t *Pinfo );
extern SQLRETURN SMSSQL_GetNextNodeSD ( sql_context ctx, alarm_info_by_sd_t *Pinfo );

extern SQLRETURN SMSSQL_GetManagerInfoFirst(sql_context ctx, manager_info_t *manager_info);
extern SQLRETURN SMSSQL_GetManagerInfoNext(sql_context ctx, manager_info_t *manager_info);


extern SQLRETURN SMSSQL_GetSDbyManufacturerModelFirst(sql_context ctx, sd_like_t *SD);
extern SQLRETURN SMSSQL_GetSDbyManufacturerModelNext(sql_context ctx, sd_like_t *SD);

extern SQLRETURN SMSSQL_GetServiceInstanceFirst(sql_context ctx, service_instance_t *service_instance);
extern SQLRETURN SMSSQL_GetServiceInstanceNext(sql_context ctx, service_instance_t *service_instance);

#endif /* __SMSSQL_H_ */
