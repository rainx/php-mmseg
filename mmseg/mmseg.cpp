
extern "C" {
    #ifdef HAVE_CONFIG_H
    #include "config.h"
    #endif
    
    #include "php.h"
    #include "php_ini.h"
    #include "ext/standard/info.h"
}

#include "php_mmseg.h"
#include <sys/stat.h>

using namespace std;

// SegmenterManager
ZEND_DECLARE_MODULE_GLOBALS(mmseg)

/* True global resources - no need for thread safety here */
static int le_mmseg;

/* {{{ mmseg_functions[]
 *
 * Every user visible function must have an entry in mmseg_functions[].
 */
const zend_function_entry mmseg_functions[] = {
	PHP_FE(mmseg_segment,	NULL)
	PHP_FE(mmseg_open,	    NULL)
	PHP_FE(mmseg_close,	    NULL)
	PHP_FE(mmseg_gendict,	NULL)
	PHP_FE(mmseg_gensynonyms,	NULL)
	PHP_FE(mmseg_genthesaurus,	NULL)
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
	"0.2", /* Replace with version number for your extension */
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
    PHP_INI_ENTRY("mmseg.autoreload", "1", PHP_INI_ALL, NULL)
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
    struct stat st;
    SegmenterManager* mgr = (SegmenterManager*) mmseg_globals->mgr;
    mgr = new SegmenterManager();
    int nRet = 0;
    mmseg_globals->dict_mtime = 0;
    nRet = mgr->init(INI_STR("mmseg.dict_dir"));
    if (nRet == 0) {
        mmseg_globals->mgr = (void*) mgr;
        // 记录最后一次字典文件更改的数值
        string dict_file_path;
        dict_file_path = INI_STR("mmseg.dict_dir");
        dict_file_path.append("/uni.lib");

        if (stat(dict_file_path.c_str(), &st) == 0) {
            mmseg_globals->dict_mtime = st.st_mtime;
            MMSEG_LOG(ctime(&st.st_mtime));
        }

        MMSEG_LOG("minit load dictionary");
	    return ;
    } else {
        if (mgr != NULL)  {
            delete mgr;
            mgr = NULL;
        }
        mmseg_globals->mgr = NULL;
        return ;
    }
}

/* Triggered at the end of a thread */
static void php_mmseg_globals_dtor(zend_mmseg_globals *mmseg_globals TSRMLS_DC)
{
    SegmenterManager* mgr = (SegmenterManager*) mmseg_globals->mgr;
    if (mgr != NULL)  {
        delete mgr;
        mgr = NULL;
    }
}

// mmseg segmenter manager句柄
static int le_mmseg_descriptor;

// mmseg 句柄的dtor函数
static void php_mmseg_descriptor_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    SegmenterManager *mgr = (SegmenterManager*)rsrc->ptr;
    if (mgr != NULL) {
        delete mgr;
        mgr = NULL;
    }
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mmseg)
{
	REGISTER_INI_ENTRIES();

    // 初始化mmseg资源句柄
    le_mmseg_descriptor = zend_register_list_destructors_ex(php_mmseg_descriptor_dtor, NULL, PHP_MMSEG_DESCRIPTOR_RES_NAME,module_number);
    
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
    SegmenterManager* mgr =  NULL;
    zval *mmseg_resource;

    zend_bool autoreload = INI_BOOL("mmseg.autoreload");

    // 判断传入参数的数量
    if (argc == 1) {
        struct stat st;
	    if (zend_parse_parameters(argc TSRMLS_CC, "s", &content, &content_len) == FAILURE) 
		    return;
        // 如果使用全局的字典文件，在这里看一下是否需要判断文件是否有改变
        if (1 == autoreload) {
            // 文件名
            string dict_file_path;
            dict_file_path = INI_STR("mmseg.dict_dir");
            dict_file_path.append("/uni.lib");

            if (stat(dict_file_path.c_str(), &st) == 0) {
                if (st.st_mtime != MMSEG_G(dict_mtime)) {
                    MMSEG_G(dict_mtime) = st.st_mtime;
                    SegmenterManager* oldMgr = (SegmenterManager*)MMSEG_G(mgr);
                    SegmenterManager* newMgr;
                    newMgr = new SegmenterManager();
                    int nRet = 0;
                    nRet = newMgr->init(INI_STR("mmseg.dict_dir"));
                    if (nRet == 0) {
                        if (oldMgr) {
                            MMSEG_LOG("trying to delete old mgr");
                            delete oldMgr;
                            oldMgr = NULL;
                        }
                        
                        MMSEG_LOG("load dictionary on change");
                        MMSEG_G(mgr) = newMgr;
                    } else {
                        // no change. still use oldMgr
                        MMSEG_LOG("no change, initialize new dict file failed");
                        newMgr = NULL;
                    }
                } else {
                    MMSEG_LOG(ctime(&st.st_mtime));
                    MMSEG_LOG(ctime(&MMSEG_G(dict_mtime)));
                    MMSEG_LOG("no change, dict file no change");
                    // no change, still  use old oldMgr
                }
            } else {
                MMSEG_LOG("no change, stat new dict file failed");
            }
        } else {
            MMSEG_LOG("no change, autoreload not set");
        }
    } else if (argc = 2){
	    if (zend_parse_parameters(argc TSRMLS_CC, "rs", &mmseg_resource, &content, &content_len) == FAILURE) 
            return;
        ZEND_FETCH_RESOURCE(mgr,SegmenterManager*,&mmseg_resource,-1,PHP_MMSEG_DESCRIPTOR_RES_NAME,le_mmseg_descriptor);
    } else {
        return;
    }

    mgr = (SegmenterManager*) MMSEG_G(mgr);
    if (mgr == NULL) {
        RETURN_NULL();
    }

    Segmenter* seg = mgr->getSegmenter();
    u4 content_length = content_len;
    seg->setBuffer((u1*)content, content_length);
    array_init(return_value);
    while(1)
    {
        u2 len = 0, symlen = 0;
        char* tok = (char*)seg->peekToken(len,symlen);
        if(!tok || !*tok || !len){
            break;
        }
        //append new item
        add_next_index_stringl(return_value, tok, len, 1);
        seg->popToken(len);
    }
    return ;
}
/* }}} */

