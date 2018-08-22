/** @file smsd.h
  This is the header file of the smsd.
  @ingroup  smsd

  */
#ifndef __SMSD_H_
#define __SMSD_H_
#include <pthread.h>
#include "smsconfig.h"
#include "error_code.h"

#include "bd.h"

/** definition of the maximum number of connections to the database
*/

#define DEFAULTMAXTHREAD 5

/** definition of the maximum number of simultaneous connections (i.e threads )*/
#define DEFAULTMAXSIMULTANEOUS 10

/** definition of the size of the prefix in the identification of a SD */
#define PREFIXSIZE  CLI_PREFIX_LEN

#define CVSNAME "$HeadURL$"

/** definition of the default port for listening */
#define DEFAULTPORT            28169
/** definition of the default log level */
#define DEFAULTLOG             15
/** definition of the default debug level */
#define DEFAULTDEBUG           15
/** definition of the default address of binding */
#define DEFAULTHADDR           "127.0.0.1"
/** definition of the default adresse of SMS server */
#define DEFAULTSMSADDR         "127.0.0.1"

#define SMSD_OK_STRING		"OK\r\n"
#define SMSD_OK_ALONE_STRING	"OK\r\n\r\n\r\n"
#define SMSD_ERROR_STRING	"ERROR\r\n"
#define SMSD_ERROR_ALONE_STRING	"ERROR\r\n\r\n\r\n"
#define SMSD_MSG_EOL        	"\r\n"
#define SMSD_MSG_END		"\r\n\r\n"

/** 60 secondes timeout on the select and socketreadline */
#define SELECTTIMEOUT  60

/** the log file pointer decriptor */
extern FILE *logfp;
/** The prog name added in the log */
extern char *prog;

/** this structure contains statistic information
    about the usage of the smsd daemon */

typedef struct sms_stat{
  time_t last10update[10];
  time_t last100update[100];
  unsigned long last10number;  /* up to 10  */
  unsigned long last100number; /* up to 100 */
  unsigned long current10;     /* the index of the current 10 to update  */
  unsigned long current100;    /* the index of the current 100 to update */

  unsigned long activeThreads; /* current Active threads             */
  unsigned long totalThreads;  /* total connection from the begining */

  unsigned long totalupdate;   /* the total Update connections   */
  unsigned long totalerrorupdate;   /* the total error Update connections   */

  unsigned long totalgetreport;   /* the total report connections   */
  unsigned long totalerrorgetreport;

  unsigned long totalgetconf;
  unsigned long totalerrorgetconf;

  unsigned long totalgetvpnreport;
  unsigned long totalerrorgetvpnreport;

  unsigned long totalgetrouterip;
  unsigned long totalerrorgetrouterip;

  unsigned long totalcheckupdate;
  unsigned long totalerrorcheckupdate;

  unsigned long totalipdown;
  unsigned long totalerroripdown;

  unsigned long totalpppipup;
  unsigned long totalerrorpppipup;

} sms_stat_t;


sms_stat_t currentStat;

/** \struct client_state smsd.h
    This structure the context of the client connection
   */
typedef struct client_state {
  /** the thread associated with the connection */
  /*thread_t thread;*/
  /** the client fd socket */
  int  cfd;
  /** the ip address string of the client */
  char *ip_addr_str;
  /** the ip address long of the client */
  long  ip_addr_long;
  /** pointer to the ctx SQL */
  database_context_t * db_ctx;
} client_state_t;




/** the array of all the connexions */
extern struct client_state clients[];

extern smsConf_t * currentConfig;

extern pthread_mutex_t *updateThreadMutex;

#define UPDATETHREADLOCK pthread_mutex_lock(updateThreadMutex);
#define UPDATETHREADUNLOCK pthread_mutex_unlock(updateThreadMutex);
#define INITUPDATETHREADLOCK  \
  if (!updateThreadMutex) \
    { \
      updateThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(updateThreadMutex,NULL); \
    }

extern int updatedServerNum;

extern pthread_mutex_t *last10ThreadMutex;
#define LAST10THREADLOCK pthread_mutex_lock(last10ThreadMutex);
#define LAST10THREADUNLOCK pthread_mutex_unlock(last10ThreadMutex);
#define INITLAST10THREADLOCK  \
  if (!last10ThreadMutex) \
    { \
      last10ThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(last10ThreadMutex,NULL); \
    }

