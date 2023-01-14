#include "request.hpp"

namespace qhr {

/*!
 * \class Request
 * \brief Request call encapsulating a network request and a thin wrapper around
 * \a\b QNetworkRequest
 *
 * The \ref Request class provides methods for opening a request, setting its
 * headers, sending it, etc with names matching the ones in \a\b
 * XmlHttpRequest
 */

Request::Request(QObject* parent)
    : QObject { parent },
      mNam(nullptr),
      mMethod(Method::INVALID)
{ }

Request::Request(QNetworkAccessManager* nam, QObject* parent)
    : Request(parent)
{
    mNam = nam;
}

/*!
 * \qmlmethod open()
 * \brief Request::open() Opens a request with the \a method as method name and
 * \a url for request url
 * \param method The name of the request method, GET, POST, PUT, etc
 * \param url The url for the request
 */
void Request::open(const QString& method, const QUrl& url)
{
    if (method == "") {
        mMethod = Method::INVALID;
    } else if (method.compare("GET", Qt::CaseInsensitive) == 0) {
        mMethod = Method::GET;
    } else if (method.compare("HEAD", Qt::CaseInsensitive) == 0) {
        mMethod = Method::HEAD;
    } else if (method.compare("POST", Qt::CaseInsensitive) == 0) {
        mMethod = Method::POST;
    } else if (method.compare("PUT", Qt::CaseInsensitive) == 0) {
        mMethod = Method::PUT;
    } else if (method.compare("PATCH", Qt::CaseInsensitive) == 0) {
        mMethod = Method::PATCH;
    } else if (method.compare("DELETE", Qt::CaseInsensitive) == 0) {
        mMethod = Method::DELETE;
    } else {
        mMethod = Method::CUSTOM;
    }
    mNRequest.setUrl(url);
}

/*!
 * \qmlmethod setRequestHeader()
 * \brief Request::setRequestHeader() Sets the request \a header to \a value.
 * \note This method must be called after \ref open() and before \ref send()
 * \param header The name of the header
 * \param value The value for \a header
 */
void Request::setRequestHeader(const QString& header, const QString& value)
{
    if (!isOpen())
        return;

    mNRequest.setRawHeader(header.toUtf8(), value.toUtf8());
}

/*!
 * \brief Request::send() Sends a method using \a\b QNetworkAccessManager
 * injected into this.
 * \param body Optional parameter for making a send request. If method is GET or
 * HEAD body is ignored
 */
void Request::send(QVariant body)
{
    if (isOpen() && mNam) {
        switch(mMethod) {
        case Method::INVALID:
            return;
        case Method::GET:
        case Method::HEAD:
        case Method::POST:
        case Method::PUT:
        case Method::PATCH:
        case Method::DELETE:
        case Method::CUSTOM:
            break;
        }
    }
}

/*!
 * \brief Request::requestHeader() Returns header value for \a header. It calls
 * \a\b QNetworkRequet::rawHeader() internally.
 * \param header
 * \return
 */
QByteArray Request::requestHeader(const QByteArray& header) const
{
    return mNRequest.rawHeader(header);
}

void Request::setNetworkAccessManager(QNetworkAccessManager* nam)
{
    mNam = nam;
}

}
