#define NS_ENABLE_SSL

#include <string.h>
#include <jni.h>

#include <stdio.h>
 #include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>  

#include <android/log.h>
#include <unistd.h>
#include <pthread.h>
#define LOGI(...) __android_log_print(4   , "openssl_sdk", __VA_ARGS__)

#include "ictcp.h"

struct icTcpContext_t *icTcpContextF;
int bind_port=0;
jobject mobj_audio;
jobject mobj;
//pthread_t thread;
JavaVM *m_vm_audio;
JavaVM *m_vm;
JNIEnv* listenerEnv;
jclass listenerClass;
static volatile int checked = 0;
static volatile int checked_audio = 0;
static volatile int audio = 0;
static volatile int video = 0;
long hvoice = 0; 
long handle = 0;
int port_callback = 0;
int conn_id_callback = 0;
int port_audio = 0;
int conn_id_audio = 0;
typedef struct
{
    pthread_mutex_t buffer_mutex;
    int port;
    int conn_id;
} ts_conn_t;
static ts_conn_t *s_current = NULL;

extern "C"{

void start_callback(icTcpContext *context, int conn_id, int port){
    port_callback = port;
    conn_id_callback = conn_id;
    checked=1;
}
void auth_callback(icTcpContext *context, void *conn, char *buf, size_t len){
}

static void* thread_video(void *arg){
    JNIEnv *env;
    m_vm->AttachCurrentThread(&env,NULL);
    jclass javaClass = env->GetObjectClass(mobj);
    if(javaClass ==NULL){
    }else{
        jmethodID javaCallback = env->GetMethodID(javaClass, "getData", "(III)V");
        if(javaCallback == NULL){
        }else{
            while(checked!=3){
                if(checked==1){
                    checked=0;
                    //pthread_mutex_lock(&(s_current->buffer_mutex));
                    env->CallVoidMethod( mobj, javaCallback, port_callback,conn_id_callback,audio);
                    //pthread_mutex_unlock(&(s_current->buffer_mutex));
                    //sleep(0.5);
                }
             };
        }
    }
    m_vm->DetachCurrentThread();
    LOGI("end");
}

JNIEXPORT void JNICALL Java_com_wetoop_camera_service_VideoService_threadInit(JNIEnv* env,jobject thiz){
    env->GetJavaVM(&m_vm);
    mobj = env->NewGlobalRef(thiz);
    checked=0;
    pthread_t threadId;
    if(pthread_create(&threadId, NULL, thread_video, NULL)!=0){
        LOGI("创建线程错误!!");
    }
}

JNIEXPORT void JNICALL Java_com_wetoop_camera_service_VideoService_threadClose(JNIEnv *env,jobject thiz)
{
    checked=3;
}

JNIEXPORT void JNICALL Java_com_wetoop_camera_CameraJni_tcpInit(JNIEnv* env,jobject thiz,jboolean debug,jstring route,jstring node,jboolean audioFrom){
    const char* route_char = env->GetStringUTFChars(route,NULL);
    const char* node_char = env->GetStringUTFChars(node,NULL);
    mobj = thiz;
    icTcpContextF = icTcpContextInit(debug,route_char,node_char);
   
    env->ReleaseStringUTFChars(route,route_char);
    env->ReleaseStringUTFChars(node,node_char);
}

JNIEXPORT int JNICALL Java_com_wetoop_camera_CameraJni_tcpSavePeer(JNIEnv* env,jobject thiz,jstring device_name,jstring device_token,jstring connect_to_device,int connect_port,jstring ca_file,jboolean audioFrom){
    char* tokenChar1 = (char*)env->GetStringUTFChars(device_token,NULL);
    char* nameChar1 = (char*)env->GetStringUTFChars(device_name,NULL);
    char* netIdChar1 = (char*)env->GetStringUTFChars(connect_to_device,NULL);
    const char* ca_file_char = env->GetStringUTFChars(ca_file,NULL);
    int saveNum = icTcpSavePeerCert(icTcpContextF,nameChar1, tokenChar1,netIdChar1,connect_port,ca_file_char);
    
    env->ReleaseStringUTFChars(device_token,tokenChar1);
    env->ReleaseStringUTFChars(device_name,nameChar1);
    env->ReleaseStringUTFChars(connect_to_device,netIdChar1);
    env->ReleaseStringUTFChars(ca_file,ca_file_char);
    return saveNum;
}

JNIEXPORT int JNICALL Java_com_wetoop_camera_CameraJni_tcpStart(JNIEnv* env,jobject thiz,jstring device_name,jstring device_token,jstring connect_to_device,jboolean audioFrom){
    char* tokenChar1 = (char*)env->GetStringUTFChars(device_token,NULL);
    char* nameChar1 = (char*)env->GetStringUTFChars(device_name,NULL);
    char* netIdChar1 = (char*)env->GetStringUTFChars(connect_to_device,NULL);
    int startNum = icTcpStart(icTcpContextF,nameChar1,tokenChar1,netIdChar1);
    
    env->ReleaseStringUTFChars(device_token,tokenChar1);
    env->ReleaseStringUTFChars(device_name,nameChar1);
    env->ReleaseStringUTFChars(connect_to_device,netIdChar1);
    return startNum;
}

JNIEXPORT int JNICALL Java_com_wetoop_camera_CameraJni_tcpAdd(JNIEnv* env,jobject thiz,jint connect_port,jstring ca_file,jint audioCome,jboolean audioFrom){
    audio = audioCome;
    const char* ca_file_char = env->GetStringUTFChars(ca_file,NULL);
    int addNum = icTcpAdd(icTcpContextF,connect_port,ca_file_char,start_callback,auth_callback);
    
    env->ReleaseStringUTFChars(ca_file,ca_file_char);
    return addNum;
}

JNIEXPORT int JNICALL Java_com_wetoop_camera_CameraJni_tcpResult(JNIEnv* env,jobject thiz,jint conn_id,jboolean audioFrom){
    int resultNum = icTcpResult(icTcpContextF,conn_id);
    
    return resultNum;
}
JNIEXPORT int JNICALL Java_com_wetoop_camera_CameraJni_tcpAuthResult(JNIEnv* env,jobject thiz,jint conn_id,jboolean audioFrom){
    int authResultNum = icTcpAuthResult(icTcpContextF,conn_id);
    
    return authResultNum;
}
JNIEXPORT int JNICALL Java_com_wetoop_camera_CameraJni_tcpErrorResult(JNIEnv* env,jobject thiz,jint conn_id,jboolean audioFrom){
    int certErrorResultNum = icTcpCertErrorResult(icTcpContextF,conn_id);
    
    return certErrorResultNum;
}
JNIEXPORT int JNICALL Java_com_wetoop_camera_CameraJni_tcpRemove(JNIEnv* env,jobject thiz,jint conn_id,jboolean audioFrom){
    int stopNum = icTcpRemove(icTcpContextF,conn_id);
    
    return stopNum;
}
JNIEXPORT int JNICALL Java_com_wetoop_camera_CameraJni_tcpStop(JNIEnv* env,jobject thiz,jboolean audioFrom){
    int stopNum = icTcpStop(icTcpContextF);
    
    return stopNum;
}
JNIEXPORT int JNICALL Java_com_wetoop_camera_CameraJni_tcpFree(JNIEnv* env,jobject thiz,jboolean audioFrom){
    int freeNum = icTcpContextFree(icTcpContextF);
    
    return freeNum;
}
}


