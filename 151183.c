ccbaRead(const char  *filename)
{
FILE     *fp;
CCBORDA  *ccba;

    PROCNAME("ccbaRead");

    if (!filename)
        return (CCBORDA *)ERROR_PTR("filename not defined", procName, NULL);

    if ((fp = fopenReadStream(filename)) == NULL)
        return (CCBORDA *)ERROR_PTR("stream not opened", procName, NULL);
    ccba = ccbaReadStream(fp);
    fclose(fp);

    if (!ccba)
        return (CCBORDA *)ERROR_PTR("ccba not returned", procName, NULL);
    return ccba;
}