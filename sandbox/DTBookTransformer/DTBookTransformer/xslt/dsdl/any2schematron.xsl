<?xml version="1.0" encoding="UTF-8"?>
<!--
	Stylesheet for extracting everything but Schematron information
-->
<xsl:transform	version="1.0" 
				xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
				xmlns:sch="http://www.ascc.net/xml/schematron">
	<!-- Set the output to be XML with an XML declaration and use indentation -->
	<xsl:output method="xml" omit-xml-declaration="no" indent="yes" standalone="yes"/>

	<xsl:template match="/">
		<sch:schema>
			<xsl:apply-templates/>
		</sch:schema>
	</xsl:template>

	<xsl:template match="sch:schema">
		<xsl:apply-templates/>
	</xsl:template>

	<xsl:template match="sch:*">
		<xsl:copy>
			<xsl:copy-of select="@*"/>
			<xsl:apply-templates/>
		</xsl:copy>
	</xsl:template>
	
	<xsl:template match="*">
		<xsl:apply-templates/>
	</xsl:template>
	
	<xsl:template match="text()">
		<xsl:if test="parent::sch:*">
			<xsl:copy-of select="."/>
		</xsl:if>
	</xsl:template>

</xsl:transform>