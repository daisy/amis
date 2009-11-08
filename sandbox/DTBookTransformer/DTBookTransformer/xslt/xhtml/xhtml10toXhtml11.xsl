<?xml version="1.0"?>
<!--
	org.daisy.util (C) 2005-2008 Daisy Consortium
	
	This library is free software; you can redistribute it and/or modify it under
	the terms of the GNU Lesser General Public License as published by the Free
	Software Foundation; either version 2.1 of the License, or (at your option)
	any later version.
	
	This library is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
	details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with this library; if not, write to the Free Software Foundation, Inc.,
	59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
-->
<xsl:stylesheet version="2.0" xmlns:xhtml="http://www.w3.org/1999/xhtml"
	xmlns="http://www.w3.org/1999/xhtml"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xs="http://www.w3.org/2001/XMLSchema"
	exclude-result-prefixes="xhtml xsl xs">

	<xsl:output method="xml" version="1.0" encoding="UTF-8"
		doctype-public="-//W3C//DTD XHTML 1.1//EN"
		doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"
		indent="yes" />

	<xsl:template match="element()">
		<xsl:copy copy-namespaces="yes">
			<xsl:copy-of select="@* except @name except @lang" />
			<xsl:if test="@name and not(@id or local-name() eq 'meta')">
				<xsl:attribute name="id" select="@name" />
			</xsl:if>
			<xsl:if test="@lang and not(@xml:lang)">
				<xsl:attribute name="xml:lang" select="@lang" />
			</xsl:if>
			<xsl:if test="local-name() eq 'meta' and not(@http-equiv)">
				<xsl:attribute name="name" select="@name" />
			</xsl:if>
			<xsl:apply-templates />
		</xsl:copy>
	</xsl:template>

</xsl:stylesheet>