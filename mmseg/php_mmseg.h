
#ifndef PHP_MMSEG_H
#define PHP_MMSEG_H

extern zend_module_entry mmseg_module_entry;
#define phpext_mmseg_ptr &mmseg_module_entry

#ifdef PHP_WIN32
#	define PHP_MMSEG_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_MMSEG_API __attribute__ ((visibility("default")))
#else
#	define PHP_MMSEG_API
#endif



extern "C" {
    #ifdef ZTS
    #include "TSRM.h"
    #endif
}

#include <fstream>
#include <string>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <map>
#include <stdlib.h>
#include <sys/stat.h>

#include "SegmenterManager.h"
#include "Segmenter.h"
#include "csr_utils.h"

using namespace css;

#define PHP_MMSEG_DESCRIPTOR_RES_NAME "mmseg segmenter manager resource"

PHP_MINIT_FUNCTION(mmseg);
PHP_MSHUTDOWN_FUNCTION(mmseg);
PHP_RINIT_FUNCTION(mmseg);
PHP_RSHUTDOWN_FUNCTION(mmseg);
PHP_MINFO_FUNCTION(mmseg);

// 使用全局的mmseg数据进行分词
PHP_FUNCTION(mmseg_segment);

// 初始化mmseg配置，或者mmseg句柄
PHP_FUNCTION(mmseg_open);
// 关闭句柄，释放空间
PHP_FUNCTION(mmseg_close);
// 字典生成
PHP_FUNCTION(mmseg_gendict);
// 生成特殊短语字典
PHP_FUNCTION(mmseg_gensynonyms);
// 生成同义词词典
PHP_FUNCTION(mmseg_genthesaurus);

ZEND_BEGIN_MODULE_GLOBALS(mmseg)
    void*       mgr; /* (SegmenterManager*) */  
    time_t      dict_mtime;
ZEND_END_MODULE_GLOBALS(mmseg)

/* In every utility function you add that needs to use variables 
   in php_mmseg_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as MMSEG_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define MMSEG_G(v) TSRMG(mmseg_globals_id, zend_mmseg_globals *, v)
#else
#define MMSEG_G(v) (mmseg_globals.v)
#endif

#ifdef MMSEG_DEBUG
#define MMSEG_LOG(w) php_error(E_WARNING, w)
#else
#define MMSEG_LOG(w) ;
#endif

#endif	/* PHP_MMSEG_H */

