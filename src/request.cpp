#include "request.hpp"

namespace qhr {

Request::Request(QObject* parent) : QObject { parent } { }

void Request::open(const QString& method, const QUrl& url)
{
    mMethodName = method;
    mNRequest.setUrl(url);
}

void Request::setRequestHeader(const QString& header, const QString& value)
{
    if (!isOpen())
        return;

    mNRequest.setRawHeader(header.toUtf8(), value.toUtf8());
}

QByteArray Request::requestHeader(const QByteArray& header) const
{
    return mNRequest.rawHeader(header);
}

}
