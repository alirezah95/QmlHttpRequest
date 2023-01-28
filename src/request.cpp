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

/*!
 * \brief Create an invalid, not-open \ref Request object
 * \param parent
 */
Request::Request(QObject* parent)
    : QObject { parent }, mNam(QNetworkAccessManagerPtr()), mMethodName(""),
      mMethod(Method::INVALID), mNReply(nullptr)
{
}

/*!
 * \brief Initialize an object of this calss inject \a nam as \a\b
 * QNetworkAccessManager and \a timeout for transfert timeout
 * \param nam
 * \param timeout
 * \param parent
 */
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
    abort();
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

    mState = State::Opened;
    mUrl = url;
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
    if (mNReply) {
        abort();
    }

    mNRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                           QNetworkRequest::ManualRedirectPolicy);
    mNRequest.setUrl(mUrl);
    mBody = body;

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
                sendBodyRequest(mBody);
            }
        }

        // Connect to signals of QNetworkReply
        if (mNReply) {
            setupReplyConnections();
        }
    }
}

void Request::abort()
{
    if (mNReply) {
        if (mNReply->isRunning()) {
            mNReply->abort();
        }

        mNReply->deleteLater();

        mResponse.status = 0;
        mResponse.statusText = "";
        mResponse.responseText = "{ \"detail\": \"Operation aborted\" }";
    }
}

void Request::destroy()
{
    deleteLater();
}

bool Request::isOpen() const
{
    return mMethod != Method::INVALID && mUrl.isValid();
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

/*!
 * \brief Set interanl network access manager to \a nam
 * \param nam
 */
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
 * \brief Returns the response object of this network request. Currently is
 * returns the results of the call to \a\b QNetworkReply::readAll()
 * \todo Change this method to return a byte array, an js object based on
 * network reply returnted content type
 * \return
 */
QVariant Request::response() const
{
    if (mNReply) {
        // Request is sent
        return mNReply->readAll();
    }
    return mResponse.response;
}

/*!
 * \brief Returns the response text of this request if content type is 'text'
 * and an empty string if request is not set or was unsuccessful
 * \return
 */
QString Request::responseText() const
{
    if (mNReply) {
        // Request is sent
        return mNReply->readAll();
    }
    return mResponse.responseText;
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
                sendBodyRequestText(body);
            } else if (contentType.startsWith("multipart/")) {
                sendBodyRequestMultipart(body);
            }
            break;
        }
        default:
            break;
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
    mNReply
        = mNam->sendCustomRequest(mNRequest, mMethodName, body.toByteArray());
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
    auto contentTypeHdr
        = mNRequest.header(QNetworkRequest::ContentTypeHeader).toString();

    if (contentTypeHdr == "multipart/form-data") {
        // Setting up the QHttpMultiPart body
        QHttpMultiPart* mpBody
            = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        if (body.canConvert<QVariantMap>()) {
            multipartAddObject(
                mpBody, "", QJsonObject::fromVariantMap(body.toMap()));
        } else if (body.canConvert<QVariantList>()) {
            multipartAddArray(
                mpBody, "", QJsonArray::fromVariantList(body.toList()));
        } else if (body.canConvert<QJsonValue>()) {
            multipartAddValue(mpBody, "", QJsonValue::fromVariant(body));
        } else {
            delete mpBody;
            return;
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
    }
    return;
}

void Request::multipartAddObject(
    QHttpMultiPart* mpBody, QString prefix, const QJsonObject& body)
{
    for (auto it = body.constBegin(); it != body.constEnd(); ++it) {
        QString fieldName
            = prefix == "" ? it.key() : QString("%1.%2").arg(prefix, it.key());
        switch (it.value().type()) {
        case QJsonValue::Bool:
        case QJsonValue::Double:
        case QJsonValue::String:
            multipartAddValue(mpBody, fieldName, it.value());
            break;
        case QJsonValue::Object:
            multipartAddObject(mpBody, fieldName, it.value().toObject());
            break;
        case QJsonValue::Array:
            multipartAddArray(mpBody, fieldName, it.value().toArray());
            break;
        case QJsonValue::Null:
        case QJsonValue::Undefined:
            break;
        }
    }

    return;
}

void Request::multipartAddArray(
    QHttpMultiPart* mpBody, QString prefix, const QJsonArray& body)
{
    QJsonDocument doc;
    doc.setArray(body);

    QHttpPart part;
    part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part.setHeader(QNetworkRequest::ContentDispositionHeader,
        QString("form-data; name=\"") + prefix + "\"");
    part.setBody(doc.toJson());

    mpBody->append(part);
    return;
}

void Request::multipartAddValue(
    QHttpMultiPart* mpBody, QString prefix, const QJsonValue& body)
{
    QHttpPart part;
    if (body.isString()) {
        if (QUrl url(body.toString()); url.isValid() && url.isLocalFile()) {
            // Open file and set it to QHttpPart
            QFile* file = new QFile(url.toLocalFile());
            if (!file->exists() || !file->open(QFile::ReadOnly)) {
                qWarning() << "Cannot open file: " << url;
            }

            auto mimeType = QMimeDatabase().mimeTypeForData(file);

            part.setHeader(QNetworkRequest::ContentTypeHeader, mimeType.name());
            part.setHeader(QNetworkRequest::ContentDispositionHeader,
                QString("form-data; name=\"" + prefix + "\"; filename=\"%1\"")
                    .arg(file->fileName()));
            part.setBodyDevice(file);

            // Set file parent to mpBody so it deleted automatically
            file->setParent(mpBody);

            mpBody->append(part);
            return;
        }
    }

    part.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
    part.setHeader(QNetworkRequest::ContentDispositionHeader,
        QString("form-data; name=\"") + prefix + "\"");
    part.setBody(body.toVariant().toString().toUtf8());

    mpBody->append(part);
    return;
}

