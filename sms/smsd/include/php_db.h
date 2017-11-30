#ifndef PHP_DB_H_
#define PHP_DB_H_

extern int sms_sd_prov_lock(long lcsp, long lsd_info);
extern int sms_sd_prov_unlock(long lcsp, long lsd_info);
extern int sms_sd_lock(long lcsp, long lsd_info);
extern int sms_sd_unlock(long lcsp, long lsd_info);
extern int sms_sd_forceasset(long lcsp, long lsd_info);
extern int sms_bd_init_provstatus(long lcsp, long lsd_info, int num_stages, zval *lstage_names);
extern int sms_bd_set_provstatus(long lcsp, long lsd_info, int stage_index, char *status, int error_code, char *next_status, char *additionalmsg);
extern long sms_bd_get_provstatus(long lcsp, long lsd_info, int stage_index);
extern char *sms_bd_get_provstatus_field_str(long lprovstatus, char *field_name);
extern void sms_bd_free_provstatus(long lprovstatus);
extern int sms_bd_set_poll_mode(long lcsp, long lsd_info, int poll_mode);
extern int sms_bd_set_ipconfig(long lcsp, long lsd_info, char *ip_addr);
extern int sms_bd_reset_conf_objects(long lcsp, long lsd_info);
extern int sms_bd_set_conf_objects(long lcsp, long lsd_info, char *name, char *value);
extern int sms_bd_delete_conf_objects(long lcsp, long lsd_info, char *name);
extern int sms_bd_set_log(long lsd_info, zval *zlog);

#endif /*PHP_DB_H_*/
