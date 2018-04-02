metadata {
    definition (name: "ESP8266 Sensor", namespace: "scott-allaire", author: "scott-allaire") {
        capability "Illuminance Measurement"
        capability "Temperature Measurement"
        capability "Relative Humidity Measurement"
        capability "Polling"
        capability "Refresh"
    }

    preferences {
        input("espip", "string",
            title: "ESP8266 IP Address",
            description: "ESP8266 IP Address",
            required: true,
            displayDuringSetup: true
        )
        input("espport", "string",
            title: "ESP8266 Port",
            description: "ESP8266 Port",
            required: true,
            displayDuringSetup: true
        )
        input("espmac", "string",
            title: "ESP8266 MAC Address",
            description: "ESP8266 MAC Address",
            required: true,
            displayDuringSetup: true
        )
    }

    simulator {

    }

    tiles {
        valueTile("temperature", "device.temperature", width: 1, height: 1) {
            state("temperature", label:'${currentValue}\u00b0', unit:"dF",
                backgroundColors:[
                    [value: 31, color: "#153591"],
                    [value: 44, color: "#1e9cbb"],
                    [value: 59, color: "#90d2a7"],
                    [value: 74, color: "#44b621"],
                    [value: 84, color: "#f1d801"],
                    [value: 95, color: "#d04e00"],
                    [value: 96, color: "#bc2323"]
                ]
            )
        }
        valueTile("humidity", "device.humidity", width: 1, height: 1) {
            state("humidity", label:'${currentValue}%\nHumidity', unit:"dF")
        }
        valueTile("luminance", "device.luminance", width: 1, height: 1) {
            state("luminance", label:'${currentValue}', unit:"dF",
                backgroundColors:[
                    [value: 30, color: "#000000"],
                    [value: 60, color: "#C0C0C0"],
                    [value: 90, color: "#FFFF00"]
                ]
            )
        }
        standardTile("refresh", "command.refresh", inactiveLabel: false) {
            state "default", label:'refresh', action:"refresh.refresh", icon:"st.secondary.refresh-icon"
        }
    }
}

def installed() {
    log.debug "Installed with settings: ${settings}"
    refresh()
}

def updated() {
    log.debug "Updated with settings: ${settings}"
    refresh()
}

// Poll for status
def poll() {
    log.debug("Get Status Notice")
    refresh()
}

// Returns the address of the hub itself
private String getCallBackAddress() {
    return device.hub.getDataValue("localIP") + ":" + device.hub.getDataValue("localSrvPortTCP")
}

def refresh() {
    log.debug "Refreshing"

    // Setting Network Device Id
    //def iphex = convertIPtoHex(rpiip)
    //def porthex = convertPortToHex(rpiport)
    //if (device.deviceNetworkId != "$iphex:$porthex") {
    //    device.deviceNetworkId = "$iphex:$porthex"
    //}
    device.deviceNetworkId = "$espmac";
    log.debug "Device Network Id set to ${device.deviceNetworkId}"

    def headers = [:]
    headers.put("HOST", "$espip:$espport")
    def hubAction = new physicalgraph.device.HubAction(
        method: "GET",
        path: "/",
        headers: headers
    )
    hubAction
}

// Parse events into attributes
def parse(String description) {
    log.debug "Parsing '${description}'"
    def msg = parseLanMessage(description)
    log.debug "data ${msg.data}"
    log.debug "headers ${msg.headers}"

    def evt1 = createEvent(name: "luminance", value: msg.data.luminance)
    def evt2 = createEvent(name: "temperature", value: msg.data.temp_f)
    def evt3 = createEvent(name: "humidity", value: msg.data.humidity)

    return [evt1, evt2, evt3]
}

