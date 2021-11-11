#ifndef _FIREDOG_ERRORCODE_H_
#define _FIREDOG_ERRORCODE_H_


///NO ERROR
#define NO_ERROR 0

///---MAIN Ö÷Àà 1-500 ---------------------------------


///---FEATURE LIBRARAY ÌØÕ÷¿â 501-1000 ---------------------------------

///content formate error,Failed to load or parse content
#define FL_CONTENT_FORMATE_ERROR 501
//content version check failed
#define FL_CONTENT_VERSION_ERROR 502
//content items check failed,items is empty
#define FL_CONTENT_ITEMS_IS_EMPTY 503
//content rule formate fail
#define FL_CONTENT_EXIST_EMPTY_FEATURES_FAIL 504
//content rule formate fail
#define FL_CONTENT_EXIST_RULE_FORMATE_FAIL 505

///---MATCHER Æ¥ÅäÆ÷ 1001-1500

//Feature Library content is null
#define M_FEATURE_LIBRARY_IS_EMPTY 1001

///---CONVERTER ×ª»»Æ÷ 1501-2000
#define C_CONTENT_FORMATE_ERROR 1501

#endif