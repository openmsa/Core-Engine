/*
 * Copyright UBIqube Solutions 2007
 * File: sms_php_wrappers.h
 * Creation Date: Thu Aug 23 15:14:22 UTC 2007
 */

#ifndef _SMS_PHP_WRAPPERS_H_
#define _SMS_PHP_WRAPPERS_H_

// char *f(int)
#define PHP_s_i(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &a) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    ret = fname(a);\
    if (ret)\
    {\
      RETURN_STRING(ret, 1);\
    }\
    RETURN_NULL();\
  }

// char *f(int, int)
#define PHP_s_ii(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    char *ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &a, &b) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    ret = fname(a, b);\
    if (ret)\
    {\
      RETURN_STRING(ret, 1);\
    }\
    RETURN_NULL();\
  }

// char *f(void)
#define PHP_s_v(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    char *ret;\
    ret = fname();\
    if (ret)\
    {\
      RETURN_STRING(ret, 1);\
    }\
    RETURN_NULL();\
  }

// char *f(int)
// with return buffer deallocated
#define PHP_sf_i(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &a) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    ret = fname(a);\
    if (ret)\
    {\
      RETVAL_STRING(ret, 1);\
      free(ret);\
      return;\
    }\
    RETURN_NULL();\
  }

// char *f(int, char*)
// with return buffer deallocated
#define PHP_sf_is(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    char *ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &a, &b, &b_len) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    ret = fname(a, b);\
    if (ret)\
    {\
      RETVAL_STRING(ret, 1);\
      free(ret);\
      return;\
    }\
    RETURN_NULL();\
  }

// char *f(char*)
// with return buffer deallocated
#define PHP_sf_s(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    char *a;\
    long a_len;\
    char *ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &a, &a_len) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    ret = fname(a);\
    if (ret)\
    {\
      RETVAL_STRING(ret, 1);\
      free(ret);\
      return;\
    }\
    RETURN_NULL();\
  }

// char *f(int)
#define PHP_s_iis(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    char *c;\
    long c_len;\
    char *ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lls", &a, &b, &c, &c_len) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    ret = fname(a, b, c);\
    if (ret)\
    {\
      RETURN_STRING(ret, 1);\
    }\
    RETURN_NULL();\
  }

// char *f(int, int)
// with return buffer deallocated
#define PHP_sf_ii(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    char *result;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &a, &b) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    result = fname(a, b);\
    if (result)\
    {\
      RETVAL_STRING(result, 1);\
      free(result);\
      return;\
    }\
    RETURN_NULL();\
  }

// char *f(int, int, char *, char *)
// with return buffer deallocated
#define PHP_sf_iiss(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    char *c;\
    long c_len;\
    char *d;\
    long d_len;\
    char *result;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llss", &a, &b, &c, &c_len, &d, &d_len) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    result = fname(a, b, c, d);\
    if (result)\
    {\
      RETVAL_STRING(result, 1);\
      free(result);\
      return;\
    }\
    RETURN_NULL();\
  }

#define PHP_v_iisssss(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    char *c;\
    long c_len;\
    char *d;\
    long d_len;\
    char *e;\
    long e_len;\
    char *f;\
    long f_len;\
    char *g;\
    long g_len;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llsssss", &a, &b, &c, &c_len, &d, &d_len, &e, &e_len, &f, &f_len, &g, &g_len) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b, c, d, e, f, g);\
  }

#define PHP_v_iss(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    char *c;\
    long c_len;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lss", &a, &b, &b_len, &c, &c_len) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b, c);\
  }

#define PHP_v_i(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &a) == FAILURE)\
    {\
      return;\
    }\
    fname(a);\
  }

#define PHP_v_ii(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &a, &b) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b);\
  }

#define PHP_v_issi(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    char *c;\
    long c_len;\
    long d;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lssl", &a, &b, &b_len, &c, &c_len, &d) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b, c, d);\
  }

#define PHP_v_isss(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    char *c;\
    long c_len;\
    char *d;\
    long d_len;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lsss", &a, &b, &b_len, &c, &c_len, &d, &d_len) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b, c, d);\
  }

#define PHP_v_issssss(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    char *c;\
    long c_len;\
    char *d;\
    long d_len;\
    char *e;\
    long e_len;\
    char *f;\
    long f_len;\
    char *g;\
    long g_len;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lssssss", &a, &b, &b_len, &c, &c_len, &d, &d_len, &e, &e_len, &f, &f_len, &g, &g_len) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b, c, d, e, f, g);\
  }

