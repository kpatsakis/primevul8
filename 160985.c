xmlNodeDump(xmlBufferPtr buf, xmlDocPtr doc, xmlNodePtr cur, int level,
            int format)
{
    xmlBufPtr buffer;
    int ret;

    if ((buf == NULL) || (cur == NULL))
        return(-1);
    buffer = xmlBufFromBuffer(buf);
    if (buffer == NULL)
        return(-1);
    ret = xmlBufNodeDump(buffer, doc, cur, level, format);
    xmlBufBackToBuffer(buffer);
    if (ret > INT_MAX)
        return(-1);
    return((int) ret);
}