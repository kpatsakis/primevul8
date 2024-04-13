xmlFreeRef(xmlLinkPtr lk) {
    xmlRefPtr ref = (xmlRefPtr)xmlLinkGetData(lk);
    if (ref == NULL) return;
    if (ref->value != NULL)
        xmlFree((xmlChar *)ref->value);
    if (ref->name != NULL)
        xmlFree((xmlChar *)ref->name);
    xmlFree(ref);
}