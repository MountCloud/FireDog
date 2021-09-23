#ifndef _FIREDOG_ERRORCODE_H_
#define _FIREDOG_ERRORCODE_H_


///NO ERROR
#define NO_ERROR 0

///---MAIN 主类 1-500 ---------------------------------


///---FEATURE LIBRARAY 特征库 501-1000 ---------------------------------

///content formate error,Failed to load or parse content
#define FL_CONTENT_FORMATE_ERROR 501
//content version check failed
#define FL_CONTENT_VERSION_ERROR 502

///---MATCHER 匹配器 1001-1500

//Feature Library content is null
#define M_FEATURE_LIBRARY_IS_EMPTY 1001

///---CONVERTER 转换器 1501-2000
#define C_CONTENT_FORMATE_ERROR 1501

#endif