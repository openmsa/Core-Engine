/** @file verb.h
  This is the header file of the verb module of the smsd daemon.
  @ingroup  smsd
  */

#ifndef __VERB_H_
#define __VERB_H_

#include "smsd.h"

#define IPUPDOWN_LOG_LEVEL 1
#define IPUP_LOG "%%VNOC-%d-IPUP: IPUP for %s router %s%lu. The address is %s\n"
#define IPUP_LOG_REFERENCE "VNOC-%d-IPUP"
#define IPDOWN_LOG "%%VNOC-%d-IPDOWN: IPDOWN for router %s%lu.\n"
#define IPDOWN_LOG_REFERENCE "VNOC-%d-IPDOWN"

#define IPUP_IP_DYN_LOG "%%VNOC-%d-IPUP: IPUP for router %s%lu with dynamic ip address. Old address %s, new address %s\n"

int VerbInit (void);

void VerbEnd (void);

void CheckVerb(char *buf, struct client_state *csp);

int send_ok (char *SDid, client_state_t *csp, char *verb, char *msg, int len);

int send_error (char *SDid, client_state_t *csp, char *verb, char *msg, int len, int error);

int parse_params(char *params, int nb_params, char **results);


#endif    /* __VERB_H_ */
