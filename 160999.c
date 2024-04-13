xmlAttrSerializeTxtContent(xmlBufferPtr buf, xmlDocPtr doc,
                           xmlAttrPtr attr, const xmlChar * string)
{
    xmlBufPtr buffer;

    if ((buf == NULL) || (string == NULL))
        return;
    buffer = xmlBufFromBuffer(buf);
    if (buffer == NULL)
        return;
    xmlBufAttrSerializeTxtContent(buffer, doc, attr, string);
    xmlBufBackToBuffer(buffer);
}