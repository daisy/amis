<?xml version="1.0" encoding="UTF-8"?>

<!-- Add ids to the ui.xml file so that we can match form results -->
<!-- $Id$ -->

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml">

  <xsl:output method="xml" version="1.0" encoding="UTF-8"/>

  <xsl:template match="text">
    <xsl:copy>
      <xsl:attribute name="id">
        <xsl:value-of select="concat('_',generate-id())"/>
      </xsl:attribute>
      <xsl:copy-of select="@*"/>
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
