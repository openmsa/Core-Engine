/**
 * Copyright UBIqube Solutions 2008
 * @file php_misc.c
 * Creation Date: Jun 16, 2008
 */

#include "sms_php.h"
#include "sms_php_functions_misc.h"
#include "sms_php_wrappers.h"

#include "log.h"
#include "net.h"
#include "smsd.h"
#include "sms_db.h"
#include "router.h"
#include "update.h"
#include "verb.h"
#include "error_code.h"
#include "gpul.h"
#include "php_misc.h"

static void internal_send_user_message(long lcsp, char *sd_id, char *result, char *status, char *message, char *code);

#define WRITE(str) \
  { \
    len = strlen(str); \
    ret = socketWrite (csp->cfd, str, len); \
    if (ret != len) \
    { \
      LogWriteExt (LOG_ERROR, sd_id, "", " Error writing [%s] response to client, ret %d expected %d (%s)\n", \
          str, ret, len, strerror (errno)); \
          return; \
    } \
  }

#define WRITE_FIRST_NAME(str) \
  { \
    result_str = gpul_str_append(result_str, "\""); \
    result_str = gpul_str_append(result_str, str); \
    result_str = gpul_str_append(result_str, "\" : "); \
  }

#define WRITE_NAME(str) \
  { \
    result_str = gpul_str_append(result_str, ",\n"); \
    result_str = gpul_str_append(result_str, "\""); \
    result_str = gpul_str_append(result_str, str); \
    result_str = gpul_str_append(result_str, "\" : "); \
  }

#define WRITE_VALUE(str) \
  { \
    result_str = gpul_str_append(result_str, "\""); \
    result_str = gpul_str_append(result_str, str); \
    result_str = gpul_str_append(result_str, "\""); \
  }

/**
 * @addtogroup usercom
 * @{
 */

/** Send back a OK message to the user
 * @param lcsp  pointer on csp context
 * @param sd_id SDID
 * @param result    result message to send to the user
 */
void sms_send_user_ok_ex(long lcsp, char *sd_id, char *result, char *message)
{
  DbgWrite(DBG_INFO, "%s - OK\n", result);
  internal_send_user_message(lcsp, sd_id, result, "OK", message, "0x00000000");
}

/** Send back a OK message to the user
 * @param lcsp  pointer on csp context
 * @param sd_id SDID
 * @param result    result message to send to the user
 */
void sms_send_user_ok(long lcsp, char *sd_id, char *result)
{
  DbgWrite(DBG_INFO, "%s - OK\n", result);
  internal_send_user_message(lcsp, sd_id, result, "OK", "", "0x00000000");
}

/**
 * Send back a error message to the user
 * @param lcsp  pointer on csp context
 * @param sd_id SDID
 * @param result    result message to send to the user
 * @param error     error code
 */
void sms_send_user_error(long lcsp, char *sd_id, char *result, int error)
{
  LogWriteExt(LOG_ERROR, sd_id, "", " %s - %s - %s\n", result, get_error_message(error), get_error_code_str(error));
  internal_send_user_message(lcsp, sd_id, result, "FAIL", get_error_message(error), get_error_code_str(error));
}

/**
 * Send back a error message to the user
 * @param lcsp  pointer on csp context
 * @param sd_id SDID
 * @param result    result message to send to the user
 * @param error     error code
 */
void sms_send_user_error_ex(long lcsp, char *sd_id, char *result, int error)
{
  char *result_str;

  LogWriteExt(LOG_ERROR, sd_id, "", " %s - %s - %s\n", result, get_error_message(error), get_error_code_str(error));

  result_str = strdup(get_error_message(error));
  if (result!=NULL){
	result_str = gpul_str_append(result_str, ". ");
	result_str = gpul_str_append(result_str, result);
  }

  internal_send_user_message(lcsp, sd_id, result, "FAIL", result_str, get_error_code_str(error));

  freez(result_str);
}



/*
 * Send back a message to the user
 */
