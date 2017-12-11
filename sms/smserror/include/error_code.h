/**
 * Copyright UBIqube Solutions 2008
 * @file error_code.h
 * Creation Date: Jun 16, 2008
 */


#ifndef ERROR_CODE_H_
#define ERROR_CODE_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "sms_error.h"

typedef struct error_code
{
  int id;
  char *id_str;
  char *message;
  char *code_str;
  unsigned int code_num;
} error_code_t;

extern error_code_t error_code_tab[];
extern char *get_error_code_str(int error_id);
extern char *get_error_id_str(int error_id);
extern char *get_error_message(int error_id);
extern unsigned int get_error_code_num(int error_id);
extern char *get_error(int error_id, char *buf, int len);
extern void log_error_code(int error_id);
extern void log_error_code2(char *sd_cli_prefix, long sd_seqnum, int error_id, char *event);

#ifdef __cplusplus
}
#endif

#endif /*ERROR_CODE_H_*/
