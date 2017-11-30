/** @file verb.c
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
#include "router.h"
#include "update.h"
#include "bd.h"
#include "report.h"
#include "generic_router.h"
#include "sms_router.h"
#include "smssnmp.h"
#include "pattern.h"
#include "php_misc.h"
#include "smsconfig.h"
#include "gpul.h"
#include "verb.h"
#include "sms_php.h"

extern char bufstartTime[64];
extern time_t startTime;

static void *verbHashTable = NULL;

static char *config_mode_list[] =
{ "DEFAULT", "BYLINE", "TFTP", "SCP" };

/** Get the log files of the selected SDID
 @param	SDid of the SD (must be a static SD)
 @param      csp : the context of the connection
 @return	0 if the function is successful;
 */
static int doGetReport(char * params, client_state_t *csp);

/** Get the log files of the selected SDID only vpn log
 @param	SDid of the SD (must be a static SD)
 @param      csp : the context of the connection
 @return	0 if the function is successful;
 */
static int doGetVPNReport(char * params, client_state_t *csp);

/** Get a configuration from the selected SDID
 @param	SDid of the SD (must be a static SD)
 @param      csp : the context of the connection
 @return	0 if the function is successful;
 */
static int doGetSDConf(char * params, client_state_t *csp);

/** Get a configuration from the selected SDID and backup it
 @param	SDid of the SD (must be a static SD)
 @param      csp : the context of the connection
 @return	0 if the function is successful;
 */
static int doBackupConf(char * params, client_state_t *csp);

/** Get the IP adress of a router ( and verify it if the router has
 a dynamic IP address)
 @param	SDid of the SD
 @param      csp : the context of the connection
 @return	0 if the function is successful;
 */
static int doGetRouterIp(char * params, client_state_t *csp);

/** Send an updated vpn configuration to the selected dynamic SDID
 @param	SDid of the SD and the ip address in a single string
 @param      csp : the context of the connection
 @return	0 if the function is successful;
 */
static int doPppIpUp(char * params, client_state_t *csp);

/** Handle an IPdown received from the POLLD server
 @param	SDid the ip address in a single string
 @param      csp : the context of the connection
 @return	0 if the function is successful;
 */
static int doIpDown(char * params, client_state_t *csp);

/** Get the IP address from a dynamic SDID and store it in the database
 This verb is used only by the sms_syslogd IPUP
 @param  SDid the SD id and ip address in a single string
 @param      csp : the context of the connection
 @return 0 if the function is successful;
 */
static int doSdIpUp(char * params, client_state_t *csp);

/** Check the status of a previous update (or aupdate)
 @param	SDid of the SD (must be a static SD)
 @param      csp : the context of the connection
 @return	0 if the function is successful;
 */
static int doCheckUpdate(char * params, client_state_t *csp);

/** Check the status of all previous update (conf or certificates).
 @param	SDid of the SD (must be a static SD)
 @param      csp : the context of the connection
 @return	0 if the function is successful;
 */
static int doCheckAllUpdate(char * params, client_state_t *csp);

/** Get the stats from the smsd
 @param	SDid of the SD (must be a static SD)
 @param      csp : the context of the connection
 @return	0 if the function is successful;
 */
static int doStat(char *params, client_state_t *csp);

/** Look if the SMSd is alive
 @param	SDid of the SD (must be a static SD)
 @param      csp : the context of the connection
 @return	0 if the function is successful;
 */
static int doIsAlive(char *params, client_state_t *csp);

/** Unlock the SD for future update
 @param	SDid of the SD
 @param      csp : the context of the connection
 @return	0 if the function is successful;
 */
static int doUnlockUpdate(char * params, client_state_t *csp);
static int doLockProvisioning(char * params, client_state_t * csp);
static int doUnLockProvisioning(char * params, client_state_t * csp);
static int doJsSmsAddonExec(char *params, client_state_t *csp);
static int doJsSmsexec(char *params, client_state_t *csp);
static int doJsSmsexecn(char *params, client_state_t *csp);
static int doCmd(char *params, client_state_t *csp);
static int doCustCmd(char *params, client_state_t *csp);
static int doCheckCmd(char *params, client_state_t *csp);
static int js_exec_script(char *params, client_state_t *csp, int synchronous);
static int doJsExecScript(char *params, client_state_t *csp);
static int doJsAExecScript(char *params, client_state_t *csp);
static int updateRouterIP(char *params, client_state_t *csp, char **trueSDid, char *event);
static int updateIP(char *sdid, char *ipaddr, client_state_t *csp, char *event);
static int handlePppIpUp(char *params, client_state_t *csp);
static int getRouterIp(char *params, client_state_t *csp, char * IpString);
static int doResolveTemplate(char *params, client_state_t *csp);
static int doResolve(char *params, client_state_t *csp);
static int doGetAsset(char * params, client_state_t *csp);
static int doForceAsset(char *params, client_state_t *csp);
static int doSetConfigMode(char *params, client_state_t *csp);
static int doProvisioning(char *params, client_state_t *csp);
static int doPushConfig(char *params, client_state_t *csp);
static int doCheckProvisioning(char *params, client_state_t *csp);
static int doPing(char *params, client_state_t *csp);
static int doStaging(char *params, client_state_t *csp);
static int doStagingMap(char *params, client_state_t *csp);
static int doUpdateConfig(char *params, client_state_t *csp);
static int doGenTemplate(char *params, client_state_t *csp);
static int doRestoreConf(char *params, client_state_t *csp);
static int doSnmpTest(char *params, client_state_t *csp);
static int doGetRunningConf(char *params, client_state_t *csp);
static int doGetConfig(char *params, client_state_t *csp);
static int doGetActivityReport(char *params, client_state_t *csp);
static int doPS(char *params, client_state_t *csp);
static int doSetLogLevel(char *params, client_state_t *csp);
static int doReinitLogs(char *dummy, client_state_t *csp);
static int do_SnmpGet(char *oid, client_state_t *csp);
static int do_SnmpGetNext(char *oid, client_state_t *csp);
static int doUnarchiveConf(char *params, client_state_t *csp);
static int doGetArchiveConf(char *params, client_state_t *csp);
static int doUpdateLicense(char *params, client_state_t *csp);
static int doUpdateFirmware(char *params, client_state_t *csp);
static int doCheckUpdateLicense(char * params, client_state_t *csp);
static int doCheckUpdateFirmware(char * params, client_state_t *csp);
static int doCheckRestore(char * params, client_state_t *csp);
static int doCheckBackup(char * params, client_state_t *csp);
static int doCheckSendDataFiles(char * params, client_state_t *csp);
static int doHaSwap(char *params, client_state_t *csp);
static int doCheckSerial(char *params, client_state_t *csp);
static int doCallCommand(char *params, client_state_t *csp);
static int doSendDataFiles(char *params, client_state_t *csp);
static int doListVerb(char *params, client_state_t *csp);
static int doImmediateSaveConf(char *params, client_state_t *csp);

/** define a structure for the verb parsing in the client->SMS protocol */
typedef struct verbAction
{
  /** the verb for which an action is to be defined */
  char *verb;
  /** the action associated with the verb */
  int (*doVerb)(char *SDid, client_state_t *csp);
  /** description of the action */
  char *description;
  int hasParams; //0: no param, 1 for params, 2 for params without validating them (sdid>4 chars)
  int log;
  int work_in_degraded_mode;
} verbAction_t;

