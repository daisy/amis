<?xml version="1.0" encoding="UTF-8"?>

<!-- Generate a script to produce all forms -->
<!-- $Id$ -->

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml">

  <xsl:output method="text" encoding="UTF-8"/>

  <xsl:template match="/">
    <xsl:text>#!/bin/sh&#xa;&#xa;</xsl:text>
    <xsl:text>for lang in ja ms ne tl th zh; do&#xa;</xsl:text>
    <xsl:text>  echo $lang&#xa;</xsl:text>
    <xsl:text>  mkdir -p cgi/$lang&#xa;</xsl:text>
    <xsl:text>  cp ui-$lang.xml cgi/$lang/ui.xml&#xa;</xsl:text>
    <xsl:text>  xsltproc -o cgi/$lang/choose-form.html --param lang "'$lang'" --param action "'/cgi-bin/choose-form.pl'" form-list.xslt ui-$lang.xml&#xa;</xsl:text>
    <xsl:apply-templates/>
    <xsl:text>done&#xa;</xsl:text>
  </xsl:template>

  <xsl:template name="outcmd">
    <xsl:param name="type"/>
    <xsl:param name="id"/>
    <xsl:text>  xsltproc -o cgi/$lang/</xsl:text>
    <xsl:value-of select="$type"/>
    <xsl:if test="$id">
      <xsl:value-of select="concat('_',$id)"/>
    </xsl:if>
    <xsl:text>.html --param lang "'$lang'" --param type "'</xsl:text>
    <xsl:value-of select="$type"/>
    <xsl:if test="$id">
      <xsl:text>'" --param id "'</xsl:text>
      <xsl:value-of select="$id"/>
    </xsl:if>
    <xsl:text>'" --param url "'/cgi-bin/choose-form.pl?lang=$lang&amp;form=</xsl:text>
    <xsl:value-of select="$type"/>
    <xsl:if test="$id">
      <xsl:value-of select="concat('_',$id)"/>
    </xsl:if>
    <xsl:text>'" xhtml-form.xslt ui-$lang.xml&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="container">
    <xsl:call-template name="outcmd">
      <xsl:with-param name="type">container</xsl:with-param>
      <xsl:with-param name="id" select="@id"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="dialog">
    <xsl:call-template name="outcmd">
      <xsl:with-param name="type">dialog</xsl:with-param>
      <xsl:with-param name="id" select="@mfcid"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="promptItemRefs">
    <xsl:call-template name="outcmd">
      <xsl:with-param name="type">promptItemRefs</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="generalPrompts">
    <xsl:call-template name="outcmd">
      <xsl:with-param name="type">generalPrompts</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <xsl:template match="text()"/>

</xsl:transform>
