
#include "log.h"
#include "smsd.h"
#include "sms_db.h"
#include "router.h"
#include "php_sdinfo.h"


/* DEPRECATED - Get the configuration step
 * @param  lsdinfo  pointer on SD info ($sms_sd_info)
 *
 */
int sms_sd_get_config_mode_from_sdinfo(long lsdinfo)
{
  sd_info_t *sd_info = (sd_info_t *) lsdinfo;
  if (sd_info == NULL)
    {
      LogWrite (LOG_ERROR, " sms_sd_get_config_mode_from_sdinfo: failed sd_info is null\n");
      return 0;
    }
  return sd_info->SD.sd_config_step;
}
