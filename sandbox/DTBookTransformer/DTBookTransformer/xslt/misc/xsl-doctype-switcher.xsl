<?xml version="1.0" encoding="utf-8"?>
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
<!-- 
  This stylesheet updates the doctype-public and doctype-system attributes on
  the xsl:output element of another stylesheet.
 --> 
<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:in="http://www.w3.org/1999/XSL/Transform"
  xmlns:out="http://www.w3.org/1999/XSL/TransformAlias"
  exclude-result-prefixes="in">
  
  <xsl:namespace-alias stylesheet-prefix="out" result-prefix="xsl"/>
  
  <xsl:output 
  	method="xml" 
  	indent="no" 
  	encoding="utf-8"/>
  
  <xsl:param name="publicId"/>
  <xsl:param name="systemId"/>
  
  <!-- Identity transformation template -->			
  <xsl:template match="/ | @* | * | comment() | processing-instruction() | text()"> 
	<xsl:copy> 
		<xsl:apply-templates select="@* | * | comment() | processing-instruction() | text()"/> 
	</xsl:copy> 
  </xsl:template> 
  
  <xsl:template match="in:output">
    <out:output>
      <xsl:copy-of select="@*[local-name()!='doctype-public' and local-name()!='doctype-system']"/>
      <xsl:attribute name="doctype-public">
        <xsl:value-of select="$publicId"/>
      </xsl:attribute>
      <xsl:attribute name="doctype-system">
        <xsl:value-of select="$systemId"/>
      </xsl:attribute>
    </out:output>
  </xsl:template>
  
</xsl:stylesheet>
