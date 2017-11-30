/*! \file smsd.c
 *  \brief core file of the sms daemon
 */

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/prctl.h>

#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <pthread.h>
#include <dirent.h>

#include <strings.h>

#include "log.h"
#include "net.h"
#include "verb.h"
#include "router.h"
#include "pattern.h"
#include "error_code.h"
#include "gpul.h"
#include "sms_php.h"
#include "update.h"
#include "smssnmp.h"
#include "sms_router.h"
#include "smsd.h"

pthread_mutex_t *updateThreadMutex = NULL;
pthread_mutex_t *last10ThreadMutex = NULL;
pthread_mutex_t *last100ThreadMutex = NULL;
pthread_mutex_t *activeThreadMutex = NULL;
pthread_mutex_t *statupdateThreadMutex = NULL;
pthread_mutex_t *statgetreportThreadMutex = NULL;
pthread_mutex_t *statgetvpnstatusThreadMutex = NULL;
pthread_mutex_t *statgetconfThreadMutex = NULL;
pthread_mutex_t *statgetvpnreportThreadMutex = NULL;
pthread_mutex_t *statgetrouteripThreadMutex = NULL;
pthread_mutex_t *statgetclientcertThreadMutex = NULL;
pthread_mutex_t *statgetclientpolicyThreadMutex = NULL;
pthread_mutex_t *statcheckupdateThreadMutex = NULL;
pthread_mutex_t *statgetcertpasswordThreadMutex = NULL;
pthread_mutex_t *statupdatecertificateThreadMutex = NULL;
pthread_mutex_t *statipdownThreadMutex = NULL;
pthread_mutex_t *statpppipupThreadMutex = NULL;
pthread_mutex_t *statcisco800pppipupThreadMutex = NULL;
pthread_mutex_t *statciscoasapppipupThreadMutex = NULL;
pthread_mutex_t *statscreenospppipupThreadMutex = NULL;
pthread_mutex_t *statfortinetpppipupThreadMutex = NULL;
pthread_mutex_t *statlnpppipupThreadMutex = NULL;
pthread_mutex_t *statdynipupThreadMutex = NULL;
pthread_mutex_t *statsendserialThreadMutex = NULL;


char * prog = "SMSD"; /*! the SMSD program name */

/*! the public Key of the SMS host to access the SMSD thru IPSEC (not used
 because the smsd doesn't ipsec directly , it relies on external devices) */
char * publicKey = NULL;

char bufstartTime[64];
time_t startTime;

/* Stat counters */
static snmp_counters_t *Stat_VmSize;
static snmp_counters_t *Stat_VmData;
static snmp_counters_t *Stat_RunTasks;
static snmp_counters_t *Stat_CurTasks;
static snmp_counters_t *Stat_OpFileDesc;

/** the complete number of connections handled */
int totalConnections = 0;
/* the current id of the updated server to use */
int updatedServerNum = 0;

/* the current id of the polld server to use */
int polldServerNum = 0;

/** temporary the log file */
FILE * logfp = NULL;

/** the current configuration structure */
smsConf_t * currentConfig = NULL;
/** the current configuration file */
char *configfile = NULL;
/** a global variable asking for a gracefull shutdown */
volatile int shutdownRequired = FALSE;
int reloadRequired = FALSE;

static void InitDBSqlCtx(void *log_handle);
static int serve(void *vcsp);
static void smsSignal(int sig, siginfo_t* info, void *p);
static void reloadConfig(void);
static int resetBdFlag(void);

SQLHENV ODBCenv;

