ccbaWrite(const char  *filename,
          CCBORDA     *ccba)
{
FILE  *fp;

    PROCNAME("ccbaWrite");

    if (!filename)
        return ERROR_INT("filename not defined", procName, 1);
    if (!ccba)
        return ERROR_INT("ccba not defined", procName, 1);

    if ((fp = fopenWriteStream(filename, "wb+")) == NULL)
        return ERROR_INT("stream not opened", procName, 1);
    if (ccbaWriteStream(fp, ccba)) {
        fclose(fp);
        return ERROR_INT("ccba not written to stream", procName, 1);
    }

    fclose(fp);
    return 0;
}