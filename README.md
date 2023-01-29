# QmlHttpRequest

[![License](https://img.shields.io/github/license/alirezah95/QmlHttpRequest?label=License)](https://opensource.org/licenses/MIT)

***QmlHttpRequest*** is a [XMLHttpRequest](https://doc.qt.io/qt-6/qtqml-javascript-qmlglobalobject.html#xmlhttprequest) module for ***Qml*** providing more capabilities than the [default](https://doc.qt.io/qt-6/qtqml-javascript-qmlglobalobject.html#xmlhttprequest) provided to ***Qml***, including the ability to set time out, upload files in [multipart/form-data](https://www.ietf.org/rfc/rfc2388.txt) format, get download or upload progress information.

## How To Use QmlHttpRequest
To include [QmlHttpRequest](https://github.com/alirezah95/QmlHttpRequest), clone it into your project or add it as a *Git submodule* using the following command:

    git submodule add https://github.com/alirezah95/QmlHttpRequest

add it to **CMake** as a subdirectory:

    add_subdirectory("QmlHttpRequest")

import it into Qml file and start using it!

## Example
- Sending a simple **GET** or **POST** request with a body of type **application/json**
    ```qml
    import QtQuick
    import QtQuick.Controls

    import QmlHttpRequest

    Window {
        id: idRoot

        width: 600
        height: 500
        visible: true

        Button {
            text: "Send Request"

            onClicked: {
                var qhr = QmlHttpRequest.newRequest()

                qhr.open("POST", "https://example.org")
                qhr.setRequestHeader("Content-Type", "application/json")
                qhr.setRequestHeader("Accept", "*/*")

                qhr.timeout = 20000 // In milliseconds

                qhr.onreadystatechange = function() {
                    if (qhr.readyState === QmlHttpRequest.Done) {
                        print(`code: ${qhr.status}, status text: ${qhr.statusText}`)
                        print(`Response: ${qhr.responseText}`)
                    }
                }

                var body = {
                    "first": "Value",
                    "second": 2,
                    "third": true
                }

                qhr.send(JSON.stringify(body))
            }
        }
    }
    ```
- Sending a **POST** request as **multipart/form-data** to upload a file:
    ```qml
    import QtQuick
    import QtQuick.Controls

    import QmlHttpRequest

    Window {
        id: idRoot

        width: 600
        height: 500
        visible: true

        Button {
            text: "Upload File"

            onClicked: {
                var qhr = QmlHttpRequest.newRequest()

                qhr.open("POST", "https://example.org")
                qhr.setRequestHeader("Content-Type", "multipart/form-data")
                qhr.setRequestHeader("Accept", "*/*")

                qhr.timeout = 20000 // In milliseconds

                qhr.onreadystatechange = function() {
                    if (qhr.readyState === QmlHttpRequest.Done) {
                        print(`code: ${qhr.status}, status text: ${qhr.statusText}`)
                        print(`Response: ${qhr.responseText}`)
                    }
                }

                qhr.onuploadprogress = function(sentBytes, totalBytes) {
                    // Update a ProgressBar
                }

                var body = {
                    "text_field": "Value",
                    "file_filed": "url-to-local-file" // Url to a file usually retrieved from  a FileDailog
                }

                qhr.send(JSON.stringify(body))
            }
        }
    }
    ```


## Port from XMLHttpRequest to QmlHttpRequest
To replace [XMLHttpRequest](https://doc.qt.io/qt-6/qtqml-javascript-qmlglobalobject.html#xmlhttprequest) by **QmlHttpRequest** in existing projects two steps are required:
- First, import **QmlHttpRequest** module
- Second, replace all `new XMLHttpRequest()` statements with `QmlHttpRequest.newRequest()`

## To do
- [ ] Retrieve and store all response headers when [Request::readyState](src/request.hpp) is `QmlHttpRequest.HeadersReceived`
- [ ] Add a separate class to handle creating form data
- [ ] Support more content types

