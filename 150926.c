xmlCopyEnumeration(xmlEnumerationPtr cur) {
    xmlEnumerationPtr ret;

    if (cur == NULL) return(NULL);
    ret = xmlCreateEnumeration((xmlChar *) cur->name);
    if (ret == NULL) return(NULL);

    if (cur->next != NULL) ret->next = xmlCopyEnumeration(cur->next);
    else ret->next = NULL;

    return(ret);
}