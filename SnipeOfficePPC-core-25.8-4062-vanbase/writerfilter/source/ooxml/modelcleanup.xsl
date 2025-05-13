<!--
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
-->
<xsl:stylesheet
    version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:rng="http://relaxng.org/ns/structure/1.0"
    xmlns:xalan="http://xml.apache.org/xalan"
    exclude-result-prefixes = "xalan"
    xml:indent="true">
  <xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes" omit-xml-declaration="no"/>


  <xsl:key name="resources"
           match="resource[not(@generated)]" use="@name" />

  <xsl:key name="resourcetags"
           match="resource/@tag" use="ancestor::resource/@name"/>

  <xsl:template name="generateresource">
    <xsl:param name="resource"/>
    <xsl:element name="resource">
      <xsl:attribute name="name">
        <xsl:value-of select="@name"/>
      </xsl:attribute>
      <xsl:attribute name="resource">
        <xsl:value-of select="$resource"/>
      </xsl:attribute>
      <xsl:attribute name="generated">
        <xsl:text>yes</xsl:text>
      </xsl:attribute>
      <xsl:for-each select="key('resourcetags', @name)">
        <xsl:attribute name="tag">
          <xsl:value-of select="."/>
        </xsl:attribute>
      </xsl:for-each>
    </xsl:element>
  </xsl:template>

  <xsl:key name="nsaliases" match="//namespace-alias" use="@name"/>

  <xsl:template name="resourcevalues">
    <xsl:variable name="definename" select="@name"/>
    <xsl:variable name="namespace" select="key('nsaliases', ancestor::namespace/rng:grammar/@ns)/@alias"/>
    <xsl:element name="resource">
      <xsl:attribute name="name">
        <xsl:value-of select="@name"/>
      </xsl:attribute>
      <xsl:attribute name="resource">List</xsl:attribute>
      <xsl:attribute name="generated">yes</xsl:attribute>
      <xsl:for-each select="key('resourcetags', @name)">
        <xsl:attribute name="tag">
          <xsl:value-of select="."/>
        </xsl:attribute>
      </xsl:for-each>
      <xsl:for-each select=".//rng:value">
        <xsl:element name="value">
          <xsl:attribute name="name">
            <xsl:value-of select="translate(., '-+ ,', 'mp__')"/>
          </xsl:attribute>
          <xsl:attribute name="tokenid">
            <xsl:text>ooxml:Value_</xsl:text>
            <xsl:value-of select="translate($namespace, '-', '_')"/>
            <xsl:text>_</xsl:text>
            <xsl:value-of select="$definename"/>
            <xsl:text>_</xsl:text>
            <xsl:value-of select="translate(., '-+ ,', 'mp__')"/>
          </xsl:attribute>
          <xsl:value-of select="."/>
        </xsl:element>
      </xsl:for-each>
    </xsl:element>
  </xsl:template>

    <xsl:template name="typeofdefine">
        <xsl:for-each select="rng:data">
            <xsl:choose>
                <xsl:when test="@type='base64Binary'"> 
                    <xsl:text>String</xsl:text>
                </xsl:when>
                <xsl:when test="@type='boolean'"> 
                    <xsl:text>Boolean</xsl:text>
                </xsl:when>
                <xsl:when test="@type='byte'"> 
                    <xsl:text>Integer</xsl:text>
                </xsl:when>
                <xsl:when test="@type='dateTime'"> 
                    <xsl:text>String</xsl:text>
                </xsl:when>
                <xsl:when test="@type='decimal'"> 
                    <xsl:text>Integer</xsl:text>
                </xsl:when>
                <xsl:when test="@type='float'"> 
                    <xsl:text>Float</xsl:text>
                </xsl:when>
                <xsl:when test="@type='hexBinary'"> 
                    <xsl:text>Hex</xsl:text>
                </xsl:when>
                <xsl:when test="@type='int'"> 
                    <xsl:text>Integer</xsl:text>
                </xsl:when>
                <xsl:when test="@type='integer'"> 
                    <xsl:text>Integer</xsl:text>
                </xsl:when>
                <xsl:when test="@type='positiveInteger'"> 
                    <xsl:text>Integer</xsl:text>
                </xsl:when>
                <xsl:when test="@type='string'"> 
                    <xsl:text>String</xsl:text>
                </xsl:when>
                <xsl:when test="@type='token'"> 
                    <xsl:text>Integer</xsl:text>
                </xsl:when>
                <xsl:when test="@type='unsignedInt'"> 
                    <xsl:text>Integer</xsl:text>
                </xsl:when>
                <xsl:when test="@type='unsignedLong'"> 
                    <xsl:text>Integer</xsl:text>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:text>Unknown</xsl:text>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:for-each>
    </xsl:template>
    
    <xsl:template name="typeofattribute">
        <xsl:for-each select="rng:ref">
            <xsl:variable name="name" select="@name"/>
            <xsl:for-each select="ancestor::namespace/rng:grammar/rng:define[@name=$name]">
                <xsl:call-template name="typeofdefine"/>
            </xsl:for-each>
        </xsl:for-each> 
    </xsl:template>
        
    <xsl:template name="generatevalueresource">
        <xsl:variable name="name" select="@name"/>
        <xsl:variable name="ns_id" select="generate-id(ancestor::namespace)"/>
        <resource>
            <xsl:attribute name="name">
                <xsl:value-of select="@name"/>
            </xsl:attribute>
            <xsl:attribute name="resource">Value</xsl:attribute>
            <xsl:attribute name="generated">yes</xsl:attribute>
            <xsl:for-each select="key('resourcetags', @name)[generate-id(ancestor::namespace) = $ns_id]">
              <xsl:attribute name="tag">
                <xsl:value-of select="."/>
              </xsl:attribute>
            </xsl:for-each>
            <xsl:for-each select=".//rng:attribute">
                <xsl:variable name="type">
                    <xsl:choose>
                        <xsl:when test="rng:ref[@name='ST_OnOff']">
                            <xsl:text>Boolean</xsl:text>
                        </xsl:when>
                        <xsl:when test="rng:text">
                            <xsl:text>String</xsl:text>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:call-template name="typeofattribute"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:variable>
                <attribute>
                    <xsl:attribute name="name">
                        <xsl:value-of select="@name"/>
                    </xsl:attribute>
                    <xsl:attribute name="tokenid">
                        <xsl:text>ooxml:</xsl:text>
                        <xsl:value-of select="$name"/>
                        <xsl:text>_</xsl:text>
                        <xsl:value-of select="@name"/>
                    </xsl:attribute>	  
                    <xsl:attribute name="action">
                        <xsl:text>setValue</xsl:text>
                    </xsl:attribute>
                </attribute>
                <xsl:if test="string-length($type) > 0">
                    <action name="start">
                        <xsl:attribute name="action">
                            <xsl:text>setDefault</xsl:text>
                            <xsl:value-of select="$type"/>
                            <xsl:text>Value</xsl:text>
                        </xsl:attribute>
                    </action>
                </xsl:if>
            </xsl:for-each>
        </resource>
    </xsl:template>
    
  <xsl:template match="namespace">
    <xsl:variable name="nsid" select="generate-id(.)"/>
    <xsl:element name="namespace">
      <xsl:copy-of select="@*"/>
      <xsl:copy-of select=".//start"/>
      <xsl:copy-of select="./rng:grammar"/>

      <xsl:for-each select=".//rng:define">
        <xsl:variable name="resources" select="key('resources', @name)[generate-id(ancestor::namespace) = $nsid]"/>
        <xsl:copy-of select="$resources"/>
        <xsl:if test="count($resources) = 0">
          <xsl:if test="substring(@name, 1, 3) = 'CT_'">
            <xsl:if test="./rng:attribute[@name='val']">
              <xsl:call-template name="generatevalueresource"/>
            </xsl:if>
          </xsl:if>
          <xsl:if test="substring(@name, 1, 3) = 'ST_'">
            <xsl:if test="./rng:data[@type='int']">
              <xsl:call-template name="generateresource">
                <xsl:with-param name="resource">Integer</xsl:with-param>
              </xsl:call-template>
            </xsl:if>
            <xsl:if test="./rng:data[@type='integer']">
              <xsl:call-template name="generateresource">
                <xsl:with-param name="resource">Integer</xsl:with-param>
              </xsl:call-template>
            </xsl:if>
            <xsl:if test="./rng:data[@type='long']">
              <xsl:call-template name="generateresource">
                <xsl:with-param name="resource">Integer</xsl:with-param>
              </xsl:call-template>
            </xsl:if>
            <xsl:if test="./rng:data[@type='unsignedInt']">
              <xsl:call-template name="generateresource">
                <xsl:with-param name="resource">Integer</xsl:with-param>
              </xsl:call-template>
            </xsl:if>
            <xsl:if test="./rng:data[@type='unsignedLong']">
              <xsl:call-template name="generateresource">
                <xsl:with-param name="resource">Integer</xsl:with-param>
              </xsl:call-template>
            </xsl:if>
            <xsl:if test="./rng:data[@type='boolean']">
              <xsl:call-template name="generateresource">
                <xsl:with-param name="resource">Boolean</xsl:with-param>
              </xsl:call-template>
            </xsl:if>
            <xsl:if test="./rng:data[@type='token']">
              <xsl:call-template name="generateresource">
                <xsl:with-param name="resource">String</xsl:with-param>
              </xsl:call-template>
            </xsl:if>
            <xsl:if test="./rng:data[@type='string']">
              <xsl:call-template name="generateresource">
                <xsl:with-param name="resource">String</xsl:with-param>
              </xsl:call-template>
            </xsl:if>
            <xsl:if test="./rng:data[@type='dateTime']">
              <xsl:call-template name="generateresource">
                <xsl:with-param name="resource">String</xsl:with-param>
              </xsl:call-template>
            </xsl:if>
            <xsl:if test="./rng:data[@type='hexBinary']">
              <xsl:call-template name="generateresource">
                <xsl:with-param name="resource">Hex</xsl:with-param>
              </xsl:call-template>
            </xsl:if>
            <xsl:if test="./rng:list">
              <xsl:call-template name="resourcevalues"/>
            </xsl:if>
          </xsl:if>
        </xsl:if>
      </xsl:for-each>
    </xsl:element>
  </xsl:template>
  
  <xsl:template match="namespace-alias">
    <namespace-alias>
      <xsl:for-each select="@*">
        <xsl:copy-of select="."/>
      </xsl:for-each>
    </namespace-alias>
  </xsl:template>

  <xsl:template match="token">
    <token>
      <xsl:for-each select="@*">
        <xsl:copy-of select="."/>
      </xsl:for-each>
    </token>
  </xsl:template>

  <xsl:template match="fasttoken">
    <xsl:copy-of select="."/>
  </xsl:template>

  <xsl:template match="/">
    <model>
      <xsl:apply-templates select=".//namespace-alias">
        <xsl:sort select="@id" data-type="number"/>
      </xsl:apply-templates>
      <xsl:apply-templates select=".//token"/>
      <xsl:apply-templates select=".//fasttoken"/>
      <xsl:apply-templates select=".//namespace"/>
    </model>
  </xsl:template>
</xsl:stylesheet>