#define PHP_v_isssss(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    char *c;\
    long c_len;\
    char *d;\
    long d_len;\
    char *e;\
    long e_len;\
    char *f;\
    long f_len;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lsssss", &a, &b, &b_len, &c, &c_len, &d, &d_len, &e, &e_len, &f, &f_len) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b, c, d, e, f);\
  }

#define PHP_v_issss(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    char *c;\
    long c_len;\
    char *d;\
    long d_len;\
    char *e;\
    long e_len;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lssss", &a, &b, &b_len, &c, &c_len, &d, &d_len, &e, &e_len) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b, c, d, e);\
  }

#define PHP_v_isis(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    long c;\
    char *d;\
    long d_len;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lsls", &a, &b, &b_len, &c, &d, &d_len) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b, c, d);\
  }

#define PHP_v_isiss(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    long c;\
    char *d;\
    long d_len;\
    char *e;\
    long e_len;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lslss", &a, &b, &b_len, &c, &d, &d_len, &e, &e_len) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b, c, d, e);\
  }

#define PHP_v_isisss(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    long c;\
    char *d;\
    long d_len;\
    char *e;\
    long e_len;\
    char *f;\
    long f_len;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lslsss", &a, &b, &b_len, &c, &d, &d_len, &e, &e_len, &f, &f_len) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b, c, d, e, f);\
  }

#define PHP_v_isissss(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    long c;\
    char *d;\
    long d_len;\
    char *e;\
    long e_len;\
    char *f;\
    long f_len;\
    char *g;\
    long g_len;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lslssss", &a, &b, &b_len, &c, &d, &d_len, &e, &e_len, &f, &f_len,&g, &g_len) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b, c, d, e, f, g);\
  }

// int f(int, int, char **, char *)
#define PHP_i_iias(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    zval *d;\
    char *e;\
    long e_len;\
    int nb_elt;\
    int i;\
    int nb_str;\
    zval **var;\
    char **str_tab;\
    long ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llas", &a, &b, &d, &e, &e_len) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    nb_elt = zend_hash_num_elements(Z_ARRVAL_P(d));\
    str_tab = calloc(nb_elt, sizeof(char*));\
    nb_str = 0;\
    for (i = 0; i < nb_elt; i++)\
    {\
      ret = zend_hash_index_find(Z_ARRVAL_P(d), i, (void**)&var);\
      if (ret != FAILURE)\
      {\
        if (var && (Z_STRVAL_P(*var)))\
        {\
          str_tab[nb_str] = strdup(Z_STRVAL_P(*var));\
          nb_str++;\
        }\
      }\
    }\
    ret = fname(a, b, str_tab, nb_str, e);\
    for (i = 0; i < nb_str; i++)\
    {\
      free(str_tab[i]);\
    }\
    free(str_tab);\
    RETURN_LONG(ret);\
  }

// char *f(char *)
#define PHP_s_s(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    char *a;\
    long a_len;\
    char *ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &a, &a_len) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    ret = fname(a);\
    if (ret)\
    {\
      RETURN_STRING(ret, 1);\
    }\
    RETURN_NULL();\
  }

// char *f(char *, char *)
#define PHP_s_ss(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    char *a;\
    long a_len;\
    char *b;\
    long b_len;\
    char *ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &a, &a_len, &b, &b_len) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    ret = fname(a, b);\
    if (ret)\
    {\
      RETURN_STRING(ret, 1);\
    }\
    RETURN_NULL();\
  }

// char *f(char *, char *)
#define PHP_sf_ss(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    char *a;\
    long a_len;\
    char *b;\
    long b_len;\
    char *ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &a, &a_len, &b, &b_len) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    ret = fname(a, b);\
    if (ret)\
    {\
      RETVAL_STRING(ret, 1);\
      free(ret);\
      return;\
    }\
    RETURN_NULL();\
  }

// int f(char *)
#define PHP_i_s(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    char *a;\
    long a_len;\
    int ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &a, &a_len) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    ret = fname(a);\
    RETURN_LONG(ret);\
  }

// int f(char *, char *)
#define PHP_i_ss(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    char *a;\
    long a_len;\
    char *b;\
    long b_len;\
    int ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &a, &a_len, &b, &b_len) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    ret = fname(a, b);\
    RETURN_LONG(ret);\
  }

