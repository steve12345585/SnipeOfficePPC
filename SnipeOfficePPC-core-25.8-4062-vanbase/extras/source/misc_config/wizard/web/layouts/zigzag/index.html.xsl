<?xml version="1.0" encoding="UTF-8"?>
<!--
 * This file is part of the SnipeOffice project.
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
<!-- =================================================

This template is a skeleton for single level TOC pages 
Do not overwrite this ! copy it and complete the missing
code.

I use the @ character whereever there is a missing code, so 
you can use a simple find to navigate and find the
places...

====================================================== -->
<!DOCTYPE xsl:stylesheet [

<!ENTITY x1 "0">
<!ENTITY x2 "250">
<!ENTITY x3 "500">

<!ENTITY y1 "0">
<!ENTITY y2 "150">
<!ENTITY y3 "0">


]>
<xsl:stylesheet version="1.0" 
   xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns="http://www.w3.org/1999/xhtml">
	
	<xsl:output method               = "html"
                media-type           = "text/html"
                indent               = "yes"
                doctype-public       = "-//W3C//DTD HTML 4.0 Transitional//EN"
                omit-xml-declaration = "yes"
                standalone           = "yes" />
  
	<xsl:include href="../layout.xsl"/>
	 
	 <!-- =============================
	               HTML BODY
	 ================================== -->
	 
	 <xsl:template name="body">
	 	<body>
	   <xsl:call-template name="title"/>
	 	<xsl:call-template name="toc"/>
	 	</body>
	 </xsl:template>
	 
	 
	 <xsl:template name="toc">
		<!--
		     @ pre toc html here
		-->
		
		<xsl:apply-templates select="/session/content/document"/>
		
		<!--
		     @ post toc html here
		-->
		
	 </xsl:template>
	 
	
	 
	<!-- also when using groups, in the end it comes 
	to this template, which is called for each document -->
	<xsl:template match="document">
	  
	  <xsl:variable name="i"	select="position() - 1"/>
	
	  <xsl:variable name="x1">
	    <xsl:call-template name="x"/>
	  </xsl:variable>
	  
		<xsl:variable name="x" select="number($x1) + 50 "/>

	  <xsl:variable name="y1">
	  	 <xsl:call-template name="y"/>
	  </xsl:variable>

		<xsl:variable name="y" select="(floor($i div 3)) * 300 + number($y1) + 80 "/>
	
	  <div style="position:absolute; padding:15px; left:{$x}px; top:{$y}px; width:170px; height:220px; z-index:1" class="tcolor">
  		<div align="center">

    		<xsl:apply-templates select="@icon"/>
    		<p>
	  		<xsl:apply-templates select="@title"/>
			<xsl:apply-templates select="@description"/>
			<xsl:apply-templates select="@author"/>
			<xsl:apply-templates select="@create-date"/>
			<xsl:apply-templates select="@update-date"/>
			<xsl:apply-templates select="@filename"/>
			<xsl:apply-templates select="@format"/>
			<xsl:apply-templates select="@pages"/>
			<xsl:apply-templates select="@size"/>
			</p>
		  </div>
	  </div>
	</xsl:template>
		
	<xsl:template name="x">
		<xsl:variable name="i" select="position()"/>
		<xsl:choose>
			<xsl:when test="$i = 2">
				&x2;
		   </xsl:when>
			<xsl:when test="$i = 3">
				&x3;
		   </xsl:when>
			<xsl:otherwise>
				<xsl:choose>
					<xsl:when test="($i mod 3) = 1">
						&x1;
					</xsl:when>
					<xsl:when test="($i mod 3) = 2">
						&x3;
					</xsl:when>
					<xsl:when test="($i mod 3) = 0">
						&x2;
					</xsl:when>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template name="y">
		<xsl:variable name="i" select="position()"/>
		<xsl:choose>
			<xsl:when test="$i = 2">
				&y2;
		   </xsl:when>
			<xsl:when test="$i = 3">
				&y3;
		   </xsl:when>
			<xsl:otherwise>
				<xsl:choose>
					<xsl:when test="($i mod 3) = 1">
						&y1;
					</xsl:when>
					<xsl:when test="($i mod 3) = 2">
						&y3;
					</xsl:when>
					<xsl:when test="($i mod 3) = 0">
						&y2;
					</xsl:when>
				</xsl:choose>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
			 
	 <xsl:template name="document-group">
	 	<xsl:param name="group"/>
	 	
	 	<!-- @ pre group code here -->
	 	
	 	<!-- - - -->

		 	<xsl:variable name="count" select="(position() - 1) * $group + 1"/>
		 	
		 	<xsl:for-each select="/session/content/document[$count &lt;= position() and position() &lt; ($count + $group)]">
		 	
				<xsl:apply-templates select="."/>
			 	  
		 	</xsl:for-each>
	 	
	 	<!-- @ post group code here -->
	 	
	 	<!-- - - -->
	 	
	 </xsl:template>

	 	 
	 <xsl:template name="title">
	    <!--
	      @ Pre title html code here
	    -->
	   <div style="position:absolute; left:280px; top:8px; width:220px; z-index:2; padding:10px" class="ccolor">
		  <div align="center" class="toctitle">
		  	<xsl:value-of select="/session/general-info/@title"/>
		  	&#160;
	    <!--
	      @ Post title html code here
	    -->
	    	</div>
    	</div>
	 </xsl:template>
	     
</xsl:stylesheet>
