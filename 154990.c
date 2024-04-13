xmlKeepBlanksDefault(int val) {
    int old = xmlKeepBlanksDefaultValue;

    xmlKeepBlanksDefaultValue = val;
    if (!val) xmlIndentTreeOutput = 1;
    return(old);
}