// int f(char *, char *, char *)
#define PHP_i_sss(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    char *a;\
    long a_len;\
    char *b;\
    long b_len;\
    char *c;\
    long c_len;\
    int ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &a, &a_len, &b, &b_len, &c, &c_len) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    ret = fname(a, b, c);\
    RETURN_LONG(ret);\
  }

// char *f(long, char *, char *)
#define PHP_sf_iss(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    char *c;\
    long c_len;\
    char *ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lss", &a, &b, &b_len, &c, &c_len) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    ret = fname(a, b, c);\
    if (ret)\
    {\
      RETVAL_STRING(ret, 1);\
      free(ret); \
      return;\
    }\
    RETURN_NULL();\
  }

// char *f(char *, char *, char *)
#define PHP_s_sss(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    char *a;\
    long a_len;\
    char *b;\
    long b_len;\
    char *c;\
    long c_len;\
    char *ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &a, &a_len, &b, &b_len, &c, &c_len) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    ret = fname(a, b, c);\
    if (ret)\
    {\
      RETURN_STRING(ret, 1);\
    }\
    RETURN_NULL();\
  }

// char *f(char *, char *, char *)
#define PHP_sf_sss(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    char *a;\
    long a_len;\
    char *b;\
    long b_len;\
    char *c;\
    long c_len;\
    char *ret;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &a, &a_len, &b, &b_len, &c, &c_len) == FAILURE)\
    {\
      RETURN_NULL();\
    }\
    ret = fname(a, b, c);\
    if (ret)\
    {\
      RETVAL_STRING(ret, 1);\
      free(ret);\
      return;\
    }\
    RETURN_NULL();\
  }

// int f(void)
#define PHP_i_v(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    RETURN_LONG(fname());\
  }

// int f(int)
#define PHP_i_i(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &a) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a));\
  }

// int f(int, int)
#define PHP_i_ii(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &a, &b) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a, b));\
  }

// int f(int, int, int)
#define PHP_i_iii(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    long c;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &a, &b, &c) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a, b, c));\
  }

// int f(int, int, int, int, int)
#define PHP_i_iiiii(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    long c;\
    long d;\
    long e;\
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lllll", &a, &b, &c, &d, &e) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a, b, c, d, e));\
  }

// int f(int, int, char *)
#define PHP_i_iis(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    char *d;\
    long d_len;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lls", &a, &b, &d, &d_len) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a, b, d));\
  }

// int f(int, int, int, char *)
#define PHP_i_iiis(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    long c;\
    char *d;\
    long d_len;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llls", &a, &b, &c, &d, &d_len) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a, b, c, d));\
  }

// int f(int, int, int, char *, int)
#define PHP_i_iiisi(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    long c;\
    char *d;\
    long d_len;\
    long e;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lllsl", &a, &b, &c, &d, &d_len, &e) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a, b, c, d, e));\
  }

// int f(int, int, int, char *, int, char *)
#define PHP_i_iiisis(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    long c;\
    char *d;\
    long d_len;\
    long e;\
    char *f;\
    long f_len;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lllsls", &a, &b, &c, &d, &d_len, &e, &f, &f_len) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a, b, c, d, e, f));\
  }

 // int f(int, int, int, char *, int, char *, char *)
#define PHP_i_iiisiss(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    long c;\
    char *d;\
    long d_len;\
    long e;\
    char *f;\
    long f_len;\
    char *g;\
    long g_len;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lllslss", &a, &b, &c, &d, &d_len, &e, &f, &f_len, &g, &g_len) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a, b, c, d, e, f, g));\
  }

// int f(int, char *)
#define PHP_i_is(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long s_len;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &a, &b, &s_len) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a,b));\
  }

// int f(int, char *, int)
#define PHP_i_isi(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long s_len;\
    long c;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lsl", &a, &b, &s_len, &c) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a,b,c));\
  }

// int f(int, char *, int, char *)
#define PHP_i_isis(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    long c;\
    char *d;\
    long d_len;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lsls", &a, &b, &b_len, &c, &d, &d_len) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a,b,c,d));\
  }

// int f(int, char *, char *)
#define PHP_i_iss(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    char *d;\
    long d_len;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lss", &a, &b, &b_len, &d, &d_len) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a,b,d));\
  }

