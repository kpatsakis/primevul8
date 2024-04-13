xmlLineNumbersDefault(int val) {
    int old = xmlLineNumbersDefaultValue;

    xmlLineNumbersDefaultValue = val;
    return(old);
}