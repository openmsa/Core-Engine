/** @file log.h
  This is the header file of the log module. The log and debug levels defined
  in this file are cumulatives.
  @ingroup log
  @author Christophe Guionneau
  @version  0.1
*/

#ifndef __LOG_H_
#define __LOG_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/** Information log level
 */
#undef LOG_INFO
#define LOG_INFO        0x0001

/** Warning log level
 */
#undef LOG_WARNING
#define LOG_WARNING     0x0002

/** Error log level
 */
#undef LOG_ERROR
#define LOG_ERROR       0x0004

/** Security log level
 */
#define LOG_SECURITY    0x0008

/** Alert log level
 */
#undef LOG_ALERT
#define LOG_ALERT       0x0010

/** Log level used to have all log levels
 */
#define LOG_ALL         0X00FF


/** Information debug level
 */
#define DBG_INFO        0X0001

/** Warning debug level
 */
#define DBG_WARNING     0X0002

/** Error debug level
 */
#define DBG_ERROR       0x0004

/** Debug level used to have all debug levels
 */
#define DBG_ALL         0X00FF


/** Init the log system
    @param      input: @e file_name path of the log file to use.
    @param      input: @e keep_open must be set to @b TRUE or @b FALSE:
                       used to define if the log file must be closed after
		       each write or not.
    @param      input: @e level current log level.
    @param      input: @e domain  current domain name of the application
                       issuing logs
		       (example: "SMS_Slave" or "HTTPS_proxy")
    @return     0 if the function is successful, -1 otherwise.
*/
extern int LogInit(const char *file_name, int keep_open, int level, const char *domain);

/** Set the current log level of the log system
    @param      input: @e level current log level.
    @return     nothing.
*/
extern void LogSetLevel(int level);


/** Write a log according to the configuration of the log system.
    The message will be written only if the @b level is matching (|) the
    log level.
    @param      input: @e level level of the message to write.
    @param      input: @e format string to write (see printf format)
    @return     0 if the function is successful, -1 otherwise.
*/
extern int LogWrite(int level, const char *format, ...) ;




/** Write a log according to the configuration of the log system.
    The message will be written only if the @b level is matching (|) the
    log level.
    @param      input: @e level level of the message to write.
    @param      input: @e sdid the identification of the router if known
    @param      input: @e event the event to log
    @param      input: @e format string to write (see printf format)
    @return     0 if the function is successful, -1 otherwise.
*/
extern int LogWriteExt(int level,char * sdid, char * event, char *format, ...) ;
extern int LogWriteExt2(int level,char * sd_cli_prefix,long sd_seqnum,  char * event, char *format, ...) ;

/** Dump a buffer in hexa and ASCII form. The buffer is dumped only if the
    @b level is matching the log level.
    @param      input: @e level level of the message to dump
    @param      input: @e buf buffer to trace
    @param      input: @e len length of the buffer
    @param      input: @e msg_head header of the message
    @return     0 if the function is successful, -1 otherwise.
*/
extern int LogDump(int level, char *buf, int len, char *msg_head);


