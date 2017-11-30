/** @file update.h
  This is the header file of the verb module of the smsd daemon.
  @ingroup  smsd
  */

#ifndef __UPDATE_H_
#define __UPDATE_H_

#include "smsd.h"

/** this function is used to check the status of the update
    @param SDid: the id of the router
    @param client_state: the current context
*/

int checkUpdate (char *SDid, client_state_t *csp, char **returnBuffer, char *event);

/** this function is used to check the status of the update Certificate
    @param SDid: the id of the router
    @param client_state: the current context
*/

int checkUpdateCertificate (char *SDid, client_state_t *csp, char **returnBuffer, char *event);

/** this function is used to check the status of all updates
    @param SDid: the id of the router
    @param client_state: the current context
*/

int checkAllUpdate (char *SDid, client_state_t *csp, char **returnBuffer, char *event);

int updateFullConfiguration(char * SDid,  client_state_t *csp) ;

/** this function is used to retreive the configuration of a SD
    It does a connection to the router and returns in configuration
    the current running configuration of the router
*/
int getRouterFullConfiguration(char * SDid,  client_state_t *csp, char ** configuration, char *event);

/** this function is used to retreive the configuration of a SD and backup it in SVN
    It does a connection to the router and returns in configuration
    the current running configuration of the router
*/
int backupRouterFullConfiguration(char *params, client_state_t *csp);

/** send a demand of update to the updated spooled server */

void  sendUpdateServer(char * SDid, char *SDtype, int action, char *event);

int dnsUpdate(char *SDid, char *ip);

int lockSD(client_state_t *csp, char *sd_cli_prefix, long sd_seqnum, char *event);
int unlockSD(client_state_t *csp, char *sd_cli_prefix, long sd_seqnum, char *event);

/** unlock the SD for update
    @param SDid the id of the router
    @param csp	the current context
*/
int unlockUpdate(char * SDid,  client_state_t *csp);

int lockProv(client_state_t *csp, char *sd_cli_prefix, long sd_seqnum, char *event);
int unlockProv(client_state_t *csp, char *sd_cli_prefix, long sd_seqnum, char *event);

/** Lock the Provisioning flag
    @param SDid the id of the router
    @param csp	the current context
*/
int lockProvisioning(char * SDid,  client_state_t * csp);

/** UnLock the Provisioning flag
    @param SDid the id of the router
    @param csp	the current context
*/
int unlockProvisioning(char * SDid,  client_state_t * csp);

int smsexec(char * SDid, client_state_t *csp, char **result, char *addon);

int smsexecn(char *SDid, client_state_t *csp, char **result, char *addon);

int exec_script(char *params, client_state_t *csp, int synchronous);

int provisioning(client_state_t *csp, char *SDid, char *event);
int checkProvisioning(client_state_t *csp, char *SDid, char *event);
int push_config(client_state_t *csp, char *SDid);

int getPingStatus(client_state_t *csp, char *params, char *event);

int updateConfig(client_state_t *csp, sd_info_t *sd_info, char *event);

int restoreConf(char *params, client_state_t *csp);

int staging(client_state_t *csp, char *SDid, int map, char *event);

int scan_updated(void *dummy);

int set_sd_updated(char *SDid, client_state_t *csp, unsigned int sd_updated_flag);

int updateLicense(char * SDid,  client_state_t * csp);

int updateFirmware(char * SDid,  client_state_t * csp);

int setUpdateStatus(char *SDid, client_state_t *csp, char *type, char *status, char *failure_mag);
int getUpdateStatus(char *SDid, client_state_t *csp, char *type, char **result);

int ha_swap(client_state_t *csp, char *SDid);

int listVmDependingState(client_state_t *csp, char *SDid);

int getHardDriveSize(client_state_t *csp, char *params);

int smsCmd(char *params, client_state_t *csp);

int smsCustCmd(char *params, client_state_t *csp);

int genTemplate(char *params, client_state_t *csp);

#endif    /* __UPDATE_H_ */
