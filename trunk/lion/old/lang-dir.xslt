<?xml version="1.0" encoding="UTF-8"?>

<!-- Clean up the language/dir attributes -->
<!-- $Id$ -->

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="xml" version="1.0" encoding="UTF-8"/>

  <xsl:template match="note|originalText|text">
    <xsl:copy>
      <xsl:copy-of select="@*[not(name()='xml:lang' or name()='dir')]"/>
      <xsl:if test="@xml:lang='en'">
        <xsl:attribute name="xml:lang">en</xsl:attribute>
        <xsl:if test="/*/@dir='rtl'">
          <xsl:attribute name="dir">ltr</xsl:attribute>
        </xsl:if>
      </xsl:if>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="*">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

</xsl:transform>
