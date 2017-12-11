#################################################################
#
# file       : Open Core Engine Makefile
# Created on : Dec 11, 2017
# Copyright (c) UBIqube 2017
# 
#################################################################


ifndef SMS_BASE

SMS_BASE = $(shell pwd)
MAKE_DEFAULT = ${SMS_BASE}/Make-default.mk

restart:
	${MAKE} SMS_BASE=${SMS_BASE} MAKE_DEFAULT=${MAKE_DEFAULT} 

endif

ifndef MAKE_DEFAULT

MAKE_DEFAULT = ${SMS_BASE}/Make-default.mk

restart2:
	${MAKE} SMS_BASE=${SMS_BASE} MAKE_DEFAULT=${MAKE_DEFAULT}

endif

SB_ROOT = ${SMS_BASE}

SUBDIRS = smsd

-include ${MAKE_DEFAULT}
