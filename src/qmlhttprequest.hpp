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

#ifndef QMLHTTPREQUEST_HPP
#define QMLHTTPREQUEST_HPP

#include <QNetworkAccessManager>
#include <QObject>
#include <QQmlEngine>
#include <QSharedPointer>

#include "request.hpp"

namespace qhr {

class QmlHttpRequest : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(RedirectPolicy redirectPolicy READ redirectPolicy WRITE
            setRedirectPolicy)

public:
    enum RedirectPolicy
    {
        ManualRedirectPolicy = QNetworkRequest::ManualRedirectPolicy,
        NoLessSafeRedirectPolicy = QNetworkRequest::NoLessSafeRedirectPolicy,
        SameOriginRedirectPolicy = QNetworkRequest::SameOriginRedirectPolicy,
        UserVerifiedRedirectPolicy
        = QNetworkRequest::UserVerifiedRedirectPolicy,
    };
    Q_ENUM(RedirectPolicy);

    enum State: uchar {
        Unsent          = uchar(qhr::Request::State::Unsent),
        Opened          = uchar(qhr::Request::State::Opened),
        HeadersReceived = uchar(qhr::Request::State::HeadersReceived),
        Loading         = uchar(qhr::Request::State::Loading),
        Done            = uchar(qhr::Request::State::Done),
    };
    Q_ENUM(State);

public:
#if QT_VERSION_MAJOR == 5
    static void registerQmlHttpRequest();
#endif

#if QT_VERSION_MAJOR == 6
    static QmlHttpRequest* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);
#endif

    QmlHttpRequest(QNetworkAccessManager* nam);

    Q_INVOKABLE qhr::Request* newRequest();
    Q_INVOKABLE void setDefaultTimeout(int timeout);

    void setNetworkAccessManager(QNetworkAccessManager* nam);
    QNetworkAccessManager* networkAccessManager() const;

    void setRedirectPolicy(RedirectPolicy rp);
    RedirectPolicy redirectPolicy() const;

private:
    QNetworkAccessManager* mNam;
};

}

#endif // QMLHTTPREQUEST_HPP