/*!
 * \brief Request::setupReplyConnections() Set up required connections for \a\b
 * QNetworkReply to call related callbacks if they exist.
 */
void Request::setupReplyConnections()
{
    connect(mNReply, &QNetworkReply::readyRead, this, &Request::onReplyReadReady);
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

void Request::callCallback(QJSValue cb, const QJSValueList& args)
{
    if (cb.isCallable()) {
        QJSValue result = cb.call(args);

        if (result.isError()) {
            qDebug("%s:%s: %s",
                qPrintable(result.property("fileName").toString()),
                qPrintable(result.property("lineNumber").toString()),
                qPrintable(result.toString().toStdString().c_str()));
        }
    }
}

void Request::onReplyReadReady()
{
    mResponse.status
        = mNReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    mResponse.statusText
        = mNReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute)
              .toString();

    if (mState < State::HeadersReceived) {
        mState = State::HeadersReceived;
        // Call onreadystatuchange callback
        callCallback(mReadyStateCb);
    }
}

/*!
 * \brief Request::onReplyFinished() Connets to \a\b QNetworkReply::finished()
 * signal and call \ref onFinished callback if any
 */
void Request::onReplyFinished()
{
    QVariant redirect
        = mNReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (redirect.isValid()) {
        QUrl url = mNReply->url().resolved(redirect.toUrl());
        if (!url.isLocalFile()) {
            // See http://www.ietf.org/rfc/rfc2616.txt, section 10.3.4 "303 See
            // Other": Result of 303 redirection should be a new "GET" request.
            const QVariant code = mNReply->attribute(
                QNetworkRequest::HttpStatusCodeAttribute);
            int codeInt = code.toInt();
            if (code.isValid() && codeInt == 303 && mMethodName != "GET") {
                mMethodName = "GET";
                mMethod = Method::GET;
            }
            mNReply->disconnect();

            mUrl = url;

            send(mBody);
            return;
        }
    }

    // Store mNReply results inside mReponse and delete mNReply
    if (mNReply->error() == QNetworkReply::NoError) {
        mResponse.response = QVariantMap();
        mResponse.responseText = mNReply->readAll();
        mResponse.responseUrl = mNReply->url();
        mResponse.responseType = mNReply->rawHeader("Content-Type");
    } else {
        mResponse.response = QVariantMap();
        mResponse.responseText = mNReply->readAll();
        mResponse.responseUrl = mNReply->url();
    }

    mResponse.status
        = mNReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    mResponse.statusText
        = mNReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute)
              .toString();

    mNReply->deleteLater();
    mNReply = nullptr;

    mState = State::Done;

    // Call ready state callback
    callCallback(mReadyStateCb);
}

/*!
 * \brief Request::onReplyErrorFinished() Connets to \a\b
 * QNetworkReply::errorOccurred(int) signal and call \ref onError callback if
 * any
 */
void Request::onReplyErrorOccured(int error)
{
    if (mNReply->error() == QNetworkReply::TimeoutError) {
        // If time out is reached only call timeout callback
        if (mTimeoutCb.isCallable()) {
            // Call timeout callback
            callCallback(mTimeoutCb);
            return;
        }
    }

    if (mNReply->error() == QNetworkReply::OperationCanceledError) {
        // If operation was aborted
        if (mAbortedCb.isCallable()) {
            // Call aborted callback
            callCallback(mAbortedCb);
            return;
        }
    }

    if (mErrorCb.isCallable()) {
        // Call error callback
        callCallback(mErrorCb, {
            mNReply->error(),
            mNReply->errorString(),
        });
    }
}

/*!
 * \brief Request::onReplyRedirected() Connets to \a\b
 * QNetworkReply::redirected(QUrl) signal and call \ref onRedirected callback if
 * any
 */
void Request::onReplyRedirected(const QUrl& url)
{
    callCallback(mRedirectedCb,
        {
            url.toString(),
        });
}

/*!
 * \brief Request::onReplyDownloadProgress() Connets to \a\b
 * QNetworkReply::downloadProgress(qint64, qint64) signal and call \ref
 * onDownloadProgressChanged callback if any
 */
void Request::onReplyDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    callCallback(mDownloadProgressCb,
        {
            double(bytesReceived),
            double(bytesTotal),
        });
}

/*!
 * \brief Request::onReplyUploadProgress() Connets to \a\b
 * QNetworkReply::uploadProgress(qint64, qint64) signal and call \ref
 * onUploadProgressChanged callback if any
 */
void Request::onReplyUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    callCallback(mUploadProgressCb,
        {
            double(bytesSent),
            double(bytesTotal),
        });
}

}

/*!
 * \related Request
 * \brief operator != Implement != operator for \a\b QJSValue so using a
 * QJSValue directly in a \a\b Q_PROPERTY as a MEBMER is possible
 * \param left
 * \param right
 * \return
 */
bool operator!=(const QJSValue& left, const QJSValue& right)
{
    return !left.equals(right);
}
