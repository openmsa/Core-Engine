/*
 * @file sms_db.h
 *	Copyright UBIqube Solutions 2009
 *  Created on: Nov 12, 2009
 */

#ifndef SMS_DB_H_
#define SMS_DB_H_

//#ifndef FOR_ORACLE_PRECOMP
//#define sql_context void *
//#endif

#include "smssql.h"

#define BD_DELAY 2

extern volatile int shutdownRequired;

/* sms_dbconfig.c */

#define DB_SMS    0
#define DB_CUST   1
#define DB_LOG    2
#define DB_REPORT 3
#define NB_DB     4

extern char *database_name_list[];

typedef struct sms_db_cnx
{
  /** the Net8 service name to use for the connection with the database*/
  char *service_name;
  /** the login used to connect to the database */
  char *login;
  /** the password used to connect to the database */
  char *password;
  char *schema_owner;
} sms_db_cnx_t;

typedef struct sms_db_config
{
  /** the databases connection definition */
  sms_db_cnx_t sms_db[NB_DB];
  char *node_name;
  int  sms_IsDistributed;
  char *ubi_sys_equipments_nat_ip;
  char *ubi_sys_equipments_ipv6;
  /** ES index name */
  char *es_logindex_name;
  int db_keep_alive;
} sms_db_config_t;

/** \struct database_context
 This structure context a context use to connect to the database
 */
typedef struct database_context
{
  /** the pointer on the allocated sql context (connection to the database )*/
  sql_context sql_ctx;
  /** state of this context (DB_CTX_ACTIVE=used by a thread , DB_CTX_NOT_ACTIVE=not used by a thread) */
  int state;
  const char *caller;
} database_context_t;

extern sms_db_config_t *smsDbConfig;
extern int sms_db_init(char *configFile);
extern void sms_db_shutdown(void);
extern int sms_db_alloc_ctx(void *log_handle, int db, int num_ctx);
extern database_context_t *_sms_db_find_ctx(void *log_handle, int db, const char *caller_name);
extern void sms_db_release_ctx(database_context_t *db_ctx);
extern int sms_db_keep_alive(void *log_handle);

#define sms_db_find_ctx(log_handle, db) _sms_db_find_ctx(log_handle, db, __FUNCTION__)

/* sms_sd.c */

struct sd_configvar
{
  GPUL_TAILQ_ENTRY(struct sd_configvar) link;
  sd_configvar_like_t sd_configvar;
};
typedef struct sd_configvar sd_configvar_t;
GPUL_TAILQ_HEAD (sd_configvar_head, sd_configvar_t);

struct sd_ext_attr
{
  GPUL_TAILQ_ENTRY(struct sd_ext_attr) link;
  sd_ext_attr_like_t sd_ext_attr;
};
typedef struct sd_ext_attr sd_ext_attr_t;
GPUL_TAILQ_HEAD (sd_ext_attr_head, sd_ext_attr_t);

struct sd_crud_object
{
  GPUL_TAILQ_ENTRY(struct sd_crud_object) link;
  sd_crud_object_like_t sd_crud_object;
};
typedef struct sd_crud_object sd_crud_object_t;
GPUL_TAILQ_HEAD (sd_crud_object_head, sd_crud_object_t);

struct sd_interface
{
  sd_interface_like_t interface;
};
typedef struct sd_interface sd_interface_t;

struct sd_dmz_interface
{
  GPUL_TAILQ_ENTRY(struct sd_dmz_interface) link;
  sd_interface_t *sd_interface;
  char dmz_network[IP_ADD_MASK_LEN + 1];
  char *dmz_mask;
};
typedef struct sd_dmz_interface sd_dmz_interface_t;
GPUL_TAILQ_HEAD (sd_dmz_interface_head, sd_dmz_interface_t);

/** this structure is used to stored the variables of the router.
 These variables replace the patterns in the configuration file,
 for  example LAN_Network will replace __LAN_NETWORK__
 */

typedef struct php_file_info
{
    /* Name of the PHP file containing the database objects */
    char *php_db_objects_filename;
    /* Name of the PHP file containing the database objects */
    char *php_db_objects_filename_latest;
    /* Keep PHP file flag */
    int php_keep_file;
} php_file_info_t;