PHP_FUNCTION(mmseg_open)
{
	char *path = NULL;
	int argc = ZEND_NUM_ARGS();
	int path_len;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &path, &path_len) == FAILURE) 
		return;

    // 生成新的对象
    SegmenterManager* mgr = new SegmenterManager();
    int nRet = 0;
    nRet = mgr->init(path);
    if (nRet == 0) {
        // 注册这个资源
        ZEND_REGISTER_RESOURCE(return_value,mgr,le_mmseg_descriptor);
    } else {
        RETURN_NULL();
    }
}


PHP_FUNCTION(mmseg_close)
{
    zval *mmseg_resource;
    SegmenterManager* mgr;
	int argc = ZEND_NUM_ARGS();

	if (zend_parse_parameters(argc TSRMLS_CC, "r", &mmseg_resource) == FAILURE) 
		return;
    ZEND_FETCH_RESOURCE(mgr,SegmenterManager*,&mmseg_resource,-1,PHP_MMSEG_DESCRIPTOR_RES_NAME,le_mmseg_descriptor);

    zend_hash_index_del(&EG(regular_list),Z_RESVAL_P(mmseg_resource));
    RETURN_TRUE; 
}


// 字典的生成
PHP_FUNCTION(mmseg_gendict)
{
	int argc = ZEND_NUM_ARGS();

	char *path = NULL;
	int path_len;

    char *target = NULL;
    int target_len;

    zend_bool b_plainText = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "ss", &path, &path_len, &target, &target_len) == FAILURE) 
		return;

    UnigramCorpusReader ur;
    ur.open(path,b_plainText?"plain":NULL);
    UnigramDict ud;
    int ret = ud.import(ur);
    ud.save(target);      
    //check
    int i = 0;
    for(i=0;i<ur.count();i++)
    {
        UnigramRecord* rec = ur.getAt(i);

        if(ud.exactMatch(rec->key.c_str()) == rec->count){
            continue;
        }else{
            RETURN_FALSE;
        }
    }//end for

    RETURN_TRUE; 
}


// 特殊短语字典的生成
PHP_FUNCTION(mmseg_gensynonyms)
{
	int argc = ZEND_NUM_ARGS();

	char *path = NULL;
	int path_len;

    char *target = NULL;
    int target_len;

    zend_bool b_plainText = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "ss", &path, &path_len, &target, &target_len) == FAILURE) 
        return;

    SynonymsDict dict;
    dict.import(path);
    if(target)
        dict.save(target);
    else
        dict.save("synonyms.dat");

    RETURN_TRUE; 
}

// 同义词词典的生成 
PHP_FUNCTION(mmseg_genthesaurus)
{
	int argc = ZEND_NUM_ARGS();

	char *path = NULL;
	int path_len;

    char *target = NULL;
    int target_len;

    zend_bool b_plainText = 0;

	if (zend_parse_parameters(argc TSRMLS_CC, "ss", &path, &path_len, &target, &target_len) == FAILURE) 
        return;

    ThesaurusDict tdict;
    tdict.import(path, target);
    RETURN_TRUE; 
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
