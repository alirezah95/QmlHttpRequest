#include "qmlhttprequest.hpp"
#include "request.hpp"

namespace qhr {

/*!
 * \class QmlHttpRequest
 * \brief QmlHttpRequest class reponsible for creating a new \ref Request. It
 * can be used in QML to create a new \ref Request
 */

QmlHttpRequest::QmlHttpRequest(QObject* parent) : QObject { parent } { }

Request* QmlHttpRequest::newRequest()
{
    return new Request(nullptr, this);
}

}