static void init_snmp_counters(void *log_handle)
{
  snmp_counters_t *info;
  snmp_counters_t *empty;

  sms_snmp_init(log_handle);

  empty = (snmp_counters_t *) calloc(1, sizeof(snmp_counters_t));
  sprintf(empty->oid, "%s.%d", OID_SEC_ENGINE, currentConfig->port);
  empty->type = SNMP_NONE;
  GPUL_TAILQ_INSERT_TAIL(&snmp_counters_list, empty, link);
  gpul_hash_add_str(log_handle, snmp_counters_hash_table, empty->oid, empty);

  empty = (snmp_counters_t *) calloc(1, sizeof(snmp_counters_t));
  sprintf(empty->oid, "%s.%d.1", OID_SEC_ENGINE, currentConfig->port);
  empty->type = SNMP_NONE;
  GPUL_TAILQ_INSERT_TAIL(&snmp_counters_list, empty, link);
  gpul_hash_add_str(log_handle, snmp_counters_hash_table, empty->oid, empty);

  info = (snmp_counters_t *) malloc(sizeof(snmp_counters_t));
  sprintf(info->oid, "%s.%d.1.1", OID_SEC_ENGINE, currentConfig->port);
  info->type = SNMP_STRING;
  info->valstr = prog;
  GPUL_TAILQ_INSERT_TAIL(&snmp_counters_list, info, link);
  gpul_hash_add_str(log_handle, snmp_counters_hash_table, info->oid, info);

  empty = (snmp_counters_t *) calloc(1, sizeof(snmp_counters_t));
  sprintf(empty->oid, "%s.%d.2", OID_SEC_ENGINE, currentConfig->port);
  empty->type = SNMP_NONE;
  GPUL_TAILQ_INSERT_TAIL(&snmp_counters_list, empty, link);
  gpul_hash_add_str(log_handle, snmp_counters_hash_table, empty->oid, empty);

  empty = (snmp_counters_t *) calloc(1, sizeof(snmp_counters_t));
  sprintf(empty->oid, "%s.%d.3", OID_SEC_ENGINE, currentConfig->port);
  empty->type = SNMP_NONE;
  GPUL_TAILQ_INSERT_TAIL(&snmp_counters_list, empty, link);
  gpul_hash_add_str(log_handle, snmp_counters_hash_table, empty->oid, empty);

  Stat_VmSize = (snmp_counters_t *) malloc(sizeof(snmp_counters_t));
  sprintf(Stat_VmSize->oid, "%s.%d.3.1", OID_SEC_ENGINE, currentConfig->port);
  Stat_VmSize->type = SNMP_GAUGE;
  Stat_VmSize->value = 0;
  GPUL_TAILQ_INSERT_TAIL(&snmp_counters_list, Stat_VmSize, link);
  gpul_hash_add_str(log_handle, snmp_counters_hash_table, Stat_VmSize->oid, Stat_VmSize);

  Stat_VmData = (snmp_counters_t *) malloc(sizeof(snmp_counters_t));
  sprintf(Stat_VmData->oid, "%s.%d.3.2", OID_SEC_ENGINE, currentConfig->port);
  Stat_VmData->type = SNMP_GAUGE;
  Stat_VmData->value = 0;
  GPUL_TAILQ_INSERT_TAIL(&snmp_counters_list, Stat_VmData, link);
  gpul_hash_add_str(log_handle, snmp_counters_hash_table, Stat_VmData->oid, Stat_VmData);

  Stat_RunTasks = (snmp_counters_t *) malloc(sizeof(snmp_counters_t));
  sprintf(Stat_RunTasks->oid, "%s.%d.3.3", OID_SEC_ENGINE, currentConfig->port);
  Stat_RunTasks->type = SNMP_GAUGE;
  Stat_RunTasks->value = 0;
  GPUL_TAILQ_INSERT_TAIL(&snmp_counters_list, Stat_RunTasks, link);
  gpul_hash_add_str(log_handle, snmp_counters_hash_table, Stat_RunTasks->oid, Stat_RunTasks);

  Stat_CurTasks = (snmp_counters_t *) malloc(sizeof(snmp_counters_t));
  sprintf(Stat_CurTasks->oid, "%s.%d.3.4", OID_SEC_ENGINE, currentConfig->port);
  Stat_CurTasks->type = SNMP_GAUGE;
  Stat_CurTasks->value = 0;
  GPUL_TAILQ_INSERT_TAIL(&snmp_counters_list, Stat_CurTasks, link);
  gpul_hash_add_str(log_handle, snmp_counters_hash_table, Stat_CurTasks->oid, Stat_CurTasks);

  Stat_OpFileDesc = (snmp_counters_t *) malloc(sizeof(snmp_counters_t));
  sprintf(Stat_OpFileDesc->oid, "%s.%d.3.5", OID_SEC_ENGINE, currentConfig->port);
  Stat_OpFileDesc->type = SNMP_GAUGE;
  Stat_OpFileDesc->value = 0;
  GPUL_TAILQ_INSERT_TAIL(&snmp_counters_list, Stat_OpFileDesc, link);
  gpul_hash_add_str(log_handle, snmp_counters_hash_table, Stat_OpFileDesc->oid, Stat_OpFileDesc);
}

