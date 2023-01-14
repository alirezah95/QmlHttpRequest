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

#include "qmlhttprequest_global.hpp"

class QNetworkAccessManager;

namespace qhr {

class QHR_EXPORT Request : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout)
    Q_PROPERTY(QJSValue onDownloadProgressChanged   MEMBER downloadProgressChangedCallback)
    Q_PROPERTY(QJSValue onUploadProgressChanged     MEMBER uploadProgressChangedCallback)
    Q_PROPERTY(QJSValue onErrorOccurred             MEMBER errorOccurredCallback)
    Q_PROPERTY(QJSValue onRedirected                MEMBER redirectedCallback)
    Q_PROPERTY(QJSValue onFinished                  MEMBER finishedCallback)
    Q_PROPERTY(QJSValue onAborted                   MEMBER abortedCallback)
    Q_PROPERTY(QJSValue onTimeout                   MEMBER timeoutCallback)

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

    Q_INVOKABLE bool isOpen() const
    {
        return mMethod != Method::INVALID && mNRequest.url().isValid();
    }

    Q_INVOKABLE void open(const QString& method, const QUrl& url);
    Q_INVOKABLE void setRequestHeader(const QString& header, const QString& value);
    Q_INVOKABLE void send(QVariant body = QVariant());

    QByteArray requestHeader(const QByteArray& header) const;

    void setNetworkAccessManager(QNetworkAccessManagerPtr nam);
    auto networkAccessManager() const { return mNam; }

    void setTimeout(int timeout);
    int timeout() const { return mNRequest.transferTimeout(); }

private:
    void sendNoBodyRequest();
    void sendBodyRequest(QVariant body);

private:
    QNetworkAccessManagerPtr mNam;
    QNetworkRequest mNRequest;
    Method mMethod;

    QJSValue downloadProgressChangedCallback;
    QJSValue uploadProgressChangedCallback;
    QJSValue redirectedCallback;
    QJSValue errorOccurredCallback;
    QJSValue finishedCallback;
    QJSValue abortedCallback;
    QJSValue timeoutCallback;
};

}

#endif // REQUEST_HPP
