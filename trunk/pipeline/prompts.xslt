<?xml version="1.0" encoding="UTF-8"?>

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml" xmlns:amis="http://amisproject.org">

  <xsl:output method="xml" version="1.0" encoding="UTF-8"
    doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
    doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"/>

  <xsl:template match="/amis:ui">
    <html>
      <xsl:copy-of select="@xml:lang"/>
      <xsl:copy-of select="@dir"/>
      <head>
        <title>U.S. English</title>
        <meta name="dtb:uid" content="amis-localization-beta"/>
        <meta name="dc:Publisher" content="md"/>
        <meta name="dc:Date" content="2008-04-15"/>
        <meta name="dc:Language" content="en"/>
        <meta http-equiv="Content-type" content="text/html; charset=UTF-8"/>
      </head>
      <body>
        <xsl:apply-templates/>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="amis:text">
    <h1>
      <xsl:copy-of select="@xml:lang"/>
      <xsl:copy-of select="@dir"/>
      <xsl:value-of select="."/>
    </h1>
  </xsl:template>

  <xsl:template match="text()"/>

</xsl:transform>
