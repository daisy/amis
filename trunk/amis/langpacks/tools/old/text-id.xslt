<?xml version="1.0" encoding="UTF-8"?>

<!-- Add ids to the text elements in the ui.xml file -->

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="xml" version="1.0" encoding="UTF-8"/>

  <!-- Add the next id at the root to easily add new ids. -->
  <xsl:template match="/*">
    <xsl:copy>
      <xsl:attribute name="next-id">
        <xsl:value-of select="concat('t',count(//text)+1)"/>
      </xsl:attribute>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <!-- Generate a new id for text elements, except for mnemonics since they
  don't need to be translated. We generate our own ids so that we are sure
  that they are the same for all translation files. -->
  <xsl:template match="text[not(parent::mnemonic)]">
    <xsl:copy>
      <xsl:attribute name="id">
        <xsl:text>t</xsl:text>
        <xsl:number level="any"/>
      </xsl:attribute>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <!-- Remove originalText while we're at it. -->
  <xsl:template match="originalText"/>

  <!-- Copy everything else as is. -->
  <xsl:template match="*">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

</xsl:transform>
