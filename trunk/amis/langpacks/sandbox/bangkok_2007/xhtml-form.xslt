<?xml version="1.0" encoding="UTF-8"?>

<!-- Create an XHTML form for one container/dialog or prompts. -->
<!-- Languages for Bangkok 2007 workshop: Chinese, Japanese, Malay, Nepali,
Filipino, Thai -->
<!-- $Id$ -->

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml">

  <xsl:output method="xml" version="1.0" encoding="UTF-8"/>

  <xsl:param name="type" value="'prompts'"/>
  <xsl:param name="id" value="'none'"/>
  <xsl:param name="lang" value="''"/>
  <xsl:param name="url" value="''"/>

  <xsl:template match="/">
    <xsl:variable name="language">
      <xsl:choose>
        <xsl:when test="$lang='ja'">
          <xsl:text>Japanese</xsl:text>
        </xsl:when>
        <xsl:when test="$lang='ms'">
          <xsl:text>Malay</xsl:text>
        </xsl:when>
        <xsl:when test="$lang='ne'">
          <xsl:text>Nepali</xsl:text>
        </xsl:when>
        <xsl:when test="$lang='tl'">
          <!-- We abuse the tl (tagalog) tag here -->
          <xsl:text>Filipino</xsl:text>
        </xsl:when>
        <xsl:when test="$lang='th'">
          <xsl:text>Thai</xsl:text>
        </xsl:when>
        <xsl:when test="$lang='zh'">
          <xsl:text>Chinese</xsl:text>
        </xsl:when>
      </xsl:choose>
    </xsl:variable>
    <html xmlns="http://www.w3.org/1999/xhtml">
      <head>
        <meta http-equiv="Content-Type" content="text/xml+html; charset=UTF-8"/>
        <title>
          <xsl:value-of select="concat('Amis localization interface for ',
            $language)"/>
        </title>
        <style type="text/css">
      body { background-color: white; color: black; font-family: sans-serif; }
      table { border-collapse: collapse }
      td { border: solid black thin; padding: 4px }
      select { border: solid black thin }
      textarea { background-color: #ffd; border: solid black thin;
        padding:4px; margin-bottom: 4px }
      input { margin-left: 2em; }
      .blank { margin-top: 90%; font-size: small; text-align: right; }
      .ok { background-color: #dfd; }
      .todo { background-color: #ffd; }
      .check { background-color: #fdd; }
        </style>
        <script type="text/javascript">
          <xsl:comment>
function check_reset()
{
  var check = window.confirm("Do you really want to reset everything?");
  return check;
}

function update_textarea(node)
{
  node.className = "ok";
  document.getElementById("okstatus" + node.name).checked = "true";
  document.getElementById("checkstatus" + node.name).removeAttribute("checked");
  document.getElementById("todostatus" + node.name).removeAttribute("checked");
}
//</xsl:comment>
        </script>
      </head>
      <body>
        <h1>
          <xsl:text>Amis localization interface</xsl:text>
          <br/>
          <xsl:value-of select="$language"/>
          <br/>
          <!-- Put the name of the form in the title as well -->
          <xsl:choose>
            <xsl:when test="$type='container'">
              <xsl:apply-templates select="//container[@id=$id]" mode="title"/>
            </xsl:when>
            <xsl:when test="$type='dialog'">
              <xsl:apply-templates select="//dialog[@mfcid=$id]" mode="title"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:apply-templates select="//prompts/*[name()=$type]"
                mode="title"/>
            </xsl:otherwise>
          </xsl:choose>
        </h1>
        <xsl:variable name="choose-href">
          <xsl:value-of
            select="concat('/cgi-bin/choose-form.pl?lang=',$lang)"/>
        </xsl:variable>
        <p>
          <a href="{$choose-href}">(Choose a different form)</a>
        </p>
        <form action="/cgi-bin/choose-form.pl" method="post"
          onreset="return check_reset()" name="translation_sheet">
          <input type="hidden" name="lang" value="{$lang}"/>
          <input type="hidden" name="form">
            <xsl:attribute name="value">
              <xsl:choose>
                <xsl:when test="$type='container'">
                  <xsl:value-of select="concat('container_',$id)"/>
                </xsl:when>
                <xsl:when test="$type='dialog'">
                  <xsl:value-of select="concat('dialog_',$id)"/>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:value-of select="$type"/>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:attribute>
          </input>
          <!-- Choose which form to create from the input parameters -->
          <xsl:choose>
            <xsl:when test="$type='container'">
              <xsl:apply-templates select="//container[@id=$id]"/>
            </xsl:when>
            <xsl:when test="$type='dialog'">
              <xsl:apply-templates select="//dialog[@mfcid=$id]"/>
            </xsl:when>
            <xsl:otherwise>
              <xsl:apply-templates select="//prompts/*[name()=$type]"/>
            </xsl:otherwise>
          </xsl:choose>
        </form>
        <p>
          <a href="{$choose-href}">(Choose a different form)</a>
        </p>
        <p class="blank">This space intentionally left blank.</p>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="containers/container" mode="title">
    <xsl:value-of select="concat('Container &#x201c;',caption/text,
      '&#x201d; (',@id,')')"/>
  </xsl:template>

  <xsl:template match="dialogs/dialog" mode="title">
    <xsl:value-of select="concat('Dialog &#x201c;',caption/text,
      '&#x201d; (',@mfcid,')')"/>
  </xsl:template>

  <xsl:template match="promptItemRefs" mode="title">
    <xsl:text>Prompt item references</xsl:text>
  </xsl:template>

  <!-- Output a form for general prompts -->
  <xsl:template match="generalPrompts" mode="title">
    <xsl:text>General Prompts</xsl:text>
  </xsl:template>

  <!-- All forms have the same body -->
  <xsl:template match="containers/container|dialogs/dialog|promptItemRefs|generalPrompts">
    <table>
      <thead>
        <tr>
          <th>English</th>
          <th>Translation</th>
        </tr>
      </thead>
      <tbody>
        <xsl:apply-templates/>
        <tr>
          <td colspan="2">
            <input type="submit" value="Save" style="margin-left:0"/>
            <!-- Reset is buggy, and dangerous anyway so let's forget about it
            at the moment -->
            <!--input type="reset"/-->
          </td>
        </tr>
      </tbody>
    </table>
  </xsl:template>

  <!-- Skip items that should not be translated -->
  <xsl:template match="text[ancestor::control[@translate='no']]"/>

  <!-- Skip mnemonics and accelerators too -->
  <xsl:template match="mnemonic/text"/>
  <xsl:template match="accelerator/text"/>

  <!-- Output a translation row: the original string on the left; and a
  textarea of the right for input -->
  <xsl:template match="text">
    <tr id="{@id}">
      <td valign="top">
        <xsl:value-of select="../originalText"/>
        <br/>
        <a href="{$url}#{@id}">[link]</a>
      </td>
      <td valign="top" style="text-align:right">
        <textarea cols="64" rows="6" name="{@id}" id="textarea{@id}" 
          onchange="update_textarea(this)">
          <xsl:if test="$lang='ar' or $lang='ur'">
            <xsl:attribute name="dir">rtl</xsl:attribute>
          </xsl:if>
          <xsl:value-of select="."/>
        </textarea>
        <br/>
        <xsl:text>Status:</xsl:text>
        <input type="radio" name="status{@id}" value="ok" id="okstatus{@id}"
          onclick="document.getElementById('textarea'+'{@id}').className='ok'">
          <xsl:if test="@status='ok'">
            <xsl:attribute name="checked" value="true"/>
          </xsl:if>
          <span class="ok">OK</span>
        </input>
        <input type="radio" name="status{@id}" value="check"
          id="checkstatus{@id}"
          onclick="document.getElementById('textarea'+'{@id}').className='check'">
          <xsl:if test="@status='check'">
            <xsl:attribute name="checked" value="true"/>
          </xsl:if>
          <span class="check">check</span>
        </input>
        <input type="radio" name="status{@id}" value="todo"
          id="todostatus{@id}"
          onclick="document.getElementById('textarea'+'{@id}').className='todo'">
          <xsl:if test="@status='todo' or not(@status)">
            <xsl:attribute name="checked" value="true"/>
          </xsl:if>
          <span class="todo">TODO</span>
        </input>
        <input type="submit" value="Save"
          onclick="document.translation_sheet.action='{$url}#{@id}'"/>
      </td>
    </tr>
  </xsl:template>

  <!-- Skip notes; they were in the translation sheets anyway -->
  <xsl:template match="note"/>

  <!-- Skip text nodes, we have all the text we need -->
  <xsl:template match="text()"/>

</xsl:transform>
