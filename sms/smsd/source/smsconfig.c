/*! \file smsconfig.c
 *  \brief core file of the sms daemon
 */

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/socketvar.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "log.h"
#include "gpul.h"
#include "smsd.h"
#include "router.h"
#include "smsconfig.h"

/** the index of the disater sms in the smsArray */
int sms_disaster = 0;
/** the index of the master sms in the smsArray */
int sms_master = 0;
/** the index of the disaster syslog in the syslogArray */
int syslog_disaster = 0;
/** the index of the master syslog in the syslogArray */
int syslog_master = 0;

void deleteConfig(smsConf_t * config)
{
  int i;
  valentry_t *jsacmd;
  valentry_t *jsacmd_prev;

  if (!config)
    return;

  freez(config->domain);
  freez(config->logfile);
  if (config->log_handle)
    gpul_log_release_handle(config->log_handle);
  freez(config->haddr);
  for (i = 0; i < config->smsNumber; i++)
  {
    freez(config->smsArray[i].sms_address_ip);
    freez(config->smsArray[i].sms_address_ipv6);
    freez(config->smsArray[i].sms_network_ip);
    freez(config->smsArray[i].sms_mask_ip);
    freez(config->smsArray[i].sms_inverted_mask_ip);
  }
  for (i = 0; i < config->syslogNumber; i++)
  {
    freez(config->syslogArray[i].syslog_add_ip);
  }
  freez(config->smsArray);
  freez(config->syslogArray);
  freez(config->ntpserver);
  freez(config->ntpbackupserver);
  freez(config->dns_default);
  freez(config->ubi_vsoc_name);
  freez(config->ubi_centralized_node_name);
  freez(config->node_name);
  freez(config->tftp_base);
  freez(config->fmc_repository);
  freez(config->fmc_entities2files);
  freez(config->smtp_server_address);
  freez(config->notif_mail_address);
  freez(config->db_conf_file);
  freez(config->router_conf_file);
  freez(config->save_all_conf_time);
  freez(config->save_all_conf_freq);

  while ((jsacmd = GPUL_TAILQ_FIRST(&currentConfig->jsacmd_list)) != NULL)
  {
    jsacmd_prev = jsacmd;
    GPUL_TAILQ_REMOVE(&currentConfig->jsacmd_list, jsacmd, link);
    freez(jsacmd_prev->val);
    freez(jsacmd_prev);
  }

  while ((jsacmd = GPUL_TAILQ_FIRST(&currentConfig->jsacustcmd_list)) != NULL)
  {
    jsacmd_prev = jsacmd;
    GPUL_TAILQ_REMOVE(&currentConfig->jsacustcmd_list, jsacmd, link);
    freez(jsacmd_prev->val);
    freez(jsacmd_prev);
  }

  freez(config);
  return;
}

/** Create a smsConfig structure and load it from the configuration file
 @param the configFile from which the conf is to be read
 @return a smsConf_t structure pointer or NULL.
 @warning the returning smsConf_t * is allocated in the function and must be
 freed  with deleteConfig
 */
