xhtmlNodeDumpOutput(xmlSaveCtxtPtr ctxt, xmlNodePtr cur) {
    int format, addmeta = 0;
    xmlNodePtr tmp;
    xmlChar *start, *end;
    xmlOutputBufferPtr buf;

    if (cur == NULL) return;
    if ((cur->type == XML_DOCUMENT_NODE) ||
        (cur->type == XML_HTML_DOCUMENT_NODE)) {
        xmlDocContentDumpOutput(ctxt, (xmlDocPtr) cur);
	return;
    }
    if (cur->type == XML_XINCLUDE_START)
	return;
    if (cur->type == XML_XINCLUDE_END)
	return;
    if (cur->type == XML_NAMESPACE_DECL) {
	xmlNsDumpOutputCtxt(ctxt, (xmlNsPtr) cur);
	return;
    }
    if (cur->type == XML_DTD_NODE) {
        xmlDtdDumpOutput(ctxt, (xmlDtdPtr) cur);
	return;
    }
    if (cur->type == XML_DOCUMENT_FRAG_NODE) {
        xhtmlNodeListDumpOutput(ctxt, cur->children);
	return;
    }
    buf = ctxt->buf;
    if (cur->type == XML_ELEMENT_DECL) {
        xmlBufDumpElementDecl(buf->buffer, (xmlElementPtr) cur);
	return;
    }
    if (cur->type == XML_ATTRIBUTE_DECL) {
        xmlBufDumpAttributeDecl(buf->buffer, (xmlAttributePtr) cur);
	return;
    }
    if (cur->type == XML_ENTITY_DECL) {
        xmlBufDumpEntityDecl(buf->buffer, (xmlEntityPtr) cur);
	return;
    }
    if (cur->type == XML_TEXT_NODE) {
	if (cur->content != NULL) {
	    if ((cur->name == xmlStringText) ||
		(cur->name != xmlStringTextNoenc)) {
                xmlOutputBufferWriteEscape(buf, cur->content, ctxt->escape);
	    } else {
		/*
		 * Disable escaping, needed for XSLT
		 */
		xmlOutputBufferWriteString(buf, (const char *) cur->content);
	    }
	}

	return;
    }
    if (cur->type == XML_PI_NODE) {
	if (cur->content != NULL) {
	    xmlOutputBufferWrite(buf, 2, "<?");
	    xmlOutputBufferWriteString(buf, (const char *)cur->name);
	    if (cur->content != NULL) {
		xmlOutputBufferWrite(buf, 1, " ");
		xmlOutputBufferWriteString(buf, (const char *)cur->content);
	    }
	    xmlOutputBufferWrite(buf, 2, "?>");
	} else {
	    xmlOutputBufferWrite(buf, 2, "<?");
	    xmlOutputBufferWriteString(buf, (const char *)cur->name);
	    xmlOutputBufferWrite(buf, 2, "?>");
	}
	return;
    }
    if (cur->type == XML_COMMENT_NODE) {
	if (cur->content != NULL) {
	    xmlOutputBufferWrite(buf, 4, "<!--");
	    xmlOutputBufferWriteString(buf, (const char *)cur->content);
	    xmlOutputBufferWrite(buf, 3, "-->");
	}
	return;
    }
    if (cur->type == XML_ENTITY_REF_NODE) {
        xmlOutputBufferWrite(buf, 1, "&");
	xmlOutputBufferWriteString(buf, (const char *)cur->name);
        xmlOutputBufferWrite(buf, 1, ";");
	return;
    }
    if (cur->type == XML_CDATA_SECTION_NODE) {
	if (cur->content == NULL || *cur->content == '\0') {
	    xmlOutputBufferWrite(buf, 12, "<![CDATA[]]>");
	} else {
	    start = end = cur->content;
	    while (*end != '\0') {
		if (*end == ']' && *(end + 1) == ']' && *(end + 2) == '>') {
		    end = end + 2;
		    xmlOutputBufferWrite(buf, 9, "<![CDATA[");
		    xmlOutputBufferWrite(buf, end - start, (const char *)start);
		    xmlOutputBufferWrite(buf, 3, "]]>");
		    start = end;
		}
		end++;
	    }
	    if (start != end) {
		xmlOutputBufferWrite(buf, 9, "<![CDATA[");
		xmlOutputBufferWriteString(buf, (const char *)start);
		xmlOutputBufferWrite(buf, 3, "]]>");
	    }
	}
	return;
    }
    if (cur->type == XML_ATTRIBUTE_NODE) {
        xmlAttrDumpOutput(ctxt, (xmlAttrPtr) cur);
	return;
    }

    format = ctxt->format;
    if (format == 1) {
	tmp = cur->children;
	while (tmp != NULL) {
	    if ((tmp->type == XML_TEXT_NODE) ||
		(tmp->type == XML_ENTITY_REF_NODE)) {
		format = 0;
		break;
	    }
	    tmp = tmp->next;
	}
    }
    xmlOutputBufferWrite(buf, 1, "<");
    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
        xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
	xmlOutputBufferWrite(buf, 1, ":");
    }

    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    if (cur->nsDef)
        xmlNsListDumpOutputCtxt(ctxt, cur->nsDef);
    if ((xmlStrEqual(cur->name, BAD_CAST "html") &&
	(cur->ns == NULL) && (cur->nsDef == NULL))) {
	/*
	 * 3.1.1. Strictly Conforming Documents A.3.1.1 3/
	 */
	xmlOutputBufferWriteString(buf,
		" xmlns=\"http://www.w3.org/1999/xhtml\"");
    }
    if (cur->properties != NULL)
        xhtmlAttrListDumpOutput(ctxt, cur->properties);

	if ((cur->type == XML_ELEMENT_NODE) &&
		(cur->parent != NULL) &&
		(cur->parent->parent == (xmlNodePtr) cur->doc) &&
		xmlStrEqual(cur->name, BAD_CAST"head") &&
		xmlStrEqual(cur->parent->name, BAD_CAST"html")) {

		tmp = cur->children;
		while (tmp != NULL) {
			if (xmlStrEqual(tmp->name, BAD_CAST"meta")) {
				xmlChar *httpequiv;

				httpequiv = xmlGetProp(tmp, BAD_CAST"http-equiv");
				if (httpequiv != NULL) {
					if (xmlStrcasecmp(httpequiv, BAD_CAST"Content-Type") == 0) {
						xmlFree(httpequiv);
						break;
					}
					xmlFree(httpequiv);
				}
			}
			tmp = tmp->next;
		}
		if (tmp == NULL)
			addmeta = 1;
	}

    if ((cur->type == XML_ELEMENT_NODE) && (cur->children == NULL)) {
	if (((cur->ns == NULL) || (cur->ns->prefix == NULL)) &&
	    ((xhtmlIsEmpty(cur) == 1) && (addmeta == 0))) {
	    /*
	     * C.2. Empty Elements
	     */
	    xmlOutputBufferWrite(buf, 3, " />");
	} else {
		if (addmeta == 1) {
			xmlOutputBufferWrite(buf, 1, ">");
			if (ctxt->format == 1) {
				xmlOutputBufferWrite(buf, 1, "\n");
				if (xmlIndentTreeOutput)
					xmlOutputBufferWrite(buf, ctxt->indent_size *
					(ctxt->level + 1 > ctxt->indent_nr ?
					ctxt->indent_nr : ctxt->level + 1), ctxt->indent);
			}
			xmlOutputBufferWriteString(buf,
				"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=");
			if (ctxt->encoding) {
				xmlOutputBufferWriteString(buf, (const char *)ctxt->encoding);
			} else {
				xmlOutputBufferWrite(buf, 5, "UTF-8");
			}
			xmlOutputBufferWrite(buf, 4, "\" />");
			if (ctxt->format == 1)
				xmlOutputBufferWrite(buf, 1, "\n");
		} else {
			xmlOutputBufferWrite(buf, 1, ">");
		}
	    /*
	     * C.3. Element Minimization and Empty Element Content
	     */
	    xmlOutputBufferWrite(buf, 2, "</");
	    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
		xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
		xmlOutputBufferWrite(buf, 1, ":");
	    }
	    xmlOutputBufferWriteString(buf, (const char *)cur->name);
	    xmlOutputBufferWrite(buf, 1, ">");
	}
	return;
    }
    xmlOutputBufferWrite(buf, 1, ">");
	if (addmeta == 1) {
		if (ctxt->format == 1) {
			xmlOutputBufferWrite(buf, 1, "\n");
			if (xmlIndentTreeOutput)
				xmlOutputBufferWrite(buf, ctxt->indent_size *
				(ctxt->level + 1 > ctxt->indent_nr ?
				ctxt->indent_nr : ctxt->level + 1), ctxt->indent);
		}
		xmlOutputBufferWriteString(buf,
			"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=");
		if (ctxt->encoding) {
			xmlOutputBufferWriteString(buf, (const char *)ctxt->encoding);
		} else {
			xmlOutputBufferWrite(buf, 5, "UTF-8");
		}
		xmlOutputBufferWrite(buf, 4, "\" />");
	}
    if ((cur->type != XML_ELEMENT_NODE) && (cur->content != NULL)) {
	xmlOutputBufferWriteEscape(buf, cur->content, ctxt->escape);
    }

