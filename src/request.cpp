#include "request.hpp"

#include <QFile>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QMimeDatabase>
#include <QNetworkAccessManager>
#include <QNetworkReply>

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
      mMethod(Method::INVALID), mNReply(nullptr)
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

Request::~Request()
{
    if (mNReply) {
        if (mNReply->isRunning()) {
            mNReply->abort();
        }

        mNReply->deleteLater();
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
void Request::send(const QVariant& body)
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

        // Connect to signals of QNetworkReply
        if (mNReply) {
            setupReplyConnections();
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

int Request::replyStatus() const
{
    if (mNReply->isFinished()) {
        QVariant status
            = mNReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        if (status.isValid()) {
            return status.toInt();
        }
    }
    return 0;
}

QString Request::replyStatusText() const
{
    if (mNReply->isFinished()) {
        QVariant statusText
            = mNReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute);

        if (statusText.isValid()) {
            return statusText.toString();
        }
    }
    return QString();
}

QString Request::replyResponseText() const
{
    if (mNReply->isFinished()) {
        return mNReply->readAll();
    }
    return QString();
}

/*!
 * \brief Request::sendNoBodyRequest() This method is used by \ref
 * Request::send() method to send a request that doesn't need a body, like GET,
 * HEAD, etc
 */
void Request::sendNoBodyRequest()
{
    if (mNam) {
        switch (mMethod) {
        case Method::GET:
        case Method::HEAD:
            mNReply = mNam->sendCustomRequest(mNRequest, mMethodName);
            break;
        default:
            break;
        }

        if (mNReply) {
            setupReplyConnections();
        }
    }
}

/*!
 * \brief Request::sendBodyRequest() This method is used by \ref Request::send()
 * to perform sending a request that needs (has) a body
 * \param body
 */
void Request::sendBodyRequest(const QVariant& body)
{
    if (mNam) {
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
                sendBodyRequestMultipart(body);
            }
            break;
        }
        default:
            break;
        }

        if (mNReply) {
            setupReplyConnections();
        }
    }
}

/*!
 * \brief Request::sendBodyRequestText() This method should be used when the
 * content-type of this request is text type, like \a application/json,
 * text/plain, etc
 * \param body The body to be sent with this request. The \a body will be
 * converted to \a\b QByteArray
 */
void Request::sendBodyRequestText(const QVariant& body)
{
    return;
}

/*!
 * \brief Request::sendBodyRequestMultipart() This method should be used when
 * the content-type of this request is multipart type, like \a
 * multipart/form-data, etc
 * \param body The body to be sent the request. It will be converted to a \a\b
 * QHttpMultiPart data
 */
void Request::sendBodyRequestMultipart(const QVariant& body)
{
    // Setting up the QHttpMultiPart body
    QHttpMultiPart* mpBody = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    const auto& bodyMap = body.toMap();
    if (!bodyMap.isEmpty()) {
        for (auto it = bodyMap.constKeyValueBegin();
             it != bodyMap.constKeyValueEnd(); ++it) {
            QHttpPart part;

            QUrl url = it->second.toUrl();
            if (url.isValid() && url.isLocalFile()) {
                // Open file and set it to QHttpPart
                QFile* file = new QFile(url.toLocalFile());
                if (!file->exists() || !file->open(QFile::ReadOnly)) {
                    qWarning() << "Cannot open file: " << url;
                    continue;
                }

                auto mimeType = QMimeDatabase().mimeTypeForData(file);

                part.setHeader(
                    QNetworkRequest::ContentTypeHeader, mimeType.name());
                part.setHeader(QNetworkRequest::ContentDispositionHeader,
                    QString("form-data; name=\"" + it->first
                        + "\"; filename=\"%1\"")
                        .arg(file->fileName()));
                part.setBodyDevice(file);

                // Set file parent to mpBody so it deleted automatically
                file->setParent(mpBody);
            } else {
                part.setHeader(
                    QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
                part.setHeader(QNetworkRequest::ContentDispositionHeader,
                    QString("form-data; name=\"") + it->first + "\"");
                part.setBody(it->second.toByteArray());
            }

            // Append new part to multi part body
            mpBody->append(part);
        }
    }

    /*!
     * \internal
     * \warning Multipart form-data content type needs a boundary so it is
     * essential to update the boundary of this content type with the
     * boundary of \a\b QHttpMultiPart
     */
    mNRequest.setHeader(QNetworkRequest::ContentTypeHeader,
        QString("multipart/form-data; boundary=" + mpBody->boundary()));

    // Send multipart data request
    mNReply = mNam->sendCustomRequest(mNRequest, mMethodName, mpBody);
    mpBody->setParent(mNReply);
    return;
}

/*!
 * \brief Request::setupReplyConnections() Set up required connections for \a\b
 * QNetworkReply to call related callbacks if they exist.
 */
void Request::setupReplyConnections()
{
    connect(mNReply, &QNetworkReply::finished, this, &Request::onReplyFinished);

    connect(mNReply, &QNetworkReply::errorOccurred, this,
        &Request::onReplyErrorOccured);

    connect(
        mNReply, &QNetworkReply::redirected, this, &Request::onReplyRedirected);

    connect(mNReply, &QNetworkReply::downloadProgress, this,
        &Request::onReplyDownloadProgress);

    connect(mNReply, &QNetworkReply::uploadProgress, this,
        &Request::onReplyUploadProgress);
}

void Request::onReplyFinished()
{
    if (mFinishedCallback.isCallable()) {
        mFinishedCallback.call();
    }
}

void Request::onReplyErrorOccured(int error)
{
    if (mNReply->error() == QNetworkReply::TimeoutError) {
        // If time out is reached only call timeout callback
        if (mTimeoutCallback.isCallable()) {
            mTimeoutCallback.call();
            return;
        }
    }

    if (mNReply->error() == QNetworkReply::OperationCanceledError) {
        // If operation was aborted
        if (mAbortedCallback.isCallable()) {
            mAbortedCallback.call();
            return;
        }
    }

    if (mErrorCallback.isCallable()) {
        mErrorCallback.call({
            mNReply->error(),
            mNReply->errorString(),
        });
    }
}

void Request::onReplyRedirected(const QUrl& url)
{
    if (mRedirectedCallback.isCallable()) {
        mRedirectedCallback.call({
            url.toString(),
        });
    }
}

void Request::onReplyDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (mDownloadProgressChangedCallback.isCallable()) {
        mDownloadProgressChangedCallback.call({
            double(bytesReceived),
            double(bytesTotal),
        });
    }
}

void Request::onReplyUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    if (mUploadProgressChangedCallback.isCallable()) {
        mUploadProgressChangedCallback.call({
            double(bytesSent),
            double(bytesTotal),
        });
    }
}

}

bool operator!=(const QJSValue& left, const QJSValue& right)
{
    return !left.equals(right);
}
