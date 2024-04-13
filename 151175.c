ccbaWriteSVG(const char  *filename,
             CCBORDA     *ccba)
{
char  *svgstr;

    PROCNAME("ccbaWriteSVG");

    if (!filename)
        return ERROR_INT("filename not defined", procName, 1);
    if (!ccba)
        return ERROR_INT("ccba not defined", procName, 1);

    if ((svgstr = ccbaWriteSVGString(filename, ccba)) == NULL)
        return ERROR_INT("svgstr not made", procName, 1);

    l_binaryWrite(filename, "w", svgstr, strlen(svgstr));
    LEPT_FREE(svgstr);

    return 0;
}