/* general verb events */
#define EVT_IPUP                  "IPUP"
#define EVT_PPPIPUP               "PPPIPUP"
#define EVT_SDIPUP                "SDIPUP"
#define EVT_CISCO800IPUP          "CISCO800IPUP"
#define EVT_CISCOASAIPUP          "CISCOASAIPUP"
#define EVT_SCREENOSIPUP          "SCREENOSIPUP"
#define EVT_FORTINETIPUP          "FORTINETIPUP"
#define EVT_DYNDNSIPUP            "DYNDNSIPUP"
#define EVT_IPDOWN                "IPDOWN"
#define EVT_GETCONF               "GETCONF"
#define EVT_GETCLIENTCONF         "GETCLIENTCONF"
#define EVT_GETLASTCONF           "GETLASTCONF"
#define EVT_GETDELTA              "GETDELTA"
#define EVT_GETCLEAN              "GETCLEAN"
#define EVT_GETDELTANETCONF       "GETDELTANETCONF"
#define EVT_GETSDCONF             "GETSDCONF"
#define EVT_DOBACKUPCONF          "DOBACKUPCONF"
#define EVT_UPDATECONFN           "UPDATECONFN"
#define EVT_AUPDATECONFN          "AUPDATECONFN"
#define EVT_UPDATECONF            "UPDATECONF"
#define EVT_RESTORECONF           "RESTORECONF"
#define EVT_BACKUPCONF            "BACKUPCONF"
#define EVT_AUPDATECONF           "AUPDATECONF"
#define EVT_CLEANCONF             "CLEANCONF"
#define EVT_SETUPDATECLIENTCONF   "SETUPDATECLIENTCONF"
#define EVT_UPDATECLIENTCONF      "UPDATECLIENTCONF"
#define EVT_AUPDATECLIENTCONF     "AUPDATECLIENTCONF"
#define EVT_CLIENTUPDATECERT      "CLIENTUPDATECERT"
#define EVT_UPDATENETCONF         "UPDATENETCONF"
#define EVT_AUPDATENETCONF        "AUPDATENETCONF"
#define EVT_GETREPORT             "GETREPORT"
#define EVT_GETVPNREPORT          "GETVPNREPORT"
#define EVT_GETVPN                "GETVPN"
#define EVT_SENDKEY               "SENDKEY"
#define EVT_SENDSERIAL            "SENDSERIAL"
#define EVT_SENDALTSERIAL         "SENDALTSERIAL"
#define EVT_GETROUTERIP           "GETROUTERIP"
#define EVT_GETCLIENTPOLICY       "GETCLIENTPOLICY"
#define EVT_GETCERTPASSWORD       "GETCERTPASSWORD"
#define EVT_GETCERTIFICATES       "GETCERTIFICATES"
#define EVT_GETCERTSANDKEY        "GETCERTSANDKEY"
#define EVT_UPDATECERTIFICATE     "UPDATECERTIFICATE"
#define EVT_AUPDATECERTIFICATE    "AUPDATECERTIFICATE"
#define EVT_SDVPNSTATUS           "SDVPNSTATUS"
#define EVT_SDLISTVPNSTATUS       "SDLISTVPNSTATUS"
#define EVT_CHECKRESTORE          "CHECKRESTORE"
#define EVT_CHECKUPDATE           "CHECKUPDATE"
#define EVT_CHECKUPDATECERT       "CHECKUPDATECERT"
#define EVT_CHECKUPDATENET        "CHECKUPDATENET"
#define EVT_CHECKALLUPDATE        "CHECKALLUPDATE"
#define EVT_CHECKSENDDATAFILES    "CHECKSENDDATAFILES"
#define EVT_CHECKBACKUP           "CHECKBACKUP"
#define EVT_GETSTATE              "GETSTATE"
#define EVT_GETSTATUS             "GETSTATUS"
#define EVT_CHANGECREDENTIALS     "CHANGECREDENTIALS"
#define EVT_IMMEDIATESAVECONF     "IMMEDIATESAVECONF"


#define EVT_POLL                  "POLL"
#define EVT_BUD                   "BUD"
#define EVT_UPDATESOFT            "UPDATESOFT"
#define EVT_UPDATELICENSE         "UPDATELICENSE"
#define EVT_UPDATEFIRMWARE        "UPDATEFIRMWARE"
#define EVT_SMSEXEC               "SMSEXEC"
#define EVT_SMSEXECN              "SMSEXECN"
#define EVT_INITCONNECTION        "INITCONNECTION"
#define EVT_APPLYINITIALCONF      "APPLYINITIALCONF"
#define EVT_SOFTTOUPDATE          "SOFTTOUPDATE"
#define EVT_CHECKUPDATESOFT       "CHECKUPDATESOFT"
#define EVT_CHECKUPDATELICENSE    "CHECKUPDATELICENSE"
#define EVT_CHECKUPDATEFIRMWARE   "CHECKUPDATEFIRMWARE"
#define EVT_GETSOFTUPDATEREPORT   "GETSOFTUPDATEREPORT"
#define EVT_GETSOFT               "GETSOFT"
#define EVT_CHECKREBOOT           "CHECKREBOOT"
#define EVT_SETUPDATESSHKEY       "SETUPDATESSHKEY"
#define EVT_GETASSET              "GETASSET"
#define EVT_FORCEASSET            "FORCEASSET"
#define EVT_SETCONFIGMODE         "SETCONFIGMODE"
#define EVT_RELOADSIGDEF          "RELOADSIGDEF"
#define EVT_PROVISIONING          "PROVISIONING"
#define EVT_CHECKPROVISIONING     "CHECKPROVISIONING"
#define EVT_DOPING			      "DOPING"
#define EVT_STAGING               "STAGING"
#define EVT_SNMPTEST              "SNMPTEST"
#define EVT_GETRUNNINGCONF        "GETRUNNINGCONF"
#define EVT_GETCONFIG             "GETCONFIG"
#define EVT_GETCLEANCONFIG        "GETCLEANCONFIG"
#define EVT_UPDATECONFIG          "UPDATECONFIG"
#define EVT_CHECKUPDATECONFIG     "CHECKUPDATECONFIG"
#define EVT_GETACTIVITYREPORT     "GETACTIVITYREPORT"
#define EVT_GETBOTNETREPORT       "GETBOTNETREPORT"
#define EVT_RESOLVETEMPLATE       "RESOLVETEMPLATE"

