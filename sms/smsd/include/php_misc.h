


#ifndef PHP_MISC_H_
#define PHP_MISC_H_

extern void sms_reply_to_user(long lcsp, char *sd_id, char *user_message);
extern void sms_send_user_message(long lcsp, char *sd_id, char *user_message);
extern char *sms_user_message_add(char *message, char *name, char *value);
extern char *sms_user_message_add_json(char *message, char *name, char *value);
extern char *sms_user_message_add_array(char *message, char *name, char *value);
extern char *sms_user_message_array_add(char *array, char *message);
extern void sms_send_user_ok(long lcsp, char *sd_id, char *result);
extern void sms_send_user_ok_ex(long lcsp, char *sd_id, char *result, char *message);
extern void sms_send_user_error(long lcsp, char *sd_id, char *result, int error);
extern void sms_send_user_error_ex(long lcsp, char *sd_id, char *result, int error);
extern void sms_close_user_socket(long lcsp);
extern int sms_set_update_status(long lcsp, char *sd_id, int error_code, char *type, char *status, char *error_msg);

#endif /*PHP_MISC_H_*/
