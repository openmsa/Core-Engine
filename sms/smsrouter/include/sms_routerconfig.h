/*
 * sms_routerconfig.h
 *
 *  Created on: Sep 12, 2011
 *  Copyright UBIqube Solutions 2011
 */

#ifndef SMS_ROUTERCONFIG_H_
#define SMS_ROUTERCONFIG_H_

#include "sms_router.h"

typedef struct router_config
{
  /** model */
  char *model;
  /** path */
  char *path;
  /** model for reporting */
  char *report_model;
  /** asset-script-name */
  char *asset_script_name;
  /** poll-script-name */
  char *poll_script_name;
  /** conf-type can be CONF_FILE or CONF_TREE (used by change management) */
  char *config_type;
  /** specific-data */
  char *model_data;
  /** host or vm */
  int vm_type;
} router_config_t;

typedef struct sms_router_config
{
  /** hash table of router config by model "manid:modid" */
  void *router_config_hash;
} sms_router_config_t;

extern sms_router_config_t *sms_router_config;

#endif /* SMS_ROUTERCONFIG_H_ */
