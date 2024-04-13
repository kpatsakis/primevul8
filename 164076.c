htmlHandleOmittedElem(int val) {
    int old = htmlOmittedDefaultValue;

    htmlOmittedDefaultValue = val;
    return(old);
}