static sds sdsTestTemplateCallback(sds varname, void *arg) {
    UNUSED(arg);
    static const char *_var1 = "variable1";
    static const char *_var2 = "variable2";

    if (!strcmp(varname, _var1)) return sdsnew("value1");
    else if (!strcmp(varname, _var2)) return sdsnew("value2");
    else return NULL;
}