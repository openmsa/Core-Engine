/** @file router.h
  This file contains the include for all the supported router
  @ingroup  smsd

 */

#ifndef __ROUTER_H_
#define __ROUTER_H_

#include "sms_db.h"
#include "gpul.h"


struct head_list
{
  GPUL_TAILQ_ENTRY(struct head_list) link;
};
typedef struct head_list head_list_t;
GPUL_TAILQ_HEAD (head_list_head, head_list_t);

struct router_cmd
{
  GPUL_TAILQ_ENTRY(struct router_cmd) link;
  char *cmd;
};
typedef struct router_cmd router_cmd_t;
GPUL_TAILQ_HEAD (router_cmd_head, router_cmd_t);

/** define the maximum size of a router configuration file in bytes*/
#define MAXCONFSIZE 500000

/** Address format type */
#define TYPE_IPV4_ADDR		1
#define TYPE_IPV4_ADDR_SUBNET	2
#define TYPE_FQDN		3

/** definition of the type containing information about the configuration
    function of each router . This structure is used to call any configuration
    module in the same way */


#define      IPCHANGED          955

/* Mise a jour ou pas */
#define NO_MAJ          0x00000000
#define MAJ_CONF        0X10000000

/** a structure to contain the peers to test from a given
    router */
typedef struct peerPingList
{
  /** The Peer VPN Address */
  char *Peer_Address;
  /** The Peer backup (hsrp) VPN Address */
  char *Peer_Backup_Address;
  /** Internal Ip  address to reach to test the Tunnel*/
  char *Peer_Internal_Address;
  /** The SDid of the source router */
  char SDidsrc[MAXHOSTSIZE];
  /** The SDid of the destination router */
  char SDiddst[MAXHOSTSIZE];
  /** The SDid of the backup destination router */
  char SDiddst_backup[MAXHOSTSIZE];
  /** The result of the ping */
  int status;
  struct peerPingList * next ;
} peerPingList_t;

/** definition of codes for the check status of peers and VPNs */
/* mettre a jour status.c:status_vpn_tab si modif */
enum status_vpn_id
{
  STATUS_NOTREACHABLE,
  STATUS_REACHABLE_BUT_NO_TUNNEL,
  STATUS_REACHABLE_BACKUP_BUT_NO_TUNNEL,
  STATUS_REACHABLE_TUNNEL_OK,
  STATUS_REACHABLE,
  STATUS_REACHABLE_BACKUP,
  STATUS_NOT_APPLICABLE
};

#define STATUS_MIN STATUS_NOTREACHABLE
#define STATUS_MAX STATUS_NOT_APPLICABLE


#define INVERT_MASK(_ip_mask) {						\
  struct in_addr sin_addr;						\
  \
  inet_pton(AF_INET, (_ip_mask), &sin_addr);			\
  sin_addr.s_addr ^= 0xFFFFFFFF;					\
  inet_ntop(AF_INET, &sin_addr, (_ip_mask), INET_ADDRSTRLEN);	\
}


#define ISSTATICSD(_ext_interface,_var)\
  (_var) = ((_ext_interface) && *((_ext_interface)->interface.int_ip_type) == 'S') ? TRUE : FALSE;


/* mot clé pour rebooter les SD (verbes SMSEXEC/SMSEXECN) */
#define SD_REBOOT "SD_REBOOT"

#define ROUTER_DOMAIN "ubiqube.net"

/* contains only the cisco router for now */
/*#include "cisco.h"*/

int routerGetIPaddress(sd_like_t * SD, sd_interface_t *sd_interface, char *ip, char *event);

int initListRouterToUpdate (void);
void *listRouterToUpdate (void *unused);

int sdGetAsset(client_state_t *csp, char *SDid, char *result, char *event);

int sdForceAsset(client_state_t *csp, char *SDid);

int sdSetConfigMode(client_state_t *csp, char *SDid, int cfg_mode, char *result, char *event);

int getSDinfo (database_context_t *sms_db_ctx, char *SDid, sd_info_t *sd_info);

void freeSDinfo(sd_info_t *sd_info);

void freeSDinfo (sd_info_t *sd_info);

int getAllInSD(sql_context * ctx, sd_info_t * sd_info, char *event);

void freeAllInSD( sd_info_t * sd_info);

int getRunningConf(client_state_t *csp, char *SDid, char *event);
int getConfig(client_state_t *csp, char *SDid, char *event);

int getActivityReport(client_state_t *csp, char *SDid, char *event);

int save_running_conf(sd_info_t *SDinfo, char *str_flag_update, int *version_changed);

int unarchive_conf(char *params, client_state_t *csp);

int get_archive_conf(char *params, client_state_t *csp);

int update_license(char *params, client_state_t *csp);
int update_firmware(char *params, client_state_t *csp);

int call_command(char *params, client_state_t *csp);
int send_data_files(char *params, client_state_t *csp);

int check_serial(char *params, client_state_t *csp);

int save_all_conf(void *dummy);

#endif /* __ROUTER_H_ */
