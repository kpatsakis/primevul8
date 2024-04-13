xmlDumpEnumeration(xmlBufferPtr buf, xmlEnumerationPtr cur) {
    if ((buf == NULL) || (cur == NULL))
        return;

    xmlBufferWriteCHAR(buf, cur->name);
    if (cur->next == NULL)
	xmlBufferWriteChar(buf, ")");
    else {
	xmlBufferWriteChar(buf, " | ");
	xmlDumpEnumeration(buf, cur->next);
    }
}