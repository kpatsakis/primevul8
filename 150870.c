xmlFreeValidCtxt(xmlValidCtxtPtr cur) {
    if (cur->vstateTab != NULL)
        xmlFree(cur->vstateTab);
    if (cur->nodeTab != NULL)
        xmlFree(cur->nodeTab);
    xmlFree(cur);
}