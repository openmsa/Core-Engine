#################################################################
#
# file       : Makefile
# Created on : Aug 14, 2008
# Copyright (c) UBIqube Solutions 2008
# 
#################################################################

#SUBDIRS = test

INCLUDES = smsrouter net log smsbd rad gpul smsphp smserror smssnmp

LIBS = net log smsbd smsrouter gpul smsphp smserror smssnmp

USE_ODBC = yes

DAEMON = smsd

SOURCES = $(shell find source -name '*.c' -type f -print)

USER_LDFLAGS = $(PHP_LDFLAGS)

PHP_SOURCES = $(shell find php -name '*.php' -type f -print)

include ${MAKE_DEFAULT}
