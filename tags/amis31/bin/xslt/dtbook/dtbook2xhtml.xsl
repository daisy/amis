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
<xsl:stylesheet version="2.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
  xmlns:dtb="http://www.daisy.org/z3986/2005/dtbook/"
  xmlns:s="http://www.w3.org/2001/SMIL20/"
  xmlns:m="http://www.w3.org/1998/Math/MathML"  
  xmlns:svg="http://www.w3.org/2000/svg"
  xmlns:xs="http://www.w3.org/2001/XMLSchema"
  xmlns="http://www.w3.org/1999/xhtml"
  exclude-result-prefixes="dtb s m svg xs">

<!--	<xsl:include href="./loc/l10n.xsl"/>-->
<!--	<xsl:include href="./dtbook2xhtml-l10n.xsl"/>-->
	<xsl:include href="../l10n/l10n.xsl"/>
	<xsl:param name="l10n_default_language" select="'en'"/>
	<!-- Sets the l10n language -->
	<xsl:param name="l10n_language" select="''"/>
	<!-- Use the language of target when generating cross-reference text? -->
	<xsl:param name="l10n_use_xref_language" select="0"/>
	<xsl:variable name="l10n_file" select="document('dtbook2xhtml-l10n.xml')" />

	<xsl:param name="filter_word"/>
	<xsl:param name="baseDir"/>
	<xsl:param name="first_smil"/>
	<xsl:param name="css_path"/>
	<xsl:param name="daisy_noteref"/>
	<xsl:param name="note_back_to_text"/>
	<xsl:param name="svg_mathml"/>
	<xsl:param name="split_simple_table"/>
  <!-- The smil element to target in href URIs (values are TEXT or PAR) -->
  <xsl:param name="hrefTarget"/>
  <!-- The prefix to add to the filename of each smil file -->
  <xsl:param name="smilPrefix"/>

	<xsl:param name="toc_gen"/>
	<xsl:param name="toc_maxdepth" select="3"/>

	<xsl:param name="chunk_gen"/>
	<xsl:param name="chunk_file_ext" select="'html'"/>
	<xsl:param name="chunk_maxdepth" select="3"/>
	<xsl:param name="chunk_navigation_showtitles" select="'true'"/>
	<xsl:param name="chunk_navigationbar_show_header" select="'true'"/>
	<xsl:param name="chunk_navigationbar_show_footer" select="'true'"/>

    <xsl:output method="xml" encoding="utf-8" indent="yes"/>

	<xsl:template match="/">
		<xsl:call-template name="write_doctype"/>
		<xsl:apply-templates/>
	</xsl:template>

<!-- <!ENTITY catts "@id|@class|@title|@xml:lang"> -->
<xsl:template name="copyCatts">
	<xsl:copy-of select="@id|@class|@title|@xml:lang"/>
</xsl:template>

<!-- <!ENTITY cncatts "@id|@title|@xml:lang"> -->
<xsl:template name="copyCncatts">
	<xsl:copy-of select="@id|@title|@xml:lang"/>
</xsl:template>

<xsl:template name="copyAttsNoId">
	<xsl:copy-of select="@class|@title|@xml:lang"/>
</xsl:template>

<xsl:template name="write_css">
	<xsl:if test="$css_path!=''">
		<link rel="stylesheet" type="text/css">
			<xsl:attribute name="href">
				<xsl:value-of select="$css_path"/>
			</xsl:attribute>
		</link>
	</xsl:if>
</xsl:template>


<xsl:template name="write_doctype">
	<xsl:choose>
		<xsl:when test="$svg_mathml='true'">
			<xsl:text disable-output-escaping="yes">&lt;!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1 plus MathML 2.0 plus SVG 1.1//EN" "http://www.w3.org/2002/04/xhtml-math-svg/xhtml-math-svg.dtd"</xsl:text>
		</xsl:when>
		<xsl:otherwise>
			<xsl:text disable-output-escaping="yes">&lt;!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd"</xsl:text>
		</xsl:otherwise>
	</xsl:choose>
	<xsl:if test="$daisy_noteref='true'">
		<xsl:text disable-output-escaping="yes"> [
	&lt;!ATTLIST span bodyref CDATA #IMPLIED&gt;
]</xsl:text>
	</xsl:if>
	<xsl:text disable-output-escaping="yes">&gt;</xsl:text>
</xsl:template>


<!-- <!ENTITY inlineParent "ancestor::*[self::dtb:h1 or self::dtb:h2 or self::dtb:h3 or self::dtb:h4 or self::dtb:h5 or self::dtb:h6 or self::dtb:hd or self::dtb:span or self::dtb:p]"> -->
   <xsl:template name="inlineParent">
	   <xsl:param name="class"/>
	   <xsl:choose>
   	 	 <xsl:when test="ancestor::*[self::dtb:h1 or self::dtb:h2 or self::dtb:h3 or self::dtb:h4 or self::dtb:h5 or self::dtb:h6 or self::dtb:hd or self::dtb:span or self::dtb:p or self::dtb:lic]">
     		 <xsl:apply-templates select="." mode="inlineOnly"/>
   	 	 </xsl:when>