static void internal_send_user_message(long lcsp, char *sd_id, char *result, char *status, char *message, char *code)
{
  client_state_t *csp = (client_state_t *) lcsp;
  int ret;
  int len;
  char *s;
  char *result_str;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  if (!csp || csp->cfd < 1)
  {
    GLogINFO(log_handle, "Socket already closed. Cannot send message %s %s %s %30s", status, (message != NULL ? message : ""), (code != NULL ? code : ""), (result != NULL ? result : ""));
    return;
  }

  result_str = strdup("{\n");

  WRITE_FIRST_NAME("sms_status");
  WRITE_VALUE(status);

  if (result && result[0] != '\0')
  {
    if ((result[0] == '{') || (result[0] == '['))
    {
      WRITE_NAME("sms_result");
      result_str = gpul_str_append(result_str, result);
    }
    else
    {
      WRITE_NAME("sms_result");
      s = gpul_backslashify(result);
      WRITE_VALUE(s);
      free(s);
    }
  }

  if (message && message[0] != '\0')
  {
    WRITE_NAME("sms_message");
    s = gpul_backslashify(message);
    WRITE_VALUE(s);
    free(s);
  }

  if (strcmp(status, "OK"))
  {
    if (code && code[0] != '\0')
    {
      WRITE_NAME("sms_code");
      WRITE_VALUE(code);
    }
  }

  result_str = gpul_str_append(result_str, "\n}\n");

  WRITE(result_str);

  free(result_str);
}

/** Send back a message to the user.
 *
 @code
 {
 message
 }
 @endcode
 *
 * @param lcsp  pointer on csp context
 * @param sd_id SDID
 * @param user_message    message to send to the user
 */
void sms_reply_to_user(long lcsp, char *sd_id, char *user_message)
{
  client_state_t *csp = (client_state_t *) lcsp;
  int ret;
  int len;

  if (!csp || csp->cfd < 1 || !user_message || user_message[0] == '\0')
  {
    return;
  }

  WRITE(user_message);
}

/** Send back a message to the user.
 * Close the user message before sending back
 *
 @code
 {
 message
 }
 @endcode
 *
 * @param lcsp  pointer on csp context
 * @param sd_id SDID
 * @param user_message    message to send to the user
 */
void sms_send_user_message(long lcsp, char *sd_id, char *user_message)
{
  client_state_t *csp = (client_state_t *) lcsp;
  char *msg = NULL;
  int ret;
  int len;

  if (!csp || csp->cfd < 1)
  {
    return;
  }

  msg = gpul_str_append(msg, "{\n");
  msg = gpul_str_append(msg, user_message);
  msg = gpul_str_append(msg, "\n}\n");

  WRITE(msg);
  free(msg);
}

/**
 * Add a name:value pair to a message
 *
 @code
 message,
 "name" : "value"
 @endcode
 * @param message current message <br> message can be NULL or empty string but MUST be allocated, it will be released by the function
 * @param name name of the value
 * @param value value to add
 *
 * @return the new message with value added
 *
 */
char *sms_user_message_add(char *message, char *name, char *value)
{
  char *msg = NULL;
  char *s;

  if (message && message[0] != '\0')
  {
    msg = gpul_str_append(msg, message);
    msg = gpul_str_append(msg, ",\n");
  }

  msg = gpul_str_append(msg, "\"");
  msg = gpul_str_append(msg, name);
  msg = gpul_str_append(msg, "\" : \"");
  s = gpul_backslashify(value);
  msg = gpul_str_append(msg, s);
  free(s);
  msg = gpul_str_append(msg, "\"");

  return msg;
}

/**
 * Add a json object to a message
 *
 @code
 message,
 "name" : { value }
 @endcode
 * @param message current message <br> message can be NULL or empty string but MUST be allocated, it will be released by the function
 * @param name name of the json object
 * @param value json object to add
 *
 * @return the new message with value added
 *
 */
char *sms_user_message_add_json(char *message, char *name, char *value)
{
  char *msg = NULL;

  if (message && message[0] != '\0')
  {
    msg = gpul_str_append(msg, message);
    msg = gpul_str_append(msg, ",\n");
  }

  msg = gpul_str_append(msg, "\"");
  msg = gpul_str_append(msg, name);
  msg = gpul_str_append(msg, "\" : {\n");
  msg = gpul_str_append(msg, value);
  msg = gpul_str_append(msg, "\n}");

  return msg;
}

/** Add an array to a message
 *
 @code
 message,
 "name" : [
 array
 ]
 @endcode
 *
 * @param message current message <br> message can be NULL or empty string but MUST be allocated, it will be released by the function
 * @param name name of the array
 * @param array array to add
 *
 * @return the new message with array added
 *
 */
