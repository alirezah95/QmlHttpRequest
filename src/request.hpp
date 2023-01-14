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

#include "qmlhttprequest_global.hpp"

class QNetworkAccessManager;

namespace qhr {

class QHR_EXPORT Request : public QObject
{
    Q_OBJECT
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
    Request(QNetworkAccessManager* nam, QObject* parent = nullptr);

    Q_INVOKABLE bool isOpen() const
    {
        return mMethod != Method::INVALID && mNRequest.url().isValid();
    }

    Q_INVOKABLE void open(const QString& method, const QUrl& url);
    Q_INVOKABLE void setRequestHeader(const QString& header, const QString& value);
    Q_INVOKABLE void send(QVariant body = QVariant());

    QByteArray requestHeader(const QByteArray& header) const;

    void setNetworkAccessManager(QNetworkAccessManager* nam);
    auto networkAccessManager() const { return mNam; }

private:
    void sendNoBodyRequest();
    void sendBodyRequest(QVariant body);

private:
    QNetworkAccessManager* mNam;
    QNetworkRequest mNRequest;
    Method mMethod;
};

}

#endif // REQUEST_HPP
