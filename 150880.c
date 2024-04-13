xmlDumpNotationDecl(xmlBufferPtr buf, xmlNotationPtr nota) {
    if ((buf == NULL) || (nota == NULL))
        return;
    xmlBufferWriteChar(buf, "<!NOTATION ");
    xmlBufferWriteCHAR(buf, nota->name);
    if (nota->PublicID != NULL) {
	xmlBufferWriteChar(buf, " PUBLIC ");
	xmlBufferWriteQuotedString(buf, nota->PublicID);
	if (nota->SystemID != NULL) {
	    xmlBufferWriteChar(buf, " ");
	    xmlBufferWriteQuotedString(buf, nota->SystemID);
	}
    } else {
	xmlBufferWriteChar(buf, " SYSTEM ");
	xmlBufferWriteQuotedString(buf, nota->SystemID);
    }
    xmlBufferWriteChar(buf, " >\n");
}