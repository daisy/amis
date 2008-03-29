<?xml version="1.0" encoding="UTF-8"?>

<!-- Create an XHTML form for one container/dialog or prompts. -->
<!-- $Id$ -->

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml">

  <xsl:output method="xml" version="1.0" encoding="UTF-8"/>

  <xsl:param name="lang" value="''"/>
  <xsl:param name="action" value="''"/>

  <xsl:template match="/">
    <xsl:variable name="language">
      <xsl:choose>
        <xsl:when test="$lang='ar'">
          <xsl:text>Arabic</xsl:text>
        </xsl:when>
        <xsl:when test="$lang='bn'">
          <xsl:text>Bangla</xsl:text>
        </xsl:when>
        <xsl:when test="$lang='hi'">
          <xsl:text>Hindi</xsl:text>
        </xsl:when>
        <xsl:when test="$lang='id'">
          <xsl:text>Indonesian</xsl:text>
        </xsl:when>
        <xsl:when test="$lang='mi'">
          <xsl:text>Maori</xsl:text>
        </xsl:when>
        <xsl:when test="$lang='ta'">
          <xsl:text>Tamil</xsl:text>
        </xsl:when>
        <xsl:when test="$lang='ur'">
          <xsl:text>Urdu</xsl:text>
        </xsl:when>
        <xsl:when test="$lang='vi'">
          <xsl:text>Vietnamese</xsl:text>
        </xsl:when>
      </xsl:choose>
    </xsl:variable>
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
        <meta http-equiv="Content-Type" content="text/xml+html; charset=UTF-8"/>
        <title>
          <xsl:value-of select="concat('Amis localization interface for ',
            $language)"/>
        </title>
        <style type="text/css">
      body { background-color: white; color: black; font-family: sans-serif }
        </style>
      </head>
      <body>
        <h1>
          <xsl:text>Amis localization interface</xsl:text>
          <br/>
          <xsl:value-of select="$language"/>
        </h1>
        <p>Please choose a translation form below:</p>
        <form action="{$action}" method="get">
          <input type="hidden" name="lang" value="{$lang}"/>
          <p>
            <select name="form">
              <xsl:apply-templates/>
            </select>
          <input type="submit" value="Go"/>
          </p>
        </form>
      </body>
    </html>
  </xsl:template>

  <!-- Output a form for a container -->
  <xsl:template match="container">
    <option> 
      <xsl:attribute name="value">
        <xsl:value-of select="concat('container_',@id)"/>
      </xsl:attribute>
      <xsl:value-of select="concat('Container &#x201c;',caption/originalText,
        '&#x201d; (',@id,')')"/>
    </option>
  </xsl:template>

  <!-- Output a form for a dialog -->
  <xsl:template match="dialog">
    <option>
      <xsl:attribute name="value">
        <xsl:value-of select="concat('dialog_',@mfcid)"/>
      </xsl:attribute>
      <xsl:value-of select="concat('Dialog &#x201c;',caption/originalText,
        '&#x201d; (',@mfcid,')')"/>
    </option>
  </xsl:template>

  <!-- Output a form for prompt item references -->
  <xsl:template match="promptItemRefs">
    <option value="promptItemRefs">Prompt item references</option>
  </xsl:template>

  <!-- Output a form for general prompts -->
  <xsl:template match="generalPrompts">
    <option value="generalPrompts">General Prompts</option>
  </xsl:template>

  <!-- Skip text nodes, we have all the text we need -->
  <xsl:template match="text()"/>

</xsl:transform>
