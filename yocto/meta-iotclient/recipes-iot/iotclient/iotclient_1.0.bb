SUMMARY = "IoT temperature client (TCP/UDP) for Project 3"
LICENSE = "CLOSED"

SRC_URI = "file://iotclient-src"
S = "${WORKDIR}/iotclient-src"

do_compile() {
    oe_runmake client_app
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 client_app ${D}${bindir}/iot_client
}
