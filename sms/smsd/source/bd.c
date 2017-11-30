/** @file bd.c
 *  @brief core file of the sms daemon
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
#include <pthread.h>

#include "log.h"
#include "net.h"
#include "verb.h"
#include "smsd.h"
#include "bd.h"

#define BD_DELAY 2

int HA_SMSSQL_SetSDIPConfig(sql_context *ctx, char *sd_prefix, long sd_seqnum, char *sd_ipconf, int connected_flag)
{
  int ret = ERR_DB_CONNECTION_LOST;
  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_SetSDIPConfig(*ctx, sd_prefix, sd_seqnum, sd_ipconf, connected_flag);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }
  return (ret);

}

int HA_SMSSQL_SetSDUpdated(sql_context *ctx, char *Pprefix, long Pseqnum, int updated_flag)
{
  int ret = ERR_DB_CONNECTION_LOST;

  if (updated_flag)
  {
    DbgWrite(DBG_INFO, " %s%lu is scheduled for update\n", Pprefix, Pseqnum);
  }

  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_SetSDUpdated(*ctx, Pprefix, Pseqnum, updated_flag);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }
  return (ret);

}

int HA_SMSSQL_SetSDprovlock(sql_context *ctx, char * sd_prefix, long sd_seqnum, int provlock)
{
  int ret = ERR_DB_CONNECTION_LOST;
  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_SetSDprovlock(*ctx, sd_prefix, sd_seqnum, provlock);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }
  return (ret);

}

int HA_SMSSQL_GetSDprovlock(sql_context *ctx, char * sd_prefix, long sd_seqnum, int *provlock)
{
  int ret = ERR_DB_CONNECTION_LOST;
  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_GetSDprovlock(*ctx, sd_prefix, sd_seqnum, provlock);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }
  return (ret);

}

int HA_SMSSQL_SetLock(sql_context *ctx, char *sd_prefix, long sd_seqnum, int * lock_state)
{
  int provlock;
  int ret;

  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  ret = HA_SMSSQL_GetSDprovlock(ctx, sd_prefix, sd_seqnum, &provlock);
  if (ret)
  {
    return ret;
  }

  if (provlock)
  {
    LogWriteExt2(LOG_ERROR, sd_prefix, sd_seqnum, EVT_UNKNOWN, " Warning Provisioning lock set. The SD needs Initial provisioning\n");
    *lock_state = ALREADY_PROVLOCKED;
    return ret;
  }

  ret = ERR_DB_CONNECTION_LOST;
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_SetLock(*ctx, sd_prefix, sd_seqnum, lock_state);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }
  return (ret);

}

int HA_SMSSQL_UnLock(sql_context *ctx, char *sd_prefix, long sd_seqnum)
{
  int ret = ERR_DB_CONNECTION_LOST;
  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_UnLock(*ctx, sd_prefix, sd_seqnum);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }
  return (ret);

}

int HA_SMSSQL_UnLockAll(sql_context *ctx, char *error_mesg)
{
  int ret = ERR_DB_CONNECTION_LOST;
  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_UnLockAll(*ctx, error_mesg);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }
  return (ret);

}

int HA_SMSSQL_SetProvDate(sql_context *ctx, char *sd_prefix, long sd_seqnum)
{
  int ret = ERR_DB_CONNECTION_LOST;
  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_SetProvDate(*ctx, sd_prefix, sd_seqnum);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_CUST);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }
  return (ret);

}

int HA_SMSSQL_SetSDConfigStep(sql_context *ctx, char *Pcli_prefix, long Pseqnum, int Pcfg_step)
{
  int ret = ERR_DB_CONNECTION_LOST;
  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_SetSDConfigStep(*ctx, Pcli_prefix, Pseqnum, Pcfg_step);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }
  return (ret);
}

//int HA_SMSSQL_GetSDConfigStep(sql_context *ctx, char *Pcli_prefix, long Pseqnum, int *Pcfg_step)
//{
//  int ret = ERR_DB_CONNECTION_LOST;
//  if (shutdownRequired)
//  {
//    return (ERR_DB_FAILED);
//  }
//  while (ret == ERR_DB_CONNECTION_LOST)
//  {
//    ret = SMSSQL_GetSDConfigStep(*ctx, Pcli_prefix, Pseqnum, Pcfg_step);
//    if (ret == ERR_DB_CONNECTION_LOST)
//    {
//      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
//      if (shutdownRequired)
//      {
//        return (ERR_DB_FAILED);
//      }
//    }
//  }
//  return (ret);
//}

int HA_SMSSQL_SetSDConnected(sql_context *ctx, char *Pcli_prefix, long Pseqnum, int connected_flag)
{
  int ret = ERR_DB_CONNECTION_LOST;
  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_SetSDConnected(*ctx, Pcli_prefix, Pseqnum, connected_flag);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }
  return ret;
}

//int HA_SMSSQL_SetSDipsExpiration(sql_context *ctx, char *sd_prefix, long sd_seqnum, char *sd_ips_exp_date)
//{
//  int ret = ERR_DB_CONNECTION_LOST;
//
//  if (shutdownRequired)
//  {
//    return (ERR_DB_FAILED);
//  }
//  while (ret == ERR_DB_CONNECTION_LOST)
//  {
//    ret = SMSSQL_SetSDipsExpiration(*ctx, sd_prefix, sd_seqnum, sd_ips_exp_date);
//    if (ret == ERR_DB_CONNECTION_LOST)
//    {
//      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
//      if (shutdownRequired)
//      {
//        return (ERR_DB_FAILED);
//      }
//    }
//  }
//
//  return ret;
//}

//int HA_SMSSQL_GetSDipsExpiration(sql_context *ctx, char *sd_prefix, long sd_seqnum, char *sd_ips_exp_date)
//{
//  int ret = ERR_DB_CONNECTION_LOST;
//
//  if (shutdownRequired)
//  {
//    return (ERR_DB_FAILED);
//  }
//  while (ret == ERR_DB_CONNECTION_LOST)
//  {
//    ret = SMSSQL_GetSDipsExpiration(*ctx, sd_prefix, sd_seqnum, sd_ips_exp_date);
//    if (ret == ERR_DB_CONNECTION_LOST)
//    {
//      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
//      if (shutdownRequired)
//      {
//        return (ERR_DB_FAILED);
//      }
//    }
//  }
//
//  return ret;
//}

//int HA_SMSSQL_SetSDavExpiration(sql_context *ctx, char *sd_prefix, long sd_seqnum, char *sd_av_exp_date)
//{
//  int ret = ERR_DB_CONNECTION_LOST;
//
//  if (shutdownRequired)
//  {
//    return (ERR_DB_FAILED);
//  }
//  while (ret == ERR_DB_CONNECTION_LOST)
//  {
//    ret = SMSSQL_SetSDavExpiration(*ctx, sd_prefix, sd_seqnum, sd_av_exp_date);
//    if (ret == ERR_DB_CONNECTION_LOST)
//    {
//      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
//      if (shutdownRequired)
//      {
//        return (ERR_DB_FAILED);
//      }
//    }
//  }
//
//  return ret;
//}

//int HA_SMSSQL_SetSDurlExpiration(sql_context *ctx, char *sd_prefix, long sd_seqnum, char *sd_url_exp_date)
//{
//  int ret = ERR_DB_CONNECTION_LOST;
//
//  if (shutdownRequired)
//  {
//    return (ERR_DB_FAILED);
//  }
//  while (ret == ERR_DB_CONNECTION_LOST)
//  {
//    ret = SMSSQL_SetSDurlExpiration(*ctx, sd_prefix, sd_seqnum, sd_url_exp_date);
//    if (ret == ERR_DB_CONNECTION_LOST)
//    {
//      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
//      if (shutdownRequired)
//      {
//        return (ERR_DB_FAILED);
//      }
//    }
//  }
//
//  return ret;
//}

//int HA_SMSSQL_SetSDasExpiration(sql_context *ctx, char *sd_prefix, long sd_seqnum, char *sd_as_exp_date)
//{
//  int ret = ERR_DB_CONNECTION_LOST;
//
//  if (shutdownRequired)
//  {
//    return (ERR_DB_FAILED);
//  }
//  while (ret == ERR_DB_CONNECTION_LOST)
//  {
//    ret = SMSSQL_SetSDasExpiration(*ctx, sd_prefix, sd_seqnum, sd_as_exp_date);
//    if (ret == ERR_DB_CONNECTION_LOST)
//    {
//      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
//      if (shutdownRequired)
//      {
//        return (ERR_DB_FAILED);
//      }
//    }
//  }
//
//  return ret;
//}

int HA_SMSSQL_GetAsset(sql_context *ctx, sd_asset_like_t *sd_asset)
{
  int ret = ERR_DB_CONNECTION_LOST;

  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_GetAsset(*ctx, sd_asset);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }

  return ret;
}

int HA_SMSSQL_GetSDToUpdateFirst(sql_context *ctx, sd_like_t *sd)
{
  int ret = ERR_DB_CONNECTION_LOST;
  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_GetSDToUpdateFirst(*ctx, sd);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }
  return ret;
}

int HA_SMSSQL_GetSDToUpdateNext(sql_context *ctx, sd_like_t *sd)
{
  int ret;
  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  ret = SMSSQL_GetSDToUpdateNext(*ctx, sd);
  if (ret == ERR_DB_CONNECTION_LOST)
  {
    if (shutdownRequired)
    {
      return (ERR_DB_FAILED);
    }
  }
  return ret;
}

int HA_SMSSQL_InitProvStatus(sql_context *ctx, char *Pprefix, long Pseqnum, int num_stages, char **stage_names)
{
  int ret = ERR_DB_CONNECTION_LOST;
  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_InitProvStatus(*ctx, Pprefix, Pseqnum, num_stages, stage_names);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }
  return ret;
}

int HA_SMSSQL_SetProvStatus(sql_context *ctx, char *Pprefix, long Pseqnum, int stage_index, char *status, char *next_status, unsigned int error_code, char *error_message)
{
  int ret = ERR_DB_CONNECTION_LOST;
  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_SetProvStatus(*ctx, Pprefix, Pseqnum, stage_index, status, next_status, error_code, error_message);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }
  return ret;
}

int HA_SMSSQL_GetProvStatus(sql_context *ctx, sd_provstatus_like_t *provstatus)
{
  int ret = ERR_DB_CONNECTION_LOST;
  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_GetProvStatus(*ctx, provstatus);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }
  return ret;
}

int HA_SMSSQL_SetPollMode(sql_context *ctx, char *sd_prefix, long sd_seqnum, int sd_poll_mode)
{
  int ret = ERR_DB_CONNECTION_LOST;
  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_SetPollMode(*ctx, sd_prefix, sd_seqnum, sd_poll_mode);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }
  return (ret);
}

int HA_SMSSQL_SetSdDown(sql_context *ctx, char *sd_prefix, long sd_seqnum)
{
  int ret = ERR_DB_CONNECTION_LOST;

  if (shutdownRequired)
  {
    return (ERR_DB_FAILED);
  }
  while (ret == ERR_DB_CONNECTION_LOST)
  {
    ret = SMSSQL_SetSDPing(*ctx, sd_prefix, sd_seqnum, 0, 0, 0);
    if (ret == ERR_DB_CONNECTION_LOST)
    {
      HA_SMSSQL_AllocateContext(ctx, DB_SMS);
      if (shutdownRequired)
      {
        return (ERR_DB_FAILED);
      }
    }
  }

  return ret;
}