/** definition of the array containing all the verbs */
// FOLLOW THE ALPHABETICAL i.e. A,B,C...
verbAction_t verbTab[] =
{
    { "CHECKALLUPDATE", doCheckAllUpdate, "Check the asynchronous update of all the router configurations (configuration, certificate, network)", 1, 0, 1 }, // Verification de toutes les mises a jour asynchrones (configuration, certificat, reseau)
    { "CHECKBACKUP", doCheckBackup, "Check asynchronous backup status (Param: SDid)", 1, 0, 1 }, // Verification de l'etat de backup (depuis la base de donnees).
    { "CHECKCMD", doCheckCmd, "Check asynchronous Command (Param: SDid Command)", 1, 0, 1 }, // Verification de la commande asynchrone.
    { "CHECKPROVISIONING", doCheckProvisioning, "*Check initial Provisioning status", 1, 0, 1 }, // *Check initial Provisioning status.
    { "CHECKRESTORE", doCheckRestore, "Check asynchronous restore status (Param: SDid)", 1, 0, 1 }, // Verification de l'etat de restore (depuis la base de donnees).
    { "CHECKSENDDATAFILES", doCheckSendDataFiles, "Check asynchronous send data files status (Param: SDid)", 1, 0, 1 }, // Verification de l'etat de sendatafiles (depuis la base de donnees).
    { "CHECKUPDATE", doCheckUpdate, "Check the asynchronous update of the router configuration. (AUPDATECONF)", 1, 0, 1 }, // Verification de la mise a jour asynchrone de la configuration (AUPDATECONF).
    { "CHECKUPDATEFIRMWARE", doCheckUpdateFirmware, "Check update firmware status (Param: SDid)", 1, 0, 1 }, // Verification de l'etat du firmware du routeur (depuis la base de donnees).
    { "CHECKUPDATELICENSE", doCheckUpdateLicense, "Check update license status (Param: SDid)", 1, 0, 1 }, // Verification de l'etat des licenses du routeur (depuis la base de donnees).
    { "FORCEASSET", doForceAsset, "Refresh the asset information of the router", 1, 1, 0 }, // Force la relecture de l'asset du routeur.
    { "GETACTIVITYREPORT", doGetActivityReport, "*Get router activity reports", 1, 1, 0 }, // *Recuperation des rapports d'activite du routeur.
    { "GETASSET", doGetAsset, "Get asset information from the router", 1, 1, 0 }, // Recuperation de l'asset (lecture base) du routeur.
    { "GETCONFIG", doGetConfig, "*Get router generated configuration", 1, 1, 0 }, // *Recuperation de la configuration generee du routeur.
    { "GETREPORT", doGetReport, "Get the router logs", 1, 1, 0 }, // Recuperation des logs complets du routeur.
    { "GETROUTERIP", doGetRouterIp, "Get the current router IP", 1, 1, 0 }, // Recuperation de l'adresse IP actuelle du routeur (adresses dynamiques validees par connexion ssh).
    { "GETRUNNINGCONF", doGetRunningConf, "*Get router running configuration", 1, 1, 0 }, // *Recuperation de la configuration du routeur.
    { "GETSDCONF", doGetSDConf, "Get the router running configuration (depuis le routeur)", 1, 1, 0 }, // Recuperation de la configuation qui tourne sur le routeur (depuis le routeur).
    { "GETVPNREPORT", doGetVPNReport, "Get the router VPN logs", 1, 1, 0 }, // Recuperation des logs VPN du routeur.
    { "IMMEDIATESAVECONF", doImmediateSaveConf, "backup all reachable router configuration in change management", 0, 1, 0 },
    { "IPDOWN", doIpDown, "IPDOWN processing (params: sdid @IP)", 1, 1, 0 }, // Traitement d'un IPDOWN venant de polld.
    { "ISALIVE", doIsAlive, "Check if SEC engine is alive", 0, 0, 1 }, // Verification de la presence du demon smsd.
    { "JSABACKUPCONF", doBackupConf, "Asynchronous backup of the router running configuration from the router (Param: sdid [addon card])", 1, 1, 0 }, // Recuperation de la configuation qui tourne sur le routeur (depuis le routeur) et sauvegarde SVN
    { "JSACMD", doCmd, "Launch Asynchronous Command (params: sdid command [command params])", 1, 1, 0 },
    { "JSACUSTCMD", doCustCmd, "Launch Asynchronous Command for customer (params: custid command [command params])", 1, 1, 0 },
    { "JSAEXECSCRIPT", doJsAExecScript, "asynchronous execution of a script on the router (Param: sdid script_file_path", 1, 1, 0 }, // Execute un script sur le routeur.
    { "JSAPROVISIONING", doProvisioning, "*initial provisioning (params: SDid [\\n @IP login passwd adminpasswd])", 1, 1, 0 }, // *Provisioning initial (params: SDid [\\n @IP login passwd adminpasswd]).
    { "JSAPUSHCONFIG", doPushConfig, "*push a configuration (params: SDid [addon card/PORTCONSOLE for console port update] [terminal server ip] [terminal server port] [\\n configuration])", 1, 1, 0 },
    { "JSARESTORECONF", doRestoreConf, "Asynchronous configuration restore (Param: SDid SVNrevisionId", 1, 1, 0 }, //restore old configuration
    { "JSAUPDATECONF", doUpdateConfig, "*Update configuration", 1, 1, 0 }, // *Mise a jour de la configuration.
    { "JSAUPDATEFIRMWARE", doUpdateFirmware, "Update Firmware of a device based on FMC (Param: SDid [NO_VERIFY][NO_REBOOT][NO_DELETE])", 1, 1, 0 },
    { "JSAUPDATELICENSE", doUpdateLicense, "Update License of a device based on FMC (Param: SDid)", 1, 1, 0 },
    { "JSCALLCOMMAND", doCallCommand, "*Call a command associated with the device via a repository file (apply_flag is: 0=no application 1=apply to base 2=apply to device) (Param: SDid command apply_flag\\r\\nJSONParameters)", 1, 1, 0 },
    { "JSCHECKPROVISIONING", doCheckProvisioning, "*Check initial Provisioning status", 1, 0, 0 }, // *Check initial Provisioning status.
    { "JSCHECKSERIAL", doCheckSerial, "*Check the serial number of the device (Param: SDid ipaddr)", 1, 1, 0 }, // *Verification du numéro de serie du routeur
    { "JSDOPING", doPing, "Check ping status on a host or ip address (params: host/ip_address)", 1, 1, 0 }, // Check ping status on a host or ip address
    { "JSEXECSCRIPT", doJsExecScript, "synchronous execution of a script on the router (Param: sdid script_file_path)", 1, 1, 0 }, // Execute un script sur le routeur.
    { "JSGENTEMPLATE", doGenTemplate, "Template generation from archived configurations (Param: SDid SVNrevisionId1 SVNrevisionId1 [addon card]", 1, 1, 0 }, //restore old configuration
    { "JSGETACTIVITYREPORT", doGetActivityReport, "*Get router activity reports", 1, 1 , 0}, // *Recuperation des rapports d'activite du routeur.
    { "JSGETARCHIVECONF", doGetArchiveConf, "Retrieve a backup from a device and create a Stormshield archive (Param: SDid DestinationDirectory)", 1, 1, 0 }, //
    { "JSGETCONF", doGetConfig, "*Get router generated configuration", 1, 1, 0 }, // *Recuperation de la configuration generee du routeur.
    { "JSGETSDCONF", doGetRunningConf, "*Get router running configuration", 1, 1, 0 }, // *Recuperation de la configuration du routeur.
    { "JSHASWAP", doHaSwap, "*HA Swap", 1, 1, 0 }, // *HA Swap.
    { "JSRESOLVE", doResolve, "*Resolve a template buffer for a device ((Param: SDid \\n template_buffer)", 1, 1 , 0}, // *Resolution d'un template sous forme de chaine pour un device
    { "JSRESOLVETEMPLATE", doResolveTemplate, "*Resolve a template for a device ((Param: SDid template_path)", 1, 1 , 0}, // *Resolution d'un template pour un device
    { "JSSENDDATAFILES", doSendDataFiles, "Send attached data files to the router (Param: sdid [addon card])", 1, 1 , 0}, // Lance une commande sur le routeur.
    { "JSSMSADDONEXEC", doJsSmsAddonExec, "Run a command on the addon card (Param: sdid addon_card \\n commands_to_exec)", 1, 1, 0 }, // Lance une commande sur la carte Addon.
    { "JSSMSEXEC", doJsSmsexec, "Run a command on the router", 1, 1, 0 }, // Lance une commande sur le routeur.
    { "JSSMSEXECN", doJsSmsexecn, "Run commands on the router", 1, 1 , 0}, // Lance une ou plusieurs commandes sur le routeur.
    { "JSSNMPTEST", doSnmpTest, "*Test SNMP connectivity (params: [get|walk] ip_addr community oid)", 1, 1, 0 }, // *Test de connectivite snmp (params: [get|walk] ip_addr community oid).
    { "JSSTAGING", doStaging, "*Staging", 1, 1, 0 }, // *Staging.
    { "JSSTAGINGMAP", doStagingMap, "*Staging", 1, 1, 0 }, // *Staging.
    { "JSUNARCHIVECONF", doUnarchiveConf, "Unarchive Stormshield configuration (Param: sdid ArchiveName DestinationDirectory)", 1, 1, 0 }, //
    { "LISTVERB", doListVerb, "list the verbs", 0, 0, 1 }, // Liste des verbes geres par le SMS.
    { "LOCKPROVISIONING", doLockProvisioning, "Lock router provisioning", 1, 1, 0 }, // Verrouillage de l'approvisionnement.
    { "PPPIPUP", doPppIpUp, "IPUP processing (Param: @IP)", 1, 1, 0 }, // Traitement d'un IPUP venant de syslogd (ipup perdu).
    { "PROVISIONING", doProvisioning, "*initial provisioning (params: SDid [\\n @IP login passwd adminpasswd])", 1, 1, 0 }, // *Provisioning initial (params: SDid [\\n @IP login passwd adminpasswd]).
    { "PS", doPS, "Get SEC engine tasks list", 0, 0, 1 }, // Liste des taches
    { "REINITLOGS", doReinitLogs, "Reinit logs", 0, 1, 1 }, // Reinit logs
    { "SDIPUP", doSdIpUp, "IPUP processing (Param: SDid @IP)", 1, 1, 0 }, // Traitement d'un IPUP venant de syslogd (Param: SDid @IP).
    { "SETCONFIGMODE", doSetConfigMode, "Set the config-step flag for the router (params: SDid mode)", 1, 1, 0 }, // Positionne sd_config_step pour selectionner le mode de mise a jour (DEFAULT, BYLINE, TFTP, SCP) (param SDid config_mode).
    { "SETLOGLEVEL", doSetLogLevel, "Set log level (params: log_level dbg_level)", 1, 1, 1 }, // Positionne le niveau de log et de debug (params: log_level dbg_level).
    { "SNMPGET", do_SnmpGet, "Get SNMP counters (get)", 1, 0, 1 }, // SNMP Counters (get)
    { "SNMPGETNEXT", do_SnmpGetNext, "Get next SNMP Counters (walk)", 1, 0, 1 }, // SNMP Counters (walk)
    { "SNMPTEST", doSnmpTest, "*Test SNMP connectivity (params: [get|walk] ip_addr community oid)", 1, 1, 0 }, // *Test de connectivite snmp (params: [get|walk] ip_addr community oid).
    { "STAGING", doStaging, "*Staging", 1, 1, 0 }, // *Staging.
    { "STAGINGMAP", doStagingMap, "*Staging", 1, 1, 0 }, // *Staging.
    { "STAT", doStat, "SEC engine statistics", 0, 0, 1 }, // Recuperation des statistiques de smsd.
    { "UNLOCKPROVISIONING", doUnLockProvisioning, "Unlock router provisioning", 1, 1, 0 }, //Deverrouillage de l'approvisionnement.
    { "UNLOCKUPDATE", doUnlockUpdate, "Unlock router update", 1, 1, 0 }, // Deverrouillage du SD pour une future mise a jour.
    { "UPDATECONFIG", doUpdateConfig, "*Update configuration", 1, 1, 0 }, // *Mise a jour de la configuration.
    { "END", NULL, "", 0, 0, 0 } };