/** definition of a structure used to store all the usefull information about a SD */
typedef struct sd_info
{
  /** the main sd attributes*/
  sd_like_t SD;
  /** the Internal interface if exists */
  sd_interface_t *sd_int_interface;
  /** the External interface if exists */
  sd_interface_t *sd_ext_interface;
  /** the Backup interface if exists */
  sd_interface_t *sd_backup_interface;
  /** the DMZ interface list if exists */
  struct sd_dmz_interface_head sd_dmz_interface_list;
  /** the addon interface if exists  */
  sd_interface_t *sd_addon_interface;
  /* the configuration variables list */
  struct sd_configvar_head sd_configvar_list;
  /* the extended attributes list */
  struct sd_ext_attr_head sd_ext_attr_list;
  /* the configuration variables list */
  struct sd_crud_object_head sd_crud_object_list;
  /* type de mise a jour */
  int update_flag;
  /* le sd est dans un vpn ? */
  int into_vpn;
  /* L'id du sd */
  char sdid[MAXHOSTSIZE];
  /** pointer to the ctx SQL */
  database_context_t * db_ctx;
  /* event */
  char *event;
  /* path of the router script */
  char *router_path;
  char *model_data;
  php_file_info_t php_file_info;
  /** */
  char firmware[FIRMWARE_LEN + 1];
} sd_info_t;

typedef struct cust_info
{
    /* CUSTID */
    char custid[SDID_LEN + 2];
    /* ID */
    long id;
    php_file_info_t php_file_info;
} cust_info_t;

extern char *sd_update_status_types[][2];
extern int sd_update_status_types_size;

extern void init_sd(sd_like_t *SD, int reset);
extern void init_sd_info(database_context_t *sms_db_ctx, char *sdid, sd_info_t *sd_info);
extern int get_sd_info(database_context_t *sms_db_ctx, char *sdid, sd_info_t *sd_info, char *prefix, int keep_file);
extern int get_cust_info(database_context_t *sms_db_ctx, char *custid, cust_info_t *cust_info, char *prefix);
extern void free_all_in_cust(cust_info_t *cust_info);
extern void free_sd(sd_like_t *SD);
extern void free_sd_info(sd_info_t *sd_info);
extern int get_all_in_sd(sql_context *ctx, sd_info_t * sd_info);
extern int read_config_var(sd_info_t *sd_info);
extern int read_crud_objects(sd_info_t *sd_info);
extern int read_crud_objects_by_name(sd_info_t *sd_info, char *crud_name);
extern char *get_crud_object_value_by_name(sd_info_t *SDinfo, char *object_name);

/* sms_db.c */

extern void HA_SMSSQL_AllocateContext(sql_context *ctx, int db);
// extern int HA_SMSSQL_WriteLog(sql_context *log_ctx, fildelog_like_t *Plog);
extern int HA_SMSSQL_GetSDinCustomerFirst(sql_context *ctx, long cust_id, sd_like_t *sd);
extern int HA_SMSSQL_GetSDinCustomerNext(sql_context *ctx, sd_like_t *sd);
extern int HA_SMSSQL_GetSD(sql_context * ctx, sd_like_t *sd, int check_node);
extern int HA_SMSSQL_GetFirstSDinterface(sql_context *ctx, char *Pcli_prefix, long Pseqnum, sd_interface_like_t *interface);
extern int HA_SMSSQL_GetNextSDinterface(sql_context *ctx, sd_interface_like_t *interface);
extern int HA_SMSSQL_GetFirstSDConfigVar(sql_context *ctx, sd_configvar_like_t *sd_configvar, char *p_cli_prefix, int p_seqnum);
extern int HA_SMSSQL_GetNextSDConfigVar(sql_context *ctx, sd_configvar_like_t *sd_configvar);
extern int HA_SMSSQL_GetFirstSDExtAttr(sql_context *ctx, sd_ext_attr_like_t *sd_ext_attr, char *p_cli_prefix, int p_seqnum);
extern int HA_SMSSQL_GetNextSDExtAttr(sql_context *ctx, sd_ext_attr_like_t *sd_ext_attr);
extern int HA_SMSSQL_GetSDHostnameFirst(sql_context *ctx, sd_like_t *sd);
extern int HA_SMSSQL_GetSDHostnameNext(sql_context *ctx, sd_like_t *sd);
//extern int HA_SMSSQL_GetSdAssetAttributeFirst(sql_context *ctx, char *prefix, long seqnum, sd_asset_attribute_like_t *assetAttribute);
extern int HA_SMSSQL_GetSdAssetAttributeNext(sql_context *ctx, sd_asset_attribute_like_t *assetAttribute);
extern int HA_SMSSQL_SetSdAssetAttribute(sql_context *ctx, sd_asset_attribute_like_t *assetAttribute);
//extern int HA_SMSSQL_UpdateAllSdAssetAttribute(sql_context *ctx, char *prefix, long seqnum, sd_asset_attribute_like_t *assetAttribute, int nb);
//extern int HA_SMSSQL_RemoveSdAssetAttribute(sql_context *ctx, sd_asset_attribute_like_t *assetAttribute);
extern int HA_SMSSQL_SetSDAssetUpdate(sql_context *ctx, char *Pprefix, long Pseqnum, int AssetUpdate_flag);

