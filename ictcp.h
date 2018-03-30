#ifndef _IC_LIB_ICTCP_H
#define _IC_LIB_ICTCP_H

#include <stdlib.h>
#include <pthread.h>

#define FORWARD_RESULT_INITIAL 0
#define FORWARD_RESULT_OK 1
#define FORWARD_RESULT_CONN_LOST -2
#define FORWARD_RESULT_PEER_OFFLINE -3
#define FORWARD_RESULT_CERT_ERROR -4
#define FORWARD_RESULT_KICKED_OUT -5
#define FORWARD_RESULT_CONN_CLOSE -6
#define FORWARD_RESULT_SYS_ERR -9
#define FORWARD_RESULT_NOT_EXISTS -10

#define FORWARD_MODE_UNKNOWN 0
#define FORWARD_MODE_LAN 1
#define FORWARD_MODE_P2P 2
#define FORWARD_MODE_RELAY 3

struct icTcpContext_t;
typedef struct icTcpContext_t icTcpContext;

#ifdef __cplusplus
extern "C" {
#endif

    icTcpContext* icTcpContextInit(bool debug, const char* route, const char* node);
    int icTcpContextFree(icTcpContext *context);
    int icTcpSavePeerCert(icTcpContext *context, char* device_name, char* device_token, char* connect_to_device, int connect_port, const char* ca_file);
    int icTcpSavePeerCertResult(icTcpContext *context);
    int icTcpStart(icTcpContext *context, char* device_name, char* device_token, char* connect_to_device);
    int icTcpAdd(icTcpContext *context, int connect_port, const char* ca_file, void (*start_callback)(icTcpContext *context, int conn_id, int bind_port), void (*auth_callback)(icTcpContext *context, void *conn, char *buf, size_t len));
    int icTcpRemove(icTcpContext *context, int conn_id);
    int icTcpStop(icTcpContext *context);
    int icTcpResult(icTcpContext *context, int conn_id);
    long icTcpCertErrorResult(icTcpContext *context, int conn_id);
    int icTcpForwardMode(icTcpContext *context);
    int icTcpAuthProcessed(icTcpContext *context, void *conn);
    int icTcpAuthResult(icTcpContext *context, int conn_id);

#ifdef __cplusplus
}
#endif


#endif // _IC_LIB_ICTCP_H
