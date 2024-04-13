maybe_pyc_file(FILE *fp, const char* filename, const char* ext, int closeit)
{
    if (strcmp(ext, ".pyc") == 0)
        return 1;

    /* Only look into the file if we are allowed to close it, since
       it then should also be seekable. */
    if (closeit) {
        /* Read only two bytes of the magic. If the file was opened in
           text mode, the bytes 3 and 4 of the magic (\r\n) might not
           be read as they are on disk. */
        unsigned int halfmagic = PyImport_GetMagicNumber() & 0xFFFF;
        unsigned char buf[2];
        /* Mess:  In case of -x, the stream is NOT at its start now,
           and ungetc() was used to push back the first newline,
           which makes the current stream position formally undefined,
           and a x-platform nightmare.
           Unfortunately, we have no direct way to know whether -x
           was specified.  So we use a terrible hack:  if the current
           stream position is not 0, we assume -x was specified, and
           give up.  Bug 132850 on SourceForge spells out the
           hopelessness of trying anything else (fseek and ftell
           don't work predictably x-platform for text-mode files).
        */
        int ispyc = 0;
        if (ftell(fp) == 0) {
            if (fread(buf, 1, 2, fp) == 2 &&
                ((unsigned int)buf[1]<<8 | buf[0]) == halfmagic)
                ispyc = 1;
            rewind(fp);
        }
        return ispyc;
    }
    return 0;
}