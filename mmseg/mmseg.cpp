
extern "C" {
    #ifdef HAVE_CONFIG_H
    #include "config.h"
    #endif
    
    #include "php.h"
    #include "php_ini.h"
    #include "ext/standard/info.h"
}

#include "php_mmseg.h"

// SegmenterManager
ZEND_DECLARE_MODULE_GLOBALS(mmseg)

/* True global resources - no need for thread safety here */
static int le_mmseg;

/* {{{ mmseg_functions[]
 *
 * Every user visible function must have an entry in mmseg_functions[].
 */
const zend_function_entry mmseg_functions[] = {
	PHP_FE(confirm_mmseg_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(mmseg_segment,	NULL)
	PHP_FE_END	/* Must be the last line in mmseg_functions[] */
};
/* }}} */

/* {{{ mmseg_module_entry
 */
zend_module_entry mmseg_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"mmseg",
	mmseg_functions,
	PHP_MINIT(mmseg),
	PHP_MSHUTDOWN(mmseg),
	PHP_RINIT(mmseg),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(mmseg),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(mmseg),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MMSEG
BEGIN_EXTERN_C()
ZEND_GET_MODULE(mmseg)
END_EXTERN_C()
#endif

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    // setting the path of the dictionary
    PHP_INI_ENTRY("mmseg.dict_dir", "/opt/etc", PHP_INI_ALL, NULL)
PHP_INI_END()
/* }}} */

/* {{{ php_mmseg_init_globals
 */
/*
static void php_mmseg_init_globals(zend_mmseg_globals *mmseg_globals)
{
	mmseg_globals->dict_dir = NULL;
}
*/
/* }}} */

/* Triggered at the beginning of a thread */
static void php_mmseg_globals_ctor(zend_mmseg_globals *mmseg_globals TSRMLS_DC)
{
    SegmenterManager* mgr = (SegmenterManager*) mmseg_globals->mgr;
    mgr = new SegmenterManager();
    int nRet = 0;
    nRet = mgr->init(INI_STR("mmseg.dict_dir"));
    if (nRet == 0) {
	    return ;
    } else {
        if (mgr != NULL)  {
            delete mgr;
        }
        return ;
    }
}

/* Triggered at the end of a thread */
static void php_mmseg_globals_dtor(zend_mmseg_globals *mmseg_globals TSRMLS_DC)
{
    SegmenterManager* mgr = (SegmenterManager*) mmseg_globals->mgr;
    if (mgr != NULL)  {
        delete mgr;
    }
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mmseg)
{
	REGISTER_INI_ENTRIES();
    
    // 初始化，如果初始化失败，则返回失败信息，(XXX: 未来将改变为如果初始化失败, 可以在程序里面调用init初始化)
#ifdef ZTS
    ts_allocate_id(&mmseg_globals_id, sizeof(zend_mmseg_globals),
                   php_mmseg_globals_ctor, php_mmseg_globals_dtor);
#else
    php_mmseg_globals_ctor(&mmseg_globals TSRMLS_CC);
#endif

    return SUCCESS;
}

/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(mmseg)
{
	UNREGISTER_INI_ENTRIES();
#ifndef ZTS
    php_mmseg_globals_dtor(&mmseg_globals TSRMLS_CC);
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(mmseg)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(mmseg)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mmseg)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "mmseg support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_mmseg_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_mmseg_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "mmseg", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

/* {{{ proto array mmseg_segment(string content)
    */
PHP_FUNCTION(mmseg_segment)
{
	char *content = NULL;
	int argc = ZEND_NUM_ARGS();
	int content_len;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &content, &content_len) == FAILURE) 
		return;

    SegmenterManager* mgr =  NULL;
    mgr = (SegmenterManager*) & MMSEG_G(mgr);
    if (mgr == NULL) {
        RETURN_NULL();
    }

    Segmenter* seg = mgr->getSegmenter();

    //seg->setBuffer((u1*)content, (u4)content_len);

    array_init(return_value);
    //while(1)
    //{
    //    u2 len = 0, symlen = 0;
    //    char* tok = (char*)seg->peekToken(len,symlen);
    //    if(!tok || !*tok || !len){
    //        break;
    //    }
    //    //append new item
    //    add_next_index_stringl(return_value, tok, len, 1);
    //    seg->popToken(len);
    //}
    return ;
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
