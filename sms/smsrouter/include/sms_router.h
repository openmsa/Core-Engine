/*
 * sms_router.h
 *
 *  Created on: Sep 12, 2011
 *      Author: ees
 */

#ifndef SMS_ROUTER_H_
#define SMS_ROUTER_H_

#define VM_TYPE_NONE 0
#define VM_TYPE_HOST 1
#define VM_TYPE_VM   2

extern int sms_router_init(char *configFile);
extern void sms_router_shutdown(void);
extern char *get_router_asset_script(int man_id, int mod_id);
extern char *get_router_poll_script(int man_id, int mod_id);
extern char *get_router_path(int man_id, int mod_id);
extern char *get_router_model_data(int man_id, int mod_id);
extern char *get_router_report_model(int man_id, int mod_id);
extern char *get_router_config_type(int man_id, int mod_id);
extern int get_vm_type(int man_id, int mod_id);

#endif /* SMS_ROUTER_H_ */