extern pthread_mutex_t *last100ThreadMutex;
#define LAST100THREADLOCK pthread_mutex_lock(last100ThreadMutex);
#define LAST100THREADUNLOCK pthread_mutex_unlock(last100ThreadMutex);
#define INITLAST100THREADLOCK  \
  if (!last100ThreadMutex) \
    { \
      last100ThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(last100ThreadMutex,NULL); \
    }

extern pthread_mutex_t *activeThreadMutex;
#define ACTIVETHREADLOCK pthread_mutex_lock(activeThreadMutex);
#define ACTIVETHREADUNLOCK pthread_mutex_unlock(activeThreadMutex);
#define INITACTIVETHREADLOCK  \
  if (!activeThreadMutex) \
    { \
      activeThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(activeThreadMutex,NULL); \
    }

extern pthread_mutex_t *statupdateThreadMutex;
#define STATUPDATETHREADLOCK pthread_mutex_lock(statupdateThreadMutex);
#define STATUPDATETHREADUNLOCK pthread_mutex_unlock(statupdateThreadMutex);
#define INITSTATUPDATETHREADLOCK  \
  if (!statupdateThreadMutex) \
    { \
      statupdateThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(statupdateThreadMutex,NULL); \
    }

extern pthread_mutex_t *statgetreportThreadMutex;
#define STATGETREPORTTHREADLOCK pthread_mutex_lock(statgetreportThreadMutex);
#define STATGETREPORTTHREADUNLOCK pthread_mutex_unlock(statgetreportThreadMutex);
#define INITSTATGETREPORTTHREADLOCK  \
  if (!statgetreportThreadMutex) \
    { \
      statgetreportThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(statgetreportThreadMutex,NULL); \
    }

extern pthread_mutex_t *statgetvpnstatusThreadMutex;
#define STATGETVPNSTATUSTHREADLOCK pthread_mutex_lock(statgetvpnstatusThreadMutex);
#define STATGETVPNSTATUSTHREADUNLOCK pthread_mutex_unlock(statgetvpnstatusThreadMutex);
#define INITSTATGETVPNSTATUSTHREADLOCK  \
  if (!statgetvpnstatusThreadMutex) \
    { \
      statgetvpnstatusThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(statgetvpnstatusThreadMutex,NULL); \
    }


extern pthread_mutex_t *statgetconfThreadMutex;
#define STATGETCONFTHREADLOCK pthread_mutex_lock(statgetconfThreadMutex);
#define STATGETCONFTHREADUNLOCK pthread_mutex_unlock(statgetconfThreadMutex);
#define INITSTATGETCONFTHREADLOCK  \
  if (!statgetconfThreadMutex) \
    { \
      statgetconfThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(statgetconfThreadMutex,NULL); \
    }

extern pthread_mutex_t *statgetvpnreportThreadMutex;
#define STATGETVPNREPORTTHREADLOCK pthread_mutex_lock(statgetvpnreportThreadMutex);
#define STATGETVPNREPORTTHREADUNLOCK pthread_mutex_unlock(statgetvpnreportThreadMutex);
#define INITSTATGETVPNREPORTTHREADLOCK  \
  if (!statgetvpnreportThreadMutex) \
    { \
      statgetvpnreportThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(statgetvpnreportThreadMutex,NULL); \
    }

extern pthread_mutex_t *statgetrouteripThreadMutex;
#define STATGETROUTERIPTHREADLOCK pthread_mutex_lock(statgetrouteripThreadMutex);
#define STATGETROUTERIPTHREADUNLOCK pthread_mutex_unlock(statgetrouteripThreadMutex);
#define INITSTATGETROUTERIPTHREADLOCK  \
  if (!statgetrouteripThreadMutex) \
    { \
      statgetrouteripThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(statgetrouteripThreadMutex,NULL); \
    }

extern pthread_mutex_t *statgetclientcertThreadMutex;
#define STATGETCLIENTCERTTHREADLOCK pthread_mutex_lock(statgetclientcertThreadMutex);
#define STATGETCLIENTCERTTHREADUNLOCK pthread_mutex_unlock(statgetclientcertThreadMutex);
#define INITSTATGETCLIENTCERTTHREADLOCK  \
  if (!statgetclientcertThreadMutex) \
    { \
      statgetclientcertThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(statgetclientcertThreadMutex,NULL); \
    }

