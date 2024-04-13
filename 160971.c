xmlNodeDumpOutputInternal(xmlSaveCtxtPtr ctxt, xmlNodePtr cur) {
    int format;
    xmlNodePtr tmp;
    xmlChar *start, *end;
    xmlOutputBufferPtr buf;

    if (cur == NULL) return;
    buf = ctxt->buf;
    if (cur->type == XML_XINCLUDE_START)
	return;
    if (cur->type == XML_XINCLUDE_END)
	return;
    if ((cur->type == XML_DOCUMENT_NODE) ||
        (cur->type == XML_HTML_DOCUMENT_NODE)) {
	xmlDocContentDumpOutput(ctxt, (xmlDocPtr) cur);
	return;
    }
#ifdef LIBXML_HTML_ENABLED
    if (ctxt->options & XML_SAVE_XHTML) {
        xhtmlNodeDumpOutput(ctxt, cur);
        return;
    }
    if (((cur->type != XML_NAMESPACE_DECL) && (cur->doc != NULL) &&
         (cur->doc->type == XML_HTML_DOCUMENT_NODE) &&
         ((ctxt->options & XML_SAVE_AS_XML) == 0)) ||
        (ctxt->options & XML_SAVE_AS_HTML)) {
	htmlNodeDumpOutputInternal(ctxt, cur);
	return;
    }
#endif
    if (cur->type == XML_DTD_NODE) {
        xmlDtdDumpOutput(ctxt, (xmlDtdPtr) cur);
	return;
    }
    if (cur->type == XML_DOCUMENT_FRAG_NODE) {
        xmlNodeListDumpOutput(ctxt, cur->children);
	return;
    }
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
	    if (cur->name != xmlStringTextNoenc) {
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
	        if (ctxt->format == 2)
	            xmlOutputBufferWriteWSNonSig(ctxt, 0);
	        else
	            xmlOutputBufferWrite(buf, 1, " ");
		xmlOutputBufferWriteString(buf, (const char *)cur->content);
	    }
	    xmlOutputBufferWrite(buf, 2, "?>");
	} else {
	    xmlOutputBufferWrite(buf, 2, "<?");
	    xmlOutputBufferWriteString(buf, (const char *)cur->name);
	    if (ctxt->format == 2)
	        xmlOutputBufferWriteWSNonSig(ctxt, 0);
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
		if ((*end == ']') && (*(end + 1) == ']') &&
		    (*(end + 2) == '>')) {
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
    if (cur->type == XML_NAMESPACE_DECL) {
	xmlNsDumpOutputCtxt(ctxt, (xmlNsPtr) cur);
	return;
    }

    format = ctxt->format;
    if (format == 1) {
	tmp = cur->children;
	while (tmp != NULL) {
	    if ((tmp->type == XML_TEXT_NODE) ||
		(tmp->type == XML_CDATA_SECTION_NODE) ||
		(tmp->type == XML_ENTITY_REF_NODE)) {
		ctxt->format = 0;
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
    if (cur->properties != NULL)
        xmlAttrListDumpOutput(ctxt, cur->properties);

    if (((cur->type == XML_ELEMENT_NODE) || (cur->content == NULL)) &&
	(cur->children == NULL) && ((ctxt->options & XML_SAVE_NO_EMPTY) == 0)) {
        if (ctxt->format == 2)
            xmlOutputBufferWriteWSNonSig(ctxt, 0);
        xmlOutputBufferWrite(buf, 2, "/>");
	ctxt->format = format;
	return;
    }
    if (ctxt->format == 2)
        xmlOutputBufferWriteWSNonSig(ctxt, 1);
    xmlOutputBufferWrite(buf, 1, ">");
    if ((cur->type != XML_ELEMENT_NODE) && (cur->content != NULL)) {
	xmlOutputBufferWriteEscape(buf, cur->content, ctxt->escape);
    }
    if (cur->children != NULL) {
	if (ctxt->format == 1) xmlOutputBufferWrite(buf, 1, "\n");
	if (ctxt->level >= 0) ctxt->level++;
	xmlNodeListDumpOutput(ctxt, cur->children);
	if (ctxt->level > 0) ctxt->level--;
	if ((xmlIndentTreeOutput) && (ctxt->format == 1))
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
    if (ctxt->format == 2)
        xmlOutputBufferWriteWSNonSig(ctxt, 0);
    xmlOutputBufferWrite(buf, 1, ">");
    ctxt->format = format;
}