// int f(int, int, char *, char *)
#define PHP_i_iiss(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    char *c;\
    long c_len;\
    char *d;\
    long d_len;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llss", &a, &b, &c, &c_len, &d, &d_len) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a,b,c,d));\
  }

// int f(char *, int, int)
#define PHP_i_sii(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    char *a;\
    long a_len;\
    long b;\
    long c;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sll", &a, &a_len, &b, &c) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a,b,c));\
  }

// int f(char *, int, int, int, int, char *, char *, char *, int)
#define PHP_i_siiiisssi(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    char *a;\
    long a_len;\
    long b;\
    long c;\
    long d;\
    long e;\
    char *f;\
    long f_len;\
    char *g;\
    long g_len;\
    char *h;\
    long h_len;\
    long i;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sllllsssl", &a, &a_len, &b, &c, &d, &e, &f, &f_len, &g, &g_len, &h, &h_len, &i) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a,b,c,d,e,f,g,h,i));\
  }

// int f(int, zval *)
#define PHP_i_ih(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    zval *b;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "la", &a, &b) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a,b));\
  }

// int f(int, int, int, zval *)
#define PHP_i_iiih(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    long c;\
    zval *d;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llla", &a, &b, &c, &d) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a,b, c, d));\
  }

// int f(int, int, zval *)
#define PHP_i_iih(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    long b;\
    zval *c;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lla", &a, &b, &c) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a,b,c));\
  }

// int f(int, char *, zval *)
#define PHP_i_ish(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    zval *c;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lsa", &a, &b, &b_len, &c) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a, b, c));\
  }

// int f(int, char *, zval *, zval *)
#define PHP_i_ishh(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    zval *c;\
    zval *d;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lsaa", &a, &b, &b_len, &c, &d) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a, b, c, d));\
  }

// int f(zval *)
#define PHP_i_h(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    zval *a;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &a) == FAILURE)\
    {\
      RETURN_LONG(-1);\
    }\
    RETURN_LONG(fname(a));\
  }

// void f(char *)
#define PHP_v_s(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    char *b;\
    long s_len;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &b, &s_len) == FAILURE)\
    {\
      return;\
    }\
    fname(b);\
  }

// void f(int, char *)
#define PHP_v_is(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &a, &b, &b_len) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b);\
  }

// int f(char *)
#define PHP_b_s(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    char *b;\
    long s_len;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &b, &s_len) == FAILURE)\
    {\
      RETURN_FALSE;\
    }\
    if (fname(b))\
    {\
      RETURN_TRUE;\
    }\
    RETURN_FALSE;\
  }

// int f(int)
#define PHP_b_i(fname)\
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &a) == FAILURE)\
    {\
      RETURN_FALSE;\
    }\
    if (fname(a))\
    {\
      RETURN_TRUE;\
    }\
    RETURN_FALSE;\
  }

#define PHP_v_issb(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    char *c;\
    long c_len;\
    zend_bool d = 0;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lssb", &a, &b, &b_len, &c, &c_len, &d) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b, c, (d ? 1 : 0));\
  }

#define PHP_v_isssssb(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    char *c;\
    long c_len;\
    char *d;\
    long d_len;\
    char *e;\
    long e_len;\
    char *f;\
    long f_len;\
    zend_bool g = 0;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lsssssb", &a, &b, &b_len, &c, &c_len, &d, &d_len, &e, &e_len, &f, &f_len, &g) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b, c, d, e, f, (g ? 1 : 0));\
  }

#define PHP_v_issssb(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    char *c;\
    long c_len;\
    char *d;\
    long d_len;\
    char *e;\
    long e_len;\
    zend_bool f = 0;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lssssb", &a, &b, &b_len, &c, &c_len, &d, &d_len, &e, &e_len, &f) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b, c, d, e, (f ? 1 : 0));\
  }

#define PHP_v_isssb(fname) \
  static ZEND_FUNCTION(fname);\
  static ZEND_FUNCTION(fname)\
  {\
    long a;\
    char *b;\
    long b_len;\
    char *c;\
    long c_len;\
    char *d;\
    long d_len;\
    zend_bool e = 0;\
   if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lsssb", &a, &b, &b_len, &c, &c_len, &d, &d_len, &e) == FAILURE)\
    {\
      return;\
    }\
    fname(a, b, c, d, (e ? 1 : 0));\
  }


#endif /* _SMS_PHP_WRAPPERS_H_ */
