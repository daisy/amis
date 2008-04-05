<?xml version="1.0" encoding="UTF-8"?>

<!-- Split the accelerator and mnemonic attributes from the language element
to make them siblings and then allow them to have a dedicated audio file -->
<!-- $Id$ -->

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="xml" version="1.0" encoding="UTF-8"/>

  <xsl:template match="*[@accelerator or caption[text[@mnemonic]]]">
    <xsl:copy>
      <xsl:copy-of select="@*[not(name()='accelerator')]"/>
      <xsl:apply-templates/>
      <xsl:if test="@accelerator">
        <accelerator>
          <text>
            <xsl:value-of select="@accelerator"/>
          </text>
        </accelerator>
      </xsl:if>
      <xsl:if test="caption/text[@mnemonic]">
        <mnemonic>
          <text>
            <xsl:value-of select="caption/text/@mnemonic"/>
          </text>
        </mnemonic>
      </xsl:if>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="text">
    <xsl:copy>
      <xsl:copy-of select="@*[not(name()='mnemonic')]"/>
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
