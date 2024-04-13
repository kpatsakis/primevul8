xmlAttrSerializeContent(xmlOutputBufferPtr buf, xmlAttrPtr attr)
{
    xmlNodePtr children;

    children = attr->children;
    while (children != NULL) {
        switch (children->type) {
            case XML_TEXT_NODE:
	        xmlBufAttrSerializeTxtContent(buf->buffer, attr->doc,
		                              attr, children->content);
		break;
            case XML_ENTITY_REF_NODE:
                xmlBufAdd(buf->buffer, BAD_CAST "&", 1);
                xmlBufAdd(buf->buffer, children->name,
                             xmlStrlen(children->name));
                xmlBufAdd(buf->buffer, BAD_CAST ";", 1);
                break;
            default:
                /* should not happen unless we have a badly built tree */
                break;
        }
        children = children->next;
    }
}