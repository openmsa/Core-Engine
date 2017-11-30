/** @file report.h
  This is the header file of the verb module of the smsd daemon.
  @ingroup  smsd
  */

#ifndef __REPORT_H_
#define __REPORT_H_

#include "smsd.h"


/** get a report from a selected SDID
    @param	SDid of the SD (must be a static SD or with known ip address)
    @param      csp : the context of the connection
    @return	0 if the function is successful;
 */
int getReport(client_state_t *csp, char *SDid, char **configuration, char *event);

/** get a report from a selected SDID
    @param	SDid of the SD (must be a static SD or with known ip address)
    @param      csp : the context of the connection
    @return	0 if the function is successful;
 */
int getVPNReport(client_state_t *csp, char *SDid, char **configuration, char *event);


#endif    /* __REPORT_H_ */