extern int HA_SMSSQL_GetFirstAvExpirationDate(sql_context *ctx, sd_licexpiration_like_t *SDAsset);
extern int HA_SMSSQL_GetNextAvExpirationDate(sql_context *ctx, sd_licexpiration_like_t *SDAsset);
extern int HA_SMSSQL_GetFirstIpsExpirationDate(sql_context *ctx, sd_licexpiration_like_t *SDAsset);
extern int HA_SMSSQL_GetNextIpsExpirationDate(sql_context *ctx, sd_licexpiration_like_t *SDAsset);

extern int HA_SMSSQL_GetFirstUrlExpirationDate(sql_context *ctx, sd_licexpiration_like_t *SDAsset);
extern int HA_SMSSQL_GetNextUrlExpirationDate(sql_context *ctx, sd_licexpiration_like_t *SDAsset);
extern int HA_SMSSQL_GetFirstAsExpirationDate(sql_context *ctx, sd_licexpiration_like_t *SDAsset);
extern int HA_SMSSQL_GetNextAsExpirationDate(sql_context *ctx, sd_licexpiration_like_t *SDAsset);

extern int HA_SMSSQL_SetSDHAActiveStatus(sql_context *ctx, char *p_sd_prefix, long p_sd_seqnum, int p_isActive);

extern int HA_SMSSQL_ResetSDCrudObjects(sql_context *ctx, sd_crud_object_like_t *crud_object);
extern int HA_SMSSQL_SetSDCrudObjects(sql_context *ctx, sd_crud_object_like_t *crud_object);
extern int HA_SMSSQL_DeleteSDCrudObjects(sql_context *ctx, sd_crud_object_like_t *crud_object);
extern int HA_SMSSQL_GetFirstSDCrudObject(sql_context *ctx, sd_crud_object_like_t *crud_object);
extern int HA_SMSSQL_GetNextSDCrudObject(sql_context *ctx, sd_crud_object_like_t *crud_object);
extern int HA_SMSSQL_GetFirstSDCrudObjectByName(sql_context *ctx, sd_crud_object_like_t *crud_object, char *crud_name);
extern int HA_SMSSQL_GetNextSDCrudObjectByName(sql_context *ctx, sd_crud_object_like_t *crud_object);

extern int HA_SMSSQL_GetUpdateStatus(sql_context *ctx, sd_updatestatus_like_t *sd_updatestatus);
extern int HA_SMSSQL_SetUpdateStatus(sql_context *ctx, sd_updatestatus_like_t *sd_updatestatus);

extern int HA_SMSSQL_GetFirstDedicatedServerByOperator(sql_context *ctx, dedicated_server_like_t *dedicated_server, char *cli_prefix);
extern int HA_SMSSQL_GetNextDedicatedServerByOperator(sql_context *ctx, dedicated_server_like_t *dedicated_server);
extern int HA_SMSSQL_GetSDconnectedFirst ( sql_context * ctx, sd_like_t *sd );
extern int HA_SMSSQL_GetSDconnectedNext ( sql_context * ctx,  sd_like_t *sd );

extern int HA_SMSSQL_GetMsaParam(sql_context *ctx, msa_parameters_like_t *msa_param);
extern int HA_SMSSQL_SetMsaParam(sql_context *ctx, msa_parameters_like_t *msa_param);

extern int HA_SMSSQL_GetSDbyManufacturerModelFirst(sql_context *ctx, sd_like_t *sd);
extern int HA_SMSSQL_GetSDbyManufacturerModelNext(sql_context *ctx, sd_like_t *sd);

/* sms_dbtophp.c */

extern int create_db_objects_php(sd_info_t *sd_info, database_context_t *db_ctx, char *prefix, int keep_file);
extern int create_cust_db_objects_php(cust_info_t *cust_info, database_context_t *db_ctx, char *prefix, int keep_file);

#endif /* SMS_DB_H_ */
