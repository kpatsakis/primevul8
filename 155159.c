static ctmbstr GetFPIFromVers(uint vers)
{
    uint i;

    for (i = 0; W3C_Doctypes[i].name; ++i)
        if (W3C_Doctypes[i].vers == vers)
            return W3C_Doctypes[i].fpi;

    return NULL;
}