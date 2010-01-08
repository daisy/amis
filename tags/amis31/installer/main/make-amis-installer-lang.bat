ECHO off
SET LANGID=%1

["afrikaans", "afr-ZA"],
["australian", "eng-AU"],
["chinese", "zho-CN"],
["icelandic", "ice-IS"],
["tamil", "tam-IN"],
["french", "fra-FR"],
["norwegian-bokmal", "nob-NO"],
["norwegian-nynorsk", "nno-NO"]

echo %LANGID%

IF %LANGID%==afr-ZA (
    echo "Building Afrikaans"
    call make-amis-installer.bat afr-ZA Afrikaans amishelp-afrikaans.html teenstelling
    call setup-amis-langpack.bat afr-ZA Afrikaans amishelp-afrikaans.html
) ELSE IF %LANGID%==eng-AU (
    echo "Building English (Australia)"
    call make-amis-installer.bat eng-AU "English (Australia)" amishelp.html contrast
    call setup-amis-langpack.bat eng-AU "English (Australia)" amishelp.html
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
    call make-amis-installer.bat nob-NO "Norwegian (Bokmål)" amis_brukerveiledning_bkm.html kontrast
    call setup-langpack.bat nob-NO "Norwegian (Bokmål)" amis_brukerveiledning_bkm.html
) ELSE IF %LANGID%==nno-NO (
    echo "Building Norwegian (Nynorsk)"
    call make-amis-installer.bat ice-IS Icelandic amis_brukarrettleiing_nynorsk_bilete.html kontrast
    call setup-langpack.bat ice-IS Icelandic amis_brukarrettleiing_nynorsk_bilete.html
) ELSE (
    echo "Language ID %LANGID% not recognized!"
)