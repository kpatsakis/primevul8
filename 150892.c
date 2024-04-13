xmlFreeRefList(xmlListPtr list_ref) {
    if (list_ref == NULL) return;
    xmlListDelete(list_ref);
}