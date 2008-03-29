<?xml version="1.0" encoding="UTF-8"?>

<!-- Insert original text elements when they are missing. Also add the
language of the target at the top and set the language attribute of all text
elements (and notes) to English. Handles right-to-left languages as well
(at least Arabic and Urdu so far.) -->
<!-- $Id: original-text.xslt 2026 2007-07-17 16:23:38Z julienq $ -->

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="xml" version="1.0" encoding="UTF-8"/>

  <!-- Set the target language from a parameter -->
  <xsl:param name="lang"/>

  <!-- Set the text direction according to the language -->
  <xsl:variable name="dir">
    <xsl:choose>
      <xsl:when test="$lang='ar' or $lang='ur'">rtl</xsl:when>
      <xsl:otherwise>ltr</xsl:otherwise>
    </xsl:choose>
  </xsl:variable>

  <!-- Add an originalText element if needed -->
  <xsl:template match="*[./text and not(./originalText)]">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:for-each select="*">
        <xsl:if test="local-name()='text' and $dir='rtl'">
          <xsl:attribute name="dir">rtl</xsl:attribute>
        </xsl:if>
        <xsl:apply-templates select="."/>
        <xsl:if test="local-name()='text'">
          <originalText xml:lang="en">
            <xsl:if test="$dir='rtl'">
              <xsl:attribute name="dir">ltr</xsl:attribute>
            </xsl:if>
            <xsl:apply-templates/>
          </originalText>
        </xsl:if>
      </xsl:for-each>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="/*">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:attribute name="xml:lang">
        <xsl:value-of select="$lang"/>
      </xsl:attribute>
      <xsl:if test="$dir='rtl'">
        <xsl:attribute name="dir">rtl</xsl:attribute>
      </xsl:if>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="text|note|originalText">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:attribute name="xml:lang">en</xsl:attribute>
      <xsl:if test="$dir='rtl'">
        <xsl:attribute name="dir">ltr</xsl:attribute>
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