smsConf_t *createConfig(char *configFile)
{
  FILE * configfp;
  smsConf_t *config;
  int line_num = 0;
  char cmd[BUFSIZ];
  char arg[BUFSIZ];
  char tmp[BUFSIZ];
  char buf[BUFSIZ];
  char *p, *q;
  int err = 0;
  int len;
  char *metacat = NULL;
  valentry_t *jsacmd;

  if (!configFile)
  {
    fprintf(stderr, "%s: can't open a NULL configuration file \n", prog);
    return NULL;
  }

  configfp = fopen(configFile, "r");
  if (!configfp)
  {
    fprintf(stderr, "%s: can't open configuration file '%s': \n", prog, configFile);
    return NULL;
  }

  config = (smsConf_t *) calloc(1, sizeof(smsConf_t));

  /** update the default value */
  config->port = DEFAULTPORT;
  config->loglevel = DEFAULTLOG;
  config->debug = DEFAULTDEBUG;
  config->SMSnumContext = DEFAULTMAXTHREAD;
  config->CUSTnumContext = 1;
  config->numSimultaneous = DEFAULTMAXSIMULTANEOUS;
  config->alarm_update_ok = 6;
  config->alarm_update_fail = 1;
  config->local_server_synchro_timeout = 300;
  GPUL_TAILQ_INIT(&(config->jsacmd_list));
  GPUL_TAILQ_INIT(&(config->jsacustcmd_list));

  /*config->archive_logfile_everyday=1;*/

  while (fgets(buf, sizeof(buf), configfp))
  {
    line_num++;
    strcpy(tmp, buf);

    if ((p = strpbrk(tmp, "#\r\n")))
      *p = '\0';
    p = tmp;
    /* leading skip whitespace */
    while (*p && ((*p == ' ') || (*p == '\t')))
      p++;
    q = cmd;
    while (*p && (*p != ' ') && (*p != '\t'))
      *q++ = *p++;
    *q = '\0';

    if (*cmd == '\0')
      continue;

    while (*p && ((*p == ' ') || (*p == '\t')))
      p++;

    strcpy(arg, p);
    p = arg + strlen(arg) - 1;

    /* ignore trailing whitespace */
    while ((p >= arg) && *p && ((*p == ' ') || (*p == '\t')))
      *p-- = '\0';

    /* insure the command field is lower case */
    for (p = cmd; *p; p++)
      if (isupper(*p))
        *p = tolower(*p);

    if (strcmp(cmd, "[sms]") == 0 || strcmp(cmd, "[sms-disaster]") == 0)
    {
      config->smsArray = realloc(config->smsArray, sizeof(sms_t) * (config->smsNumber + 1));

      if (strcmp(cmd, "[sms-disaster]") == 0)
        sms_disaster = config->smsNumber;
      else
        sms_master = config->smsNumber;

      config->smsNumber++;
      continue;
    }

    if (strcmp(cmd, "db-conf-file") == 0)
    {
      config->db_conf_file = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "router-conf-file") == 0)
    {
      config->router_conf_file = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "[syslog]") == 0 || strcmp(cmd, "[syslog-disaster]") == 0)
    {
      config->syslogArray = realloc(config->syslogArray, sizeof(smssyslog_t) * (config->syslogNumber + 1));

      if (strcmp(cmd, "[syslog-disaster]") == 0)
        syslog_disaster = config->syslogNumber;
      else
        syslog_master = config->syslogNumber;

      config->syslogNumber++;
      continue;
    }

    if (strcmp(cmd, "domain") == 0)
    {
      config->domain = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "port") == 0)
    {
      config->port = atoi(arg);
      continue;
    }

    if (strcmp(cmd, "loglevel") == 0)
    {
      config->loglevel = atoi(arg);
      continue;
    }

    if (strcmp(cmd, "debug") == 0)
    {
      config->debug = atoi(arg);
      continue;
    }

    if (strcmp(cmd, "sms-num-context") == 0)
    {
      config->SMSnumContext = atoi(arg);
      continue;
    }

    if (strcmp(cmd, "cust-num-context") == 0)
    {
      config->CUSTnumContext = atoi(arg);
      continue;
    }

    if (strcmp(cmd, "numsimultaneous") == 0)
    {
      config->numSimultaneous = atoi(arg);
      continue;
    }

    if (strcmp(cmd, "numsimultaneousingetvpnstatus") == 0)
    {
      config->numSimultaneousIngetVPNstatus = atoi(arg);
      continue;
    }

    if (strcmp(cmd, "logfile") == 0)
    {
      config->logfile = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "listen-address") == 0)
    {
      config->haddr = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "sms-address-ip") == 0)
    {
      config->smsArray[config->smsNumber - 1].sms_address_ip = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "sms-address-ipv6") == 0)
    {
       config->smsArray[config->smsNumber - 1].sms_address_ipv6 = strdup(arg);
       continue;
    }

    if (strcmp(cmd, "sms-server-network") == 0)
    {
      config->smsArray[config->smsNumber - 1].sms_network_ip = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "local-server-synchro-timeout") == 0)
    {
      config->local_server_synchro_timeout = atoi(arg);
      continue;
    }

    if (strcmp(cmd, "sms-server-mask") == 0)
    {
      struct in_addr network;
      config->smsArray[config->smsNumber - 1].sms_mask_ip = strdup(arg);
      inet_pton(AF_INET, config->smsArray[config->smsNumber - 1].sms_mask_ip, &network);
      network.s_addr ^= 0xFFFFFFFF;
      config->smsArray[config->smsNumber - 1].sms_inverted_mask_ip = malloc(INET_ADDRSTRLEN);
      inet_ntop(AF_INET, &network, config->smsArray[config->smsNumber - 1].sms_inverted_mask_ip, INET_ADDRSTRLEN);
      continue;
    }

    if (strcmp(cmd, "ntp-ip-address") == 0)
    {
      config->ntpserver = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "ntp-backup-ip-address") == 0)
    {
      config->ntpbackupserver = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "syslog-server") == 0)
    {
      config->syslogArray[config->syslogNumber - 1].syslog_add_ip = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "dns-default") == 0)
    {
      config->dns_default = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "ubi-vsoc-name") == 0)
    {
      config->ubi_vsoc_name = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "ubi-centralized-node-name") == 0)
    {
      config->ubi_centralized_node_name = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "tftp-base") == 0)
    {
      config->tftp_base = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "fmc-repository") == 0)
    {
      config->fmc_repository = strdup(arg);
      // si le dernier caractere est un '/', supprimer celui-ci!
      len = strlen(config->fmc_repository) - 1;
      if (config->fmc_repository[len] == '/')
      {
        config->fmc_repository[len] = '\0';
      }
      continue;
    }

    if (strcmp(cmd, "fmc-entities2files") == 0)
    {
      config->fmc_entities2files = strdup(arg);
      // si le dernier caractere est un '/', supprimer celui-ci!
      len = strlen(config->fmc_entities2files) - 1;
      if (config->fmc_entities2files[len] == '/')
      {
        config->fmc_entities2files[len] = '\0';
      }
      continue;
    }

    if (strcmp(cmd, "nightly-save-conf") == 0)
    {
      config->nightly_save_conf = atoi(arg);
      continue;
    }

    if (strcmp(cmd, "save-all-conf-time") == 0)
    {
      config->save_all_conf_time = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "save-all-conf-freq") == 0)
    {
      config->save_all_conf_freq = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "alarm-sev-conf-changed") == 0)
    {
      config->alarm_sev_conf_changed = atoi(arg);
      continue;
    }

    if (strcmp(cmd, "alarm-sev-update-ok") == 0)
    {
      config->alarm_update_ok = atoi(arg);
      continue;
    }

    if (strcmp(cmd, "alarm-sev-update-failed") == 0)
    {
      config->alarm_update_fail = atoi(arg);
      continue;
    }

    if (strcmp(cmd, "list-router-to-update-from-base-at-start") == 0)
    {
      config->list_router_to_update_from_base_at_start = atoi(arg);
      continue;
    }

    if (strcmp(cmd, "heart-beat") == 0)
    {
      config->heart_beat = atoi(arg);
      continue;
    }

    if (strcmp(cmd, "smtp-server-address") == 0)
    {
      config->smtp_server_address = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "notif-mail-address") == 0)
    {
      config->notif_mail_address = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "scp-username") == 0)
    {
       config->scp_user = strdup(arg);
       continue;
    }

    if (strcmp(cmd, "scp-password") == 0)
    {
       config->scp_pass = strdup(arg);
       continue;
    }

    if (strcmp(cmd, "node-name") == 0)
    {
      config->node_name = strdup(arg);
      continue;
    }

    if (strcmp(cmd, "[jsacmd]") == 0)
    {
      continue;
    }

    if (strncmp(cmd, "cmd-", sizeof("cmd-") - 1) == 0)
    {
      jsacmd = (valentry_t *) malloc(sizeof(valentry_t));
      jsacmd->val = strdup(arg);
      GPUL_TAILQ_INSERT_TAIL(&config->jsacmd_list, jsacmd, link);
      continue;
    }

    if (strcmp(cmd, "[jsacustcmd]") == 0)
    {
      continue;
    }

    if (strncmp(cmd, "cust-cmd-", sizeof("cust-cmd-") - 1) == 0)
    {
      jsacmd = (valentry_t *) malloc(sizeof(valentry_t));
      jsacmd->val = strdup(arg);
      GPUL_TAILQ_INSERT_TAIL(&config->jsacustcmd_list, jsacmd, link);
      continue;
    }

    fprintf(stderr, "%s: unrecognized directive in configuration file at line number %d:\n%s", prog, line_num, buf);
    err = 1;
  }

  freez(metacat);

  fclose(configfp);

  if (err)
    goto err;

  /** check the mandatory fields */

  if (config->syslogArray == NULL || config->syslogNumber == 0)
  {
    fprintf(stderr, "%s: [syslog] is a missing mandatory field !\n", prog);
    goto err;
  }
  else
  {
    if (config->syslogArray[0].syslog_add_ip == NULL)
    {
      fprintf(stderr, "%s: syslog-server is a missing mandatory field !\n", prog);
      goto err;
    }
  }

  if (config->syslogNumber != 2)
  {
    fprintf(stderr, "%s: [syslog] and [syslog-disaster] are mandatory fields !\n", prog);
    goto err;
  }
  else
  {
    if (config->syslogArray[1].syslog_add_ip == NULL)
    {
      fprintf(stderr, "%s: syslog-server is a missing mandatory field !\n", prog);
      goto err;
    }
  }

  if (config->smsArray == NULL || config->smsNumber == 0)
  {
    fprintf(stderr, "%s: [sms] section is a missing mandatory field !\n", prog);
    goto err;
  }
  else
  {
    if (config->smsArray[0].sms_address_ip == NULL)
    {
      fprintf(stderr, "%s: sms-address-ip is a missing mandatory field !\n", prog);
      goto err;
    }
    if (config->smsArray[0].sms_network_ip == NULL)
    {
      fprintf(stderr, "%s: sms-server-network is a missing mandatory field !\n", prog);
      goto err;
    }
    if (config->smsArray[0].sms_mask_ip == NULL)
    {
      fprintf(stderr, "%s: sms-server-mask is a missing mandatory field !\n", prog);
      goto err;
    }
  }

  if (config->smsNumber != 2)
  {
    fprintf(stderr, "%s: [sms] and [sms-disaster] sections are missing mandatory fields !\n", prog);
    goto err;
  }
  else
  {
    if (config->smsArray[1].sms_address_ip == NULL)
    {
      fprintf(stderr, "%s: sms-address-ip is a missing mandatory field !\n", prog);
      goto err;
    }
    if (config->smsArray[1].sms_network_ip == NULL)
    {
      fprintf(stderr, "%s: sms-server-network is a missing mandatory field !\n", prog);
      goto err;
    }
    if (config->smsArray[1].sms_mask_ip == NULL)
    {
      fprintf(stderr, "%s: sms-server-mask is a missing mandatory field !\n", prog);
      goto err;
    }
  }

  if (config->numSimultaneousIngetVPNstatus == 0)
  {
    fprintf(stderr, "%s: numSimultaneousIngetVPNstatus is a missing mandatory field and should be >0!\n", prog);
    goto err;
  }
  if (config->domain == NULL)
  {
    fprintf(stderr, "%s: domain is a missing field using default value %s\n", prog, DEFAULT_ROUTER_DOMAIN);
    config->domain = strdup(DEFAULT_ROUTER_DOMAIN);
  }
  if (config->dns_default == NULL)
  {
    fprintf(stderr, "%s: dns-default is a missing mandatory field !\n", prog);
    goto err;
  }
  if (config->ubi_vsoc_name == NULL)
  {
    fprintf(stderr, "%s: ubi-vsoc-name is a missing mandatory field !\n", prog);
    goto err;
  }
  if (config->ubi_centralized_node_name == NULL)
  {
    fprintf(stderr, "%s: ubi-centralized-node-name is a missing mandatory field !\n", prog);
    goto err;
  }

  /** update the default value */
  /* no default value for haddr to let the NULL value to accept connection on ANYADDR */
  /*if (config->haddr==NULL)    config->haddr=strdup(DEFAULTHADDR);*/

  return config;

  err: free(config);
  return NULL;
}
