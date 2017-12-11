/*
 * @file sms_php_functions_db.h
 *	Copyright UBIqube Solutions 2009
 *  Created on: Nov 10, 2009
 */

#ifndef SMS_PHP_FUNCTIONS_DB_H_
#define SMS_PHP_FUNCTIONS_DB_H_

extern int sms_store_in_database(long ldb_ctx, char *table_name, zval *lvalues, zval *ldbkeys);
extern int sms_delete_in_database(long ldb_ctx, char *table_name, zval *ldbkeys);
extern long sms_find_db_context(char *database);
extern void sms_release_db_context(long ldb_ctx);

#endif /* SMS_PHP_FUNCTIONS_DB_H_ */