int VerbInit()
{
  verbAction_t *va;
  int ret;

  verbHashTable = gpul_hash_create(currentConfig->log_handle, GPUL_HASH_TYPE_STR);
  if (!verbHashTable)
  {
    LogWrite(LOG_ERROR, "Unable to create verb hash table\n");
    return -1;
  }

  va = verbTab;

  while (va->doVerb)
  {
    ret = gpul_hash_add_str(currentConfig->log_handle, verbHashTable, va->verb, va);
    if (ret)
    {
      gpul_hash_release(currentConfig->log_handle, verbHashTable, 0);
      LogWrite(LOG_ERROR, "Unable to add verb in hash table\n");
      return -1;
    }
    va++;
  }

  return 0;
}

void VerbEnd()
{
  gpul_hash_release(currentConfig->log_handle, verbHashTable, 0);
}

/*
 * Store parameters in a table considering the quotes (sigle and double).
 * Stops at the end of the line or when nb_params is reached.
 * Modify params.
 */
int parse_params(char *params, int nb_params, char **result)
{
  int found_params = 0;
  char *p; // primary (start)
  char *q; // stop
  int state = 0;
  char *select = " '\"\r\n";

  p = params;

  q = strpbrk(p, select);
  while (q != NULL)
  {
    switch (*q)
    {
      case ' ':
        if (state == 0)
        {
          // space is a separator
          *q = '\0';
          if (strlen(p))
          {
            result[found_params++] = p;
          }
          p = q + 1;
        }
        break;

      case '"':
        if (state == 0)
        {
          // start quote
          p = q + 1;
          state = 1;
        }
        else if (state == 1)
        {
          // end quote
          *q = '\0';
          result[found_params++] = p;
          p = q + 1;
          state = 0;
        }
        break;

      case '\'':
        if (state == 0)
        {
          // start quote
          p = q + 1;
          state = 2;
        }
        else if (state == 2)
        {
          // end quote
          *q = '\0';
          result[found_params++] = p;
          p = q + 1;
          state = 0;
        }
        break;

      case '\n':
      case '\r':
        // end of line
        *q = '\0';
        result[found_params++] = p;
        return found_params;
    }
    if (found_params == nb_params)
    {
      return nb_params;
    }
    q = strpbrk(q + 1, select);
  }

  if (strlen(p))
    result[found_params++] = p;
  return found_params;
}

/** checkverb is used to dispatch the execution depending
 on the verb in the command */
void CheckVerb(char *buf, struct client_state *csp)
{
  char cmd[BUFSIZ];
  char arg[BUFSIZ];
  verbAction_t *va;
  char *p;
  char *q;
  int cmdResult;
  char *task_name;
  char *event = EVT_SMSINFO;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if ((p = strpbrk(buf, "#\r\n")))
    *p = '\0';
  p = buf;

  /* leading skip whitespace */
  while (*p && ((*p == ' ') || (*p == '\t')))
    p++;

  q = cmd;

  while (*p && (*p != ' ') && (*p != '\t'))
  {
    /* insure the command field is upper case */
    *q = toupper(*p);
    q++;
    p++;
  }
  *q = '\0';

  if (*p != '\0')
  {
    while (*p && ((*p == ' ') || (*p == '\t')))
      p++;

    strcpy(arg, p);

    p = arg + strlen(arg) - 1;

    /* ignore trailing whitespace */
    while (*p && ((*p == ' ') || (*p == '\t')))
      *p-- = '\0';
  }
  else
  {
    arg[0] = '\0';
  }

  task_name = strdup(cmd);
  task_name = gpul_str_append(task_name, " ");
  task_name = gpul_str_append(task_name, arg);
  gpul_sched_set_task_name(sched_handle, gpul_sched_get_my_task_handle(sched_handle), task_name);
  gpul_log_set_event(log_handle, cmd);
  free(task_name);
  task_name = strdup(arg);
  p = strchr(task_name, ' ');
  if (p)
  {
    *p = '\0';
  }
  gpul_log_set_sd_id(log_handle, task_name);
  free(task_name);

  va = (verbAction_t *) gpul_hash_get_str(log_handle, verbHashTable, cmd);
  if (!va)
  {
    if (strcmp(cmd, "WAKE_UP_SMS"))
    {
      LogWriteExt(LOG_ERROR, "N/A", event, "Unknown verb %s\n", cmd);
      send_error("", csp, cmd, "", sizeof(""), ERR_VERB_UNKNWN);
    }
    return;
  }

  if (va->log)
  {
    LogWriteExt(LOG_INFO, "N/A", event, "analysing verb %s arg %s\n", cmd, arg);
  }

  /** call the function associated to the verb analyzed in the command line */
  if (va->hasParams)
  {
    if ((va->hasParams == 1) && (strlen(arg) < 4))
    {
      LogWriteExt(LOG_SECURITY, "N/A", event, "Invalid  SD id length <4 SDid:%s\n", arg);
      return;
    }
    /* find a BD context now ! */
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
    cmdResult = va->doVerb(arg, csp);
  }
  else
  {
    cmdResult = va->doVerb("N/A", csp);
  }

  if (csp->db_ctx != NULL)
  {
    RELEASE_BD_CTX(csp);
  }

  if (cmdResult != 0)
  {
    LogWriteExt(LOG_ERROR, "N/A", event, "Unable to deal with verb : %s\n", cmd);
  }

  return;
}

int send_ok(char *SDid, client_state_t *csp, char *verb, char *msg, int len)
{
  int ret;

  if (csp->cfd > 0)
  {
    if (msg)
    {
      ret = socketWrite(csp->cfd, SMSD_OK_STRING, sizeof(SMSD_OK_STRING) - 1);
      if (ret != sizeof(SMSD_OK_STRING) - 1)
      {
        LogWriteExt(LOG_ERROR, SDid, verb, "Error writing OK response to client, ret %d expected %d (%s)\n", ret, sizeof(SMSD_OK_STRING) - 1, strerror(errno));
        return -1;
      }

      ret = socketWrite(csp->cfd, msg, len);
      if (ret != len)
      {
        LogWriteExt(LOG_ERROR, SDid, verb, "Error writing OK response to client, ret %d expected %d (%s)\n", ret, len, strerror(errno));
        return -1;
      }

      ret = socketWrite(csp->cfd, SMSD_MSG_END, sizeof(SMSD_MSG_END) - 1);
      if (ret != sizeof(SMSD_MSG_END) - 1)
      {
        LogWriteExt(LOG_ERROR, SDid, verb, "Error writing OK response to client, ret %d expected %d (%s)\n", ret, sizeof(SMSD_MSG_END) - 1, strerror(errno));
        return -1;
      }
    }
    else
    {
      ret = socketWrite(csp->cfd, SMSD_OK_ALONE_STRING, sizeof(SMSD_OK_ALONE_STRING) - 1);
      if (ret != sizeof(SMSD_OK_ALONE_STRING) - 1)
      {
        LogWriteExt(LOG_ERROR, SDid, verb, "Error writing OK response to client, ret %d expected %d (%s)\n", ret, sizeof(SMSD_OK_ALONE_STRING) - 1, strerror(errno));
        return -1;
      }
    }
  }
  else
  {
    LogWriteExt(LOG_ERROR, SDid, verb, "Error writing response to client, socket closed\n");
  }

  return 0;
}

int send_error(char *SDid, client_state_t *csp, char *verb, char *msg, int len, int error)
{
  char error_code[128];
  char *buf;
  int l;
  int ret;

  if (csp->cfd < 1)
  {
    LogWriteExt(LOG_ERROR, SDid, verb, "Error writing ERROR response to client, socket closed\n");
    return 0;
  }

  ret = socketWrite(csp->cfd, SMSD_ERROR_STRING, sizeof(SMSD_ERROR_STRING) - 1);
  if (ret != sizeof(SMSD_ERROR_STRING) - 1)
  {
    LogWriteExt(LOG_ERROR, SDid, verb, "Error writing ERROR response to client, ret %d expected %d (%s)\n", ret, sizeof(SMSD_ERROR_STRING) - 1, strerror(errno));
    return -1;
  }

  l = sprintf(error_code, "%04d: ", error);
  ret = socketWrite(csp->cfd, error_code, l);
  if (ret != l)
  {
    LogWriteExt(LOG_ERROR, SDid, verb, "Error writing ERROR response to client, ret %d expected %d (%s)\n", ret, l, strerror(errno));
    return -1;
  }

  buf = get_error_message(error);
  if (buf)
  {
    l = strlen(buf);
    ret = socketWrite(csp->cfd, buf, l);
    if (ret != l)
    {
      LogWriteExt(LOG_ERROR, SDid, verb, "Error writing ERROR response to client, ret %d expected %d (%s)\n", ret, l, strerror(errno));
      return -1;
    }
  }

  if (msg)
  {
    ret = socketWrite(csp->cfd, SMSD_MSG_EOL, sizeof(SMSD_MSG_EOL) - 1);
    if (ret != sizeof(SMSD_MSG_EOL) - 1)
    {
      LogWriteExt(LOG_ERROR, SDid, verb, "Error writing ERROR response to client, ret %d expected %d (%s)\n", ret, sizeof(SMSD_MSG_EOL) - 1, strerror(errno));
      return -1;
    }

    ret = socketWrite(csp->cfd, msg, len);
    if (ret != len)
    {
      LogWriteExt(LOG_ERROR, SDid, verb, "Error writing ERROR response to client, ret %d expected %d (%s)\n", ret, len, strerror(errno));
      return -1;
    }
  }

  ret = socketWrite(csp->cfd, SMSD_MSG_END, sizeof(SMSD_MSG_END) - 1);
  if (ret != sizeof(SMSD_MSG_END) - 1)
  {
    LogWriteExt(LOG_ERROR, SDid, verb, "Error writing ERROR response to client, ret %d expected %d (%s)\n", ret, sizeof(SMSD_MSG_END) - 1, strerror(errno));
    return -1;
  }

  return 0;
}

