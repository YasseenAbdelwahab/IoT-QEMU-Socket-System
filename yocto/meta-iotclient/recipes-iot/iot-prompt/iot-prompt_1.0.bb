SUMMARY = "Colored shell prompt for the Project 3 image"
LICENSE = "CLOSED"

SRC_URI = "file://color-prompt.sh"
S = "${WORKDIR}"

do_install() {
    install -d ${D}${sysconfdir}/profile.d
    install -m 0644 ${WORKDIR}/color-prompt.sh ${D}${sysconfdir}/profile.d/color-prompt.sh
}

FILES:${PN} = "${sysconfdir}/profile.d/color-prompt.sh"
