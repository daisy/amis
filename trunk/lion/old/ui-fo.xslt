<?xml version="1.0" encoding="UTF-8"?>

<!-- Create the translation sheet for the all UI items. The output is an XSLFO
file that can be transformed into a PDF with FOP. -->

<!-- $Id: ui-fo.xslt 2026 2007-07-17 16:23:38Z julienq $ -->

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:fo="http://www.w3.org/1999/XSL/Format">

  <!-- Prepare the layout for the document. -->

  <xsl:template match="/">
    <fo:root>
      <fo:layout-master-set>
        <fo:simple-page-master master-name="simple-page">
          <fo:region-body margin="20mm"/>
          <fo:region-after extent="20mm" region-name="footer"/>
        </fo:simple-page-master>
      </fo:layout-master-set>
      <fo:page-sequence master-reference="simple-page" font-family="Helvetica"
        font-size="14pt" text-align="justify" leader-pattern="rule"
        leader-length.optimum="100%" rule-thickness=".5pt">
        <!-- Page number in the footer -->
        <fo:static-content flow-name="footer">
          <fo:block text-align="center" margin-top="1em">
            <fo:page-number/>
          </fo:block>
        </fo:static-content>
        <fo:flow flow-name="xsl-region-body">
          <xsl:apply-templates mode="__TEXT__"/>
        </fo:flow>
      </fo:page-sequence>
    </fo:root>
  </xsl:template>

  <xsl:template match="accelerator/text" mode="__TEXT__"/>

  <xsl:template match="mnemonic/text" mode="__TEXT__"/>

  <xsl:template match="text" mode="__TEXT__">
    <fo:block>
      <fo:inline font-size="80%">
        <xsl:choose>
          <xsl:when test="(parent::caption|parent::description)
            [parent::action|parent::dialog|parent::control|parent::switch]">
            <xsl:choose>
              <xsl:when test="parent::*[parent::switch[parent::control]]">
                <xsl:value-of select="concat('Control ', local-name(..),
                  ' [condition: ',../@condition,'] &gt; ')"/>
              </xsl:when>
              <xsl:when test="parent::*[parent::switch[parent::dialog]]">
                <xsl:value-of select="concat('Control ', local-name(..),
                  ' [condition: ',../@condition,'] &gt; ')"/>
              </xsl:when>
              <xsl:when test="parent::caption[../description/text]">
                <xsl:text>Caption &gt; </xsl:text>
              </xsl:when>
              <xsl:when test="parent::description[../caption/text]">
                <xsl:text>Description &gt; </xsl:text>
              </xsl:when>
              <xsl:when test="parent::*[parent::action]">
                <xsl:value-of select="concat('Action ',local-name(..),' [',
                  ../../@mfcid,'] &gt; ')"/>
              </xsl:when>
              <xsl:when test="parent::*[parent::control]">
                <xsl:value-of select="concat('Control ',local-name(..),' [',
                  ../../@mfcid,'] &gt; ')"/>
              </xsl:when>
              <xsl:otherwise>
                <xsl:value-of select="concat('Dialog ',local-name(..),' [',
                  ../../@mfcid,'] &gt; ')"/>
              </xsl:otherwise>
            </xsl:choose>
          </xsl:when>
          <xsl:when test="parent::caption[parent::container]">
            <xsl:value-of select="concat('Container caption [',../../@id,
              '] &gt; ')"/>
          </xsl:when>
          <xsl:when test="parent::caption[parent::dialog]">
            <xsl:value-of select="concat('Dialog caption [',../../@mfcid,
              '] &gt; ')"/>
          </xsl:when>
          <xsl:when test="parent::promptItem[@id]">
            <xsl:value-of select="concat('Prompt item [',../@id,'] &gt; ')"/>
          </xsl:when>
          <xsl:when test="parent::promptItem">
            <xsl:text>Prompt item &gt; </xsl:text>
          </xsl:when>
          <xsl:when
            test="parent::promptItem[parent::prompt[@id][count(.//text)=1]]">
            <xsl:value-of select="concat('Prompt item [',../../@id,'] &gt; ')"/>
          </xsl:when>
        </xsl:choose>
      </fo:inline>
      <fo:inline font-weight="bold">
        <xsl:value-of select="normalize-space(.)"/>
      </fo:inline>
      <xsl:if test="parent::caption/../note and
        not(parent::caption/../description)">
        <xsl:apply-templates select="parent::caption/../note"
          mode="__TEXT__footnote"/>
      </xsl:if>
      <xsl:if test="parent::promptItem/note">
        <xsl:apply-templates select="parent::promptItem/note"
          mode="__TEXT__footnote"/>
      </xsl:if>
      <xsl:text>: </xsl:text>
      <xsl:choose>
        <xsl:when test="ancestor-or-self::*[@translate='no']">
          <xsl:text>This item need not be translated.</xsl:text>
        </xsl:when>
        <xsl:otherwise>
          <xsl:call-template name="make-lines">
            <xsl:with-param name="str" select="."/>
          </xsl:call-template>
        </xsl:otherwise>
      </xsl:choose>
    </fo:block>
  </xsl:template>

  <xsl:template match="note" mode="__TEXT__"/>

  <xsl:template match="note" mode="__TEXT__footnote">
    <xsl:variable name="num">
      <xsl:number count="note[../caption/text]" level="any"
        from="containers/container|dialogs/dialog"/>
    </xsl:variable>
    <fo:footnote>
      <fo:inline baseline-shift="super" font-size="60%"><xsl:value-of
          select="$num"/></fo:inline>
      <fo:footnote-body>
        <fo:list-block provisional-label-separation="0pt"
          provisional-distance-between-starts="18pt"
          space-after.optimum="6pt" font-size="80%">
          <fo:list-item>
            <fo:list-item-label end-indent="label-end()">
              <fo:block><xsl:value-of select="$num"/></fo:block>
            </fo:list-item-label>
            <fo:list-item-body start-indent="body-start()">
              <fo:block><xsl:value-of select="."/></fo:block>
            </fo:list-item-body>
          </fo:list-item>
        </fo:list-block>
      </fo:footnote-body>
    </fo:footnote>
  </xsl:template>

  <!-- Start a new page for every container -->
  <xsl:template match="containers/container" mode="__TEXT__">
    <xsl:call-template name="start-page">
      <xsl:with-param name="title" select="concat('Container &#x201c;',
        caption/text,'&#x201d; (',@id,')')"/>
      <xsl:with-param name="word-count">
        <xsl:apply-templates select="." mode="__TEXT__word-count"/>
      </xsl:with-param>
    </xsl:call-template>
    <fo:block line-height="2">
      <xsl:apply-templates mode="__TEXT__"/>
    </fo:block>
  </xsl:template>

  <!-- Start a new page for every dialog -->
  <xsl:template match="dialogs/dialog" mode="__TEXT__">
    <xsl:call-template name="start-page">
      <xsl:with-param name="title" select="concat('Dialog &#x201c;',
        caption/text,'&#x201d; (',@mfcid,')')"/>
      <xsl:with-param name="word-count">
        <xsl:apply-templates select="." mode="__TEXT__word-count"/>
      </xsl:with-param>
    </xsl:call-template>
    <fo:block line-height="2">
      <xsl:apply-templates mode="__TEXT__"/>
    </fo:block>
  </xsl:template>

  <!-- Start a new page for prompts -->
  <xsl:template match="prompts" mode="__TEXT__">
    <xsl:call-template name="start-page">
      <xsl:with-param name="title" select="'Prompt items'"/>
      <xsl:with-param name="word-count">
        <xsl:apply-templates select="." mode="__TEXT__word-count"/>
      </xsl:with-param>
    </xsl:call-template>
    <fo:block line-height="2">
      <xsl:apply-templates mode="__TEXT__"/>
    </fo:block>
  </xsl:template>

  <!-- Frame caption and description for the same action -->
  <xsl:template match="action[./caption/text and ./description/text]"
    mode="__TEXT__">
    <fo:block border="dotted black" padding="1em" margin-top="1em">
      <fo:block>
        <xsl:value-of select="concat('Action [',@mfcid,']')"/>
        <xsl:if test="note">
          <xsl:apply-templates select="note" mode="__TEXT__footnote"/>
        </xsl:if>
      </fo:block>
      <xsl:apply-templates mode="__TEXT__"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="control[./caption/text and ./description/text]"
    mode="__TEXT__">
    <fo:block border="dotted black" padding="1em" margin-top="1em">
      <fo:block>
        <xsl:value-of select="concat('Control [',@mfcid,']')"/>
        <xsl:if test="note">
          <xsl:apply-templates select="note" mode="__TEXT__footnote"/>
        </xsl:if>
      </fo:block>
      <xsl:apply-templates mode="__TEXT__"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="switch[count(caption)&gt;1]" mode="__TEXT__">
    <fo:block border="dotted black" padding="1em" margin-top="1em">
      <xsl:apply-templates mode="__TEXT__"/>
    </fo:block>
  </xsl:template>

  <!-- Frame several prompt items that belong to the same prompt -->
  <xsl:template match="prompt[count(.//text)&gt;1]" mode="__TEXT__">
    <fo:block border="dotted black" padding="1em" margin-top="1em">
      <fo:block>
        <xsl:value-of select="concat('Prompt items [',@id,']')"/>
      </fo:block>
      <xsl:apply-templates mode="__TEXT__"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="*" mode="__TEXT__word-count">
    <xsl:text>NOT YET</xsl:text>
  </xsl:template>

  <xsl:template match="*" mode="__TEXT__">
    <xsl:apply-templates mode="__TEXT__"/>
  </xsl:template>

  <xsl:template match="text()" mode="__TEXT__"/>

  <!-- Do all containers, dialogs, and lists of prompts on separate pages -->

  <xsl:template match="containers">
    <xsl:apply-templates mode="new-page"/>
  </xsl:template>

  <xsl:template match="dialogs">
    <xsl:apply-templates mode="new-page"/>
  </xsl:template>

  <xsl:template match="prompts">
    <xsl:apply-templates mode="new-page"/>
  </xsl:template>

  <!-- Container stuff -->

  <xsl:template match="container" mode="new-page">
    <xsl:call-template name="start-page">
      <xsl:with-param name="title" select="concat('Container &#x201c;',
        caption/text,'&#x201d;')"/>
    </xsl:call-template>
    <!-- Everything in the page is double-spaced -->
    <fo:block line-height="2">
      <xsl:apply-templates select="caption|action|container"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="action[descendant::text]">
    <xsl:apply-templates select="." mode="frame"/>
  </xsl:template>
  <xsl:template match="action"/>

  <xsl:template match="container[descendant::text]">
    <xsl:apply-templates select="caption[text]|switch"/>
  </xsl:template>
  <xsl:template match="container"/>

  <xsl:template match="caption">
    <fo:block>
      <fo:inline font-size="80%">
        <xsl:if test="../description/text">
          <xsl:attribute name="keep-with-next">always</xsl:attribute>
        </xsl:if>
        <xsl:value-of select="concat(local-name(..),' caption ')"/>
        <xsl:if test="../@mfcid">
          <xsl:value-of select="concat('[',../@mfcid,'] ')"/>
        </xsl:if>
      </fo:inline>
      <fo:inline font-weight="bold">
        <xsl:value-of select="text"/>
      </fo:inline>
      <xsl:apply-templates select="../note"/>
      <xsl:text>: </xsl:text>
      <xsl:call-template name="make-lines">
        <xsl:with-param name="str" select="text"/>
      </xsl:call-template>
    </fo:block>
  </xsl:template>

  <xsl:template match="description">
    <fo:block>
      <fo:inline font-size="80%">
        <xsl:value-of select="concat(local-name(..),' description ')"/>
      </fo:inline>
      <fo:inline font-weight="bold">
        <xsl:value-of select="text"/>
      </fo:inline>
      <xsl:text>: </xsl:text>
      <xsl:call-template name="make-lines">
        <xsl:with-param name="str" select="text"/>
      </xsl:call-template>
    </fo:block>
  </xsl:template>

  <!-- Dialog stuff -->

  <xsl:template match="dialog" mode="new-page">
    <xsl:call-template name="start-page">
      <xsl:with-param name="title" select="concat('Dialog &#x201c;',
        caption/text,'&#x201d; (',@mfcid,')')"/>
    </xsl:call-template>
    <fo:block line-height="2">
      <xsl:apply-templates select="." mode="frame"/>
      <xsl:apply-templates select="prompt|control"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="control[*//text]">
    <xsl:apply-templates select="." mode="frame"/>
  </xsl:template>

  <xsl:template match="text">
    <xsl:variable name="type">
      <xsl:choose>
        <xsl:when test="ancestor::caption">caption</xsl:when>
        <xsl:when test="ancestor::description">description</xsl:when>
        <xsl:otherwise>prompt</xsl:otherwise>
      </xsl:choose>
    </xsl:variable>
    <fo:block>
      <fo:inline font-size="80%">
        <xsl:value-of select="concat($type,' instruction ')"/>
      </fo:inline>
      <fo:inline font-weight="bold">
        <xsl:value-of select="."/>
      </fo:inline>
      <xsl:text>: </xsl:text>
      <xsl:call-template name="make-lines">
        <xsl:with-param name="str" select="text"/>
      </xsl:call-template>
    </fo:block>
  </xsl:template>

  <xsl:template match="control"/>

  <!-- Prompts stuff -->

  <xsl:template match="promptItemRefs" mode="new-page">
    <xsl:call-template name="start-page">
      <xsl:with-param name="title" select="'Prompt item references'"/>
    </xsl:call-template>
    <fo:block line-height="2">
      <xsl:apply-templates/>
    </fo:block>
  </xsl:template>

  <xsl:template match="promptItem[@ref]"/>

  <xsl:template match="generalPrompts" mode="new-page">
    <xsl:call-template name="start-page">
      <xsl:with-param name="title" select="'General prompts'"/>
    </xsl:call-template>
    <!--fo:block line-height="2">
      <xsl:apply-templates select="caption|action|container"/>
    </fo:block-->
  </xsl:template>

  <!-- Start a new page -->
  <xsl:template name="start-page">
    <xsl:param name="title"/>
    <xsl:param name="word-count"/>
    <!-- Title: "AMIS localization sheet - $title" -->
    <fo:block font-weight="bold" font-size="120%" text-align="center"
      space-after="24pt" break-before="page">
      <xsl:value-of select="concat('Amis localization sheet&#xa0;&#x2014; ',
        $title)"/>
    </fo:block>
    <!-- Space for language, translator(s) and date information -->
    <fo:block line-height="2" space-after="18pt">
      <xsl:text>Language: </xsl:text>
      <xsl:call-template name="make-line"/>
      <xsl:text>Translator(s): </xsl:text>
      <xsl:call-template name="make-line"/>
      <xsl:call-template name="make-line"/>
      <xsl:text>Date: </xsl:text>
      <xsl:call-template name="make-line"/>
      <!--xsl:value-of select="concat('Word count: ',$word-count)"/-->
    </fo:block>
    <!-- Move the instructions to a separate document -->
    <!--
    <fo:block>
      Instruction for translators: please provide a translation for the
      following captions and descriptions in bold below.
    </fo:block>
    -->
  </xsl:template>

  <!-- Draw a horizontal line that spans the whole page -->
  <xsl:template name="make-line">
    <fo:leader baseline-shift="-20%"/>
    <fo:block/>
  </xsl:template>

  <!-- Frame a caption/description pair in a box -->
  <xsl:template match="dialog" mode="frame">
    <fo:block>
      <xsl:if test="caption//text and description//text">
        <xsl:attribute name="border">dotted black</xsl:attribute>
        <xsl:attribute name="padding">1em</xsl:attribute>
        <xsl:attribute name="margin-top">1em</xsl:attribute>
      </xsl:if>
      <xsl:apply-templates select="(caption|description)[text|*//text]"/>
    </fo:block>
  </xsl:template>

  <xsl:template match="*" mode="frame">
    <fo:block>
      <xsl:if test="caption//text and description//text">
        <xsl:attribute name="border">dotted black</xsl:attribute>
        <xsl:attribute name="padding">1em</xsl:attribute>
        <xsl:attribute name="margin-top">1em</xsl:attribute>
      </xsl:if>
      <xsl:apply-templates select="(.//caption|.//description)[text|*//text]"/>
    </fo:block>
  </xsl:template>

  <!-- Footnotes are normally skipped unless called from another template -->
  <xsl:template match="note">
    <xsl:variable name="num">
      <xsl:number count="note[../caption/text]" level="any"
        from="containers/container|dialogs/dialog"/>
    </xsl:variable>
    <fo:footnote>
      <fo:inline baseline-shift="super" font-size="60%"><xsl:value-of
          select="$num"/></fo:inline>
      <fo:footnote-body>
        <fo:list-block provisional-label-separation="0pt"
          provisional-distance-between-starts="18pt"
          space-after.optimum="6pt" font-size="80%">
          <fo:list-item>
            <fo:list-item-label end-indent="label-end()">
              <fo:block><xsl:value-of select="$num"/></fo:block>
            </fo:list-item-label>
            <fo:list-item-body start-indent="body-start()">
              <fo:block><xsl:value-of select="."/></fo:block>
            </fo:list-item-body>
          </fo:list-item>
        </fo:list-block>
      </fo:footnote-body>
    </fo:footnote>
  </xsl:template>

  <!-- Tries to make enough lines for the translators; i.e. 3 lines + one line
  per extra 32 characters -->
  <xsl:template name="make-lines">
    <xsl:param name="str"/>
    <xsl:call-template name="make-line"/>
    <xsl:call-template name="make-line"/>
    <xsl:call-template name="make-line"/>
    <xsl:call-template name="make-more-lines">
      <xsl:with-param name="str" select="$str"/>
    </xsl:call-template>
  </xsl:template>

  <xsl:template name="make-more-lines">
    <xsl:param name="str"/>
    <xsl:if test="string-length($str)&gt;32">
      <xsl:call-template name="make-line"/>
      <xsl:call-template name="make-more-lines">
        <xsl:with-param name="str" select="substring($str,32)"/>
      </xsl:call-template>
    </xsl:if>
  </xsl:template>

</xsl:transform>