static int doReinitLogs(char *dummy, client_state_t *csp)
{
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  gpul_log_reinit(log_handle);

  send_ok("N/A", csp, "REINITLOGS", NULL, 0);

  LogWrite(LOG_INFO, "REINITLOGS ends OK\n");

  return 0;
}

static int doGetReport(char *params, client_state_t *csp)
{
  char *configuration = NULL;
  char *event = EVT_GETREPORT;
  int result;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  STATGETREPORTTHREADLOCK;
  currentStat.totalgetreport++;
  STATGETREPORTTHREADUNLOCK;
  /* need to get the log from the distant router */
  result = getReport(csp, params, &configuration, event);
  if (!result)
  {
    if (send_ok(params, csp, event, configuration, configuration ? strlen(configuration) : 0))
    {
      freez(configuration);
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    freez(configuration);
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    freez(configuration);
    STATGETREPORTTHREADLOCK;
    currentStat.totalerrorgetreport++;
    STATGETREPORTTHREADUNLOCK;

    if (send_error(params, csp, event, 0, 0, result))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doGetVPNReport(char *params, client_state_t *csp)
{
  char *configuration = NULL;
  char *event = EVT_GETVPNREPORT;
  int result;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  STATGETVPNREPORTTHREADLOCK;
  currentStat.totalgetvpnreport++;
  STATGETVPNREPORTTHREADUNLOCK;
  /* need to get the log from the distant router */
  result = getVPNReport(csp, params, &configuration, event);
  if (!result)
  {
    if (send_ok(params, csp, event, configuration, configuration ? strlen(configuration) : 0))
    {
      freez(configuration);
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    freez(configuration);
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    freez(configuration);
    STATGETVPNREPORTTHREADLOCK;
    currentStat.totalerrorgetreport++;
    STATGETVPNREPORTTHREADUNLOCK;

    if (send_error(params, csp, event, 0, 0, result))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doGetSDConf(char *params, client_state_t *csp)
{
  char *configuration = NULL;
  char *event = EVT_GETSDCONF;
  int result;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  result = getRouterFullConfiguration(params, csp, &configuration, event);
  if (result == 0)
  {
    if (send_ok(params, csp, event, configuration, strlen(configuration)))
    {
      freez(configuration);
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }

    freez(configuration);
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    freez(configuration);
    if (send_error(params, csp, event, 0, 0, result))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  sleep(2);
  return 0;
}

/** this function is used to retreive the SDID from an IP address
 The ip address is stored in the SD records in the database
 There is no update of the running SD configuration
 */
static int updateIP(char *sdid, char *ipaddr, client_state_t *csp, char *event)
{
  sd_info_t sd_info;
  sd_like_t *SD = &(sd_info.SD);
  int ret;
  int iphaschanged = FALSE;
  fildelog_like_t fildelog;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  memset(&sd_info, 0, sizeof(sd_info_t));

  ret = getSDinfo(csp->db_ctx, sdid, &sd_info);

  if (ret != SMS_OK)
  {
    if (ret == ERR_DB_NOTFOUND)
    {
      LogWriteExt(LOG_SECURITY, sdid, event, "SD NOT FOUND in the database\n");
    }
    else
    {
      LogWriteExt(LOG_ERROR, sdid, event, "Cannot get SD FAILED\n");
    }
    return ret;
  }

  RELEASE_BD_CTX(csp);

  if (SD->sd_prov_lock)
  {
    goto end;
  }

  if (!sd_info.sd_ext_interface)
  {
    goto end;
  }

  // check serial number
  ret = genericCheckSerialNumber(&sd_info, ipaddr);
  if (ret)
  {
    goto end;
  }

  /* get a new BD ctx */
  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
  }

  // Check for address change
  if (strcmp(ipaddr, SD->sd_ip_config) != 0)
  {
    if (sd_info.sd_ext_interface->interface.int_ip_type[0] != 'S')
    {
    iphaschanged = TRUE;
    ret = HA_SMSSQL_SetSDIPConfig(&(csp->db_ctx->sql_ctx), SD->sd_cli_prefix, SD->sd_seqnum, ipaddr, TRUE);
    if (ret == ERR_DB_FAILED || ret == ERR_DB_NOTFOUND)
    {
      if (ret == ERR_DB_FAILED)
      {
        LogWriteExt(LOG_ERROR, sdid, event, "Cannot update IP config add (%s) FAILED\n", ipaddr);
      }
      else
      {
        LogWriteExt(LOG_SECURITY, sdid, event, "SD NOT FOUND in the database\n");
      }
      goto end;
    }

    memset(&fildelog, 0, sizeof(fildelog));
    strncpy(fildelog.cli_prefix, SD->sd_cli_prefix, CLI_PREFIX_LEN);
    fildelog.seqnum = SD->sd_seqnum;

      fildelog.log_level = IPUPDOWN_LOG_LEVEL;
      sprintf(fildelog.log_msg, IPUP_IP_DYN_LOG, IPUPDOWN_LOG_LEVEL, SD->sd_cli_prefix, SD->sd_seqnum, SD->sd_ip_config, ipaddr);
      sprintf(fildelog.log_reference, IPUP_LOG_REFERENCE, IPUPDOWN_LOG_LEVEL);

    ret = set_log_by_id(csp->db_ctx, &fildelog);

    LogWriteExt((sd_info.sd_ext_interface->interface.int_ip_type[0] == 'S') ? LOG_WARNING : LOG_INFO, sdid, event, fildelog.log_msg);
    if (ret == ERR_DB_FAILED || ret == ERR_DB_NOTFOUND)
    {
      if (ret == ERR_DB_FAILED)
      {
        LogWriteExt(LOG_ERROR, sdid, event, "Cannot write log FAILED\n");
      }
      else
      {
        LogWriteExt(LOG_SECURITY, sdid, event, "SD NOT FOUND in the database\n");
      }
      goto end;
    }

    dnsUpdate(sdid, ipaddr);
    }
  }
  else
  {
    if (SD->sd_connected == 0)
    {
      ret = HA_SMSSQL_SetSDConnected(&(csp->db_ctx->sql_ctx), SD->sd_cli_prefix, SD->sd_seqnum, TRUE);
      if (ret == ERR_DB_FAILED || ret == ERR_DB_NOTFOUND)
      {
        if (ret == ERR_DB_FAILED)
        {
          LogWriteExt(LOG_ERROR, sdid, event, "Cannot update connected flag FAILED\n");
        }
        else
        {
          LogWriteExt(LOG_SECURITY, sdid, event, "SD NOT FOUND in the database\n");
        }
        goto end;
      }

      memset(&fildelog, 0, sizeof(fildelog));
      strncpy(fildelog.cli_prefix, SD->sd_cli_prefix, CLI_PREFIX_LEN);
      fildelog.seqnum = SD->sd_seqnum;
      fildelog.log_level = IPUPDOWN_LOG_LEVEL;
      sprintf(fildelog.log_msg, IPUP_LOG, IPUPDOWN_LOG_LEVEL, (sd_info.sd_ext_interface->interface.int_ip_type[0] == 'S' ? "static" : "dynamic"), SD->sd_cli_prefix, SD->sd_seqnum, ipaddr);
      sprintf(fildelog.log_reference, IPUP_LOG_REFERENCE, IPUPDOWN_LOG_LEVEL);

      ret = set_log_by_id(csp->db_ctx, &fildelog);

      LogWriteExt(LOG_INFO, sdid, event, fildelog.log_msg);
      if (ret == ERR_DB_FAILED || ret == ERR_DB_NOTFOUND)
      {
        if (ret != ERR_DB_NOTFOUND)
        {
          LogWriteExt(LOG_ERROR, sdid, event, "Cannot write IPUP FAILED\n");
        }
        else
        {
          LogWriteExt(LOG_SECURITY, sdid, event, "SD NOT FOUND in the database\n");
        }
        goto end;
      }
    }
  }

  RELEASE_BD_CTX(csp);

  end:

  freeSDinfo(&sd_info);
  if ((ret == 0) && iphaschanged)
  {
    ret = IPCHANGED;
  }
  return ret;
}

/** this function is used to retreive the SDID from an IP address
 The ip address is stored in the SD records in the database
 There is no update of the running SD configuration
 */
static int updateRouterIP(char *params, client_state_t *csp, char **trueSDid, char *event)
{
  sd_info_t sd_info;
  sd_like_t *SD = &(sd_info.SD);
  int ret;
  char buf[BUFSIZ];
  int iphaschanged = FALSE;
  fildelog_like_t fildelog;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  memset(&sd_info, 0, sizeof(sd_info_t));

  /* get informations about the SD and its IP address to look if its address has changed */
  ret = getSDinfo(csp->db_ctx, SD->sdid, &sd_info);

  if (ret != SMS_OK)
  {
    if (ret == ERR_DB_NOTFOUND)
    {
      LogWriteExt(LOG_SECURITY, SD->sdid, event, "SD NOT FOUND in the database\n");
    }
    else
    {
      LogWriteExt(LOG_ERROR, SD->sdid, event, "Cannot get SD FAILED\n");
    }
    return ret;
  }

  RELEASE_BD_CTX(csp);

  if (!sd_info.sd_ext_interface)
  {
    freeSDinfo(&sd_info);
    return SMS_OK;
  }

  // check serial number
  ret = genericCheckSerialNumber(&sd_info, params);
  if (ret)
  {
    freeSDinfo(&sd_info);
    return ret;
  }

  /* get a new BD ctx */
  if (csp->db_ctx == NULL)
  {
    csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
  }

  /* params contains the ip */
  if (strcmp(params, SD->sd_ip_config) != 0)
  {
    if (sd_info.sd_ext_interface->interface.int_ip_type[0] != 'S')
    {
      iphaschanged = TRUE;
      ret = HA_SMSSQL_SetSDIPConfig(&(csp->db_ctx->sql_ctx), SD->sd_cli_prefix, SD->sd_seqnum, params, TRUE);
      if (ret == ERR_DB_FAILED || ret == ERR_DB_NOTFOUND)
      {
        if (ret == ERR_DB_FAILED)
        {
          LogWriteExt2(LOG_ERROR, SD->sd_cli_prefix, SD->sd_seqnum, event, "Cannot update IP config add (%s) FAILED\n", params);
        }
        else
        {
          LogWriteExt2(LOG_SECURITY, SD->sd_cli_prefix, SD->sd_seqnum, event, "SD NOT FOUND in the database\n");
        }
        freeSDinfo(&sd_info);
        return ret;
      }

      memset(&fildelog, 0, sizeof(fildelog));
      strncpy(fildelog.cli_prefix, SD->sd_cli_prefix, CLI_PREFIX_LEN);
      fildelog.seqnum = SD->sd_seqnum;
      fildelog.log_level = IPUPDOWN_LOG_LEVEL;
      sprintf(fildelog.log_msg, IPUP_IP_DYN_LOG, IPUPDOWN_LOG_LEVEL, SD->sd_cli_prefix, SD->sd_seqnum, SD->sd_ip_config, params);
      sprintf(fildelog.log_reference, IPUP_LOG_REFERENCE, IPUPDOWN_LOG_LEVEL);

      ret = set_log_by_id(csp->db_ctx, &fildelog);

      LogWriteExt2(LOG_INFO, SD->sd_cli_prefix, SD->sd_seqnum, event, fildelog.log_msg);
      if (ret == ERR_DB_FAILED || ret == ERR_DB_NOTFOUND)
      {
        if (ret == ERR_DB_FAILED)
        {
          LogWriteExt2(LOG_ERROR, SD->sd_cli_prefix, SD->sd_seqnum, event, "Cannot write IPUP FAILED\n");
        }
        else
        {
          LogWriteExt2(LOG_SECURITY, SD->sd_cli_prefix, SD->sd_seqnum, event, "SD NOT FOUND in the database\n");
        }
        freeSDinfo(&sd_info);
        return ret;
      }
    }
  }
  else
  {
    if (SD->sd_connected == 0)
    {
      ret = HA_SMSSQL_SetSDConnected(&(csp->db_ctx->sql_ctx), SD->sd_cli_prefix, SD->sd_seqnum, TRUE);
      if (ret == ERR_DB_FAILED || ret == ERR_DB_NOTFOUND)
      {
        if (ret == ERR_DB_FAILED)
        {
          LogWriteExt2(LOG_ERROR, SD->sd_cli_prefix, SD->sd_seqnum, event, "Cannot update connected flag FAILED\n");
        }
        else
        {
          LogWriteExt2(LOG_SECURITY, SD->sd_cli_prefix, SD->sd_seqnum, event, "SD NOT FOUND in the database\n");
        }
        freeSDinfo(&sd_info);
        return ret;
      }

      memset(&fildelog, 0, sizeof(fildelog));
      strncpy(fildelog.cli_prefix, SD->sd_cli_prefix, CLI_PREFIX_LEN);
      fildelog.seqnum = SD->sd_seqnum;
      fildelog.log_level = IPUPDOWN_LOG_LEVEL;
      sprintf(fildelog.log_msg, IPUP_LOG, IPUPDOWN_LOG_LEVEL, (sd_info.sd_ext_interface->interface.int_ip_type[0] == 'S' ? "static" : "dynamic"), SD->sd_cli_prefix, SD->sd_seqnum, params);
      sprintf(fildelog.log_reference, IPUP_LOG_REFERENCE, IPUPDOWN_LOG_LEVEL);

      ret = set_log_by_id(csp->db_ctx, &fildelog);

      LogWriteExt2(LOG_INFO, SD->sd_cli_prefix, SD->sd_seqnum, event, fildelog.log_msg);
      if (ret == ERR_DB_FAILED || ret == ERR_DB_NOTFOUND)
      {
        if (ret == ERR_DB_FAILED)
        {
          LogWriteExt2(LOG_ERROR, SD->sd_cli_prefix, SD->sd_seqnum, event, "Cannot write IPUP FAILED\n");
        }
        else
        {
          LogWriteExt2(LOG_SECURITY, SD->sd_cli_prefix, SD->sd_seqnum, event, "SD NOT FOUND in the database\n");
        }
        freeSDinfo(&sd_info);
        return ret;
      }
    }
  }

  RELEASE_BD_CTX(csp);

  if (iphaschanged)
  {
    sprintf(buf, "%s%lu", SD->sd_cli_prefix, SD->sd_seqnum);
    *trueSDid = strdup(buf);

    dnsUpdate(*trueSDid, params);
  }

  freeSDinfo(&sd_info);

  if (iphaschanged)
  {
    return IPCHANGED;
  }
  else
  {
    return 0;
  }
}

/** this function uses a SDid formatted as follow:
 "<dotted_ip_add>\0"
 This function is used to handle a PPPIPUP coming from
 the sms_syslogd process.
 Model is unknown (ipup perdu)
 */
static int doPppIpUp(char *params, client_state_t *csp)
{
  return handlePppIpUp(params, csp);
}

/** this function uses a SDid formatted as follow:
 "<dotted_ip_add>\0"
 This function is used to handle a <model>PPPIPUP coming from
 the sms_syslogd process.
 Once the ip is stored in the database the connection to the client
 is closed there is no response for this verb.
 There is no configuration update for the sd connected or to the
 partners of this SD.
 */
static int handlePppIpUp(char *params, client_state_t *csp)
{
  int result;
  char *trueSDid = NULL;
  char *event;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  event = EVT_PPPIPUP;
  gpul_log_set_event(log_handle, event);
  STATPPPIPUPTHREADLOCK;
  currentStat.totalpppipup++;
  STATPPPIPUPTHREADUNLOCK;

  LogWriteExt(LOG_INFO, params, event, "%s of add %s\n", event, params);

  result = updateRouterIP(params, csp, &trueSDid, event);

  if (result != 0)
  {
    STATPPPIPUPTHREADLOCK;
    currentStat.totalerrorpppipup++;
    STATPPPIPUPTHREADUNLOCK;
  }

  if (result == 0 || result == IPCHANGED)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  freez(trueSDid);
  return 0;
}

/** this function uses a SDid formatted as follow:
 "<SDID> <dotted_ip_add>\0"
 This function is used to handle an IPUP coming from the sms_syslogd process.
 Once the ip is stored in the database the connection to the client
 is closed there is no response for this verb.
 */
static int doSdIpUp(char *params, client_state_t *csp)
{
  int result;
  char *p;
  char *sdid;
  char *ipaddr;
  char *event = EVT_SDIPUP;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  STATPPPIPUPTHREADLOCK;
  currentStat.totalpppipup++;
  STATPPPIPUPTHREADUNLOCK;
  LogWriteExt(LOG_INFO, params, event, "SDid @IP : %s\n", params);

  /* no user info returned */
  socketClose(csp->cfd);
  csp->cfd = -1;

  sdid = params;
  p = strchr(params, ' ');
  if (p != NULL)
  {
    *p = '\0';
    p++;
    if (!*p)
    {
      LogWriteExt(LOG_ERROR, params, event, "%s\n", get_error_message(ERR_VERB_BAD_PARAM));
      return 0;
    }
    ipaddr = p;
  }
  else
  {
    LogWriteExt(LOG_ERROR, params, event, "%s\n", get_error_message(ERR_VERB_BAD_PARAM));
    return 0;
  }

  result = updateIP(sdid, ipaddr, csp, event);

  if (result != 0)
  {
    STATPPPIPUPTHREADLOCK;
    currentStat.totalerrorpppipup++;
    STATPPPIPUPTHREADUNLOCK;
    LogWriteExt(LOG_ERROR, sdid, event, "%s\n", get_error_message(result));
  }

  if (result == 0 || result == IPCHANGED)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doIpDown(char *SDidandIP, client_state_t *csp)
{
  int result = 0;
  sd_info_t sd_info;
  sd_like_t *SD = &(sd_info.SD);
  int ret;
  char SDid[64];
  char ip[64];
  char *event = EVT_IPDOWN;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sscanf(SDidandIP, "%s %s", SDid, ip);
  STATIPDOWNTHREADLOCK;
  currentStat.totalipdown++;
  STATIPDOWNTHREADUNLOCK;
  LogWriteExt(LOG_INFO, SDid, event, "%s of SDid : %s\n", event, SDid);

  ret = getSDinfo(csp->db_ctx, SDid, &sd_info);
  if (ret)
  {
    LogWriteExt(LOG_ERROR, SDid, event, "Cannot get sd info\n");
    result = ERR_DB_NOTFOUND;
    goto error;
  }

  if (SD->sd_connected)
  {
    if (strcmp(ip, SD->sd_ip_config) == 0 || *SD->sd_ip_config == '\0')
    {
      /* the address of the IPdown and the previous IPup is the same
       there is no race condition beetween a down and a UP */

      fildelog_like_t fildelog;

      memset(&fildelog, 0, sizeof(fildelog));
      strncpy(fildelog.cli_prefix, SD->sd_cli_prefix, CLI_PREFIX_LEN);
      fildelog.seqnum = SD->sd_seqnum;
      fildelog.log_level = IPUPDOWN_LOG_LEVEL;
      sprintf(fildelog.log_msg, IPDOWN_LOG, IPUPDOWN_LOG_LEVEL, SD->sd_cli_prefix, SD->sd_seqnum);
      sprintf(fildelog.log_reference, IPDOWN_LOG_REFERENCE, IPUPDOWN_LOG_LEVEL);

      result = set_log_by_id(csp->db_ctx, &fildelog);

      LogWriteExt2(LOG_INFO, SD->sd_cli_prefix, SD->sd_seqnum, event, fildelog.log_msg);
      if (result == ERR_DB_FAILED || result == ERR_DB_NOTFOUND)
      {
        if (result == ERR_DB_FAILED)
        {
          LogWriteExt(LOG_ERROR, SDid, event, "Cannot write IPDOWN FAILED\n");
        }
        else
        {
          LogWriteExt(LOG_SECURITY, SDid, event, "SD NOT FOUND in the database\n");
        }
        goto error;
      }

      result = HA_SMSSQL_SetSDConnected(&(csp->db_ctx->sql_ctx), SD->sd_cli_prefix, SD->sd_seqnum, FALSE);
      if (result == ERR_DB_FAILED || result == ERR_DB_NOTFOUND)
      {
        if (result == ERR_DB_FAILED)
        {
          LogWriteExt(LOG_ERROR, SDid, event, "Cannot set sd_connected FAILED\n");
        }
        else
        {
          LogWriteExt(LOG_SECURITY, SDid, event, "SD NOT FOUND in the database\n");
        }
        goto error;
      }
    }
    else
    {
      LogWriteExt(LOG_INFO, SDid, event, "%s: race condition for %s old ip=%s new ip=%s\n", event, SDid, ip, SD->sd_ip_config);
    }
  }

  if (send_ok(SDid, csp, event, 0, 0))
  {
    LogWriteExt(LOG_ERROR, SDid, event, "Error handling %s: writing response to client\n", event);
  }
  GLogINFO(log_handle, "ends OK");

  freeSDinfo(&sd_info);

  return 0;

  error:

  STATIPDOWNTHREADLOCK;
  currentStat.totalerroripdown++;
  STATIPDOWNTHREADUNLOCK;

  freeSDinfo(&sd_info);

  if (send_error(SDid, csp, event, 0, 0, result))
  {
    LogWriteExt(LOG_ERROR, SDid, event, "Error handling %s: writing response to client\n", event);
  }
  GLogINFO(log_handle, "ends ERROR");

  return 0;
}

static int doCheckUpdate(char *params, client_state_t *csp)
{
  char *configuration = NULL;
  char *event = EVT_CHECKUPDATE;
  int result;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  result = checkUpdate(params, csp, &configuration, event);
  if (result == 0)
  {
    if (send_ok(params, csp, event, configuration, strlen(configuration)))
    {
      freez(configuration);
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    freez(configuration);
  }
  else
  {
    freez(configuration);
    if (send_error(params, csp, event, 0, 0, result))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doCheckAllUpdate(char * params, client_state_t *csp)
{
  char *configuration = NULL;
  char *event = EVT_CHECKALLUPDATE;
  int result;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  result = checkAllUpdate(params, csp, &configuration, event);
  if (result == 0)
  {
    if (send_ok(params, csp, event, configuration, strlen(configuration)))
    {
      freez(configuration);
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    freez(configuration);
  }
  else
  {
    freez(configuration);
    if (send_error(params, csp, event, 0, 0, result))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doIsAlive(char *params, client_state_t *csp)
{
  char *event = EVT_ISALIVE;

  if (send_ok(params, csp, event, 0, 0))
  {
    LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
    return -1;
  }

  return 0;
}

static int doStat(char *params, client_state_t *csp)
{
  char buf[BUFSIZ];
  struct timeval tv;
  struct tm tc;
  char bufcu[64];
  char *event = EVT_STAT;

  /*
   * For debug purpose:  Assert signals handler was not redefined.

   struct sigaction action;
   memset (&action, '\0', sizeof(action));
   if(sigaction(SIGHUP, NULL, &action) == 0)
   {
   size_t fctptr;
   if(action.sa_flags == SA_SIGINFO)
   {
   fctptr = action.sa_sigaction;
   } else {
   fctptr = action.sa_handler;
   }
   GLogDEBUG(currentConfig->log_handle, 15, "signal function pointer: %p", action.sa_sigaction);
   }*/

  gettimeofday(&tv, NULL);
  gpul_get_date(&tc, 0);
  snprintf(bufcu, sizeof(bufcu), "%d/%02d/%02d:%02d:%02d:%02d", tc.tm_year + 1900, tc.tm_mon + 1, tc.tm_mday, tc.tm_hour, tc.tm_min, tc.tm_sec);
  sprintf(
      buf,
      "\ntotalThreads=%lu\nactiveThreads=%lu\nupdate=%lu\nerror_update=%lu\ngetreport=%lu\nerror_getreport=%lu\ngetconf=%lu\nerror_getconf=%lu\ngetvpnreport=%lu\nerror_getvpnreport=%lu\ngetrouterip=%lu\nerror_getrouterip=%lu\ncheckupdate=%lu\nerror_checkupdate=%lu\nipdown=%lu\nerror_ipdown=%lu\nstart_time=%s\ncurrent_time=%s\nstart_time_seconds=%lu\ncurrent_time_second=%lu\n",
      currentStat.totalThreads, currentStat.activeThreads, currentStat.totalupdate, currentStat.totalerrorupdate, currentStat.totalgetreport, currentStat.totalerrorgetreport,
      currentStat.totalgetconf, currentStat.totalerrorgetconf, currentStat.totalgetvpnreport, currentStat.totalerrorgetvpnreport, currentStat.totalgetrouterip,
      currentStat.totalerrorgetrouterip, currentStat.totalcheckupdate,
      currentStat.totalerrorcheckupdate, currentStat.totalipdown,
      currentStat.totalerroripdown,
      bufstartTime, bufcu, startTime, tv.tv_sec);

  if (send_ok(params, csp, event, buf, strlen(buf)))
  {
    LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
    return -1;
  }

  return 0;
}

static int getRouterIp(char *params, client_state_t *csp, char *IpString)
{
  sd_info_t sd_info;
  char *event = EVT_GETROUTERIP;
  int ret;

  LogWriteExt(LOG_INFO, params, event, "Getting IP for %s ... \n", params);

  ret = getSDinfo(csp->db_ctx, params, &sd_info);
  if (ret)
  {
    LogWriteExt(LOG_ERROR, params, event, "Cannot get sd info\n");
    return ret;
  }

  /* release its db ctx */
  RELEASE_BD_CTX(csp);

  if (*(sd_info.SD.sd_ip_config) != '\0') /* sd_connected est a 1 si sd_ip_config != 0 donc pas teste */
  {
    sprintf(IpString, "%s", sd_info.SD.sd_ip_config);
  }
  else if (sd_info.sd_ext_interface && *(sd_info.sd_ext_interface->interface.int_ip_addr) != '\0')
  {
    sprintf(IpString, "%s", sd_info.sd_ext_interface->interface.int_ip_addr);
  }
  else
  {
    ret = ERR_SD_NOIP;
  }

  freeSDinfo(&sd_info);
  return ret;
}

static int doGetRouterIp(char *params, client_state_t *csp)
{
  char *event = EVT_GETROUTERIP;
  char IpString[BUFSIZ];
  int result;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  STATGETROUTERIPTHREADLOCK;
  currentStat.totalgetrouterip++;
  STATGETROUTERIPTHREADUNLOCK;

  result = getRouterIp(params, csp, IpString);
  if (!result)
  {
    if (send_ok(params, csp, event, IpString, strlen(IpString)))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    STATGETROUTERIPTHREADLOCK;
    currentStat.totalerrorgetrouterip++;
    STATGETROUTERIPTHREADUNLOCK;

    if (send_error(params, csp, event, 0, 0, result))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

/** UNLOCKUPDATE verb
 *
 */
static int doUnlockUpdate(char *params, client_state_t *csp)
{
  char *event = EVT_UNLOCKUPDATE;
  int result;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  result = unlockUpdate(params, csp);
  if (result == 0)
  {
    if (send_ok(params, csp, event, 0, 0))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    if (send_error(params, csp, event, 0, 0, result))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

/** LOCKPROVISIONING verb
 *
 */
static int doLockProvisioning(char *params, client_state_t *csp)
{
  char *event = EVT_LOCKPROVISIONING;
  int result;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  result = lockProvisioning(params, csp);
  if (result == 0)
  {
    if (send_ok(params, csp, event, 0, 0))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    if (send_error(params, csp, event, 0, 0, result))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

/** UNLOCKPROVISIONING verb
 *
 */
static int doUnLockProvisioning(char *params, client_state_t *csp)
{
  char *event = EVT_UNLOCKPROVISIONING;
  int result;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  result = unlockProvisioning(params, csp);
  if (result == 0)
  {
    if (send_ok(params, csp, event, 0, 0))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    if (send_error(params, csp, event, 0, 0, result))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doJsSmsAddonExec(char *params, client_state_t *csp)
{
  int error;
  void *log_handle;
  char sdid[BUFSIZ];
  char addon[BUFSIZ];
  int ret;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  sdid[0] = '\0';
  addon[0] = '\0';

  if (strlen(params) >= BUFSIZ)
  {
    GLogERROR(log_handle, "params too long");
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  ret = sscanf(params, "%s %s", sdid, addon);
  if (ret < 2)
  {
    GLogERROR(log_handle, "missing params");
    sms_send_user_error((long) csp, "", "", ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  error = smsexecn(sdid, csp, NULL, addon);
  if (!error)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doJsSmsexec(char *params, client_state_t *csp)
{
  int error;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  error = smsexec(params, csp, NULL, NULL);
  if (!error)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doJsSmsexecn(char *params, client_state_t *csp)
{
  int error;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  error = smsexecn(params, csp, NULL, NULL);
  if (!error)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doCmd(char *params, client_state_t *csp)
{
  int error;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  error = smsCmd(params, csp);
  if (!error)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doCustCmd(char *params, client_state_t *csp)
{
  int error;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  error = smsCustCmd(params, csp);
  if (!error)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doCheckCmd(char *params, client_state_t *csp)
{
  char sdid[BUFSIZ];
  char cmd[BUFSIZ];
  char event[BUFSIZ];
  int ret;
  char *result = NULL;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = sscanf(params, "%s %s", sdid, cmd);
  if (ret < 1)
  {
    send_error(sdid, csp, "CHECKCMD", NULL, 0, ERR_VERB_BAD_PARAM);
    return ERR_VERB_BAD_PARAM;
  }

  sprintf(event, "CHECKCMD %s", cmd);

  ret = getUpdateStatus(sdid, csp, cmd, &result);
  if (!ret)
  {
    if (send_ok(sdid, csp, event, result, strlen(result)))
    {
      freez(result);
      GLogERROR(log_handle, "Error handling %s: writing response to client");
      return -1;
    }
    freez(result);
  }
  else
  {
    freez(result);
    if (send_error(sdid, csp, event, NULL, 0, ret))
    {
      GLogERROR(log_handle, "Error handling %s: writing response to client");
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}



static int js_exec_script(char *params, client_state_t *csp, int synchronous)
{
  int error;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  error = exec_script(params, csp, synchronous);
  if (!error)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doJsExecScript(char *params, client_state_t *csp)
{
  return js_exec_script(params, csp, 1);
}

static int doJsAExecScript(char *params, client_state_t *csp)
{
  return js_exec_script(params, csp, 0);
}

/** Lecture en base des infos d'asset management
 */
static int doGetAsset(char *params, client_state_t *csp)
{
  char buf[BUFSIZ];
  char *event = EVT_GETASSET;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = sdGetAsset(csp, params, buf, event);
  if (!ret)
  {
    if (send_ok(params, csp, event, buf, strlen(buf)))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    if (send_error(params, csp, event, 0, 0, ret))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

/** Force la relecture sur le routeur des informations d'asset
 */
static int doForceAsset(char *params, client_state_t *csp)
{
  char *event = EVT_FORCEASSET;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = sdForceAsset(csp, params);
  if (!ret)
  {
    if (send_ok(params, csp, event, NULL, 0))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    if (send_error(params, csp, event, 0, 0, ret))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

/** Modification en base de sd_config_step
 * SDid contient SDid config_mode
 */
static int doSetConfigMode(char *params, client_state_t *csp)
{
  char buf[BUFSIZ];
  char *event = EVT_SETCONFIGMODE;
  char trueSDid[BUFSIZ];
  char config_mode[BUFSIZ];
  int cfg_mode;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  config_mode[0] = '\0';
  sscanf(params, "%s %s", trueSDid, config_mode);

  for (cfg_mode = 0; cfg_mode < (sizeof(config_mode_list) / sizeof(char *)); cfg_mode++)
  {
    if (!strcmp(config_mode, config_mode_list[cfg_mode]))
    {
      break;
    }
  }
  if (cfg_mode == (sizeof(config_mode_list) / sizeof(char *)))
  {
    cfg_mode = 0;
  }

  ret = sdSetConfigMode(csp, trueSDid, cfg_mode, buf, event);
  if (!ret)
  {
    if (send_ok(trueSDid, csp, event, buf, strlen(buf)))
    {
      LogWriteExt(LOG_ERROR, trueSDid, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    if (send_error(trueSDid, csp, event, buf, strlen(buf), ret))
    {
      LogWriteExt(LOG_ERROR, trueSDid, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

/** PROVISIONING verb
 */
static int doProvisioning(char *params, client_state_t *csp)
{
  char *event = EVT_PROVISIONING;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = provisioning(csp, params, event);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

/**
 * JSDOPING
 */
static int doPing(char *params, client_state_t *csp)
{
  int ret;
  void *log_handle;
  char *event = EVT_DOPING;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = getPingStatus(csp, params, event);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
    return -1;
  }
  return 0;
}

/** JSAPUSHCONFIG verb
 */
static int doPushConfig(char *params, client_state_t *csp)
{
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = push_config(csp, params);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doCheckProvisioning(char *params, client_state_t *csp)
{
  char *event = EVT_CHECKPROVISIONING;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = checkProvisioning(csp, params, event);
  if (!ret)
  {
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

/** STAGING verb
 */
static int doStaging(char *params, client_state_t *csp)
{
  char *event = EVT_STAGING;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = staging(csp, params, 0, event);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

/** STAGINGMAP verb
 */
static int doStagingMap(char *params, client_state_t *csp)
{
  char *event = EVT_STAGING;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = staging(csp, params, 1, event);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

/** UPDATECONFIG verb
 */
static int doUpdateConfig(char *params, client_state_t *csp)
{
  char *event = EVT_UPDATECONFIG;
  sd_info_t sd_info;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = getSDinfo(csp->db_ctx, params, &sd_info);
  if (ret)
  {
    sms_send_user_error((long) csp, params, "Cannot get sd info", ret);
  }
  else
  {
    ret = updateConfig(csp, &sd_info, event);
  }

  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  freeSDinfo(&sd_info);

  return 0;
}

/** JSARESTORECONF verb
 */
static int doRestoreConf(char *params, client_state_t *csp)
{
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = restoreConf(params, csp);

  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

/** JSGENTEMPLATE verb
 */
static int doGenTemplate(char *params, client_state_t *csp)
{
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = genTemplate(params, csp);

  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doBackupConf(char *params, client_state_t *csp)
{
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = backupRouterFullConfiguration(params, csp);

  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doGetRunningConf(char *params, client_state_t *csp)
{
  char *event = EVT_GETRUNNINGCONF;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = getRunningConf(csp, params, event);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;

}

static int doResolveTemplate(char *params, client_state_t *csp)
{
  char *event = EVT_RESOLVETEMPLATE;
  int ret;
  char path[BUFSIZ];
  char SDid[BUFSIZ];
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);
  sscanf(params, "%s %s", SDid, path);

  ret = resolve_template(csp, SDid, path);
  if (ret)
  {
    if (send_error(params, csp, event, 0, 0, ret))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doResolve(char *params, client_state_t *csp)
{
  char *event = EVT_RESOLVETEMPLATE;
  sd_info_t sd_info;
  int ret;
  char buf[BUFSIZ];
  char *configuration;
  char SDid[SDID_LEN];
  size_t len;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);
  strncpy(SDid, params, sizeof(SDid) - 1);
  SDid[sizeof(SDid) - 1] = '\0';

  ret = getSDinfo(csp->db_ctx, SDid, &sd_info);
  if (ret)
  {
    GLogERROR(log_handle, " Cannot get sd info\n");
    goto err;
  }

  /* Read configuration */
  configuration = NULL;
  ret = socketReadLine(csp->cfd, buf, sizeof(buf), 2);
  while (ret >= 0)
  {
    configuration = gpul_str_append(configuration, buf);
    if (ret != (sizeof(buf) -1))
    {
      configuration = gpul_str_append(configuration, "\n");
    }
    ret = socketReadLine(csp->cfd, buf, sizeof(buf), 2);
  }

  len = strlen(configuration);
  if ((configuration == NULL) || len == 0)
  {
    ret = ERR_VERB_BAD_PARAM;
    goto end;
  }

  configuration[len - 1] = '\0';
  ret = resolve_template_buffer(csp, SDid, configuration);
  if (ret)
  {
    goto end;
  }

  end:

  freeSDinfo(&sd_info);
  freez(configuration);

  err:

  if (ret)
  {
    if (send_error(params, csp, event, 0, 0, ret))
    {
      LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doGetConfig(char *params, client_state_t *csp)
{
  char *event = EVT_GETCONFIG;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = getConfig(csp, params, event);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;

}

static int doGetActivityReport(char *params, client_state_t *csp)
{
  char *event = EVT_GETACTIVITYREPORT;
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = getActivityReport(csp, params, event);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

/*
 * Params are snmp_cmd ip_address community oid
 * snmp_cmd = get or walk
 */
static int doSnmpTest(char *params, client_state_t *csp)
{
  char *param[4];
  char *p = params;
  int i;
  int ret;
  char *event = EVT_SNMPTEST;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  for (i = 0; i < 4; i++)
  {
    param[i] = p;
    while (*p && (*p != ' ') && (*p != '\t'))
    {
      p++;
    }
    if (*p)
    {
      *p = '\0';
      p++;
    }
  }

  ret = genericSnmpTest(csp, param[0], param[1], param[2], param[3], event);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;

}

/** PS verb
 *
 */
static int doPS(char *params, client_state_t *csp)
{
  char *p;
  p = gpul_sched_stats(sched_handle);
  send_ok(params, csp, "PS", p, strlen(p));
  free(p);

  return 0;
}

/** SETLOGLEVEL verb
 *  SDid contient <log_level> <dbg_level>
 */
static int doSetLogLevel(char *params, client_state_t *csp)
{
  char *event = EVT_SETLOGLEVEL;
  int log_level = -1;
  int dbg_level = -1;

  sscanf(params, "%d %d", &log_level, &dbg_level);

  if (log_level != -1)
  {
    LogWrite(LOG_INFO, "SMSD daemon set loglevel %d\n", log_level);
    LogSetLevel(log_level);
  }

  if (dbg_level != -1)
  {
    LogWrite(LOG_INFO, "SMSD daemon set dbglevel %d\n", dbg_level);
    DbgSetLevel(dbg_level);
    gpul_log_set_level(gpul_sched_get_my_log_handle(sched_handle), dbg_level);
  }

  if (send_ok("N/A", csp, event, NULL, 0))
  {
    LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
    return -1;
  }

  return 0;
}

/** LISTVERB verb
 *
 */
static int doListVerb(char *params, client_state_t *csp)
{
  verbAction_t *v;
  char buf[4 * BUFSIZ];
  char *event = EVT_LISTVERB;
  char *p;
  int cfg_mode;
  int n;
  valentry_t *jsacmd;

  p = buf;
  p += sprintf(p, "\n");

  for (n = 0, v = verbTab; v->doVerb; v++)
  {
    p += sprintf(p, "%-24s %s\n", v->verb, v->description);
    n++;
  }

  p += sprintf(p, "\n%d verbs\n", n);

  p += sprintf(p, "\nCommands parameters for JSACMD verb:\n");
  jsacmd = GPUL_TAILQ_FIRST(&currentConfig->jsacmd_list);
  while (jsacmd)
  {
    p += sprintf(p, "\tJSACMD SDID %s\n", jsacmd->val);
    jsacmd = GPUL_TAILQ_NEXT(jsacmd, link);
  }

  p += sprintf(p, "\nCommands parameters for JSACUSTCMD verb:\n");
  jsacmd = GPUL_TAILQ_FIRST(&currentConfig->jsacustcmd_list);
  while (jsacmd)
  {
    p += sprintf(p, "\tJSACUSTCMD CUSTID %s\n", jsacmd->val);
    jsacmd = GPUL_TAILQ_NEXT(jsacmd, link);
  }

  p += sprintf(p, "\n\nModes for SETCONFIGMODE:\n");
  for (cfg_mode = 0; cfg_mode < (sizeof(config_mode_list) / sizeof(char *)); cfg_mode++)
  {
    p += sprintf(p, "%s ", config_mode_list[cfg_mode]);
  }
  p += sprintf(p, "\n");

  if (send_ok(params, csp, event, buf, strlen(buf)))
  {
    LogWriteExt(LOG_ERROR, params, event, "Error handling %s: writing response to client\n", event);
    return -1;
  }

  return 0;
}

static int do_SnmpGet(char *oid, client_state_t *csp)
{
  doSnmpGet(oid, csp->cfd, NULL);
  return 0;
}

static int do_SnmpGetNext(char *oid, client_state_t *csp)
{
  doSnmpGetNext(oid, csp->cfd, NULL);
  return 0;
}

static int doUnarchiveConf(char *params, client_state_t *csp)
{
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = unarchive_conf(params, csp);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return ret;
}

static int doGetArchiveConf(char *params, client_state_t *csp)
{
  int ret;

  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = get_archive_conf(params, csp);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return ret;
}

static int doUpdateLicense(char *params, client_state_t *csp)
{
  int result;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  result = updateLicense(params, csp);

  if (!result)
  {
    if (csp->db_ctx == NULL)
    {
      csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
    }
    // ask to update asset
    sdForceAsset(csp, params);
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doUpdateFirmware(char *params, client_state_t *csp)
{
  int result;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  result = updateFirmware(params, csp);

  if (!result)
  {
    if (csp->db_ctx == NULL)
    {
      csp->db_ctx = sms_db_find_ctx(log_handle, DB_SMS);
    }
    // ask to update asset
    sdForceAsset(csp, params);
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doCheckUpdateLicense(char *params, client_state_t *csp)
{
  char *result = NULL;
  int error;
  char *event = EVT_CHECKUPDATELICENSE;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  error = getUpdateStatus(params, csp, "LICENSE", &result);
  if (!error)
  {
    if (send_ok(params, csp, event, result, strlen(result)))
    {
      freez(result);
      GLogERROR(log_handle, "Error handling %s: writing response to client");
      return -1;
    }
    freez(result);
  }
  else
  {
    freez(result);
    if (send_error(params, csp, event, 0, 0, error))
    {
      GLogERROR(log_handle, "Error handling %s: writing response to client");
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doCheckUpdateFirmware(char *params, client_state_t *csp)
{
  char *result = NULL;
  int error;
  char *event = EVT_CHECKUPDATEFIRMWARE;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  error = getUpdateStatus(params, csp, "FIRMWARE", &result);
  if (!error)
  {
    if (send_ok(params, csp, event, result, strlen(result)))
    {
      freez(result);
      GLogERROR(log_handle, "Error handling %s: writing response to client");
      return -1;
    }
    freez(result);
  }
  else
  {
    freez(result);
    if (send_error(params, csp, event, 0, 0, error))
    {
      GLogERROR(log_handle, "Error handling %s: writing response to client");
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doCheckBackup(char *params, client_state_t *csp)
{
  char *result = NULL;
  int error;
  char *event = EVT_CHECKBACKUP;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  error = getUpdateStatus(params, csp, "BACKUP", &result);
  if (!error)
  {
    if (send_ok(params, csp, event, result, strlen(result)))
    {
      freez(result);
      GLogERROR(log_handle, "Error handling %s: writing response to client");
      return -1;
    }
    freez(result);
  }
  else
  {
    freez(result);
    if (send_error(params, csp, event, 0, 0, error))
    {
      GLogERROR(log_handle, "Error handling %s: writing response to client");
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doCheckSendDataFiles(char *params, client_state_t *csp)
{
  char *result = NULL;
  int error;
  char *event = EVT_CHECKSENDDATAFILES;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  error = getUpdateStatus(params, csp, "SENDDATAFILES", &result);
  if (!error)
  {
    if (send_ok(params, csp, event, result, strlen(result)))
    {
      freez(result);
      GLogERROR(log_handle, "Error handling %s: writing response to client");
      return -1;
    }
    freez(result);
  }
  else
  {
    freez(result);
    if (send_error(params, csp, event, 0, 0, error))
    {
      GLogERROR(log_handle, "Error handling %s: writing response to client");
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doCheckRestore(char *params, client_state_t *csp)
{
  char *result = NULL;
  int error;
  char *event = EVT_CHECKRESTORE;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  error = getUpdateStatus(params, csp, "RESTORE", &result);
  if (!error)
  {
    if (send_ok(params, csp, event, result, strlen(result)))
    {
      freez(result);
      GLogERROR(log_handle, "Error handling %s: writing response to client");
      return -1;
    }
    freez(result);
  }
  else
  {
    freez(result);
    if (send_error(params, csp, event, 0, 0, error))
    {
      GLogERROR(log_handle, "Error handling %s: writing response to client");
      return -1;
    }
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

/** HASWAP verb
 */
static int doHaSwap(char *params, client_state_t *csp)
{
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = ha_swap(csp, params);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

/** CALLCOMMAND verb
 */
static int doCallCommand(char *params, client_state_t *csp)
{
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = call_command(params, csp);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doSendDataFiles(char *params, client_state_t *csp)
{
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = send_data_files(params, csp);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }
  return 0;
}

/** CHECKSERIAL verb
 */
static int doCheckSerial(char *params, client_state_t *csp)
{
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = check_serial(params, csp);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
  }

  return 0;
}

static int doImmediateSaveConf(char *params, client_state_t *csp)
{
  int ret;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  ret = save_all_conf(csp);
  if (!ret)
  {
    GLogINFO(log_handle, "ends OK");
  }
  else
  {
    GLogINFO(log_handle, "ends ERROR");
    return -1;
  }
  return 0;
}
