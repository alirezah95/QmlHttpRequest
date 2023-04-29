#include "response.hpp"

namespace qhr {

/*!
 * \class Response
 * \brief Response class which holds the information of a network request, like
 * status code, status text, response text, etc
 */

Response::Response() : status(0) { }

void Response::clear()
{
    response = QVariant();
    responseText = QString();
    responseContent = QByteArray();
    responseType = QString();
    responseUrl = QUrl();
    statusText = QString();
    status = 0;
}

}
