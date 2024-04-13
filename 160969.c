xmlDocDumpMemory(xmlDocPtr cur, xmlChar**mem, int *size) {
    xmlDocDumpFormatMemoryEnc(cur, mem, size, NULL, 0);
}