<!-- jpritchett@rfbd.org:  Fixed bug in setting @class value (missing braces) -->
   	 	 <xsl:otherwise>
		   	 <div class="{$class}">
		       <xsl:call-template name="copyCncatts"/>
		       <xsl:apply-templates/>
		     </div>
   	 	 </xsl:otherwise>
   	 </xsl:choose>  
   </xsl:template>
   

	<xsl:template match="dtb:dtbook">
		<xsl:element name="html" namespace="http://www.w3.org/1999/xhtml">
			<xsl:if test="@xml:lang">
				<xsl:copy-of select="@xml:lang"/>
				<xsl:if test="$svg_mathml=null">
					<xsl:attribute name="lang">
						<xsl:value-of select="@xml:lang"/>
					</xsl:attribute>
				</xsl:if>	
			</xsl:if>			
			<xsl:if test="@dir">
				<xsl:copy-of select="@dir"/>
			</xsl:if>			
     <xsl:apply-templates/>
   </xsl:element>
   </xsl:template>


   <xsl:template match="dtb:head">
     <head>
     	 <meta http-equiv="Content-Type" content="application/xhtml+xml; charset=utf-8"/>
       <title>
         <xsl:value-of select="dtb:meta[@name='dc:Title']/@content"/>
       </title>
      <xsl:apply-templates/>
      <xsl:call-template name="write_css"/>
    </head>
   </xsl:template>


   <xsl:template match="dtb:meta">
     <meta>
       <xsl:if test="@name">
         <xsl:attribute name="name">
           <xsl:choose>
             <xsl:when test="@name='dtb:uid'"><xsl:value-of select="'dc:identifier'"/></xsl:when>
             <xsl:when test="@name='dc:Title'"><xsl:value-of select="'dc:title'"/></xsl:when>
             <xsl:when test="@name='dc:Subject'"><xsl:value-of select="'dc:subject'"/></xsl:when>
             <xsl:when test="@name='dc:Description'"><xsl:value-of select="'dc:description'"/></xsl:when>
             <xsl:when test="@name='dc:Type'"><xsl:value-of select="'dc:type'"/></xsl:when>
             <xsl:when test="@name='dc:Source'"><xsl:value-of select="'dc:source'"/></xsl:when>
             <xsl:when test="@name='dc:Relation'"><xsl:value-of select="'dc:relation'"/></xsl:when>
             <xsl:when test="@name='dc:Coverage'"><xsl:value-of select="'dc:coverage'"/></xsl:when>
             <xsl:when test="@name='dc:Creator'"><xsl:value-of select="'dc:creator'"/></xsl:when>
             <xsl:when test="@name='dc:Publisher'"><xsl:value-of select="'dc:publisher'"/></xsl:when>
             <xsl:when test="@name='dc:Contributor'"><xsl:value-of select="'dc:contributor'"/></xsl:when>
             <xsl:when test="@name='dc:Rights'"><xsl:value-of select="'dc:rights'"/></xsl:when>
             <xsl:when test="@name='dc:Date'"><xsl:value-of select="'dc:date'"/></xsl:when>
             <xsl:when test="@name='dc:Format'"><xsl:value-of select="'dc:format'"/></xsl:when>
             <xsl:when test="@name='dc:Identifier'"><xsl:value-of select="'dc:identifier'"/></xsl:when>
             <xsl:when test="@name='dc:Language'"><xsl:value-of select="'dc:language'"/></xsl:when>
             <xsl:otherwise><xsl:value-of select="@name"/></xsl:otherwise>
           </xsl:choose>
         </xsl:attribute>
       </xsl:if>	
       <xsl:copy-of select="@http-equiv"/>
       <xsl:copy-of select="@content"/>
       <xsl:copy-of select="@scheme"/>       
     </meta>
   </xsl:template>

 <!-- Unsure. How does this position for a copy? -->
   <xsl:template match="dtb:link">
   <!--
     <link>
       <xsl:copy-of select="@*"/>
     </link>
	-->
   </xsl:template>




   <xsl:template match="dtb:book">
     <body>
     
			<xsl:for-each select="(//dtb:doctitle)[1]">
				<h1 class="title" id="h1classtitle">
					<xsl:choose>
						<xsl:when test="$first_smil and $hrefTarget='TEXT'">
							<a href="{$smilPrefix}{$first_smil}#doctitleText">
								<xsl:value-of select="."/>
							</a>
						</xsl:when>
						<xsl:when test="$first_smil">
							<a href="{$smilPrefix}{$first_smil}#doctitle">
								<xsl:value-of select="."/>
							</a>
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="."/>
						</xsl:otherwise>
					</xsl:choose>
				</h1>
			</xsl:for-each>
			<xsl:if test="$toc_gen='true'">
				<xsl:call-template name="tocgen"/>
			</xsl:if>
			<xsl:apply-templates/>
			<xsl:if test="$chunk_gen='true'">
				<xsl:if test="$toc_gen='true'">  
					<p><a href="index-toc.{$chunk_file_ext}">
						<xsl:call-template name="getString">
							<xsl:with-param name="key" select="'tableofcontents'"/>
						</xsl:call-template>
					</a></p>
				</xsl:if>
				<xsl:variable name="cn"><xsl:call-template name="chunk_name"><xsl:with-param as="element()?" name="node"><xsl:call-template name="chunk_firstchunk"/></xsl:with-param></xsl:call-template></xsl:variable>
				<p><a>
					<xsl:attribute name="href">
						<xsl:value-of select="concat($cn,'.',$chunk_file_ext)"/>
					</xsl:attribute>
					<xsl:call-template name="getString">
						<xsl:with-param name="key" select="'beginofthebook'"/>
					</xsl:call-template>
				</a></p>
			</xsl:if>
		</body>
   </xsl:template>
		
	<xsl:template match="dtb:frontmatter|dtb:bodymatter|dtb:rearmatter">
		<xsl:apply-templates/>
	</xsl:template>

	<xsl:template match="dtb:level1 | dtb:level2 | dtb:level3 | dtb:level4 | dtb:level5 | dtb:level6 | dtb:level">
		<xsl:choose>
			<xsl:when test="$chunk_gen='true'">
				<xsl:variable name="ic"><xsl:call-template name="chunk_ischunk"/></xsl:variable>
				<xsl:variable name="title"><xsl:call-template name="chunk_title"/></xsl:variable>
				<xsl:variable name="cn"><xsl:call-template name="chunk_name"/></xsl:variable>
				<xsl:choose>
					<xsl:when test="$ic='1'">
						<xsl:result-document href="{concat($cn,'.',$chunk_file_ext)}" method="xhtml" indent="yes">
							<xsl:call-template name="write_doctype"/>
							<xsl:element name="html" namespace="http://www.w3.org/1999/xhtml">              
								<head>
									<meta http-equiv="Content-Type" content="application/xhtml+xml; charset=utf-8"/>
									<title>
										<xsl:value-of select="//dtb:meta[@name='dc:Title']/@content"/>
									</title>
									<xsl:call-template name="write_css"/>
									<xsl:call-template name="chunk_gennavigationbar"><xsl:with-param name="where" select="'inhead'"/></xsl:call-template>
								</head>
								<body>
									<xsl:if test="$chunk_navigationbar_show_header='true'">
										<xsl:call-template name="chunk_gennavigationbar"><xsl:with-param name="where" select="'header'"/></xsl:call-template>
									</xsl:if>
									<div>
										<xsl:call-template name="copyCatts"/>
										<xsl:if test="not(@class)">
											<xsl:attribute name="class"><xsl:value-of select="local-name()"/></xsl:attribute>
										</xsl:if>
										<xsl:apply-templates/>
										<xsl:apply-templates select="dtb:level1/dtb:h1|dtb:level2/dtb:h2|dtb:level3/dtb:h3|dtb:level4/dtb:h4|dtb:level5/dtb:h5|dtb:level6/dtb:h6|dtb:level/dtb:hd" mode="chunk_copytitles"/>
									</div>
									<xsl:if test="$chunk_navigationbar_show_footer='true'">
										<xsl:call-template name="chunk_gennavigationbar"><xsl:with-param name="where" select="string('footer')"/></xsl:call-template>
									</xsl:if>
								</body>
							</xsl:element>
						</xsl:result-document>
					</xsl:when>
					<xsl:otherwise>
						<xsl:call-template name="copylevelinsidediv"/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:otherwise>
				<xsl:call-template name="copylevelinsidediv"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template name="copylevelinsidediv">
		<div>
			<xsl:call-template name="copyCatts"/>
			<xsl:if test="not(@class)">
				<xsl:attribute name="class"><xsl:value-of select="local-name()"/></xsl:attribute>
			</xsl:if>
			<xsl:apply-templates/>
		</div>
	</xsl:template>


   <xsl:template match="dtb:covertitle">
     <p>
       <xsl:call-template name="copyCatts"/>
       <xsl:call-template name="maybeSmilrefInlineOnly"/>
     </p>
   </xsl:template>



   <xsl:template match="dtb:p">
     <p>
       <xsl:call-template name="copyCatts"/>
       <xsl:call-template name="maybeSmilrefInlineOnly"/>
     </p>
   </xsl:template>


   <xsl:template name="pagenum">
		<span class="pagenum">
			<xsl:call-template name="copyCncatts"/>
			<xsl:choose>
				<xsl:when test="@page='front'">
					<xsl:attribute name="class">page-front</xsl:attribute>
				</xsl:when>
				<xsl:when test="@page='special'">
					<xsl:attribute name="class">page-special</xsl:attribute>
				</xsl:when>
				<xsl:otherwise>
					<xsl:attribute name="class">page-normal</xsl:attribute>
				</xsl:otherwise>
			</xsl:choose>
			<xsl:call-template name="maybeSmilref"/>
			<!--<xsl:apply-templates/>-->
		</span>
	</xsl:template>
	
	<xsl:template match="dtb:pagenum">
		<xsl:call-template name="pagenum"/>
	</xsl:template>
	
   <xsl:template match="dtb:list/dtb:pagenum" priority="1">
     <xsl:param name="inlineFix"/>
     <xsl:choose>
       <xsl:when test="not(preceding-sibling::*) or $inlineFix='true'">
         <li><xsl:call-template name="pagenum"/></li>
       </xsl:when>
       <xsl:otherwise>
         <!--<xsl:message>Skipping pagenum element <xsl:value-of select="@id"/></xsl:message>-->
       </xsl:otherwise>
     </xsl:choose>
   </xsl:template>
   
   <xsl:template match="dtb:list/dtb:pagenum" mode="pagenumInLi">
     <xsl:call-template name="pagenum"/>
     <xsl:apply-templates select="following-sibling::*[1][self::dtb:pagenum]" mode="pagenumInLi"/>
   </xsl:template>
   
   

   <xsl:template match="dtb:list/dtb:prodnote">
     <li class="optional-prodnote">
     	<xsl:apply-templates/>
     	<xsl:apply-templates select="following-sibling::*[1][self::dtb:pagenum]" mode="pagenumInLi"/>
     </li>
   </xsl:template>

   <xsl:template match="dtb:blockquote/dtb:pagenum">
     <div class="dummy"><xsl:call-template name="pagenum"/></div>
   </xsl:template>

   <xsl:template match="dtb:address">
   <div class="address">
     <xsl:call-template name="copyCncatts"/>
     <xsl:call-template name="maybeSmilref"/>
   </div>
   </xsl:template>

  <xsl:template match="dtb:h1 | dtb:h2 | dtb:h3 | dtb:h4 |dtb:h5 | dtb:h6">
    <xsl:element name="{local-name()}">
      <xsl:call-template name="copyCatts"/>
      <xsl:if test="not(@id)">
        <xsl:attribute name="id">
          <xsl:value-of select="generate-id()"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:call-template name="maybeSmilref"/>
    </xsl:element>
    <xsl:if test="$toc_gen='true' and not($chunk_gen='true')">
      <xsl:variable name="depth">
        <xsl:call-template name="getdepth"/>
      </xsl:variable>
      <xsl:if test="$depth &lt; $toc_maxdepth + 1">
        <p><a href="#toc-html">
	    <xsl:call-template name="getString">
	      <xsl:with-param name="key" select="'backtotableofcontents'"/>
	    </xsl:call-template>
        </a></p>
      </xsl:if>
    </xsl:if>
  </xsl:template>

   <xsl:template match="dtb:bridgehead">
     <div class="bridgehead">
       <xsl:call-template name="copyCncatts"/>
       <xsl:call-template name="maybeSmilref"/>
     </div>
   </xsl:template>
   
   
   <xsl:template match="dtb:list[not(@type)]">
     <ul>
       <xsl:call-template name="copyCatts"/>
       <xsl:apply-templates/>
     </ul>
   </xsl:template>


   <xsl:template match="dtb:lic">
     <span class="lic">
       <xsl:call-template name="copyCncatts"/>
       <xsl:call-template name="maybeSmilref"/>
     </span>
   </xsl:template>

	<xsl:template match="dtb:br">
    <xsl:element name="br">
      <xsl:copy-of select="@id|@class|@title"/>
    </xsl:element>
	</xsl:template>


	<xsl:template match="dtb:noteref">
		<xsl:choose>
			<xsl:when test="$daisy_noteref='true'">
			  <span class="noteref">
			    <xsl:call-template name="copyCncatts"/>
			    <xsl:attribute name="bodyref">
			      <xsl:if test="not(contains(@idref,'#'))">
			        <xsl:text>#</xsl:text>
			      </xsl:if>
			      <xsl:value-of select="@idref"/>
			    </xsl:attribute>
			    <xsl:apply-templates/>
			  </span>
			</xsl:when>
			<xsl:otherwise>
			  <a class="noteref">
			    <xsl:call-template name="copyCncatts"/>
			    <xsl:attribute name="href">
				  <xsl:choose>
					<xsl:when test="@smilref">
						<xsl:call-template name="hrefValue">
							<xsl:with-param name="smilref" select="@smilref"/>
						</xsl:call-template>
					</xsl:when>
					<xsl:otherwise>
						<xsl:choose>
							<xsl:when test="$chunk_gen='true'">
								<xsl:variable name="cn"><xsl:call-template name="chunk_findid"><xsl:with-param name="id" select="translate(@idref, '#', '')"/></xsl:call-template></xsl:variable>
								<xsl:value-of select="concat($cn,'.',$chunk_file_ext,'#',translate(@idref, '#', ''))"/>
							</xsl:when>
							<xsl:otherwise>
								<xsl:text>#</xsl:text>
								<xsl:value-of select="translate(@idref, '#', '')"/>
							</xsl:otherwise>
						</xsl:choose>
					</xsl:otherwise>
				  </xsl:choose>
			    </xsl:attribute>			
			    <xsl:apply-templates/>
		      </a>
			</xsl:otherwise>
		</xsl:choose>		
	</xsl:template>


   <xsl:template match="dtb:img">
		<img>
			<xsl:call-template name="copyCatts"/>
			<xsl:copy-of select="@src|@alt|@longdesc|@height|@width"/>
		</img>
	</xsl:template>


   <xsl:template match="dtb:caption">
     <caption>
       <xsl:call-template name="copyCatts"/>
       <xsl:call-template name="maybeSmilrefInlineOnly"/>
     </caption>
   </xsl:template>


   <xsl:template match="dtb:imggroup/dtb:caption">   
   	 <div class="caption">
		    <xsl:call-template name="copyCncatts"/>
		    <xsl:call-template name="maybeSmilref"/>
		 </div>
   </xsl:template>

   <xsl:template match="dtb:div">
     <div>
       <xsl:call-template name="copyCatts"/>
       <xsl:call-template name="maybeSmilref"/>
     </div>
   </xsl:template>

   <xsl:template match="dtb:imggroup">
	   <xsl:call-template name="inlineParent">
		   <xsl:with-param name="class" select="'imggroup'"/>
	   </xsl:call-template>
   </xsl:template>
   
	<xsl:template match="dtb:prodnote">
	   <xsl:call-template name="inlineParent">
		   <xsl:with-param name="class" select="'optional-prodnote'"/>
	   </xsl:call-template> 
   </xsl:template>

  <xsl:template match="dtb:annotation">
    <div class="annotation">
    	<xsl:call-template name="copyCncatts"/>
    	<xsl:call-template name="maybeSmilref"/>
    </div>
   </xsl:template>

  <xsl:template match="dtb:author">
    <div class="author">
    	<xsl:call-template name="copyCncatts"/>
    	<xsl:call-template name="maybeSmilref"/>
    </div>
   </xsl:template>

   <xsl:template match="dtb:blockquote">
     <blockquote>
       <xsl:call-template name="copyCatts"/>
       <xsl:call-template name="maybeSmilref"/>
    </blockquote>
   </xsl:template>


  <xsl:template match="dtb:byline">
    <div class="byline">
    	<xsl:call-template name="copyCncatts"/>
    	<xsl:call-template name="maybeSmilref"/>
    </div>
   </xsl:template>

  <xsl:template match="dtb:dateline">
    <div class="dateline">
    	<xsl:call-template name="copyCncatts"/>
    	<xsl:call-template name="maybeSmilref"/>
    </div>
   </xsl:template>

  <xsl:template match="dtb:doctitle">
    <div class="doctitle">
    	<xsl:call-template name="copyCncatts"/>
    	<xsl:call-template name="maybeSmilref"/>
    </div>
  </xsl:template>
  
   <xsl:template match="dtb:docauthor">
     <div class="docauthor">
       <xsl:call-template name="copyCncatts"/>
       <xsl:call-template name="maybeSmilref"/>
     </div>
   </xsl:template>

   <xsl:template match="dtb:epigraph">
     <div class="epigraph">
       <xsl:call-template name="copyCncatts"/>
       <xsl:call-template name="maybeSmilref"/>
       <xsl:apply-templates/>
     </div>
   </xsl:template>

	<xsl:template match="dtb:note">
		<div class="notebody">
			<xsl:call-template name="copyCncatts"/>
			<xsl:if test="$note_back_to_text='true'">
				<xsl:variable name="noteref">
					<xsl:call-template name="whereisnoteref">
						<xsl:with-param name="idref" select="@id"/>
					</xsl:call-template>
				</xsl:variable>
				<a href="{$noteref}">^</a>
			</xsl:if>
			<xsl:call-template name="maybeSmilref"/>
		</div>
	</xsl:template>

   <xsl:template match="dtb:sidebar">
      <div class="sidebar">
        <xsl:call-template name="copyCncatts"/>
        <xsl:call-template name="maybeSmilref"/>
      </div>
   </xsl:template>

	<xsl:template match="dtb:hd">
		<xsl:choose>
			<xsl:when test="parent::dtb:level">
				<xsl:variable name="depth">
					<xsl:value-of select="count(ancestor-or-self::dtb:level/dtb:hd)"/>
				</xsl:variable>

				<xsl:choose>
					<xsl:when test="$depth &lt; 7">
						<xsl:element name="{concat('h', $depth)}">
							<xsl:call-template name="copyCatts"/>
							<xsl:if test="not(@id)">
								<xsl:attribute name="id">
									<xsl:value-of select="generate-id()"/>
								</xsl:attribute>
							</xsl:if>
							<xsl:call-template name="maybeSmilref"/>
						</xsl:element>
					</xsl:when>
					<xsl:otherwise>
						<div>
							<xsl:attribute name="class">
								<xsl:value-of select="concat('h', $depth)"/>
							</xsl:attribute>
							<xsl:call-template name="copyCncatts"/>
							<xsl:call-template name="maybeSmilref"/>
							<xsl:if test="not(@id)">
								<xsl:attribute name="id">
									<xsl:value-of select="generate-id()"/>
								</xsl:attribute>
							</xsl:if>
						</div>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:otherwise>
				<div class="hd">
					<xsl:call-template name="copyCncatts"/>
					<xsl:call-template name="maybeSmilref"/>
				</div>
			</xsl:otherwise>
		</xsl:choose>
		<xsl:if test="$toc_gen='true' and not($chunk_gen='true')">
			<xsl:variable name="depth">
				<xsl:call-template name="getdepth"/>
			</xsl:variable>
			<xsl:if test="$depth &lt; $toc_maxdepth + 1">
				<p><a href="#toc-html">
					<xsl:call-template name="getString">
						<xsl:with-param name="key" select="'backtotableofcontents'"/>
					</xsl:call-template>
				</a></p>
			</xsl:if>
		</xsl:if>
	</xsl:template>

   <xsl:template match="dtb:list/dtb:hd">
      <li class="hd">
        <xsl:call-template name="copyCncatts"/>
        <xsl:call-template name="maybeSmilref"/>
        <xsl:apply-templates select="following-sibling::*[1][self::dtb:pagenum]" mode="pagenumInLi"/>
      </li>
   </xsl:template>




   <xsl:template match="dtb:list[@type='ol']">
     <ol> 
		<xsl:choose>
			<xsl:when test="@enum='i'">
				<xsl:attribute name="class">lower-roman</xsl:attribute>
			</xsl:when>
			<xsl:when test="@enum='I'">
				<xsl:attribute name="class">upper-roman</xsl:attribute>
			</xsl:when>
			<xsl:when test="@enum='a'">
				<xsl:attribute name="class">lower-alpha</xsl:attribute>
			</xsl:when>	
			<xsl:when test="@enum='A'">
				<xsl:attribute name="class">upper-alpha</xsl:attribute>
			</xsl:when>	
		</xsl:choose>
     	<xsl:call-template name="copyCncatts"/>
        <xsl:apply-templates/>
     </ol>
   </xsl:template>





   <xsl:template match="dtb:list[@type='ul']">
     <ul> <xsl:call-template name="copyCatts"/>
       <xsl:apply-templates/>
     </ul> 
   </xsl:template>

   <xsl:template match="dtb:list[@type='pl']">
     <ul class="plain"> <xsl:call-template name="copyCncatts"/>
       <xsl:apply-templates/>
     </ul>
   </xsl:template>

   <xsl:template match="dtb:li">
     <li>
       <xsl:call-template name="copyCatts"/>
       <xsl:call-template name="maybeSmilref"/>
       <xsl:apply-templates select="following-sibling::*[1][self::dtb:pagenum]" mode="pagenumInLi"/>
     </li>
   </xsl:template>

 

   <xsl:template match="dtb:dl">
     <dl>
       <xsl:call-template name="copyCatts"/>
       <xsl:apply-templates/>
     </dl>
   </xsl:template>
   
   <xsl:template match="dtb:dl/dtb:pagenum" priority="1">
     <dt><xsl:call-template name="pagenum"/></dt>
     <dd>
     	<xsl:comment>empty</xsl:comment>
     </dd>
   </xsl:template>

  <xsl:template match="dtb:dt">
     <dt>
       <xsl:call-template name="copyCatts"/>
       <xsl:call-template name="maybeSmilref"/>
     </dt>
   </xsl:template>

  <xsl:template match="dtb:dd">
     <dd>
       <xsl:call-template name="copyCatts"/>
       <xsl:call-template name="maybeSmilref"/>
     </dd>
   </xsl:template>


	<xsl:template match="dtb:table">
		<xsl:choose>
			<xsl:when test="count(dtb:*[local-name()!='tr' and local-name()!='pagenum' and local-name()!='caption'])=0 and
			                element-available('xsl:for-each-group') and
			                $split_simple_table='true'">
				<xsl:call-template name="simpleTable"/>
			</xsl:when>
			<xsl:otherwise>
				<table>
					<xsl:call-template name="copyCatts"/>
					<xsl:apply-templates/>
				</table>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template name="simpleTable">
		<xsl:variable name="tableAtts">
			<dummy>
				<xsl:call-template name="copyAttsNoId"/>
			</dummy>
		</xsl:variable>
		<xsl:variable name="idAtt">
			<dummy>
				<xsl:copy-of select="@id"/>
			</dummy>
		</xsl:variable>
		<xsl:variable name="elemName" as="xs:string" select="name()"/>
		<xsl:for-each-group select="node()" group-starting-with="dtb:pagenum">
			<xsl:apply-templates select="current-group()[self::dtb:pagenum]" mode="pagenumonly"/>
			<xsl:if test="current-group()[not(self::dtb:pagenum)]">
				<xsl:element name="{$elemName}">
					<xsl:if test="position()=1">
						<xsl:copy-of select="$idAtt/*/@*"/>
					</xsl:if>
					<xsl:copy-of select="$tableAtts/*/@*"/>

					<xsl:apply-templates select="current-group()[not(self::dtb:pagenum)]"/>
				</xsl:element>
			</xsl:if>
		</xsl:for-each-group>
	</xsl:template>

	<xsl:template match="dtb:blockquote/dtb:table/dtb:pagenum" mode="pagenumonly">
		<div class="dummy"><xsl:call-template name="pagenum"/></div>
	</xsl:template>


	<xsl:template match="dtb:pagenum" mode="pagenumonly">
		<xsl:call-template name="pagenum"/>
	</xsl:template>
	
	<xsl:template match="dtb:table/dtb:pagenum|dtb:tbody/dtb:pagenum">
		<tr>
			<td class="noborder">
				<xsl:attribute name="colspan">
					<xsl:variable name="tdsInRow" select="number(sum(ancestor::dtb:table[1]/descendant::*[self::dtb:td or self::dtb:th]/(@colspan * @rowspan))) div count(ancestor::dtb:table[1]/descendant::dtb:tr)"/>
					<!-- <xsl:message>tdsInRow:<xsl:value-of select="$tdsInRow"/></xsl:message> -->
					<xsl:if test="$tdsInRow != round($tdsInRow) and $tdsInRow != NaN">
    					<xsl:message>Warning: Colspan and rowspan values in table don't add up.</xsl:message>
            		</xsl:if>
            		<xsl:value-of select="floor($tdsInRow)"/>
				</xsl:attribute>
				<xsl:call-template name="pagenum"/>				
			</td>
		</tr>
	</xsl:template>

   <xsl:template match="dtb:tbody">
     <tbody>
       <xsl:call-template name="copyCatts"/>
       <xsl:apply-templates/>
     </tbody>
   </xsl:template>

  

   <xsl:template match="dtb:thead">
     <thead>
       <xsl:call-template name="copyCatts"/>
       <xsl:apply-templates/>
     </thead>
   </xsl:template>

   <xsl:template match="dtb:tfoot">
     <tfoot>
       <xsl:call-template name="copyCatts"/>
       <xsl:apply-templates/>
     </tfoot>
   </xsl:template>

   <xsl:template match="dtb:tr">
     <tr>
       <xsl:call-template name="copyCatts"/>
       <xsl:copy-of select="@rowspan|@colspan"/>
       <xsl:apply-templates/>
     </tr>
   </xsl:template>

   <xsl:template match="dtb:th">
     <th>
       <xsl:call-template name="copyCatts"/>
       <xsl:copy-of select="@rowspan|@colspan"/>
       <xsl:call-template name="maybeSmilref"/>
     </th>
   </xsl:template>

   <xsl:template match="dtb:td">
     <td>
       <xsl:call-template name="copyCatts"/>
       <xsl:copy-of select="@rowspan|@colspan"/>
       <xsl:call-template name="maybeSmilref"/>
     </td>
   </xsl:template>

   <xsl:template match="dtb:colgroup">
     <colgroup>
       <xsl:call-template name="copyCatts"/>
       <xsl:apply-templates/>
     </colgroup>
   </xsl:template>

   <xsl:template match="dtb:col">
     <col>
       <xsl:call-template name="copyCatts"/>
       <xsl:apply-templates/>
     </col>
   </xsl:template>

 






   <xsl:template match="dtb:poem">
  <div class="poem">
    <xsl:call-template name="copyCncatts"/>
      <xsl:apply-templates/>
    </div>
   </xsl:template>


   <xsl:template match="dtb:poem/dtb:title">
     <p class="title">
       <xsl:call-template name="maybeSmilref"/>
     </p>
   </xsl:template>

   <xsl:template match="dtb:cite/dtb:title">
     <span class="title">
       <xsl:call-template name="maybeSmilref"/>
     </span>
   </xsl:template>

	<xsl:template match="dtb:cite/dtb:author">
     <span class="author">
       <xsl:call-template name="maybeSmilref"/>
     </span>
   </xsl:template>

   <xsl:template match="dtb:cite">
     <cite>
       <xsl:call-template name="copyCatts"/>
       <xsl:call-template name="maybeSmilref"/>
     </cite>
   </xsl:template>



   <xsl:template match="dtb:code">
     <code>
       <xsl:call-template name="copyCatts"/>
       <xsl:call-template name="maybeSmilref"/>
     </code>
   </xsl:template>

   <xsl:template match="dtb:kbd">
     <kbd>
       <xsl:call-template name="copyCatts"/>
       <xsl:call-template name="maybeSmilref"/>
     </kbd>
   </xsl:template>

   <xsl:template match="dtb:q">
     <q>
       <xsl:call-template name="copyCatts"/>
       <xsl:call-template name="maybeSmilref"/>
       <!--<xsl:apply-templates/>-->
     </q>
   </xsl:template>

   <xsl:template match="dtb:samp">
     <samp>
       <xsl:call-template name="copyCatts"/>
       <xsl:call-template name="maybeSmilref"/>
     </samp>
   </xsl:template>



   <xsl:template match="dtb:linegroup">
     <div class="linegroup">
       <xsl:call-template name="copyCncatts"/>
       <xsl:call-template name="maybeSmilref"/>  
    </div>
   </xsl:template>


   <xsl:template match="dtb:line">
   <p class="line">
       <xsl:call-template name="copyCncatts"/>
       <xsl:call-template name="maybeSmilrefInlineOnly"/>  
    </p>
   </xsl:template>

   <xsl:template match="dtb:linenum">
   <span class="linenum">
       <xsl:call-template name="copyCncatts"/>
       <xsl:call-template name="maybeSmilref"/>  
    </span>
   </xsl:template>







   <!-- Inlines -->

   <xsl:template match="dtb:a">
     <span class="anchor">
     	<xsl:call-template name="copyCncatts"/>
     	<xsl:apply-templates/>
     </span>
   </xsl:template>

 <xsl:template match="dtb:em">
   <em>
     <xsl:call-template name="copyCatts"/>
     <xsl:call-template name="maybeSmilref"/>
    </em>
   </xsl:template>

 <xsl:template match="dtb:strong">
   <strong>
     <xsl:call-template name="copyCatts"/>
     <xsl:call-template name="maybeSmilref"/>
    </strong>
   </xsl:template>


   <xsl:template match="dtb:abbr">
     <abbr>
       <xsl:call-template name="copyCatts"/>
       <xsl:call-template name="maybeSmilref"/>
     </abbr>
   </xsl:template>

  <xsl:template match="dtb:acronym">
     <acronym>
       <xsl:call-template name="copyCatts"/>
       <xsl:call-template name="maybeSmilref"/>
     </acronym>
   </xsl:template>

  <xsl:template match="dtb:bdo">
    <bdo>
       <xsl:call-template name="copyCatts"/>
       <xsl:copy-of select="@dir"/>
       <xsl:call-template name="maybeSmilref"/>
    </bdo>
  </xsl:template>

  <xsl:template match="dtb:dfn">
     <span class="definition">
       <xsl:call-template name="copyCncatts"/>
       <xsl:call-template name="maybeSmilref"/>
     </span>
   </xsl:template>

  <xsl:template match="dtb:sent">
     <span class="sentence">
     	<xsl:call-template name="copyCncatts"/>
     	<xsl:call-template name="maybeSmilref"/>
     </span>
   </xsl:template>


	<xsl:template match="dtb:w">
		<xsl:choose>
  		<xsl:when test="$filter_word='yes'">
  			<xsl:call-template name="maybeSmilref"/>
  		</xsl:when>
  		<xsl:otherwise>
  			<span class="word">
  			  <xsl:call-template name="copyCncatts"/>
  			  <xsl:call-template name="maybeSmilref"/>
  			</span>
  		</xsl:otherwise>
  	</xsl:choose>     
	</xsl:template>




 <xsl:template match="dtb:sup">
   <sup>
     <xsl:call-template name="copyCatts"/>
      <xsl:call-template name="maybeSmilref"/>
    </sup>
   </xsl:template>
   
 <xsl:template match="dtb:sub">
   <sub>
     <xsl:call-template name="copyCatts"/>
     <xsl:call-template name="maybeSmilref"/>
    </sub>
   </xsl:template>


	<xsl:template match="dtb:span">
		<span>
			<xsl:call-template name="copyCatts"/>
			<xsl:call-template name="maybeSmilref"/>
		</span>
	</xsl:template>


	<!-- FIXME internal and external -->
   <xsl:template match="dtb:a[@href]">
     <xsl:choose>
       <xsl:when test="ancestor::dtb:*[@smilref]">
         <span class="anchor">
           <xsl:call-template name="copyCncatts"/>
           <xsl:apply-templates/>
         </span>
       </xsl:when>
       <xsl:when test="@smilref">
         <xsl:variable name="url" select="substring-before(@smilref, '#')"/>
		 <xsl:variable name="fragment" select="substring-after(@smilref, '#')"/>
		 <xsl:choose>
		   <xsl:when test="document(concat($baseDir, $url))//*[@id=$fragment and self::s:par] and not(ancestor::dtb:note) and not(descendant::*[@smilref])">
		     <a id="{@id}">
			   <xsl:attribute name="href">
					<xsl:call-template name="hrefValue">
						<xsl:with-param name="smilref" select="@smilref"/>
					</xsl:call-template>
			   </xsl:attribute>
			   <xsl:apply-templates/>
			 </a>
		   </xsl:when>
		   <xsl:otherwise>
		     <xsl:apply-templates/>
		   </xsl:otherwise>
	     </xsl:choose>
       </xsl:when>
			<xsl:otherwise>
				<a>
					<xsl:call-template name="copyCatts"/>
					<xsl:choose>
						<xsl:when test="$chunk_gen='true' and @href and substring(@href,1,1)='#'">
							<xsl:variable name="cn"><xsl:call-template name="chunk_findid"><xsl:with-param name="id" select="translate(@href, '#', '')"/></xsl:call-template></xsl:variable>
							<xsl:attribute name="href">
								<xsl:value-of select="concat($cn,'.',$chunk_file_ext,'#',translate(@href, '#', ''))"/>
							</xsl:attribute>
						</xsl:when>
						<xsl:otherwise>
							<xsl:copy-of select="@href"/>
						</xsl:otherwise>
					</xsl:choose>
					<xsl:apply-templates/>
				</a>
			</xsl:otherwise>
     </xsl:choose>
   </xsl:template>

  <xsl:template match="dtb:annoref">
     <a class="annoref">
     	<xsl:call-template name="copyCncatts"/>
     	<xsl:attribute name="href">
			<xsl:choose>
				<xsl:when test="@smilref">
					<xsl:call-template name="hrefValue">
						<xsl:with-param name="smilref" select="@smilref"/>
					</xsl:call-template>
				</xsl:when>
				<xsl:otherwise>
					<xsl:choose>
						<xsl:when test="$chunk_gen='true'">
							<xsl:variable name="cn"><xsl:call-template name="chunk_findid"><xsl:with-param name="id" select="translate(@idref, '#', '')"/></xsl:call-template></xsl:variable>
							<xsl:value-of select="concat($cn,'.',$chunk_file_ext,'#',translate(@idref, '#', ''))"/>
						</xsl:when>
						<xsl:otherwise>
							<xsl:text>#</xsl:text>
							<xsl:value-of select="translate(@idref, '#', '')"/>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:attribute>			
     	<xsl:apply-templates/>
     </a>
   </xsl:template>

   <xsl:template match="dtb:*">
     <xsl:message>
  *****<xsl:value-of select="name(..)"/>/{<xsl:value-of select="namespace-uri()"/>}<xsl:value-of select="name()"/>******
   </xsl:message>
   </xsl:template>


	<xsl:template name="maybeSmilref">
		<xsl:choose>
			<xsl:when test="@smilref">
				<xsl:variable name="url" select="substring-before(@smilref, '#')"/>
				<xsl:variable name="fragment" select="substring-after(@smilref, '#')"/>
				<xsl:variable name="smilElem" select="document(concat($baseDir, $url))//*[@id=$fragment]"/>
				<xsl:choose>
					<xsl:when test="$smilElem[self::s:par] and not(ancestor::dtb:note) and not(descendant::*[@smilref])">
						<a>
							<xsl:attribute name="href">
								<xsl:call-template name="hrefValue">
									<xsl:with-param name="smilref" select="@smilref"/>
								</xsl:call-template>
							</xsl:attribute>
							<xsl:apply-templates/>
						</a>
					</xsl:when>
					<xsl:otherwise>
						<xsl:apply-templates/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template name="maybeSmilrefInlineOnly">
		<xsl:choose>
			<xsl:when test="@smilref">
				<xsl:variable name="url" select="substring-before(@smilref, '#')"/>
				<xsl:variable name="fragment" select="substring-after(@smilref, '#')"/>
				<xsl:variable name="smilElem" select="document(concat($baseDir, $url))//*[@id=$fragment]"/>
				<xsl:choose>
					<xsl:when test="$smilElem[self::s:par] and not(ancestor::dtb:note) and not(descendant::*[@smilref])">
						<a>
							<xsl:attribute name="href">
								<xsl:call-template name="hrefValue">
									<xsl:with-param name="smilref" select="@smilref"/>
								</xsl:call-template>
							</xsl:attribute>
							<xsl:apply-templates mode="inlineOnly"/>
						</a>
					</xsl:when>
					<xsl:otherwise>
						<xsl:apply-templates mode="inlineOnly"/>
					</xsl:otherwise>
				</xsl:choose>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates mode="inlineOnly"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template name="hrefValue">
		<xsl:param name="smilref"/>
		<xsl:variable name="url" select="substring-before($smilref, '#')"/>
		<xsl:variable name="fragment" select="substring-after($smilref, '#')"/>
		<xsl:variable name="smilElem" select="document(concat($baseDir, $url))//*[@id=$fragment]"/>
	  	<xsl:choose>
	  		<xsl:when test="$smilElem[self::s:par] and $hrefTarget='TEXT'">
				<xsl:value-of select="concat(concat($smilPrefix,$url),'#',$smilElem/s:text/@id)"/>
	  		</xsl:when>
	  		<xsl:otherwise>
				<xsl:value-of select="concat($smilPrefix,$smilref)"/>
	  		</xsl:otherwise>
	  	</xsl:choose>
	</xsl:template>
	
	<!--   <!ENTITY isInline "self::dtb:a or self::dtb:em or self::dtb:strong or self::dtb:abbr or self::dtb:acronym or self::dtb:bdo or self::dtb:dfn or self::dtb:sent or self::dtb:w or self::dtb:sup or self::dtb:sub or self::dtb:span or self::dtb:annoref or self::dtb:noteref or self::dtb:img or self::dtb:br or self::dtb:q or self::dtb:samp or self::dtb:pagenum"> -->
	<xsl:template match="dtb:*" mode="inlineOnly">
		<xsl:choose>
			<xsl:when test="self::dtb:a or self::dtb:em or self::dtb:strong or self::dtb:abbr or self::dtb:acronym or self::dtb:bdo or self::dtb:dfn or self::dtb:sent or self::dtb:w or self::dtb:sup or self::dtb:sub or self::dtb:span or self::dtb:annoref or self::dtb:noteref or self::dtb:img or self::dtb:br or self::dtb:q or self::dtb:samp or self::dtb:pagenum">
				<xsl:apply-templates select=".">
					<xsl:with-param name="inlineFix" select="'true'"/>
				</xsl:apply-templates>
			</xsl:when>
			<xsl:otherwise>
				<span>					
					<xsl:call-template name="get_class_attribute">
						<xsl:with-param name="element" select="."/>
					</xsl:call-template>					
					<xsl:call-template name="copyCncatts"/>
					<xsl:call-template name="maybeSmilrefInlineOnly"/>
				</span>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template name="get_class_attribute">
		<xsl:param name="element"/>
		<xsl:choose>
			<xsl:when test="name($element)='imggroup'"><xsl:attribute name="class">imggroup</xsl:attribute></xsl:when>
			<xsl:when test="name($element)='caption'"><xsl:attribute name="class">caption</xsl:attribute></xsl:when>	
			<xsl:when test="$element/@class"><xsl:attribute name="class"><xsl:value-of select="$element/@class"/></xsl:attribute></xsl:when>
			<xsl:otherwise>
				<xsl:attribute name="class">
					<xsl:text>inline-</xsl:text>
					<xsl:value-of select="name($element)"/>
				</xsl:attribute>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	
	<!-- MathML -->	
	<xsl:template match="m:math">
		<xsl:call-template name="process_math"/>
	</xsl:template>	
	
	<xsl:template match="m:math" mode="inlineOnly" >
    	<xsl:call-template name="process_math"/>
	</xsl:template>
	
	<xsl:template name="process_math">
		<xsl:choose>
			<xsl:when test="$svg_mathml='true'">
				<!-- Deep copy -->
				<xsl:copy-of select="."/>
			</xsl:when>
			<xsl:when test="@altimg and @dtb:smilref and @alttext">
				<a>
					<xsl:attribute name="href">
						<xsl:call-template name="hrefValue">
							<xsl:with-param name="smilref">
								<xsl:value-of select="@dtb:smilref"/>
							</xsl:with-param>
						</xsl:call-template>
					</xsl:attribute>
					<img src="{@altimg}" alt="{@alttext}">
						<xsl:copy-of select="@id"/>
					</img>
				</a>
			</xsl:when>
			<xsl:otherwise>
				<xsl:message>***** Warning: Skipping unsupported MathML content *****</xsl:message>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<!-- SVG; deep copy -->	
	<xsl:template match="svg:svg">
		<xsl:copy-of select="."/>
	</xsl:template>	
	
	<xsl:template match="svg:svg" mode="inlineOnly" >
    	<xsl:copy-of select="."/>  	
	</xsl:template>

	<xsl:template name="getdepth">
		<xsl:param name="node" select="."/>
		<xsl:value-of select="count($node/ancestor-or-self::dtb:level)+count($node/ancestor-or-self::dtb:level1)+count($node/ancestor-or-self::dtb:level2)+count($node/ancestor-or-self::dtb:level3)+count($node/ancestor-or-self::dtb:level4)+count($node/ancestor-or-self::dtb:level5)+count($node/ancestor-or-self::dtb:level6)"/>
	</xsl:template>


	<xsl:template name="tocgen">
		<xsl:choose>
			<xsl:when test="$chunk_gen='true'">
				<xsl:result-document href="index-toc.{$chunk_file_ext}" method="xhtml" indent="yes">
					<xsl:call-template name="write_doctype"/>
					<xsl:element name="html" namespace="http://www.w3.org/1999/xhtml">              
						<head>
							<meta http-equiv="Content-Type" content="application/xhtml+xml; charset=utf-8"/>
							<title>
								<xsl:value-of select="dtb:meta[@name='dc:Title']/@content"/>
							</title>
							<xsl:call-template name="write_css"/>
						</head>
						<body>
							<div id="toc-html" class="toc-html">
								<p class="toc-title">
									<xsl:call-template name="getString">
										<xsl:with-param name="key" select="'tableofcontents'"/>
									</xsl:call-template>
								</p>
								<ul class="toc">
									<xsl:apply-templates mode="toc"/>
								</ul>
							</div>
						</body>
					</xsl:element>
				</xsl:result-document>
			</xsl:when>
			<xsl:otherwise>
				<div id="toc-html" class="toc-html">
					<p class="toc-title">
						<xsl:call-template name="getString">
							<xsl:with-param name="key" select="'tableofcontents'"/>
						</xsl:call-template>
					</p>
					<ul class="toc">
						<xsl:apply-templates mode="toc"/>
					</ul>
				</div>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<xsl:template match="dtb:frontmatter | dtb:bodymatter | dtb:rearmatter" mode="toc">
		<xsl:apply-templates mode="toc"/>
	</xsl:template>


	<xsl:template mode="toc" match="dtb:level1 | dtb:level2 | dtb:level3 | dtb:level4 | dtb:level5 | dtb:level6 | dtb:level">
		<xsl:variable name="depth">
			<xsl:call-template name="getdepth"/>
		</xsl:variable>
		<xsl:if test="$depth &lt; ($toc_maxdepth + 1)">
			<xsl:apply-templates select="dtb:h1 | dtb:h2 | dtb:h3 | dtb:h4 | dtb:h5 | dtb:h6 | dtb:hd" mode="toc"/>
		</xsl:if>
	</xsl:template>



	<xsl:template mode="toc" match="dtb:h1 | dtb:h2 | dtb:h3 | dtb:h4 | dtb:h5 | dtb:h6 | dtb:hd">
		<xsl:variable name="depth">
			<xsl:call-template name="getdepth"/>
		</xsl:variable>
		<li>
			<a>
				<xsl:attribute name="href">
					<xsl:choose>
						<xsl:when test="$chunk_gen='true'">
							<xsl:variable name="cn"><xsl:call-template name="chunk_name"><xsl:with-param as="element()?" name="node"><xsl:call-template name="chunk_beginchunk"><xsl:with-param name="node" select="."/></xsl:call-template></xsl:with-param></xsl:call-template></xsl:variable>
							<xsl:choose>
								<xsl:when test="@id">
									<xsl:value-of select="concat($cn,'.',$chunk_file_ext,'#',@id)"/>
								</xsl:when>
								<xsl:otherwise>
									<xsl:value-of select="concat($cn,'.',$chunk_file_ext,'#',generate-id())"/>
								</xsl:otherwise>
							</xsl:choose>
						</xsl:when>
						<xsl:otherwise>
							<xsl:choose>
								<xsl:when test="@id">
									<xsl:value-of select="concat('#',@id)"/>
								</xsl:when>
								<xsl:otherwise>
									<xsl:value-of select="concat('#',generate-id())"/>
								</xsl:otherwise>
							</xsl:choose>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:attribute>
				<xsl:value-of select=".//text()"/>
			</a>
			<xsl:if test="($depth &lt; $toc_maxdepth) and (../dtb:level2/dtb:h2 | ../dtb:level3/dtb:h3 | ../dtb:level4/dtb:h4 | ../dtb:level5/dtb:h5 | ../dtb:level6/dtb:h6 | ../dtb:level/dtb:hd)">
				<ul class="toc">
					<xsl:apply-templates select="../dtb:level2 | ../dtb:level3 | ../dtb:level4 | ../dtb:level5 | ../dtb:level6 | ../dtb:level" mode="toc"/>
				</ul>
			</xsl:if>
		</li>
	</xsl:template>


	<xsl:template match="*" mode="toc"/>



	<xsl:template name="whereisnoteref">
		<xsl:param name="idref"/>
		<xsl:variable name="id">
			<xsl:value-of select="//*[@idref=concat('#',$idref)]/@id"/>
		</xsl:variable>
		<xsl:choose>
			<xsl:when test="$chunk_gen='true'">
				<xsl:variable name="cn">
					<xsl:call-template name="chunk_name">
						<xsl:with-param as="element()?" name="node">
							<xsl:call-template name="chunk_beginchunk"><xsl:with-param name="node" select="//*[@idref=concat('#',$idref)]" /></xsl:call-template>
						</xsl:with-param>
					</xsl:call-template>
				</xsl:variable>
				<xsl:value-of select="concat($cn,'.',$chunk_file_ext,'#',$id)"/>
			</xsl:when>
			 <xsl:otherwise>
				<xsl:value-of select="$id"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template name="chunk_num">
		<xsl:param name="node" select="."/>
		<xsl:value-of select="$node/count(preceding-sibling::dtb:level)+$node/count(preceding-sibling::dtb:level1)+$node/count(preceding-sibling::dtb:level2)+$node/count(preceding-sibling::dtb:level3)+$node/count(preceding-sibling::dtb:level4)+$node/count(preceding-sibling::dtb:level5)+$node/count(preceding-sibling::dtb:level6)+1"/>
	</xsl:template>

	<xsl:template name="chunk_name">
		<xsl:param name="node" select="."/>
		<xsl:choose>
			<xsl:when test="$node/local-name()='level' or $node/local-name()='level1' or $node/local-name()='level2' or $node/local-name()='level3' or $node/local-name()='level4' or $node/local-name()='level4' or $node/local-name()='level5' or $node/local-name()='level6'">
				<xsl:call-template name="chunk_name"><xsl:with-param name="node" select="$node/parent::*"/></xsl:call-template>
				<xsl:text>-l</xsl:text><xsl:call-template name="chunk_num"><xsl:with-param name="node" select="$node"/></xsl:call-template>
			</xsl:when>
			<xsl:when test="$node/local-name()='frontmatter'">
				<xsl:call-template name="chunk_name"><xsl:with-param name="node" select="$node/parent::*"/></xsl:call-template>
				<xsl:text>-fm</xsl:text>
			</xsl:when>
			<xsl:when test="$node/local-name()='bodymatter'">
				<xsl:call-template name="chunk_name"><xsl:with-param name="node" select="$node/parent::*"/></xsl:call-template>
				<xsl:text>-bm</xsl:text>
			</xsl:when>
			<xsl:when test="$node/local-name()='rearmatter'">
				<xsl:call-template name="chunk_name"><xsl:with-param name="node" select="$node/parent::*"/></xsl:call-template>
				<xsl:text>-rm</xsl:text>
			</xsl:when>
			<xsl:when test="$node/local-name()='book'">
				<xsl:text>dtb</xsl:text>
			</xsl:when>
		</xsl:choose>
	</xsl:template>


	<xsl:template name="chunk_ischunk">
		<xsl:param name="node" select="."/>
		<xsl:variable name="depth"><xsl:call-template name="getdepth"><xsl:with-param name="node" select="$node"/></xsl:call-template></xsl:variable>
		<xsl:choose>
			<xsl:when test="($node/local-name()='level' or $node/local-name()='level1' or $node/local-name()='level2' or $node/local-name()='level3' or $node/local-name()='level4' or $node/local-name()='level5' or $node/local-name()='level6') and ($depth &lt; $chunk_maxdepth + 1)">
				<xsl:text>1</xsl:text>
			</xsl:when>
			<xsl:otherwise>
				<xsl:text>0</xsl:text>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template name="chunk_beginchunk">
		<xsl:param name="node" select="."/>
		<xsl:variable name="ic">
			<xsl:call-template name="chunk_ischunk">
				<xsl:with-param name="node" select="$node"/>
			</xsl:call-template>
		</xsl:variable>
		<xsl:choose>
			<xsl:when test="$ic='1'"> 
				<xsl:sequence select="$node"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:call-template name="chunk_beginchunk">
					<xsl:with-param name="node" select="$node/.."/>
				</xsl:call-template>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template name="chunk_parentchunk">
		<xsl:param name="node" select="."/>
		<xsl:variable name="depth"><xsl:call-template name="getdepth"><xsl:with-param name="node" select="$node"/></xsl:call-template></xsl:variable>
		<xsl:choose>
			<xsl:when test="($node/../local-name()='level' or $node/../local-name()='level1' or $node/../local-name()='level2' or $node/../local-name()='level3' or $node/../local-name()='level4' or $node/../local-name()='level5' or $node/../local-name()='level6')">
				<xsl:sequence select="$node/.."/>
			</xsl:when>
			<xsl:when test="($node/../local-name()='frontmatter' or $node/../local-name()='bodymatter' or $node/../local-name()='rearmatter')"/>
			<xsl:otherwise>
				<xsl:call-template name="chunk_parentchunk">
					<xsl:with-param name="node" select="$node/.."/>
				</xsl:call-template>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template name="chunk_prevchunk">
		<xsl:param name="node" select="."/>
		<xsl:variable name="pc" as="element()?" select="(($node/preceding-sibling::dtb:level)|($node/preceding-sibling::dtb:level1)|($node/preceding-sibling::dtb:level2)|($node/preceding-sibling::dtb:level3)|($node/preceding-sibling::dtb:level4)|($node/preceding-sibling::dtb:level5)|($node/preceding-sibling::dtb:level6)|($node/preceding::dtb:level)|($node/preceding::dtb:level1)|($node/preceding::dtb:level2)|($node/preceding::dtb:level3)|($node/preceding::dtb:level4)|($node/preceding::dtb:level5)|($node/preceding::dtb:level6)|($node/parent::*))[last()]"/>
		<xsl:if test="$pc">
			<xsl:variable name="ic"><xsl:call-template name="chunk_ischunk"><xsl:with-param name="node" select="$pc"/></xsl:call-template></xsl:variable>
			<xsl:choose>
				<xsl:when test="$ic='0'">
					<xsl:call-template name="chunk_prevchunk"><xsl:with-param name="node" select="$pc"/></xsl:call-template>
				</xsl:when>
				<xsl:otherwise>
					<xsl:sequence select="$pc"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:if>
	</xsl:template>

	<xsl:template name="chunk_nextchunk">
		<xsl:param name="node" select="."/>
		<xsl:variable name="nc" as="element()?" select="(($node/child::dtb:level)|($node/child::dtb:level1)|($node/child::dtb:level2)|($node/child::dtb:level3)|($node/child::dtb:level4)|($node/child::dtb:level5)|($node/child::dtb:level6)|($node/following-sibling::dtb:level)|($node/following-sibling::dtb:level1)|($node/following-sibling::dtb:level2)|($node/following-sibling::dtb:level3)|($node/following-sibling::dtb:level4)|($node/following-sibling::dtb:level5)|($node/following-sibling::dtb:level6)|($node/following::dtb:level)|($node/following::dtb:level1)|($node/following::dtb:level2)|($node/following::dtb:level3)|($node/following::dtb:level4)|($node/following::dtb:level5)|($node/following::dtb:level6))[1]"/>
		<xsl:if test="$nc">
			<xsl:variable name="ic"><xsl:call-template name="chunk_ischunk"><xsl:with-param name="node" select="$nc"/></xsl:call-template></xsl:variable>
			<xsl:choose>
				<xsl:when test="$ic='0'">
					<xsl:call-template name="chunk_nextchunk"><xsl:with-param name="node" select="$nc"/></xsl:call-template>
				</xsl:when>
				<xsl:otherwise>
					<xsl:sequence select="$nc"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:if>
	</xsl:template>

	<xsl:template name="chunk_firstchunk">
		<xsl:sequence select="(//dtb:level1|//dtb:level2|//dtb:level3|//dtb:level4|//dtb:level5|//dtb:level6|//dtb:level)[1]"/>
	</xsl:template>

	<xsl:template name="chunk_title">
		<xsl:param name="node" select="."/>
		<xsl:value-of select="$node/dtb:h1|$node/dtb:h2|$node/dtb:h3|$node/dtb:h4|$node/dtb:h5|$node/dtb:h6|$node/dtb:hd"/>
	</xsl:template>


	<xsl:template match="dtb:level1/dtb:h1 | dtb:level2/dtb:h2 | dtb:level3/dtb:h3 | dtb:level4/dtb:h4 | dtb:level5/dtb:h5 | dtb:level6/dtb:h6" mode="chunk_copytitles">
		<xsl:variable name="cn"><xsl:call-template name="chunk_name"><xsl:with-param name="node" select=".."/></xsl:call-template></xsl:variable>
		<xsl:element name="{local-name()}">
			<xsl:copy-of select="@*"/>
			<a>
				<xsl:attribute name="href">
					<xsl:value-of select="concat($cn,'.',$chunk_file_ext)"/>
				</xsl:attribute>
			</a>
		</xsl:element>
	</xsl:template>

	<xsl:template match="dtb:level/dtb:hd" mode="chunk_copytitles">
		<xsl:variable name="cn"><xsl:call-template name="chunk_name"><xsl:with-param name="node" select=".."/></xsl:call-template></xsl:variable>
		<xsl:variable name="depth"><xsl:value-of select="count(ancestor-or-self::dtb:level/dtb:hd)"/></xsl:variable>
		<xsl:choose>
			<xsl:when test="$depth &lt; 7">
				<xsl:element name="{concat('h', $depth)}">
					<a>
						<xsl:attribute name="href">
							<xsl:value-of select="concat($cn,'.',$chunk_file_ext)"/>
						</xsl:attribute>
					</a>
				</xsl:element>
			</xsl:when>
			<xsl:otherwise>
				<xsl:element name="h6">
					<a>
						<xsl:attribute name="href">
							<xsl:value-of select="concat($cn,'.',$chunk_file_ext)"/>
						</xsl:attribute>
					</a>
				</xsl:element>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template name="chunk_findid">
		<xsl:param name="id"/>
		<xsl:call-template name="chunk_name">
			<xsl:with-param as="element()?" name="node">
				<xsl:call-template name="chunk_beginchunk"><xsl:with-param name="node" select="//*[@id=$id]" /></xsl:call-template>
			</xsl:with-param>
		</xsl:call-template>
	</xsl:template>


	<xsl:template name="chunk_gennavigationbar">
		<xsl:param name="where" />
		<xsl:variable as="element()?" name="prev">
			<xsl:call-template name="chunk_prevchunk" />
		</xsl:variable>
		<xsl:variable name="prevname">
			<xsl:call-template name="chunk_name">
				<xsl:with-param as="element()?" name="node" select="$prev" />
			</xsl:call-template>
		</xsl:variable>
		<xsl:variable name="prevtitle">
			<xsl:call-template name="chunk_title">
				<xsl:with-param as="element()?" name="node" select="$prev" />
			</xsl:call-template>
		</xsl:variable>
		<xsl:variable as="element()?" name="next">
			<xsl:call-template name="chunk_nextchunk" />
		</xsl:variable>
		<xsl:variable name="nextname">
			<xsl:call-template name="chunk_name">
				<xsl:with-param as="element()?" name="node" select="$next" />
			</xsl:call-template>
		</xsl:variable>
		<xsl:variable name="nexttitle">
			<xsl:call-template name="chunk_title">
				<xsl:with-param as="element()?" name="node" select="$next" />
			</xsl:call-template>
		</xsl:variable>
		<xsl:variable as="element()?" name="parent">
			<xsl:call-template name="chunk_parentchunk" />
		</xsl:variable>
		<xsl:variable name="parentname">
			<xsl:call-template name="chunk_name">
				<xsl:with-param as="element()?" name="node" select="$parent" />
			</xsl:call-template>
		</xsl:variable>
		<xsl:variable name="parenttitle">
			<xsl:call-template name="chunk_title">
				<xsl:with-param as="element()?" name="node" select="$parent" />
			</xsl:call-template>
		</xsl:variable>
		<xsl:variable as="element()?" name="home">
			<xsl:call-template name="chunk_firstchunk" />
		</xsl:variable>
		<xsl:variable name="homename">
			<xsl:call-template name="chunk_name">
				<xsl:with-param as="element()?" name="node" select="$home" />
			</xsl:call-template>
		</xsl:variable>
		<xsl:variable name="hometitle">
			<xsl:value-of select="(//dtb:doctitle)[1]" />
		</xsl:variable>
		<xsl:if test="$where='inhead'">
			<xsl:if test="$home">
				<link rel="home">
					<xsl:attribute name="href">
						<xsl:value-of select="concat($homename,'.',$chunk_file_ext)" />
					</xsl:attribute>
					<xsl:attribute name="title">
						<xsl:value-of select="$hometitle" />
					</xsl:attribute>
				</link>
			</xsl:if>
			<xsl:if test="$parent">
				<link rel="up">
					<xsl:attribute name="href">
						<xsl:value-of select="concat($parentname,'.',$chunk_file_ext)" />
					</xsl:attribute>
					<xsl:attribute name="title">
						<xsl:value-of select="$parenttitle" />
					</xsl:attribute>
				</link>
			</xsl:if>
			<xsl:if test="$prev">
				<link rel="previous">
					<xsl:attribute name="href">
						<xsl:value-of select="concat($prevname,'.',$chunk_file_ext)" />
					</xsl:attribute>
					<xsl:attribute name="title">
						<xsl:value-of select="$prevtitle" />
					</xsl:attribute>
				</link>
			</xsl:if>
			<xsl:if test="$next">
				<link rel="next">
					<xsl:attribute name="href">
						<xsl:value-of select="concat($nextname,'.',$chunk_file_ext)" />
					</xsl:attribute>
					<xsl:attribute name="title">
						<xsl:value-of select="$nexttitle" />
					</xsl:attribute>
				</link>
			</xsl:if>
		</xsl:if>
		<xsl:if test="$where='header'">
			<div class="nav header">
				<xsl:if test="$chunk_navigation_showtitles='true'">
					<p class="title">
						<xsl:call-template name="chunk_title" />
					</p>
				</xsl:if>
				<p>
					<span class="prev">
						<xsl:if test="$prev">
							<a accesskey="p">
								<xsl:attribute name="href">
										<xsl:value-of select="concat($prevname,'.',$chunk_file_ext)" />
									</xsl:attribute>
								<xsl:call-template name="chunk_navigation_content">
									<xsl:with-param name="direction" select="'prev'" />
								</xsl:call-template>
							</a>
						</xsl:if>
						<xsl:text>&#160;</xsl:text>
					</span>
					<span class="main">
						<xsl:if test="$parent">
							<xsl:value-of select="$parenttitle" />
						</xsl:if>
						<xsl:text>&#160;</xsl:text>
					</span>
					<span class="next">
						<xsl:if test="$next">
							<a accesskey="n">
								<xsl:attribute name="href">
										<xsl:value-of select="concat($nextname,'.',$chunk_file_ext)" />
									</xsl:attribute>
								<xsl:call-template name="chunk_navigation_content">
									<xsl:with-param name="direction" select="'next'" />
								</xsl:call-template>
							</a>
						</xsl:if>
						<xsl:text>&#160;</xsl:text>
					</span>
				</p>


			</div>

		</xsl:if>
		<xsl:if test="$where='footer'">
			<div class="nav footer">
				<xsl:if test="$prev or $next or $parent">
					<p>
						<span class="prev">
							<xsl:if test="$prev">
								<a accesskey="p">
									<xsl:attribute name="href">
							<xsl:value-of select="concat($prevname,'.',$chunk_file_ext)" />
						</xsl:attribute>
									<xsl:call-template name="chunk_navigation_content">
										<xsl:with-param name="direction" select="'prev'" />
									</xsl:call-template>
								</a>
							</xsl:if>
							<xsl:text>&#160;</xsl:text>
						</span>

						<span class="main">
							<xsl:if test="$parent">
								<a accesskey="u">
									<xsl:attribute name="href">
								<xsl:value-of select="concat($parentname,'.',$chunk_file_ext)" />
							</xsl:attribute>
									<xsl:call-template name="chunk_navigation_content">
										<xsl:with-param name="direction" select="'up'" />
									</xsl:call-template>
								</a>
							</xsl:if>
							<xsl:text>&#160;</xsl:text>
						</span>
						<span class="next">
							<xsl:if test="$next">
								<a accesskey="n">
									<xsl:attribute name="href">
							<xsl:value-of select="concat($nextname,'.',$chunk_file_ext)" />
						</xsl:attribute>
									<xsl:call-template name="chunk_navigation_content">
										<xsl:with-param name="direction" select="'next'" />
									</xsl:call-template>
								</a>
							</xsl:if>
							<xsl:text>&#160;</xsl:text>
						</span>
					</p>
				</xsl:if>
				<p>
					<span class="prev">
						<xsl:if test="$chunk_navigation_showtitles='true'">
							<xsl:value-of select="$prevtitle" />
						</xsl:if>
						<xsl:text>&#160;</xsl:text>
					</span>
					<span class="main">
						<xsl:if test=". != $home">
							<a accesskey="h">
								<xsl:attribute name="href">
								<xsl:value-of select="concat($homename,'.',$chunk_file_ext)" />
							</xsl:attribute>
								<xsl:call-template name="chunk_navigation_content">
									<xsl:with-param name="direction" select="'home'" />
								</xsl:call-template>
							</a>
							<xsl:text>&#160;|&#160;</xsl:text>
						</xsl:if>
						<xsl:if test="$toc_gen = 'true'">
							<a accesskey="t">
								<xsl:attribute name="href">
									<xsl:value-of select="concat('index-toc','.',$chunk_file_ext)"/>
								</xsl:attribute>
								<xsl:call-template name="getString">
									<xsl:with-param name="key" select="'nav-toc'"/>
								</xsl:call-template>
							</a>
						</xsl:if>
						<xsl:text>&#160;</xsl:text>
					</span>
					<span class="next">
						<xsl:if test="$chunk_navigation_showtitles = 'true'">
							<xsl:value-of select="$nexttitle"/>
						</xsl:if>
						<xsl:text>&#160;</xsl:text>
					</span>
				</p>
			</div>
		</xsl:if>
	</xsl:template>

	<xsl:template name="chunk_navigation_content">
		<xsl:param name="direction" select="next"/>
		<xsl:variable name="navtext">
			<xsl:choose>
				<xsl:when test="$direction = 'prev'">
    				<xsl:call-template name="getString">
      					<xsl:with-param name="key" select="'nav-prev'"/>
    				</xsl:call-template>
				</xsl:when>
				<xsl:when test="$direction = 'next'">
    				<xsl:call-template name="getString">
      					<xsl:with-param name="key" select="'nav-next'"/>
    				</xsl:call-template>
				</xsl:when>
				<xsl:when test="$direction = 'up'">
    				<xsl:call-template name="getString">
      					<xsl:with-param name="key" select="'nav-up'"/>
    				</xsl:call-template>
				</xsl:when>
				<xsl:when test="$direction = 'home'">
    				<xsl:call-template name="getString">
      					<xsl:with-param name="key" select="'nav-home'"/>
    				</xsl:call-template>
				</xsl:when>
			</xsl:choose>
		</xsl:variable>

		<xsl:value-of select="$navtext"/>
	</xsl:template>

</xsl:stylesheet>
