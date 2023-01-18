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
#include <QSharedPointer>
#include <QQmlEngine>
#include <QObject>

namespace qhr {

class Request;

class QmlHttpRequest : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(RedirectPolicy redirectPolicy READ redirectPolicy WRITE setRedirectPolicy)

    using QNetworkAccessManagerPtr = QSharedPointer<QNetworkAccessManager>;
public:
    enum RedirectPolicy {
        ManualRedirectPolicy = QNetworkRequest::ManualRedirectPolicy,
        NoLessSafeRedirectPolicy = QNetworkRequest::NoLessSafeRedirectPolicy,
        SameOriginRedirectPolicy = QNetworkRequest::SameOriginRedirectPolicy,
        UserVerifiedRedirectPolicy = QNetworkRequest::UserVerifiedRedirectPolicy,
    };
    Q_ENUM(RedirectPolicy)

public:
    static QmlHttpRequest& singleton();

    Q_INVOKABLE qhr::Request* newRequest();
    Q_INVOKABLE void setDefaultTimeout(int timeout);

    void setRedirectPolicy(RedirectPolicy rp);
    RedirectPolicy redirectPolicy() const;

protected:
    explicit QmlHttpRequest(QObject* parent = nullptr);

private:
    QNetworkAccessManagerPtr mNam;
};

}

#endif // QMLHTTPREQUEST_HPP
