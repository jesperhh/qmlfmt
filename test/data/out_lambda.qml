import QtQuick 2.7

Item {
    MouseArea {
        onPressed: mouse => console.log("Hello World")
    }
}
