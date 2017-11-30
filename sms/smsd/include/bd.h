/** @file bd.h
  This is the header file of the verb module of the smsd daemon.
  @ingroup  smsd
  */
#ifndef __BD_H_
#define __BD_H_

#include "sms_db.h"

#define RELEASE_BD_CTX(_csp) {\
  sms_db_release_ctx((_csp)->db_ctx);\
  (_csp)->db_ctx=NULL;\
}

extern int HA_SMSSQL_SetSDIPConfig ( sql_context * ctx,  char *sd_prefix, long sd_seqnum, char *sd_ipconf, int connected_flag );
extern int HA_SMSSQL_SetSDUpdated ( sql_context * ctx, char *Pprefix, long Pseqnum, int updated_flag);

/* SMS BD functions for locking and unlocking a SD during updates */
extern int HA_SMSSQL_SetLock ( sql_context * ctx, char *sd_prefix, long sd_seqnum, int * lock_state );
extern int HA_SMSSQL_UnLock ( sql_context  * ctx, char *sd_prefix, long sd_seqnum);
extern int HA_SMSSQL_UnLockAll ( sql_context * ctx, char *error_mesg);
extern int HA_SMSSQL_SetProvDate(sql_context *ctx, char *sd_prefix, long sd_seqnum);
extern int HA_SMSSQL_SetSDprovlock (sql_context * ctx, char * sd_prefix, long sd_seqnum, int provlock);
extern int HA_SMSSQL_GetSDprovlock (sql_context * ctx, char * sd_prefix, long sd_seqnum, int * provlock);
extern int HA_SMSSQL_SetSDConfigStep ( sql_context * ctx, char *Pcli_prefix, long Pseqnum, int Pcfg_step);
//extern int HA_SMSSQL_GetSDConfigStep ( sql_context * ctx, char *Pcli_prefix, long Pseqnum, int *Pcfg_step);
extern int HA_SMSSQL_SetSDConnected (sql_context *ctx, char *Pcli_prefix, long Pseqnum, int connected_flag);
//extern int HA_SMSSQL_SetSDipsExpiration (sql_context *ctx, char *sd_prefix, long sd_seqnum, char *sd_ips_exp_date);
//extern int HA_SMSSQL_GetSDipsExpiration (sql_context *ctx, char *sd_prefix, long sd_seqnum, char *sd_ips_exp_date);
//extern int HA_SMSSQL_SetSDavExpiration (sql_context *ctx, char *sd_prefix, long sd_seqnum, char *sd_av_exp_date);
//extern int HA_SMSSQL_SetSDurlExpiration (sql_context *ctx, char *sd_prefix, long sd_seqnum, char *sd_url_exp_date);
//extern int HA_SMSSQL_SetSDasExpiration (sql_context *ctx, char *sd_prefix, long sd_seqnum, char *sd_as_exp_date);
extern int HA_SMSSQL_GetAsset (sql_context *ctx, sd_asset_like_t *sd_asset);
extern int HA_SMSSQL_GetSDToUpdateFirst ( sql_context *ctx, sd_like_t *sd );
extern int HA_SMSSQL_GetSDToUpdateNext ( sql_context *ctx, sd_like_t *sd );
extern int HA_SMSSQL_InitProvStatus ( sql_context *ctx, char *Pprefix, long Pseqnum, int num_stages, char **stage_names);
extern int HA_SMSSQL_SetProvStatus ( sql_context *ctx, char *Pprefix, long Pseqnum, int stage_index, char *status, char *next_status, unsigned int error_code, char *error_message);
extern int HA_SMSSQL_GetProvStatus ( sql_context *ctx, sd_provstatus_like_t *provstatus);
extern int HA_SMSSQL_SetPollMode ( sql_context * ctx, char *sd_prefix, long sd_seqnum, int sd_poll_mode);
extern int HA_SMSSQL_SetSdDown(sql_context *ctx, char *sd_prefix, long sd_seqnum);


#endif
