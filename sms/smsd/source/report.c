/** @file report.c
  @ingroup  smsd
 */

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include "log.h"
#include "net.h"
#include "smsd.h"
#include "verb.h"
#include "router.h"
#include "report.h"
#include "generic_router.h"

int getReport(client_state_t *csp, char *SDid, char **configuration, char *event)
{
  sd_info_t sd_info;
  char	*p;
  uint startDate = 0;
  uint endDate = 0;
  int len;
  int ret;

  len = strlen(SDid);
  p = strchr(SDid, ' ');
  if (p)
    {
      *p = '\0';
      p++;
      if ((p - SDid) < len)
        {
          sscanf(p, "%d %d", &startDate, &endDate);
        }
    }

  LogWrite(LOG_INFO, " Getting report for %s %u %u...\n", SDid, startDate, endDate);

  ret = getSDinfo (csp->db_ctx, SDid, &sd_info);
  if (ret)
    {
      LogWriteExt (LOG_ERROR, SDid, event, " Cannot get sd info\n");
      return ret;
    }

  /* release its db ctx */
  RELEASE_BD_CTX(csp) ;

  ret = genericGetReport(&sd_info, configuration, TRUE, startDate, endDate);
  if (ret)
    {
      LogWrite(LOG_ERROR, " Cannot get report the SD SDid=%s, ret:%d\n", SDid, ret);
    }

  freeSDinfo (&sd_info);
  return ret;
}



int getVPNReport(client_state_t *csp, char *SDid, char **configuration, char *event)
{
  sd_info_t sd_info;
  int ret;

  LogWrite(LOG_INFO, " Getting report for %s ...\n", SDid);

  ret = getSDinfo (csp->db_ctx, SDid, &sd_info);
  if (ret)
    {
      LogWriteExt (LOG_ERROR, SDid, event, " Cannot get sd info\n");
      return ret;
    }

  /* release its db ctx */
  RELEASE_BD_CTX(csp) ;

  /* now need to get the router's report */
  ret = genericGetReport(&sd_info, configuration, FALSE, 0, 0);
  if (ret)
    {
      LogWrite(LOG_ERROR, " Cannot get report the SD SDid=%s, ret:%d\n", SDid, ret);
    }

  freeSDinfo (&sd_info);
  return ret;
}
