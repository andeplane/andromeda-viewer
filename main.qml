import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import Controller 1.0

Item {
    id: controllerRoot

    property real aspectRatio: width/height
    property bool applicationActive: {
        if(Qt.platform.os === "android" || Qt.platform.os === "ios") {
            if(Qt.application.state === Qt.ApplicationActive) {
                return true
            } else {
                return false
            }
        } else {
            return true
        }
    }

    width: 1080
    height: 1080

    focus: true

    Component.onCompleted: {
        console.log("Controller started.")
        console.log("Platform: " + Qt.platform.os)
    }

    Controller {
        id: controller
        anchors.fill: parent

        running: true

        function moveCamera(newPos) {
            console.log("Trying to move camera to (x,y)=("+newPos.x+","+newPos.y+") with z="+controller.cameraPos.z);
            var edgeX1 = newPos.x + controller.cameraPos.z
            var edgeY1 = newPos.y + controller.cameraPos.z

            var edgeX2 = controller.cameraPos.z - newPos.x
            var edgeY2 = controller.cameraPos.z - newPos.y

            if(edgeX1 > 1.0) {
                newPos.x = 1.0 - controller.cameraPos.z
            } else if(edgeX2 > 1.0) {
                newPos.x = controller.cameraPos.z - 1.0
            }

            if(edgeY1 > 1.0) {
                newPos.y = 1.0 - controller.cameraPos.z
            } else if(edgeY2 > 1.0) {
                newPos.y = controller.cameraPos.z - 1.0
            }

            controller.cameraPos.x = newPos.x
            controller.cameraPos.y = newPos.y
        }

        Timer {
            id: timer
            property real lastTime: Date.now()
            property real lastSampleTime: Date.now()
            running: controller.running && controllerRoot.applicationActive
            repeat: true
            interval: 1
            onTriggered: {
                if(!controller.previousStepCompleted) {
                    return
                }

                var currentTime = Date.now()
                var dt = currentTime - lastTime
                dt /= 1000
                controller.step(dt)

                var sampleTimeDifference = (currentTime - lastSampleTime)/1000
                lastTime = currentTime
            }
        }
    }

    Keys.onPressed: {
        if(event.key === Qt.Key_1) {
            controller.setRenderSky(!controller.renderSky)
        } else if(event.key === Qt.Key_2) {
            controller.setRenderAndromeda1x(!controller.renderAndromeda1x)

        } else if(event.key === Qt.Key_3) {
            controller.setRenderAndromeda2x(!controller.renderAndromeda2x)
        } else if(event.key === Qt.Key_4) {
            controller.setRenderAndromeda3x(!controller.renderAndromeda3x)
        }
    }

    MouseArea {
        property real minZoom: 0.00001
        property real maxZoom: 1.0
        property real mouseStartX: 0.0
        property real mouseStartY: 0.0
        property real cameraStartX: 0.0
        property real cameraStartY: 0.0
        width: parent.width
        height: parent.height
        onWheel: {
            var mousePos = scaledMousePos(wheel)

            var oldZoom = controller.cameraPos.z
            var newZoom = oldZoom*(1+wheel.angleDelta.y*0.001);
            if(newZoom < minZoom) newZoom = minZoom;
            if(newZoom > maxZoom) newZoom = maxZoom;

            var ratio = oldZoom
            // Mouse cursor image coordinates (scale invariant)
            var x = mousePos.x*ratio - controller.cameraPos.x
            var y = mousePos.y*ratio - controller.cameraPos.y
            // Distance to focus center
            var xFromFocus = x + controller.cameraPos.x
            var yFromFocus = y + controller.cameraPos.y

            var tanThetaX = xFromFocus / oldZoom
            var tanThetaY = yFromFocus / oldZoom
            var deltaZ = newZoom - oldZoom
            var deltaX = deltaZ*tanThetaX
            var deltaY = deltaZ*tanThetaY
            var newCameraPosX = controller.cameraPos.x + deltaX
            var newCameraPosY = controller.cameraPos.y + deltaY

            controller.cameraPos.z = newZoom
            controller.moveCamera(Qt.vector2d(newCameraPosX, newCameraPosY))
        }

        function scaledMousePos(mouse) {
            var mouseX = 2*( (mouse.x - 0.5*controllerRoot.width) / controllerRoot.width)
            var mouseY = 2*( (mouse.y - 0.5*controllerRoot.height) / controllerRoot.height)
            return Qt.vector2d(mouseX, mouseY)
        }

        onPressed: {
            var mousePos = scaledMousePos(mouse);

            mouseStartX = mousePos.x
            mouseStartY = mousePos.y
            cameraStartX = controller.cameraPos.x
            cameraStartY = controller.cameraPos.y
            console.log("Clicked mouse at "+mouseStartX+", "+mouseStartY)
        }

        onMouseXChanged: {
            var mousePos = scaledMousePos(mouse);

            var deltaX = (mousePos.x - mouseStartX)*controller.cameraPos.z
            var deltaY = (mousePos.y - mouseStartY)*controller.cameraPos.z

            var newCameraPosX = cameraStartX + deltaX
            var newCameraPosY = cameraStartY + deltaY
            controller.moveCamera(Qt.vector2d(newCameraPosX, newCameraPosY))
        }
    }
}

