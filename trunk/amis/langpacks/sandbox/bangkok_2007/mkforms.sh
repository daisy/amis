#!/bin/sh

for lang in ja ms ne tl th zh; do
  echo $lang
  mkdir -p cgi/$lang
  cp ui-$lang.xml cgi/$lang/ui.xml
  xsltproc -o cgi/$lang/choose-form.html --param lang "'$lang'" --param action "'/cgi-bin/choose-form.pl'" form-list.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/container_file.html --param lang "'$lang'" --param type "'container'" --param id "'file'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=container_file'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/container_view.html --param lang "'$lang'" --param type "'container'" --param id "'view'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=container_view'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/container_play.html --param lang "'$lang'" --param type "'container'" --param id "'play'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=container_play'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/container_navigate.html --param lang "'$lang'" --param type "'container'" --param id "'navigate'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=container_navigate'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/container_bookmarks.html --param lang "'$lang'" --param type "'container'" --param id "'bookmarks'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=container_bookmarks'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/container_info.html --param lang "'$lang'" --param type "'container'" --param id "'info'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=container_info'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/container_help.html --param lang "'$lang'" --param type "'container'" --param id "'help'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=container_help'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/dialog_IDD_FIND.html --param lang "'$lang'" --param type "'dialog'" --param id "'IDD_FIND'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=dialog_IDD_FIND'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/dialog_IDD_CDSEARCH.html --param lang "'$lang'" --param type "'dialog'" --param id "'IDD_CDSEARCH'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=dialog_IDD_CDSEARCH'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/dialog_IDD_ABOUTBOX.html --param lang "'$lang'" --param type "'dialog'" --param id "'IDD_ABOUTBOX'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=dialog_IDD_ABOUTBOX'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/dialog_IDD_SEARCHDAISY.html --param lang "'$lang'" --param type "'dialog'" --param id "'IDD_SEARCHDAISY'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=dialog_IDD_SEARCHDAISY'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/dialog_IDD_SKIP.html --param lang "'$lang'" --param type "'dialog'" --param id "'IDD_SKIP'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=dialog_IDD_SKIP'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/dialog_IDD_PROPPAGE4.html --param lang "'$lang'" --param type "'dialog'" --param id "'IDD_PROPPAGE4'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=dialog_IDD_PROPPAGE4'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/dialog_IDD_PROPPAGE2.html --param lang "'$lang'" --param type "'dialog'" --param id "'IDD_PROPPAGE2'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=dialog_IDD_PROPPAGE2'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/dialog_IDD_CURRENTPOSITION.html --param lang "'$lang'" --param type "'dialog'" --param id "'IDD_CURRENTPOSITION'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=dialog_IDD_CURRENTPOSITION'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/dialog_IDD_PROPPAGE1.html --param lang "'$lang'" --param type "'dialog'" --param id "'IDD_PROPPAGE1'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=dialog_IDD_PROPPAGE1'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/dialog_IDD_NAVIGATIONOPTIONS.html --param lang "'$lang'" --param type "'dialog'" --param id "'IDD_NAVIGATIONOPTIONS'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=dialog_IDD_NAVIGATIONOPTIONS'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/dialog_IDD_GOTOPAGE.html --param lang "'$lang'" --param type "'dialog'" --param id "'IDD_GOTOPAGE'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=dialog_IDD_GOTOPAGE'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/dialog_IDD_FULLSUMMARY.html --param lang "'$lang'" --param type "'dialog'" --param id "'IDD_FULLSUMMARY'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=dialog_IDD_FULLSUMMARY'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/promptItemRefs.html --param lang "'$lang'" --param type "'promptItemRefs'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=promptItemRefs'" xhtml-form.xslt ui-$lang.xml
  xsltproc -o cgi/$lang/generalPrompts.html --param lang "'$lang'" --param type "'generalPrompts'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&form=generalPrompts'" xhtml-form.xslt ui-$lang.xml
done
