#include "qmlhttprequest.hpp"
#include "request.hpp"

#include "config.hpp"

namespace qhr {

/*!
 * \class QmlHttpRequest
 * \brief QmlHttpRequest class reponsible for creating a new \ref Request. It
 * can be used in QML to create a new \ref Request
 */

#if QT_VERSION_MAJOR == 5
void QmlHttpRequest::registerQmlHttpRequest()
{
    qmlRegisterSingletonType<QmlHttpRequest>(PROJECT_NAME,
        PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, "QmlHttpRequest",
        [](QQmlEngine* engine, QJSEngine* script) -> QmlHttpRequest* {
            if (auto engineNam = engine->networkAccessManager()) {
                auto instance = new QmlHttpRequest(engineNam);

                QQmlEngine::setObjectOwnership(
                    instance, QQmlEngine::JavaScriptOwnership);
                return instance;
            }
            return nullptr;
        });
    qmlRegisterUncreatableType<qhr::Request>("QmlHttpRequest",
        PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, "Request",
        "Request can not be created from QML");
}
#endif

#if QT_VERSION_MAJOR == 6
QmlHttpRequest* QmlHttpRequest::create(
    QQmlEngine* qmlEngine, QJSEngine* jsEngine)
{
    if (auto engineNam = qmlEngine->networkAccessManager()) {
        auto instance = new QmlHttpRequest(engineNam);

        QQmlEngine::setObjectOwnership(
            instance, QQmlEngine::JavaScriptOwnership);
        return instance;
    }
    return nullptr;
}
#endif

QmlHttpRequest::QmlHttpRequest(QNetworkAccessManager* nam)
    : QObject { nullptr }, mNam { nam }
{
}

/*!
 * \brief QmlHttpRequest::newRequest() Creates a new \ref Request object that
 * can be used to make HTTP request.
 * \note This \ref Request object should be deleted when done with using \ref
 * Request::destroy() from QML or \a\b Request::deleteLater()
 * \return A \ref Request
 */
Request* QmlHttpRequest::newRequest()
{
    return new Request(mNam);
}

/*!
 * \brief QmlHttpRequest::setDefaultTimeout() Set the default timeout for all
 * requests created using this class. Zero means no timeout.
 * \param timeout Time out in milliseconds
 */
void QmlHttpRequest::setDefaultTimeout(int timeout)
{
    mNam->setTransferTimeout(timeout);
}

void QmlHttpRequest::setNetworkAccessManager(QNetworkAccessManager *nam)
{
    mNam = nam;
}

QNetworkAccessManager *QmlHttpRequest::networkAccessManager() const
{
    return mNam;
}

/*!
 * \brief QmlHttpRequest::setRedirectPolicy() Set redirect policy.
 * \see \a\b QNetworkRequest::RedirectPolicy
 * \param rp
 */
void QmlHttpRequest::setRedirectPolicy(RedirectPolicy rp)
{
    mNam->setRedirectPolicy(QNetworkRequest::RedirectPolicy(rp));
}

QmlHttpRequest::RedirectPolicy QmlHttpRequest::redirectPolicy() const
{
    return RedirectPolicy(mNam->redirectPolicy());
}

}
