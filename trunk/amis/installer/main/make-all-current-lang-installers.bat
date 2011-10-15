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
call make-amis-installer.bat nld-NL Nederlands amis_gebruikers_handleiding.html contrast
call make-amis-installer.bat heb-IL Hebrew amishelp.html contrast
call make-amis-installer.bat hun-HU Magyar amis-hun-help3.html kontrasztos


REM the langpacks
call make-amis-langpack.bat afr-ZA Afrikaans amishelp-afrikaans.html
call make-amis-langpack.bat eng-AU "Australian English" amishelp.html
call make-amis-langpack.bat zho-CN Chinese amis.html 
call make-amis-langpack.bat ice-IS Icelandic amis_frumbirtingarmynd_april09.html
call make-amis-langpack.bat tam-IN Tamil amistamil.htm
call make-amis-langpack.bat fra-FR French content.html
call make-amis-langpack.bat nob-NO "Norwegian (Bokmaal)" amis_brukerveiledning_bkm.html
call make-amis-langpack.bat nno-NO "Norwegian (Nynorsk)" amis_brukarrettleiing_nynorsk_bilete.html
call make-amis-langpack.bat jpn-JP Japanese amishelp.html
call make-amis-langpack.bat spa "Espanol A L" amis_guia_del_usuario.html
call make-amis-langpack.bat zho-TW "Traditional Chinese" amis.htm
call make-amis-langpack.bat vie-VN Vietnamese ncc.html
call make-amis-langpack.bat fin-FI Suomi kopiokopiokorj.html
call make-amis-langpack.bat srp-RS Serbian amishelp.html
call make-amis-langpack.bat swe-SE Svenska amishelp.html
call make-amis-langpack.bat nld-NL Nederlands amis_gebruikers_handleiding.html
call make-amis-langpack.bat heb-IL Hebrew amishelp.html
call make-amis-langpack.bat hun-HU Magyar amis-hun-help3.html

ECHO Done