static int updateStats(void *dummy)
{
  FILE *fproc;
  char buf[BUFSIZ];
  gpul_sched_stats_t sched_stats;

  gpul_sched_upd_stats(sched_handle, &sched_stats);
  Stat_RunTasks->value = sched_stats.avg_running_tasks;
  Stat_CurTasks->value = sched_stats.avg_tasks;

  // Compute opened file descriptors
  Stat_OpFileDesc->value = 0;
  sprintf(buf, "/proc/%d/fd", getpid());
  DIR *d = opendir(buf);
  if(d != NULL)
  {
    struct dirent de;
    struct dirent *de_ptr;

    readdir_r( d, &de, &de_ptr );
    while( de_ptr != NULL )
    {
      if( ( strncmp( de.d_name, ".", PATH_MAX ) == 0 ) ||
          ( strncmp( de.d_name, "..", PATH_MAX ) == 0 ) )
      {
        readdir_r( d, &de, &de_ptr );
        continue;
      }
      Stat_OpFileDesc->value++;
      readdir_r( d, &de, &de_ptr );
    }
    closedir(d);
  }

  sprintf(buf, "/proc/%d/status", getpid());
  fproc = fopen(buf, "r");
  if (fproc)
    {
      while (fgets(buf, sizeof(buf), fproc))
        {
          if (strstr(buf, "VmSize:") == buf)
            {
              Stat_VmSize->value = strtol(buf + strlen("VmSize:"), NULL, 0);
            }
          else if (strstr(buf, "VmData:") == buf)
            {
              Stat_VmData->value = strtol(buf + strlen("VmData:"), NULL, 0);
            }
        }
      fclose(fproc);
    }
  else
    {
      LogWrite(LOG_ERROR, "Cannot open file %s (%s)\n", buf, strerror(errno));
    }

  return 0;
}

static int resetBdFlag(void)
{
  void *log_handle;
  database_context_t *db_sms_ctx;
  int ret;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);
  db_sms_ctx = sms_db_find_ctx(log_handle, DB_SMS);
  DbgWrite(DBG_INFO,"resetBdFlag sb_sms_ctx->schema_owner = %s \n", db_sms_ctx->sql_ctx->schema_owner );

  ret = HA_SMSSQL_UnLockAll(&(db_sms_ctx->sql_ctx), get_error_message(ERR_SD_INTRUPDT));
  if (ret == ERR_DB_FAILED)
    {
      goto end;
    }

  sms_db_release_ctx(db_sms_ctx);

  return 0;

  end:

  sms_db_release_ctx(db_sms_ctx);
  return -1;
}

static void InitDBSqlCtx(void *log_handle)
{
  int ret;

  ret = sms_db_alloc_ctx(log_handle, DB_SMS, currentConfig->SMSnumContext);
  if (ret)
  {
    GLogERROR(log_handle,"Cannot connect SMS database, exiting...");
    exit(-1);
  }

  ret = sms_db_alloc_ctx(log_handle, DB_CUST, currentConfig->CUSTnumContext);
  if (ret)
  {
    GLogERROR(log_handle,"Cannot connect CUST database, exiting...");
    exit(-1);
  }
}

