#include "request.hpp"

#include <QNetworkAccessManager>

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
    : QObject { parent }, mNam(QNetworkAccessManagerPtr()), mMethodName(""),
      mMethod(Method::INVALID)
{
}

Request::Request(QNetworkAccessManagerPtr nam, int timeout, QObject* parent)
    : Request(parent)
{
    mNam = nam;
    if (timeout != 0) {
        mNRequest.setTransferTimeout(timeout);
    }
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
    mMethodName = method.toUtf8();
    if (mMethodName == "") {
        mMethod = Method::INVALID;
    } else if (mMethodName.compare("GET", Qt::CaseInsensitive) == 0) {
        mMethod = Method::GET;
    } else if (mMethodName.compare("HEAD", Qt::CaseInsensitive) == 0) {
        mMethod = Method::HEAD;
    } else if (mMethodName.compare("POST", Qt::CaseInsensitive) == 0) {
        mMethod = Method::POST;
    } else if (mMethodName.compare("PUT", Qt::CaseInsensitive) == 0) {
        mMethod = Method::PUT;
    } else if (mMethodName.compare("PATCH", Qt::CaseInsensitive) == 0) {
        mMethod = Method::PATCH;
    } else if (mMethodName.compare("DELETE", Qt::CaseInsensitive) == 0) {
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
        switch (mMethod) {
        case Method::INVALID:
            return;
        case Method::GET:
        case Method::HEAD:
            sendNoBodyRequest();
            break;
        case Method::POST:
        case Method::PUT:
        case Method::PATCH:
        case Method::DELETE:
            sendBodyRequest(body);
            break;
        case Method::CUSTOM:
            if (body.isNull() || !body.isValid()) {
                sendNoBodyRequest();
            } else {
                sendBodyRequest(body);
            }
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

void Request::setNetworkAccessManager(QNetworkAccessManagerPtr nam)
{
    mNam = nam;
}

/*!
 * \brief Request::setTimeout() Set transfer time out of this request. Zero
 * means no time out.
 * \param timeout Time in milliseconds
 */
void Request::setTimeout(int timeout)
{
    mNRequest.setTransferTimeout(timeout);
}

/*!
 * \brief Request::sendNoBodyRequest() This method is used by \ref
 * Request::send() method to send a request that doesn't need a body, like GET,
 * HEAD, etc
 */
void Request::sendNoBodyRequest()
{
    if (mNam) {
        QNetworkReply* reply;
        switch (mMethod) {
        case Method::GET:
        case Method::HEAD:
            reply = mNam->sendCustomRequest(mNRequest, mMethodName);
            break;
        default:
            return;
        }

        // Connect to signals of QNetworkReply
    }
}

/*!
 * \brief Request::sendBodyRequest() This method is used by \ref Request::send()
 * to perform sending a request that needs (has) a body
 * \param body
 */
void Request::sendBodyRequest(QVariant body)
{
    if (mNam) {
        QNetworkReply* reply;

        switch (mMethod) {
        case Method::POST:
        case Method::PUT:
        case Method::PATCH:
        case Method::DELETE:
        case Method::CUSTOM: {
            QByteArray contentType
                = mNRequest.header(QNetworkRequest::ContentTypeHeader)
                      .toByteArray();

            if (contentType.isEmpty()) {
                mNRequest.setHeader(
                    QNetworkRequest::ContentTypeHeader, "application/json");
            }

            if (contentType.startsWith("application/")
                || contentType.startsWith("text/")) {
            } else if (contentType.startsWith("multipart/")) {
            }
            break;
        }
        default:
            return;
        }
    }
}

}
