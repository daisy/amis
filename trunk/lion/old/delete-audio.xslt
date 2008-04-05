<?xml version="1.0" encoding="UTF-8"?>

<!-- Delete audio nodes from a file -->
<!-- $Id$ -->

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="xml" version="1.0" encoding="UTF-8"/>

  <xsl:template match="audio"/>

  <xsl:template match="*">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

</xsl:transform>
