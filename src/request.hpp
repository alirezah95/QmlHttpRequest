/*!
 * Copyright (c) 2023 Alireza
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <QJSValue>
#include <QNetworkRequest>
#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>

#include "qmlhttprequest_global.hpp"
#include "response.hpp"

class QNetworkAccessManager;
class QNetworkReply;
class QHttpMultiPart;

namespace qhr {

class QHR_EXPORT Request : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    // Response properties
    Q_PROPERTY(QVariant response        READ response       CONSTANT)
    Q_PROPERTY(QString  responseText    READ responseText   CONSTANT)
    Q_PROPERTY(QString  responseType    READ responseType   CONSTANT)
    Q_PROPERTY(QUrl     responseUrl     READ responseUrl    CONSTANT)
    Q_PROPERTY(QString  statusText      READ statusText     CONSTANT)
    Q_PROPERTY(int      status          READ status         CONSTANT)
    // Request properties
    Q_PROPERTY(int      timeout     READ    timeout WRITE setTimeout)
    Q_PROPERTY(State    readyState  READ    readyState() CONSTANT)

    Q_PROPERTY(QJSValue ondownloadprogress  MEMBER  mDownloadProgressCb)
    Q_PROPERTY(QJSValue onuploadprogress    MEMBER  mUploadProgressCb)
    Q_PROPERTY(QJSValue onreadystatechange  MEMBER  mReadyStateCb)
    Q_PROPERTY(QJSValue onredirected        MEMBER  mRedirectedCb)
    Q_PROPERTY(QJSValue onaborted           MEMBER  mAbortedCb)
    Q_PROPERTY(QJSValue ontimeout           MEMBER  mTimeoutCb)
    Q_PROPERTY(QJSValue onerror             MEMBER  mErrorCb)

    using QNetworkAccessManagerPtr = QSharedPointer<QNetworkAccessManager>;

public:
    enum class Method : char
    {
        INVALID = -1,
        GET = 0,
        HEAD,
        POST,
        PUT,
        PATCH,
        DELETE,
        CUSTOM,
    };

    enum class State : uchar
    {
        Unsent = 0,
        Opened,
        HeadersReceived,
        Loading,
        Done
    };
    Q_ENUM(State);

    explicit Request(QObject* parent = nullptr);
    Request(QNetworkAccessManagerPtr nam, int timeout = 0,
        QObject* parent = nullptr);
    virtual ~Request();

    Q_INVOKABLE void open(const QString& method, const QUrl& url);
    Q_INVOKABLE void setRequestHeader(
        const QString& header, const QString& value);
    Q_INVOKABLE void send(const QVariant& body = QVariant());
    Q_INVOKABLE void abort();
    Q_INVOKABLE void destroy();

    bool isOpen() const;

    QByteArray requestHeader(const QByteArray& header) const;

    void setNetworkAccessManager(QNetworkAccessManagerPtr nam);
    auto networkAccessManager() const { return mNam; }

    void setTimeout(int timeout);
    int timeout() const { return mNRequest.transferTimeout(); }

    State readyState() const { return mState; }

    // Response's values methods
    QVariant response() const;
    QString responseText() const;
    auto responseType() const { return mResponse.responseType; };
    auto responseUrl() const { return mResponse.responseUrl; };
    auto statusText() const { return mResponse.statusText; };
    auto status() const { return mResponse.status; };

private:
    void sendNoBodyRequest();
    void sendBodyRequest(const QVariant& body);

    void sendBodyRequestText(const QVariant& body);
    void sendBodyRequestMultipart(const QVariant& body);

    void multipartAddObject(
        QHttpMultiPart* mpBody, QString prefix, const QJsonObject& object);
    void multipartAddArray(
        QHttpMultiPart* mpBody, QString prefix, const QJsonArray& array);
    void multipartAddValue(
        QHttpMultiPart* mpBody, QString prefix, const QJsonValue& value);

    void setupReplyConnections();

    void callCallback(QJSValue cb, const QJSValueList &args = QJSValueList());

private:
    void onReplyReadReady();
    void onReplyFinished();
    void onReplyErrorOccured(int error);
    void onReplyRedirected(const QUrl& url);
    void onReplyDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onReplyUploadProgress(qint64 bytesSent, qint64 bytesTotal);

private:
    QNetworkAccessManagerPtr mNam;
    QNetworkRequest mNRequest;
    QNetworkReply* mNReply;
    QByteArray mMethodName;

    State mState;
    Method mMethod;
    Response mResponse;

    QJSValue mDownloadProgressCb;
    QJSValue mUploadProgressCb;
    QJSValue mReadyStateCb;
    QJSValue mRedirectedCb;
    QJSValue mAbortedCb;
    QJSValue mTimeoutCb;
    QJSValue mErrorCb;
};

}

bool operator!=(const QJSValue& left, const QJSValue& right);

#endif // REQUEST_HPP
