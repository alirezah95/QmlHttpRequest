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

#include <QObject>
#include <QNetworkRequest>
#include <QSharedPointer>
#include <QJSValue>
#include <QQmlEngine>

#include "qmlhttprequest_global.hpp"

class QNetworkAccessManager;
class QNetworkReply;

namespace qhr {

class QHR_EXPORT Request : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    // Response properties
    Q_PROPERTY(int      status          READ replyStatus        CONSTANT)
    Q_PROPERTY(QString  statusText      READ replyStatusText    CONSTANT)
    Q_PROPERTY(QString  responseText    READ replyResponseText  CONSTANT)
    // Request properties
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout)
    Q_PROPERTY(QJSValue onDownloadProgressChanged   MEMBER mDownloadProgressChangedCallback)
    Q_PROPERTY(QJSValue onUploadProgressChanged     MEMBER mUploadProgressChangedCallback)
    Q_PROPERTY(QJSValue onRedirected                MEMBER mRedirectedCallback)
    Q_PROPERTY(QJSValue onFinished                  MEMBER mFinishedCallback)
    Q_PROPERTY(QJSValue onAborted                   MEMBER mAbortedCallback)
    Q_PROPERTY(QJSValue onTimeout                   MEMBER mTimeoutCallback)
    Q_PROPERTY(QJSValue onError                     MEMBER mErrorCallback)

    using QNetworkAccessManagerPtr = QSharedPointer<QNetworkAccessManager>;
public:
    enum class Method: char {
        INVALID = -1,
        GET = 0,
        HEAD,
        POST,
        PUT,
        PATCH,
        DELETE,
        CUSTOM,
    };

    explicit Request(QObject* parent = nullptr);
    Request(QNetworkAccessManagerPtr nam, int timeout = 0, QObject* parent = nullptr);
    virtual ~Request();

    Q_INVOKABLE bool isOpen() const
    {
        return mMethod != Method::INVALID && mNRequest.url().isValid();
    }

    Q_INVOKABLE void open(const QString& method, const QUrl& url);
    Q_INVOKABLE void setRequestHeader(const QString& header, const QString& value);
    Q_INVOKABLE void send(const QVariant& body = QVariant());

    QByteArray requestHeader(const QByteArray& header) const;

    void setNetworkAccessManager(QNetworkAccessManagerPtr nam);
    auto networkAccessManager() const { return mNam; }

    void setTimeout(int timeout);
    int timeout() const { return mNRequest.transferTimeout(); }

    int replyStatus() const;
    QString replyStatusText() const;
    QString replyResponseText() const;

private:
    void sendNoBodyRequest();
    void sendBodyRequest(const QVariant& body);

    void sendBodyRequestText(const QVariant& body);
    void sendBodyRequestMultipart(const QVariant& body);

    void setupReplyConnections();

private slots:
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
    Method mMethod;

    QJSValue mDownloadProgressChangedCallback;
    QJSValue mUploadProgressChangedCallback;
    QJSValue mRedirectedCallback;
    QJSValue mFinishedCallback;
    QJSValue mAbortedCallback;
    QJSValue mTimeoutCallback;
    QJSValue mErrorCallback;
};

}

bool operator!=(const QJSValue& left, const QJSValue& right);

#endif // REQUEST_HPP
