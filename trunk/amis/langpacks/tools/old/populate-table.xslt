<?xml version="1.0" encoding="UTF-8"?>

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="text"/>

  <!-- The table for this language. -->
  <xsl:param name="TABLE"/>

  <!-- Set this variable to "create" to create. -->
  <xsl:param name="CREATE"/>

  <!-- Set this variable to "drop" to drop the table as well. -->
  <xsl:param name="DROP"/>

  <!-- Issue the create table statement if necessary -->
  <xsl:template match="/">
    <xsl:if test="$DROP='drop'">
      <xsl:value-of select="concat('DROP TABLE ',$TABLE,';&#xa;')"/>
    </xsl:if>
    <xsl:if test="$CREATE='create'">
      <xsl:text>CREATE TABLE </xsl:text>
      <xsl:value-of select="$TABLE"/>
      <xsl:text> (id VARCHAR(16) not null primary key, textstring TEXT, audiodata LONGBLOB, audiouri VARCHAR(255), textflag INT, audioflag INT, remarks TEXT);&#xa;</xsl:text>
    </xsl:if>
    <xsl:apply-templates/>
  </xsl:template>

  <!-- Skip mnemonics (shouldn't be translated) -->
  <xsl:template match="mnemonic"/>

  <!-- Output all text items -->
  <xsl:template match="text">
    <xsl:text>INSERT INTO </xsl:text>
    <xsl:value-of select="$TABLE"/>
    <xsl:text> (id, textstring, audiouri, textflag, audioflag) VALUES ('</xsl:text>
    <xsl:value-of select="@id"/>
    <xsl:text>', '</xsl:text>
    <xsl:value-of select="."/>
    <xsl:text>', '</xsl:text>
    <xsl:value-of select="../audio/@src"/>
    <xsl:text>', 1, 1);&#xa;</xsl:text>
  </xsl:template>

  <xsl:template match="text()"/>

</xsl:transform>
