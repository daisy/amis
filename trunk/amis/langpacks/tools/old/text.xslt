<?xml version="1.0" encoding="UTF-8"?>
<!-- $Id: text.xslt 2026 2007-07-17 16:23:38Z julienq $ -->
<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="text"/>
  <xsl:template match="text|note">
    <xsl:value-of select="concat(normalize-space(.),'&#xa;')"/>
  </xsl:template>
  <xsl:template match="text()"/>
</xsl:transform>