/** serve : main function of the threads
 This function get the context of the client, read the first line,
 and calls CheckVerb() to start the command
 @return nothing, data are released and the thread is destroyed
  */
static int serve(void *vcsp)
{
  struct client_state *csp = vcsp;
  char buf[BUFSIZ];
  int n;
  int ret;

    n = socketReadLine(csp->cfd, buf, sizeof(buf), SELECTTIMEOUT);
    if (n > 0)
    {
      CheckVerb(buf, csp);
    }
    else
    {
      if (errno)
      {
        LogWriteExt(LOG_ERROR, "N/A", EVT_SMSINFO, " main socketReadLine FAILED: %s from %s\n", strerror(errno), csp->ip_addr_str);
      }
    }

    freez(csp->ip_addr_str);

    /* cfd vaut -1 pour les verbes asynchrones  */
    if (csp->cfd != -1)
    {
      ret = socketClose(csp->cfd);
      if (ret < 0)
      {
        LogWriteExt(LOG_ERROR, "N/A", EVT_SMSINFO, " main socketClose FAILED: %s\n", strerror(errno));
      }
    }
  free(csp);

  return 0;
}

/** CallBack function used by "signal" handler : Used both for Reload (SIGHUP) or shutdown (SIINT) command .
 @param      sig Signal Value: If set to 0 do not exit.
 @return     none
 @warning    Threads are stopped during this call.
 */
static void smsSignal(int sig, siginfo_t* info, void *p)
{
  int socketFd;
  int ret;

  //GLogDEBUG(currentConfig->log_handle, 15, "received signal %d", sig);
  switch (sig)
  {
    case SIGHUP:
      reloadRequired = TRUE;
      //LogWrite(LOG_INFO, " reload received ... \n");
      break;
    case SIGINT:
      shutdownRequired = TRUE;
      //LogWrite(LOG_INFO, " shutdown received ... \n");
      break;
    case SIGCHLD:
      while (waitpid(-1, NULL, WNOHANG) > 0) { }
      return;
    default:
      return;
  }

  /* Reveiller le demon en envoyant un verbe */
  /* afin de sortir de socketAccept et traiter le signal re�u */
  socketFd = socketConnect("127.0.0.1", currentConfig->port);
  if (socketFd == -1)
    {
      //LogWriteExt(LOG_ERROR, "N/A", EVT_SMSINFO, " error when trying to open a socket to handle signal %d\n", sig);
      return;
    }
  //LogWriteExt(DBG_INFO, "N/A", EVT_SMSINFO, " received signal %d and sending WAKE_UP_SMS\n", sig);
  ret = socketWrite(socketFd, "WAKE_UP_SMS", sizeof("WAKE_UP_SMS") - 1);
  if (ret != sizeof("WAKE_UP_SMS") - 1)
    {
      //LogWriteExt(LOG_ERROR, "N/A", EVT_SMSINFO, " error when trying to send data in socket to handle signal %d\n", sig);
    }
  socketClose(socketFd);
}

