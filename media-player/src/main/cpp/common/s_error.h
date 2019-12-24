/**
 * @date : 2019-09-04 16:42
 * @author: lichen
 * @email : 1960003945@qq.com
 * @description : 
 */
//
#ifndef SMART_MEDIA_CONST_H
#define SMART_MEDIA_CONST_H



// ---------- 播放错误码 start ----------
#define ERROR_CODE_FIND_STREAM -0x10
#define ERROR_CODE_CODEC_FIND_DECODER -0x11
#define ERROR_CODE_CODEC_ALLOC_CONTEXT -0x12
#define ERROR_CODE_SWR_ALLOC_SET_OPTS -0x13
#define ERROR_CODE_SWR_CONTEXT_INIT -0x14

//查找解码器失败
#define ERROR_CODE_FIND_STREAM_DECODER -0x15

// ---------- 播放错误码 end ----------

//static bool is_exit = false;//是否退出，一般在切换不同的url触发 true

#endif //SMART_MEDIA_SMART_CONST_H
