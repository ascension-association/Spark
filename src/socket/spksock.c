/*
 * Copyright (c) 2016-2017 Jacopo De Luca
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <stdlib.h>
#include <string.h>

#include <spksock.h>
#include "spksock_common.h"

struct ErrorInfo {
    int value;
    char *msg;
};

static const struct ErrorInfo __spk_error_table[] =
        {
                {SPKSOCK_SUCCESS,    "Success"},
                {SPKSOCK_ERROR,      "Generic internal error"},
                {SPKSOCK_ENINIT,     "Uninitialized socket"},
                {SPKSOCK_ENOSUPPORT, "Operation not supported"},
                {SPKSOCK_ENOMEM,     "Out of memory"},
                {SPKSOCK_EPERM,      "Permission denied"},
                {SPKSOCK_ENODEV,     "No such device"},
                {SPKSOCK_EINTR,      "Interrupted system call"},
                {SPKSOCK_ESIZE,      "Message too large"}
        };

char *spark_strerror(int error) {
    char *ret = NULL;

    for (int i = 0; i < (sizeof(__spk_error_table) / sizeof(struct ErrorInfo)); i++)
        if (error == __spk_error_table[i].value) {
            ret = __spk_error_table[i].msg;
            return ret;
        }
    return ret;
}

int spark_getltype(struct SpkSock *ssock) {
    if (ssock == NULL)
        return SPKSOCK_ENINIT;
    return ssock->lktype;
}

int spark_opensock(char *device, unsigned int buflen, struct SpkSock **ssock) {
    int errcode;

    if (device == NULL || ssock == NULL)
        return SPKSOCK_ERROR;

    if (((*ssock) = calloc(1, sizeof(struct SpkSock))) == NULL)
        return SPKSOCK_ENOMEM;

    (*ssock)->iface_name = strdup(device);
    (*ssock)->bufl = buflen;
    (*ssock)->lktype = -1;

    if ((errcode = __ssock_init_socket(*ssock)) < 0)
        free(*ssock);

    return errcode;
}

int spark_read(struct SpkSock *ssock, unsigned char *buf, struct SpkTimeStamp *ts) {
    if (ssock == NULL)
        return SPKSOCK_ENINIT;
    return ssock->op.read(ssock, buf, ts);
}

int spark_setdirection(struct SpkSock *ssock, enum SpkDirection direction) {
    if (ssock == NULL)
        return SPKSOCK_ENINIT;
    if (ssock->op.setdir == NULL)
        return SPKSOCK_ENOSUPPORT;
    return ssock->op.setdir(ssock, direction);
}

int spark_setnblock(struct SpkSock *ssock, bool nonblock) {
    if (ssock == NULL)
        return SPKSOCK_ENINIT;
    if (ssock->op.setnblk == NULL)
        return SPKSOCK_ENOSUPPORT;
    return ssock->op.setnblk(ssock, nonblock);
}

int spark_setpromisc(struct SpkSock *ssock, bool promisc) {
    if (ssock == NULL)
        return SPKSOCK_ENINIT;
    if (ssock->op.setpromisc == NULL)
        return SPKSOCK_ENOSUPPORT;
    return ssock->op.setpromisc(ssock, promisc);
}

int spark_settsprc(struct SpkSock *ssock, enum SpkTimesPrc prc) {
    if (ssock == NULL)
        return SPKSOCK_ENINIT;
    if (ssock->op.setprc == NULL)
        return SPKSOCK_ENOSUPPORT;
    return ssock->op.setprc(ssock, prc);
}

int spark_write(struct SpkSock *ssock, unsigned char *buf, unsigned int len) {
    if (ssock == NULL)
        return SPKSOCK_ENINIT;
    return ssock->op.write(ssock, buf, len);
}

inline void spark_close(struct SpkSock *ssock) {
    if (ssock != NULL) {
        ssock->op.finalize(ssock);
        free(ssock->iface_name);
        free(ssock);
    }
}

inline void spark_getsstats(struct SpkSock *ssock, struct SpkStats *stats) {
    if (ssock != NULL)
        memcpy(stats, &ssock->sock_stats, sizeof(struct SpkStats));
}

inline void spark_setbuf(struct SpkSock *ssock, unsigned int size) {
    if (ssock != NULL)
        ssock->bufl = size;
}