#if 0
    /*
    * This was removed due to problems with HTML processors.
    * See bug #345147.
    */
    /*
     * 4.8. Script and Style elements
     */
    if ((cur->type == XML_ELEMENT_NODE) &&
	((xmlStrEqual(cur->name, BAD_CAST "script")) ||
	 (xmlStrEqual(cur->name, BAD_CAST "style"))) &&
	((cur->ns == NULL) ||
	 (xmlStrEqual(cur->ns->href, XHTML_NS_NAME)))) {
	xmlNodePtr child = cur->children;

	while (child != NULL) {
	    if (child->type == XML_TEXT_NODE) {
		if ((xmlStrchr(child->content, '<') == NULL) &&
		    (xmlStrchr(child->content, '&') == NULL) &&
		    (xmlStrstr(child->content, BAD_CAST "]]>") == NULL)) {
		    /* Nothing to escape, so just output as is... */
		    /* FIXME: Should we do something about "--" also? */
		    int level = ctxt->level;
		    int indent = ctxt->format;

		    ctxt->level = 0;
		    ctxt->format = 0;
		    xmlOutputBufferWriteString(buf, (const char *) child->content);
		    /* (We cannot use xhtmlNodeDumpOutput() here because
		     * we wish to leave '>' unescaped!) */
		    ctxt->level = level;
		    ctxt->format = indent;
		} else {
		    /* We must use a CDATA section.  Unfortunately,
		     * this will break CSS and JavaScript when read by
		     * a browser in HTML4-compliant mode. :-( */
		    start = end = child->content;
		    while (*end != '\0') {
			if (*end == ']' &&
			    *(end + 1) == ']' &&
			    *(end + 2) == '>') {
			    end = end + 2;
			    xmlOutputBufferWrite(buf, 9, "<![CDATA[");
			    xmlOutputBufferWrite(buf, end - start,
						 (const char *)start);
			    xmlOutputBufferWrite(buf, 3, "]]>");
			    start = end;
			}
			end++;
		    }
		    if (start != end) {
			xmlOutputBufferWrite(buf, 9, "<![CDATA[");
			xmlOutputBufferWrite(buf, end - start,
			                     (const char *)start);
			xmlOutputBufferWrite(buf, 3, "]]>");
		    }
		}
	    } else {
		int level = ctxt->level;
		int indent = ctxt->format;

		ctxt->level = 0;
		ctxt->format = 0;
		xhtmlNodeDumpOutput(ctxt, child);
		ctxt->level = level;
		ctxt->format = indent;
	    }
	    child = child->next;
	}
    }
#endif

    if (cur->children != NULL) {
	int indent = ctxt->format;

	if (format == 1) xmlOutputBufferWrite(buf, 1, "\n");
	if (ctxt->level >= 0) ctxt->level++;
	ctxt->format = format;
	xhtmlNodeListDumpOutput(ctxt, cur->children);
	if (ctxt->level > 0) ctxt->level--;
	ctxt->format = indent;
	if ((xmlIndentTreeOutput) && (format == 1))
	    xmlOutputBufferWrite(buf, ctxt->indent_size *
	                         (ctxt->level > ctxt->indent_nr ?
				  ctxt->indent_nr : ctxt->level),
				 ctxt->indent);
    }
    xmlOutputBufferWrite(buf, 2, "</");
    if ((cur->ns != NULL) && (cur->ns->prefix != NULL)) {
        xmlOutputBufferWriteString(buf, (const char *)cur->ns->prefix);
	xmlOutputBufferWrite(buf, 1, ":");
    }

    xmlOutputBufferWriteString(buf, (const char *)cur->name);
    xmlOutputBufferWrite(buf, 1, ">");
}