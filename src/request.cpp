#include "request.hpp"

namespace qhr {

Request::Request(QObject* parent) : QObject { parent } { }

void Request::open(const QString& method, const QUrl& url)
{
    mMethodName = method;
    mNRequest.setUrl(url);
}

}