extern pthread_mutex_t *statgetclientpolicyThreadMutex;
#define STATGETCLIENTPOLICYTHREADLOCK pthread_mutex_lock(statgetclientpolicyThreadMutex);
#define STATGETCLIENTPOLICYTHREADUNLOCK pthread_mutex_unlock(statgetclientpolicyThreadMutex);
#define INITSTATGETCLIENTPOLICYTHREADLOCK  \
  if (!statgetclientpolicyThreadMutex) \
    { \
      statgetclientpolicyThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(statgetclientpolicyThreadMutex,NULL); \
    }

extern pthread_mutex_t *statcheckupdateThreadMutex;
#define STATCHECKUPDATETHREADLOCK pthread_mutex_lock(statcheckupdateThreadMutex);
#define STATCHECKUPDATETHREADUNLOCK pthread_mutex_unlock(statcheckupdateThreadMutex);
#define INITSTATCHECKUPDATETHREADLOCK  \
  if (!statcheckupdateThreadMutex) \
    { \
      statcheckupdateThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(statcheckupdateThreadMutex,NULL); \
    }

extern pthread_mutex_t *statgetcertpasswordThreadMutex;
#define STATGETCERTPASSWORDTHREADLOCK pthread_mutex_lock(statgetcertpasswordThreadMutex);
#define STATGETCERTPASSWORDTHREADUNLOCK pthread_mutex_unlock(statgetcertpasswordThreadMutex);
#define INITSTATGETCERTPASSWORDTHREADLOCK  \
  if (!statgetcertpasswordThreadMutex) \
    { \
      statgetcertpasswordThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(statgetcertpasswordThreadMutex,NULL); \
    }

extern pthread_mutex_t *statupdatecertificateThreadMutex;
#define STATUPDATECERTIFICATETHREADLOCK pthread_mutex_lock(statupdatecertificateThreadMutex);
#define STATUPDATECERTIFICATETHREADUNLOCK pthread_mutex_unlock(statupdatecertificateThreadMutex);
#define INITSTATUPDATECERTIFICATETHREADLOCK  \
  if (!statupdatecertificateThreadMutex) \
    { \
      statupdatecertificateThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(statupdatecertificateThreadMutex,NULL); \
    }

extern pthread_mutex_t *statipdownThreadMutex;
#define STATIPDOWNTHREADLOCK pthread_mutex_lock(statipdownThreadMutex);
#define STATIPDOWNTHREADUNLOCK pthread_mutex_unlock(statipdownThreadMutex);
#define INITSTATIPDOWNTHREADLOCK  \
  if (!statipdownThreadMutex) \
    { \
      statipdownThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(statipdownThreadMutex,NULL); \
    }

extern pthread_mutex_t *statpppipupThreadMutex;
#define STATPPPIPUPTHREADLOCK pthread_mutex_lock(statpppipupThreadMutex);
#define STATPPPIPUPTHREADUNLOCK pthread_mutex_unlock(statpppipupThreadMutex);
#define INITSTATPPPIPUPTHREADLOCK  \
  if (!statpppipupThreadMutex) \
    { \
      statpppipupThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(statpppipupThreadMutex,NULL); \
    }

extern pthread_mutex_t *statdynipupThreadMutex;
#define STATDYNIPUPTHREADLOCK pthread_mutex_lock(statdynipupThreadMutex);
#define STATDYNIPUPTHREADUNLOCK pthread_mutex_unlock(statdynipupThreadMutex);
#define INITSTATDYNIPUPTHREADLOCK  \
  if (!statdynipupThreadMutex) \
    { \
      statdynipupThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(statdynipupThreadMutex,NULL); \
    }

extern pthread_mutex_t *statsendserialThreadMutex;
#define STATSENDSERIALTHREADLOCK pthread_mutex_lock(statsendserialThreadMutex);
#define STATSENDSERIALTHREADUNLOCK pthread_mutex_unlock(statsendserialThreadMutex);
#define INITSTATSENDSERIALTHREADLOCK  \
  if (!statsendserialThreadMutex) \
    { \
      statsendserialThreadMutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t)); \
      pthread_mutex_init(statsendserialThreadMutex,NULL); \
    }


int new_thread(void *(*func)(void *), void *data);

extern void *sched_handle;

#endif /* __SMSD_H_*/


