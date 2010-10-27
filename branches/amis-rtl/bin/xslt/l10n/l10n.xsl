<?xml version="1.0" encoding="utf-8"?>
	<!--
		org.daisy.util (C) 2005-2008 Daisy Consortium This library is free
		software; you can redistribute it and/or modify it under the terms of
		the GNU Lesser General Public License as published by the Free
		Software Foundation; either version 2.1 of the License, or (at your
		option) any later version. This library is distributed in the hope
		that it will be useful, but WITHOUT ANY WARRANTY; without even the
		implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
		PURPOSE. See the GNU Lesser General Public License for more details.

		You should have received a copy of the GNU Lesser General Public
		License along with this library; if not, write to the Free Software
		Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
		USA
	-->
<xsl:stylesheet version="2.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<!--
		given the info needed to produce a set of candidates ($str), pick the
		best of the bunch: 
		1. $str[lang($Lang)][1]
		2. $str[lang($PrimaryLang)][1]
		3. $str[1]
		4. if not($str) then issue warning to STDERR
	-->
	<xsl:template name="getString">
		<xsl:param name="key" />
		<xsl:param name="lang">
			<xsl:call-template name="getLanguage" />
		</xsl:param>
		<xsl:variable name="primaryLang" select="substring-before($lang,'-')" />
		<xsl:variable name="str" select="$l10n_file/strings/str[@name=$key]" />
		<xsl:choose>
			<xsl:when test="$str[lang($lang)]">
				<xsl:value-of select="$str[lang($lang)][1]" />
			</xsl:when>
			<xsl:when test="$str[lang($primaryLang)]">
				<xsl:value-of select="$str[lang($primaryLang)][1]" />
			</xsl:when>
			<xsl:when test="$str">
				<xsl:value-of select="$str[1]" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:message terminate="no">
					<xsl:text>Warning: no string named '</xsl:text>
					<xsl:value-of select="$key" />
					<xsl:text>' found.</xsl:text>
				</xsl:message>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<!-- 
		Get the best l10n language to use:
		
		1. $l10n_language if it is specified
		2. the closest @xml:lang or @lang of the $xref-context if it is specified
		3. the closest @xml:lang or @lang of the current node
		
		The returned language case is normalized (e.g. en-GB)
	-->
	<!-- Get the l10n language to use-->
	<xsl:template name="getLanguage">
		<xsl:param name="target" select="." />
		<xsl:param name="xref-context" select="false()" />

		<xsl:variable name="language">
			<xsl:choose>
				<xsl:when test="$l10n_language != ''">

					<xsl:message terminate="no">
						<xsl:text>$l10n_language</xsl:text>
						<xsl:value-of select="$l10n_language" />
					</xsl:message>

					<xsl:value-of select="$l10n_language" />
				</xsl:when>
				<xsl:when test="$xref-context or $l10n_use_xref_language != 0">


					<xsl:message terminate="no">
						<xsl:text>xref context</xsl:text>
					</xsl:message>

					<!-- can't do this one step: attributes are unordered! -->
					<xsl:variable name="lang-scope"
						select="($target/ancestor-or-self::*[@lang] | $target/ancestor-or-self::*[@xml:lang])[last()]" />
					<xsl:variable name="lang-attr"
						select="($lang-scope/@lang | $lang-scope/@xml:lang)[1]" />
					<xsl:choose>
						<xsl:when test="string($lang-attr) = ''">
							<xsl:value-of select="$l10n_default_language" />
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="$lang-attr" />
						</xsl:otherwise>
					</xsl:choose>
				</xsl:when>
				<xsl:otherwise>
					<!-- can't do this one step: attributes are unordered! -->
					<xsl:variable name="lang-scope"
						select="(ancestor-or-self::*[@lang] | ancestor-or-self::*[@xml:lang])[last()]" />
					<xsl:variable name="lang-attr"
						select="($lang-scope/@lang | $lang-scope/@xml:lang)[1]" />
					<xsl:choose>
						<xsl:when test="string($lang-attr) = ''">
							<xsl:value-of select="$l10n_default_language" />
						</xsl:when>
						<xsl:otherwise>
							<xsl:value-of select="$lang-attr" />
						</xsl:otherwise>
					</xsl:choose>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>


		<xsl:choose>
			<xsl:when test="contains($language,'-')">
				<xsl:value-of
					select="translate(substring-before($language,'-'),
                                          'ABCDEFGHIJKLMNOPQRSTUVWXYZ',
                                          'abcdefghijklmnopqrstuvwxyz')" />
				<xsl:text>-</xsl:text>
				<xsl:value-of
					select="translate(substring-after($language,'-'),
                                          'abcdefghijklmnopqrstuvwxyz',
                                          'ABCDEFGHIJKLMNOPQRSTUVWXYZ')" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of
					select="translate($language,
                                          'ABCDEFGHIJKLMNOPQRSTUVWXYZ',
                                          'abcdefghijklmnopqrstuvwxyz')" />
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

</xsl:stylesheet>