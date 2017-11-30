/** @file pattern.h
  This file contains the include file of the module used to
  provide functions on pattern matching on the configuration file
  @ingroup  smsd

  */

#ifndef __PATTERN_H_
#define __PATTERN_H_
#include "router.h"

int resolve_template(client_state_t *csp, char *SDid, char *path);

int resolve_template_buffer(client_state_t *csp, char *SDid, char *template_buffer);

#endif  /*__PATTERN_H_*/
