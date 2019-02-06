import QtQuick 2.5
import QtQuick.Window 2.2
import "qrc:/test/"

Window {
	visible: true

	MouseArea {
		anchors.fill: parent
		onClicked: {	Qt.quit()
		/* test
				Testing
				Testing */
		}
	}
	Test {
	}

	Text {
		text: qsTr("Hello World")
		anchors.centerIn: parent
	}
}
