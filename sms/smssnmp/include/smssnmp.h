/*
 * @file smssnmp.h
 *	Copyright UBIqube Solutions 2009
 *  Created on: Sep 1, 2009
 */

#ifndef SMSSNMP_H_
#define SMSSNMP_H_

#ifdef __cplusplus
extern "C"{
#endif

#define OID_SEC_ENGINE      "1.3.6.1.4.1.13567.3"

#define SNMP_OID_MAX 127

typedef struct snmp_type
{
  char *type_name;
  int  is_string;
} snmp_type_t;

#define SNMP_NONE           0
#define SNMP_COUNTER        1
#define SNMP_STRING         2
#define SNMP_GAUGE          3
#define SNMP_INTEGER        4

typedef struct snmp_counters
{
  char oid[SNMP_OID_MAX];
  int type;
  unsigned int value;
  char *valstr;
  GPUL_TAILQ_ENTRY (struct snmp_counters) link;
} snmp_counters_t;

GPUL_TAILQ_HEAD(snmp_counters_list_head, snmp_counters_t);
extern struct snmp_counters_list_head snmp_counters_list;
extern void *snmp_counters_hash_table;

extern int sms_snmp_init(void *log_handle);
extern int doSnmpGet(char *oid, int sfd, void *userdata);
extern int doSnmpGetNext(char *oid, int sfd, void *userdata);

#ifdef __cplusplus
}
#endif

#endif /* SMSSNMP_H_ */