/* SSH events */
#define EVT_SSHCONNECTION         "SSH_CONNECTION"
#define EVT_SCPCONNECTION         "SCP_CONNECTION"

/* generic module events */
#define EVT_ISALIVE               "FCT_ISALIVE"
#define EVT_LISTVERB              "FCT_LISTVERB"
#define EVT_STAT                  "FCT_STAT"
#define EVT_UPDATEIP              "FCT_UPDATEIP"
#define EVT_GETSDINFO             "FCT_GETSDINFO"
#define EVT_BUILDVPN              "FCT_BUILDVPN"
#define EVT_GETPRECONF            "FCT_GETPRECONF"
#define EVT_SENDUPDATE            "FCT_SENDUPDATE"
#define EVT_SENDDNSUPDATE         "FCT_SENDDNSUPDATE"
#define EVT_UNLOCKUPDATE          "FCT_UNLOCKUPDATE"
#define EVT_LOCKUPDATE            "FCT_LOCKUPDATE"
#define EVT_LOCKPROVISIONING      "FCT_LOCKPROVISIONING"
#define EVT_UNLOCKPROVISIONING    "FCT_UNLOCKPROVISIONING"
#define EVT_DNSUPDATE             "FCT_DNSUPDATE"
#define EVT_CHECKPUBKEY           "FCT_CHECKPUBKEY"
#define EVT_SMSINIT               "FCT_INIT"
#define EVT_SMSINFO               "FCT_INFO"
#define EVT_ERROR                 "FCT_ERROR"
#define EVT_WARNING               "FCT_WARNING"
#define EVT_SETLOGLEVEL           "FCT_SETLOGLEVEL"
#define EVT_REBOOTLOOP            "FCT_REBOOTLOOP"
#define EVT_ASSET                 "FCT_ASSET"


#define EVT_UNKNOWN               "UNKNOWN"


extern void *sched_handle;


/** Init the debug system
    @param      input: @e file_name path of the debug file to use.
    @param      input: @e keep_open must be set to @b TRUE or @b FALSE:
                       used to define if the debug file must be closed after
		       each write or not.
    @param      input: @e level current debug level.
    @param      input: @e domain  current domain name of the application
                       issuing debugs
		       (example: "SMS_Slave" or "HTTPS_proxy")
    @return     0 if the function is successful, -1 otherwise.
*/
extern int DbgInit(const char *file_name, int keep_open, int level, const char *domain) ;



/** Check the current debug level against the debug system
    @param      input: @e level current debug level.
    @return     1 if the message can be written, 0 otherwise.
*/
extern int DbgCanWrite(int level);

/** Set the current debug level of the debug system
    @param      input: @e level current debug level.
    @return     nothing.
*/
extern void DbgSetLevel(int level);


/** Write a debug message according to the configuration of the debug system.
    The message will be written only if the @b level is matching (|) the
    debug level.
    @param      input: @e level level of the message to write.
    @param      input: @e format string to write (see printf format)
    @return     0 if the function is successful, -1 otherwise.
*/
extern int DbgWrite(int level, char *format, ...) ;


/** Dump a buffer in hexa and ASCII form. The buffer is dumped only if the
    @b level is matching the debug level.
    @param      input: @e level level of the message to dump
    @param      input: @e buf buffer to trace
    @param      input: @e len length of the buffer
    @param      input: @e msg_head header of the message
    @return     0 if the function is successful, -1 otherwise.
*/
extern int DbgDump(int level, char *buf, int len, char *msg_head);


/* Defines disponibles pour tous les demons, pas joli mais tellement pratique */

/* Types de mise a jour (action pour updated) */
#define CONF_VPN             0x00000001
#define CONF_VPN_CLEAN       0x00000002
#define CONF_QOS             0x00000004
#define CONF_IPS             0x00000008
#define IPS_SIG              0x00000010
#define CONF_AV              0x00000020
#define CONF_URL             0x00000040
#define CONF_AS              0x00000080
#define CONF_ALL             (CONF_VPN | CONF_QOS | CONF_IPS | IPS_SIG | CONF_AV | CONF_URL | CONF_AS)

#define CONF_INIT            0x00000100  /* used for initial config */

#ifdef __cplusplus
}
#endif

#endif /*__LOG_H_ */
