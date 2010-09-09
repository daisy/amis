ECHO off
SET LANGID=%1

echo %LANGID%

IF %LANGID%==afr-ZA (
    echo "Building Afrikaans"
    call make-amis-installer.bat afr-ZA Afrikaans amishelp-afrikaans.html kontras
    call setup-langpack.bat afr-ZA Afrikaans amishelp-afrikaans.html
) ELSE IF %LANGID%==eng-AU (
    echo "Building English (Australia)"
    call make-amis-installer.bat eng-AU "English (Australia)" amishelp.html contrast
    call setup-langpack.bat eng-AU "English (Australia)" amishelp.html
) ELSE IF %LANGID%==zho-CN (
    echo "Building Chinese"
    call make-amis-installer.bat zho-CN Chinese amis.html contrast
    call setup-langpack.bat zho-CN Chinese amis.html
) ELSE IF %LANGID%==ice-IS (
    echo "Building Icelandic"
    call make-amis-installer.bat ice-IS Icelandic amis_frumbirtingarmynd_april09.html litasamsetning
    call setup-langpack.bat ice-IS Icelandic amis_frumbirtingarmynd_april09.html
) ELSE IF %LANGID%==tam-IN (
    echo "Building Tamil"
    call make-amis-installer.bat tam-IN Tamil amistamil.htm contrast
    call setup-langpack.bat tam-IN Tamil amistamil.htm
) ELSE IF %LANGID%==fra-FR (
    echo "Building French"
    call make-amis-installer.bat fra-FR French content.html contraste
    call setup-langpack.bat fra-FR French content.html
) ELSE IF %LANGID%==nob-NO (
    echo "Building Norwegian (Bokmål)"
    call make-amis-installer.bat nob-NO "Norwegian (Bokmaal)" amis_brukerveiledning_bkm.html kontrast
    call setup-langpack.bat nob-NO "Norwegian (Bokmaal)" amis_brukerveiledning_bkm.html
) ELSE IF %LANGID%==nno-NO (
    echo "Building Norwegian (Nynorsk)"
    call make-amis-installer.bat nno-NO "Norwegian (Nynorsk)" amis_brukarrettleiing_nynorsk_bilete.html kontrast
    call setup-langpack.bat nno-NO "Norwegian (Nynorsk)" amis_brukarrettleiing_nynorsk_bilete.html
) ELSE IF %LANGID%==jpn-JP (
    echo "Building Japanese"
    call make-amis-installer.bat jpn-JP Japanese amishelp.html contrast
    call setup-langpack.bat jpn-JP Japanese amishelp.html
) ELSE IF %LANGID%==spa (
    echo "Building Spanish (Latin America)"
    call make-amis-installer.bat spa "Espanol A L" amis_guia_del_usuario.html contraste
    call setup-langpack.bat spa "Espanol A L" amis_guia_del_usuario.html
) ELSE IF %LANGID%==swe-SE (
    echo "Building Swedish"
    call make-amis-installer.bat swe-SE "Svenska" amishelp.html kontrast
    call setup-langpack.bat swe-SE "Svenska" amishelp.html
) ELSE IF %LANGID%==fin-FI ( 
	echo "Building Finnish"
    call make-amis-installer.bat fin-FI "Suomi" kopiokopiokorj.html kontrasti
    call setup-langpack.bat fin-FI "Suomi" kopiokopiokorj.html
) ELSE IF %LANGID%==vie-VN ( 
	echo "Building Vietnamese"
    call make-amis-installer.bat vie-VN "Vietnamese" ncc.html contrast
    call setup-langpack.bat vie-VN "Vietnamese" ncc.html
) ELSE IF %LANGID%==zho-TW ( 
	echo "Building Chinese (Traditional)"
    call make-amis-installer.bat zho-TW "Traditional Chinese" amis.htm contrast
    call setup-langpack.bat zho-TW "Traditional Chinese" amis.htm
) ELSE IF %LANGID%==srp-RS (
	echo "Building Serbian"
	call make-amis-installer.bat srp-RS "Serbian" dtbook.xml contrast
	call setup-langpack.bat srp-RS "Serbian" dtbook.xml
) ELSE IF %LANGID%==nld-NL (
  echo "Building Dutch"
  call make-amis-installer.bat nld-NL "Nederlands" amis_gebruikers_handleiding.html contrast
  call setup-langpack.bat nld-NL "Nederlands" amis_gebruikers_handleiding.html
) ELSE (
    echo "Language ID %LANGID% not recognized!"
)