char *sms_user_message_add_array(char *message, char *name, char *array)
{
  char *msg = NULL;

  if (message && message[0] != '\0')
  {
    msg = gpul_str_append(msg, message);
    msg = gpul_str_append(msg, ",\n");
  }

  msg = gpul_str_append(msg, "\"");
  msg = gpul_str_append(msg, name);
  msg = gpul_str_append(msg, "\" : [\n");
  msg = gpul_str_append(msg, array);
  msg = gpul_str_append(msg, "\n]");

  return msg;
}

/** Add a row to an array
 *
 @code
 array,
 {
 row
 }
 @endcode
 *
 * @param array current array <br> array can be NULL or empty string but MUST be allocated, it will be released by the function
 * @param row a message containing "name:value" pairs
 *
 * @return the new array with row added
 *
 */
char *sms_user_message_array_add(char *array, char *row)
{
  char *arr = NULL;

  if (array && array[0] != '\0')
  {
    arr = gpul_str_append(arr, array);
    arr = gpul_str_append(arr, ",\n");
  }

  arr = gpul_str_append(arr, "{\n");
  arr = gpul_str_append(arr, row);
  arr = gpul_str_append(arr, "\n}");

  return arr;
}

/** Close the user socket
 *  Used for asynchronous commands
 * @param lcsp  pointer on csp context
 */
void sms_close_user_socket(long lcsp)
{
  client_state_t *csp = (client_state_t *) lcsp;

  socketClose(csp->cfd);
  csp->cfd = -1;
}

/**
 * @}
 */

/**
 * @addtogroup phpapimisc
 * @{
 */

/** Set the status of an update of an equipement
 * @param lcsp  pointer on csp context
 * @param sd_id SDID
 * @param error_code    error code
 * @param type type of update (LICENSE, FIRMWARE)
 * @param status status of the update
 *                      @arg 'ENDED'         Ends OK
 *                      @arg 'FAILED'        Ends ERROR
 *                      @arg 'WORKING'       Update in progress
 *                      @arg 'NOTPROCESSED'  Not processed
 * @param error_msg   msg to display instead of matching it with error_code
 * @return 0 if OK<br>
 *       != 0 if error
 */
int sms_set_update_status(long lcsp, char *sd_id, int error_code, char *type, char *status, char *error_msg)
{
  client_state_t *csp = (client_state_t *) lcsp;
  int ret = SMS_OK;
  char cli_prefix[4];
  long seqnum;
  int sev;
  void *log_handle;

  log_handle = gpul_sched_get_my_log_handle(sched_handle);

  GLogDEBUG(log_handle, 15, "UPDATE STATUS %s %s %s  %s (%s) %s", sd_id, type, status, get_error_message(error_code), get_error_code_str(error_code), error_msg);

  if (!csp || !sd_id || !type || !status || !error_msg)
  {
    return ERR_SD_FAILED;
  }

  if (strncmp(status, "WORKING", sizeof("WORKING") - 1) == 0)
  {
    ret = set_sd_updated(sd_id, csp, 0);
  }
  else if (strncmp(status, "NOTPROCESSED", sizeof("NOTPROCESSED") - 1) == 0)
  {
    ret = set_sd_updated(sd_id, csp, 0);
  }
  else if ((strncmp(status, "ENDED", sizeof("ENDED") - 1) == 0) || (strncmp(status, "FAILED", sizeof("FAILED") - 1) == 0))
  {
    if (error_msg[0] == '\0')
    {
      error_msg = get_error_message(error_code);
    }
    if (status[0] == 'F')
    {
      sev = currentConfig->alarm_update_fail;
    }
    else
    {
      sev = currentConfig->alarm_update_ok;
    }
    memcpy(cli_prefix, sd_id, 3);
    cli_prefix[3] = '\0';
    seqnum = strtol(sd_id + 3, NULL, 0);
    if (sev > 0)
    {
      set_sd_alarm(log_handle, cli_prefix, seqnum, sev, type, "%s", error_msg);
     }
  }

  if (ret)
  {
    GLogERROR(log_handle, "set_sd_updated returned %s (%s)", get_error_message(ret), get_error_code_str(ret));
    return ret;
  }

  return setUpdateStatus(sd_id, csp, type, status, error_msg);
}

/**
 * @}
 */
