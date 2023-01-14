#include "qmlhttprequest.hpp"
#include "request.hpp"

namespace qhr {

/*!
 * \class QmlHttpRequest
 * \brief QmlHttpRequest class reponsible for creating a new \ref Request. It
 * can be used in QML to create a new \ref Request
 */

QmlHttpRequest::QmlHttpRequest(QObject* parent)
    : QObject { parent },
      mNam(new QNetworkAccessManager())
{ }

/*!
 * \brief QmlHttpRequest::newRequest() Creates a new \ref Request object that
 * can be used to make HTTP request.
 * \note This \ref Request object should be deleted when done with using \ref
 * Request::destroy() from QML or \a\b Request::deleteLater()
 * \return A \ref Request
 */
Request* QmlHttpRequest::newRequest()
{
    return new Request(mNam, mNam->transferTimeout(), this);
}

void QmlHttpRequest::setDefaultTimeout(int timeout)
{
    mNam->setTransferTimeout(timeout);
}

}
