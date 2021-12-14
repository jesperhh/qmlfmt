import QtQuick 2.7

Item {
MouseArea {
onPressed: (mouse) => {
if (mouse.modifiers & Qt.AltModifier) {
if (mouse.button === Qt.LeftButton) {
state = 1;
} else if (mouse.button === Qt.RightButton) {
state = 2;
} else if (mouse.button === Qt.MiddleButton) {
state = 3;
}
}
}
}
}