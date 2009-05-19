<?xml version="1.0"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
    <html>
    <body>
    <h2>Log Output</h2>
    <table border="1">
    <xsl:apply-templates/>
    </table>
    </body>
    </html>
</xsl:template>

<xsl:template match="/*">
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="exception">
  <tr>
  <td valign="top">
  <i><xsl:value-of select="time"/></i>
  </td>
  <td>
  <font color="#ff0000"><b>EXCEPTION:</b></font>
  <xsl:value-of select="description"/><br/><br/>
  <xsl:call-template name="backtrace">
  </xsl:call-template>
  </td>
  </tr>
</xsl:template>

<xsl:template name="backtrace" match="backtrace">
        <tr>
            <td align="left">object</td>
            <td align="right">function</td>
            <td align="center">offset</td>
            <td align="center">return</td>
        </tr>
    <xsl:for-each select="backtrace/frame">
        <tr>
        <td align="left"><xsl:value-of select="object"/></td>
        <td align="right"><xsl:value-of select="function"/></td>
        <td align="center"><xsl:value-of select="offset"/></td>
        <td align="center"><xsl:value-of select="return"/></td>
        </tr>
    </xsl:for-each>
</xsl:template>

</xsl:stylesheet>