static void reloadConfig()
{
  smsConf_t * config;

  reloadRequired = FALSE;

  config = createConfig(configfile);
  if (!config)
    {
      LogWrite(LOG_ERROR, " can't open configuration file '%s': : cannot continue, exiting\n", configfile);
      exit(-1);
    }

  deleteConfig(currentConfig);

  currentConfig = config;

  if (LogInit(currentConfig->logfile, FALSE, currentConfig->loglevel, prog) == -1)
    fprintf(stderr, "%s: can't open logfile '%s': ", prog, currentConfig->logfile);

  LogWriteExt(LOG_INFO, "N/A", EVT_SMSINIT, " SMSD daemon reloaded with loglevel=%d version :%s\n", currentConfig->loglevel, CVSNAME);

  /*fprintf(stdout,"SMSD daemon reloaded with loglevel=%d",currentConfig->loglevel);*/

  if (DbgInit(currentConfig->logfile, FALSE, currentConfig->debug, prog) == -1)
    fprintf(logfp, "%s: can't open debug logfile '%s': ", prog, currentConfig->logfile);

  LogWriteExt(LOG_INFO, "N/A", EVT_SMSINIT, " SMSD daemon reloaded with debug=%d\n", currentConfig->debug);

  currentConfig->log_handle = gpul_log_init("SMSD", currentConfig->logfile);
  if (!currentConfig->log_handle)
    {
      LogWrite(LOG_ERROR, " cannot init log\n");
    }

  return;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////DEBUT MAIN///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
  int cfd, bfd;
  char *p;
  struct client_state *csp;
  char *remote_ip_str;
  long remote_ip_long;
  char *default_configfile = NULL;
  smsConf_t * config;
  pid_t pid = 0;
  /*struct sigaction action;*/
  int ret = 0;
  int err = 0;
  struct timeval tv;
  struct tm startts;
  void *task_handle;
  void *log_handle;


  prog = argv[0];

  logfp = stdout;

  cfd = -1;

  default_configfile = "smsd.conf";
  configfile = default_configfile;

  gpul_get_date(&startts, 0);
  snprintf(bufstartTime, sizeof(bufstartTime), "%d/%02d/%02d:%02d:%02d:%02d", startts.tm_year + 1900, startts.tm_mon + 1, startts.tm_mday, startts.tm_hour, startts.tm_min, startts.tm_sec);

  gettimeofday(&tv, NULL);
  startTime = tv.tv_sec;

  /* the mutex for speaking with the polld and softupdate servers */
  INITUPDATETHREADLOCK;

  /* the last n commands */
  INITLAST10THREADLOCK;
  INITLAST100THREADLOCK;

  /* the mutex for the stat of the commands */
  INITACTIVETHREADLOCK;
  INITSTATUPDATETHREADLOCK;
  INITSTATGETREPORTTHREADLOCK;
  INITSTATGETVPNSTATUSTHREADLOCK;
  INITSTATGETCONFTHREADLOCK;
  INITSTATGETVPNREPORTTHREADLOCK;
  INITSTATGETROUTERIPTHREADLOCK;
  INITSTATGETCLIENTCERTTHREADLOCK;
  INITSTATGETCLIENTPOLICYTHREADLOCK;
  INITSTATCHECKUPDATETHREADLOCK;
  INITSTATGETCERTPASSWORDTHREADLOCK;
  INITSTATUPDATECERTIFICATETHREADLOCK;
  INITSTATIPDOWNTHREADLOCK;
  INITSTATPPPIPUPTHREADLOCK;
  INITSTATDYNIPUPTHREADLOCK;
  INITSTATSENDSERIALTHREADLOCK;

  /* reset the stat struct */
  memset(&currentStat, '\0', sizeof(sms_stat_t));

  if (argc > 2)
    {
      if (strcasecmp(argv[1], "-c") == 0)
        configfile = argv[2];
      else
        {
          printf("usage:%s -c file [-d]", argv[0]);
          exit(-1);
        }
    }

  config = createConfig(configfile);
  if (!config)
    {
      printf("%s: can't open configuration file '%s': : cannot continue, exiting\n", prog, configfile);
      exit(-1);
    }
  currentConfig = config;

  gpul_conf_init();

  init_snmp_counters(currentConfig->log_handle);

  if (currentConfig->logfile)
    {
      currentConfig->log_handle = gpul_log_init("smsd", currentConfig->logfile);
      if (!currentConfig->log_handle)
        {
          printf("cannot init log\n");
          exit(-1);
        }
      gpul_log_set_level(currentConfig->log_handle, currentConfig->debug);
      gpul_log_set_event(currentConfig->log_handle, EVT_SMSINIT);

      if (LogInit(currentConfig->logfile, FALSE, currentConfig->loglevel, prog) == -1)
        fprintf(stderr, "%s: can't open logfile '%s': ", prog, currentConfig->logfile);

      GLogINFO(currentConfig->log_handle, "SMSD daemon started with loglevel=%d version :%s", currentConfig->loglevel, CVSNAME);

      /*fprintf(stdout,"SMSD daemon started with loglevel=%d",currentConfig->loglevel);*/

      if (DbgInit(currentConfig->logfile, FALSE, currentConfig->debug, prog) == -1)
        fprintf(logfp, "%s: can't open debug logfile '%s': ", prog, currentConfig->logfile);

      GLogINFO(currentConfig->log_handle, "SMSD daemon started with debug=%d", currentConfig->debug);

    }

  ret = sms_db_init(currentConfig->db_conf_file);
  if (ret)
  {
    exit(-1);
  }

  ret = sms_router_init(currentConfig->router_conf_file);
  if (ret)
  {
    exit(-1);
  }

  ret = VerbInit();
  if (ret)
    {
      fprintf(stderr, "error %d in initializing verb\n", ret);
      exit(-1);
    }

  ret = sms_sapi_php_init();
  if (ret)
    {
      fprintf(stderr, "error %d in initializing php\n", ret);
      exit(-1);
    }

  /** parse the address to see if the port is concatenated */
  if (currentConfig->haddr)
    {
      if ((p = strchr(currentConfig->haddr, ':')))
        {
          *p++ = '\0';
          if (*p)
            currentConfig->port = atoi(p);
        }

      if (currentConfig->port <= 0)
        {
          *--p = ':';
          GLogERROR(currentConfig->log_handle, "invalid bind port spec %s", currentConfig->haddr);
          fprintf(stderr, "invalid bind port spec %s", currentConfig->haddr);
          err = 1;
        }
      if (*currentConfig->haddr == '\0')
        currentConfig->haddr = NULL;
    }

  GLogINFO(currentConfig->log_handle, "bind (%s, %d)", currentConfig->haddr ? currentConfig->haddr : "INADDR_ANY", currentConfig->port);

  bfd = socketBind(currentConfig->haddr, currentConfig->port);

  if (bfd < 0)
    {
      GLogERROR(currentConfig->log_handle, "can't bind %s:%d ", currentConfig->haddr ? currentConfig->haddr : "INADDR_ANY", currentConfig->port);
      fprintf(stderr, "can't bind %s:%d \n", currentConfig->haddr ? currentConfig->haddr : "INADDR_ANY", currentConfig->port);
      GLogERROR(currentConfig->log_handle, "There may be another smsd running on port %d", currentConfig->port);
      fprintf(stderr, "There may be another smsd running on port %d\n", currentConfig->port);
      err = 1;
    }

  if (err)
    exit(-1);

  if(gpul_init_signals(currentConfig->log_handle, &smsSignal) != 0)
  {
    GLogERROR(currentConfig->log_handle, "Unable to setup signal handlers");
    fprintf(stderr, "Unable to setup signal handlers\n");
    exit(-1);
  }

  pid = fork();

  if (!pid)
    {
      printf("%d\n", getpid());

      /* pour etre sur de ne pas reutiliser ces fd */
      freopen("/dev/null", "r+", stdin);
      freopen("/dev/null", "r+", stdout);
      if (currentConfig->logfile)
        freopen("/dev/null", "r+", stderr);

      setpgrp();

      while (waitpid(-1, NULL, WNOHANG) > 0)
      {
        /* zombie children */
      }

      if (prctl(PR_SET_DUMPABLE, 1, 0, 0, 0) < 0)
      {
        GLogERROR(currentConfig->log_handle, "FAILED to set PR_SET_DUMPABLE flag, %s", strerror(errno));
        GLogERROR(currentConfig->log_handle, "exiting ");
        fprintf(stderr, "%s: FAILED to set PR_SET_DUMPABLE flag, %s\n", prog, strerror(errno));
        exit(-1);
      }

      sched_handle = gpul_sched_init(currentConfig->heart_beat, currentConfig->numSimultaneous + 2, currentConfig->numSimultaneous + 10, 0, 1024 * 1024, "smsd", currentConfig->logfile);
      if (!sched_handle)
        {
          GLogERROR(currentConfig->log_handle, "Cannot initialize scheduler");
          fprintf(stderr, " Cannot initialize scheduler, exiting... \n");
          exit(-1);
        }
      log_handle = gpul_sched_get_my_log_handle(sched_handle);
      gpul_log_set_level(log_handle, currentConfig->debug);
      gpul_log_set_event(log_handle, EVT_SMSINIT);

      InitDBSqlCtx(log_handle);

      /* unlock the SD on update if necessary */
      GLogINFO(currentConfig->log_handle, "Unlocking the site on update locks ... please wait");
      if (resetBdFlag())
      {
        GLogERROR(currentConfig->log_handle, "Unable to unlock sites. Database error. Exiting");
        fprintf(stderr, " Unable to unlock sites. Database error. Exiting \n");
        exit(-1);
      }
      GLogINFO(currentConfig->log_handle, "Unlock done.");

      task_handle = gpul_sched_new_task(sched_handle, scan_updated, NULL, NULL, "scan-updated", 60 * GPUL_MILLI_PER_SEC, 60 * GPUL_MILLI_PER_SEC, GPUL_SCHED_ABSOLUTE);
      if (!task_handle)
        {
          LogWriteExt(LOG_ERROR, "N/A", EVT_SMSINIT, " can't start the scan-updated task. Exiting !\n");
          fprintf(stderr, "can't start the scan-updated task. Exiting !\n");
          exit(-1);
        }

      task_handle = gpul_sched_new_task(sched_handle, updateStats, NULL, NULL, "update stats", 58 * GPUL_MILLI_PER_SEC, 60 * GPUL_MILLI_PER_SEC, GPUL_SCHED_ABSOLUTE);
      if (!task_handle)
        {
          GLogERROR(currentConfig->log_handle, "can't start the update stats task. Exiting !");
          fprintf(stderr, " can't start the update stats task. Exiting !\n");
          exit(-1);
        }

      if (currentConfig->nightly_save_conf)
      {
    	char time[15];
    	strcpy(time,currentConfig->save_all_conf_time);
    	//char time[] = "08:00:00";
    	char delims[] = ":";
    	int hour;
    	int min;
    	int sec;
    	char split_time[5];
    	char *result = NULL;
    	result = strtok(time,delims);
    	int i = 0;
    	while(result != NULL)
    	{
    		strcpy(split_time,result);
    		result = strtok(NULL, delims);
    		switch (i){
    		case 0:
    			hour = atoi(split_time);
    			break;
    		case 1:
    			min = atoi(split_time);
    			break;
    		case 2:
    			sec = atoi(split_time);
    			break;
    		default:
    			break;
    		}
    		i++;
    	}

    	char period[15];
    	strcpy(period,currentConfig->save_all_conf_freq);
    	//char period[] = "24:00:00";
    	int period_hour;
    	int period_min;
    	int period_sec;
    	char split_period[5];
    	char *results = NULL;
    	results = strtok(period,delims);
    	i = 0;
    	while(results != NULL)
    	{
    	   strcpy(split_period,results);
    	   results = strtok(NULL, delims);
    	   switch (i){
    	   case 0:
    		   period_hour = atoi(split_period);
    	       break;
    	   case 1:
    		   period_min = atoi(split_period);
    		   break;
    	   case 2:
    		   period_sec = atoi(split_period);
    		   break;
    	   default:
    		   break;
    	   }
    	   i++;
    	 }
    	int freq = period_hour * 3600 + period_min * 60 + period_sec;

        task_handle = gpul_sched_new_task(sched_handle, save_all_conf, NULL, NULL, "save all conf", gpul_sched_milliseconds_to_date(hour,min,sec), freq * GPUL_MILLI_PER_SEC, GPUL_SCHED_ABSOLUTE);
        if (!task_handle)
        {
           GLogERROR(currentConfig->log_handle, "can't start the Save All Conf task. Exiting !");
           exit(-1);
        }
      }

      LogWriteExt(LOG_INFO, "N/A", EVT_SMSINIT, " accepting connection ... \n");

      for (;;) /* endless loop to handle incoming connections */
        {
          ACTIVETHREADLOCK;
          currentStat.activeThreads = gpul_sched_get_nb_tasks(sched_handle);
          currentStat.totalThreads = totalConnections;
          ACTIVETHREADUNLOCK;

          if (shutdownRequired == TRUE)
            {
              goto shutdown;
            }
          if (reloadRequired == TRUE)
            {
              reloadConfig();
            }

          /*LogWrite(LOG_INFO, " accepting connection ... \n");*/

          remote_ip_str = NULL;
          remote_ip_long = 0;
          cfd = socketAccept(bfd, &remote_ip_str, &remote_ip_long);

          totalConnections++;
          if (shutdownRequired == TRUE)
            {
              goto shutdown;
            }

          if (reloadRequired == TRUE)
            {
              reloadConfig();
            }

          if (cfd < 0)
            {
              LogWriteExt(LOG_ERROR, "N/A", EVT_SMSINFO, " accept FAILED: %s\n", strerror(errno));
              freez(remote_ip_str);
              sleep(2);
              continue;
            }

          csp = (struct client_state *) malloc(sizeof(*csp));
          if (csp == NULL)
            {
              LogWriteExt(LOG_ERROR, "N/A", EVT_SMSINFO, " malloc(%d) for csp FAILED: %s\n", sizeof(*csp), strerror(errno));
              freez(remote_ip_str);
              socketClose(cfd);
              continue;
            }

          csp->cfd = cfd;
          csp->ip_addr_str = remote_ip_str;
          csp->ip_addr_long = remote_ip_long;
          csp->db_ctx = NULL;

          task_handle = gpul_sched_new_prio_task(sched_handle, serve, csp, NULL, "serve", 0, 0, GPUL_SCHED_ABSOLUTE);
          if (!task_handle)
            {
              LogWriteExt(LOG_ERROR, "N/A", EVT_SMSINFO, " can't fork: %s\n", strerror(errno));
              socketClose(csp->cfd);
              freez(csp->ip_addr_str);
              free(csp);
              sleep(5);
            }
        }

      /**  free structs */
      shutdown:

      GLogINFO(currentConfig->log_handle, "shutdown started");
      gpul_sched_shutdown(sched_handle);

      freez(updateThreadMutex);
      freez(last10ThreadMutex);
      freez(last100ThreadMutex);
      freez(statupdateThreadMutex);
      freez(statgetreportThreadMutex);
      freez(statgetvpnstatusThreadMutex);
      freez(statgetconfThreadMutex);
      freez(statgetvpnreportThreadMutex);
      freez(statgetrouteripThreadMutex);
      freez(statgetclientcertThreadMutex);
      freez(statgetclientpolicyThreadMutex);
      freez(statcheckupdateThreadMutex);
      freez(statgetcertpasswordThreadMutex);
      freez(statupdatecertificateThreadMutex);
      freez(statipdownThreadMutex);
      freez(statpppipupThreadMutex);
      freez(statcisco800pppipupThreadMutex);
      freez(statciscoasapppipupThreadMutex);
      freez(statscreenospppipupThreadMutex);
      freez(statfortinetpppipupThreadMutex);
      freez(statlnpppipupThreadMutex);
      freez(statdynipupThreadMutex);
      freez(statsendserialThreadMutex);

      sms_sapi_php_shutdown();
      VerbEnd();
      close(bfd);
      freez(activeThreadMutex);
      sms_db_shutdown();
      GLogINFO(currentConfig->log_handle, "shutdown complete");
      deleteConfig(currentConfig);

      exit(0);
    }
  else
    {
      /*father is exiting */
      GLogDEBUG(currentConfig->log_handle, 15, "father exiting");
      exit(0);
    }
}
