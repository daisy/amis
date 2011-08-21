REM list of languages supported
REM "afr-ZA", "eng-AU", "zho-CN", "ice-IS", "tam-IN", "fra-FR", "nob-NO", "nno-NO", 
REM "jpn-JP", "spa", "zho-TW", "vie-VN", "fin-FI", "srp-RS", "swe-SE", "hun-HU", 
REM "nld-NL", "heb-IL"

REM the full installers
call make-default-amis-installer.bat
call make-amis-installer.bat afr-ZA Afrikaans amishelp-afrikaans.html kontras
call make-amis-installer.bat eng-AU "Australian English" amishelp.html contrast
call make-amis-installer.bat zho-CN Chinese amis.html contrast
call make-amis-installer.bat ice-IS Icelandic amis_frumbirtingarmynd_april09.html litasamsetning
call make-amis-installer.bat tam-IN Tamil amistamil.htm contrast
call make-amis-installer.bat fra-FR French content.html contraste
call make-amis-installer.bat nob-NO "Norwegian (Bokmaal)" amis_brukerveiledning_bkm.html kontrast
call make-amis-installer.bat nno-NO "Norwegian (Nynorsk)" amis_brukarrettleiing_nynorsk_bilete.html kontrast
call make-amis-installer.bat jpn-JP Japanese amishelp.html contrast
call make-amis-installer.bat spa "Espanol A L" amis_guia_del_usuario.html contraste
call make-amis-installer.bat zho-TW "Traditional Chinese" amis.htm contrast
call make-amis-installer.bat vie-VN Vietnamese ncc.html contrast
call make-amis-installer.bat fin-FI Suomi kopiokopiokorj.html kontrasti
call make-amis-installer.bat srp-RS Serbian amishelp.html contrast
call make-amis-installer.bat swe-SE Svenska amishelp.html kontrast
call make-amis-installer.bat hun-HU Magyar amis-hun-help3.html kontrasztos
call make-amis-installer.bat nld-NL Nederlands amis_gebruikers_handleiding.html contrast
call make-amis-installer.bat heb-IL Hebrew amishelp.html contrast

REM the langpacks
call setup-langpack.bat afr-ZA Afrikaans amishelp-afrikaans.html
call setup-langpack.bat eng-AU "Australian English" amishelp.html
call setup-langpack.bat zho-CN Chinese amis.html 
call setup-langpack.bat ice-IS Icelandic amis_frumbirtingarmynd_april09.html
call setup-langpack.bat tam-IN Tamil amistamil.htm
call setup-langpack.bat fra-FR French content.html
call setup-langpack.bat nob-NO "Norwegian (Bokmaal)" amis_brukerveiledning_bkm.html
call setup-langpack.bat nno-NO "Norwegian (Nynorsk)" amis_brukarrettleiing_nynorsk_bilete.html
call setup-langpack.bat jpn-JP Japanese amishelp.html
call setup-langpack.bat spa "Espanol A L" amis_guia_del_usuario.html
call setup-langpack.bat zho-TW "Traditional Chinese" amis.htm
call setup-langpack.bat vie-VN Vietnamese ncc.html
call setup-langpack.bat fin-FI Suomi kopiokopiokorj.html
call setup-langpack.bat srp-RS Serbian amishelp.html
call setup-langpack.bat swe-SE Svenska amishelp.html
call setup-langpack.bat hun-HU Magyar amis-hun-help3.html
call setup-langpack.bat nld-NL Nederlands amis_gebruikers_handleiding.html
call setup-langpack.bat heb-IL Hebrew amishelp.html


ECHO Done
