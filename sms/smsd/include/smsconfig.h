/** @file verb.h
  This is the header file of the verb module of the smsd daemon.
  @ingroup  smsd
  */

#ifndef __SMSCONFIG_H_
#define __SMSCONFIG_H_
#include <gpul.h>

typedef struct sms
{
  /** the IP address of the SMS */
  char * sms_address_ip;
  /** the IPV6 address of the SMS */
  char * sms_address_ipv6;
  /** ip addresses of the SMS used in router's ACL */
  char * sms_network_ip;
  /** mask of the sms network */
  char * sms_mask_ip;
  /** mask of the sms network( inverted for cisco ) */
  char * sms_inverted_mask_ip;



  /* ip address of the SMS used in router's ACL in clear*/
  /* no more used :char * sms_clearadd_ip;*/
  /* ip of the router of the SMS used int the router's ACL */
  /* no more used :char * sms_routeradd_ip;*/
} sms_t;

typedef struct smssyslog
{
  /** the IP address of the sms_syslog_server */
  char * syslog_add_ip;
} smssyslog_t;

typedef struct valentry
{
  GPUL_TAILQ_ENTRY(struct valentry) link;
  char *val;
} valentry_t;
GPUL_TAILQ_HEAD(valentry_head, valentry_t);


#define DEFAULT_ROUTER_DOMAIN "ubiqube.net"

typedef struct smsConf
{
  /* the domain name used in certificates an other , default is netcelo.net */
  char * domain ;
  /** the port of the daemon */
  int port;
  /** the level of log */
  int loglevel;
  /** the level of debug */
  int debug;
  /** the file where to write log*/
  char *logfile;
  /** the handle of the log file */
  void *log_handle;
  /** number of simultaneous connexions */
  int numSimultaneous;
  /** number of SMS DB connections */
  int SMSnumContext;
  /** number of CUST DB connections */
  int CUSTnumContext;
  /** number of simultaneous connexions during a getVPNStatus */
  int numSimultaneousIngetVPNstatus;
  /** host address for the listen */
  char * haddr ;
  /** ip address of the ntp server */
  char *ntpserver;
  /** ip address of the backup ntp server */
  char *ntpbackupserver;
  /** number of sms in use in the configuration */
  int smsNumber;
  /** the definition of all the sms in use for the sms tunnels */
  sms_t * smsArray;
  /** number of sms_syslogd host in use in the configuration */
  int syslogNumber;
  /** the definition of all the syslog server */
  smssyslog_t * syslogArray;

  /** default DNS used in the configuration of router */
  char * dns_default;

  /* the name of the MSA */
  char *ubi_vsoc_name;

  char *ubi_centralized_node_name;

  /** the base path for the TFTP directory */
  char *tftp_base;

  /** the base path for the file management configuration repository */
  char *fmc_repository;

  /** the base path for the file management configuration description */
  char *fmc_entities2files;

  /** flag to determine if the list of router to update must be done at start */
  int list_router_to_update_from_base_at_start;

  char *smtp_server_address;
  char *notif_mail_address;

  /** heart beat in ms, each time check for a task to be executed */
  int heart_beat;

  /* database configuration file */
  char *db_conf_file;

  /* database configuration file */
  char *router_conf_file;

  char *clm_server_ip;

  char *scp_user;
  char *scp_pass;

  int nightly_save_conf;
  char *save_all_conf_time;
  char *save_all_conf_freq;


  int alarm_sev_conf_changed;
  int alarm_update_fail;
  int alarm_update_ok;

  int local_server_synchro_timeout;

  char *node_name;

  struct valentry_head jsacmd_list;
  struct valentry_head jsacustcmd_list;

} smsConf_t;

/** Create a smsConfig structure and load it from the configuration file
    @param the configFile from which the conf is to be read
    @return a smsConf_t structure pointer or NULL.
    @warning the returning smsConf_t * is allocated in the function and must be
    freed  with deleteConfig
 */
smsConf_t *createConfig(char *configFile);

/** Delete a previous created configuration
 */
void deleteConfig (smsConf_t * config);

void printConfig( smsConf_t * config);

extern int sms_disaster;
extern int sms_master;
extern int syslog_disaster;
extern int syslog_master;
extern int ipsec_master;

#endif /*__SMSCONFIG_H_*/
