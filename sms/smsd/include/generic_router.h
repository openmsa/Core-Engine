/*
 * Copyright UBIqube Solutions 2008
 * File: generic_router.h
 * Creation Date: May 30, 2008
 */


#ifndef GENERIC_ROUTER_H_
#define GENERIC_ROUTER_H_

extern int genericStaging(client_state_t *csp, sd_info_t *SDinfo);
extern int genericStagingMap(client_state_t *csp, sd_info_t *SDinfo);
extern int genericInitialProvisioning(client_state_t *csp, sd_info_t *SDinfo, char * ipaddr, char * login, char * passwd, char * adminpasswd);
extern int genericCheckProvisioning(client_state_t *csp, sd_info_t *SDinfo);
extern int genericSnmpTest(client_state_t *csp, char *cmd, char *address, char *community, char *oid, char *event);
extern int genericGetRunningConf(client_state_t *csp, sd_info_t *SDinfo);
extern int genericGetActivityReport(client_state_t *csp, sd_info_t *SDinfo);
extern int genericGetBotnetReport(client_state_t *csp, sd_info_t *SDinfo);
extern int genericUpdateConf(client_state_t *csp, sd_info_t *SDinfo);
extern int genericRestoreConf(client_state_t *csp, sd_info_t *SDinfo, char *revision_id, char *msg);
extern int genericBackupConf(client_state_t *csp, sd_info_t *SDinfo, char *backup_module, char *msg);
extern int genericGetConfig(client_state_t *csp, sd_info_t *SDinfo);
extern int genericGetConfiguration(sd_info_t *SDinfo, char **config, char **running_config);
extern int genericSmsexecCmd(client_state_t *csp, sd_info_t *SDinfo, struct router_cmd_head *router_cmd_list);
extern int genericUnarchiveConfiguration(client_state_t *csp, sd_info_t *SDinfo, char *archive, char *folder);
extern int genericGetArchiveConfiguration(client_state_t *csp, sd_info_t *SDinfo, char *folder);
extern int genericUpdateLicense(client_state_t *csp, sd_info_t * SDinfo);
extern int genericUpdateFirmware(client_state_t *csp, sd_info_t * SDinfo, char *param, char *server_addr);
extern int genericExecScript(client_state_t *csp, sd_info_t *SDinfo, char *script, int synchronous);
extern int genericGetReport(sd_info_t * SDinfo, char **reportBuffer, int fullReport, int startDate, int endDate);
extern int genericHaSwap(client_state_t *csp, sd_info_t *SDinfo);
extern int genericCheckSerialNumber(sd_info_t *SDinfo, char *ipaddr);
extern int genericCallCommand(client_state_t *csp, sd_info_t *SDinfo, char *command, int apply_conf, char *params, char **result, char **output);
extern int genericSendDataFiles(client_state_t *csp, sd_info_t *SDinfo, char *addon, char *server_addr, char *server_ftp_login, char *server_ftp_passwd);
extern int genericPushConfig(client_state_t *csp, sd_info_t *SDinfo, char *addon, char *ts_ip, char *ts_port, char *configuration);
extern int generic_sms_cmd(client_state_t *csp, sd_info_t *SDinfo, char *cmd, char *optional_params);
extern int generic_smsd_cmd(client_state_t *csp, char *cmd, char *optional_params);
extern int generic_sms_cust_cmd(client_state_t *csp, cust_info_t *cust_info, char *cmd, char *optional_params);
extern int generic_gen_template(client_state_t *csp, sd_info_t *SDinfo, char *revision1, char *revision2, char *addon);
extern int genericSmsAddonExecCmd(client_state_t *csp, sd_info_t *SDinfo, char *addon, struct router_cmd_head *router_cmd_list);

#endif /*GENERIC_ROUTER_H_*/
