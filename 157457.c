rfbBool rfbFilenameTranslate2UNIX(rfbClientPtr cl, char *path, char *unixPath)
{
    int x;
    char *home=NULL;

    FILEXFER_ALLOWED_OR_CLOSE_AND_RETURN("", cl, FALSE);

    /* C: */
    if (path[0]=='C' && path[1]==':')
      strcpy(unixPath, &path[2]);
    else
    {
      home = getenv("HOME");
      if (home!=NULL)
      {
        strcpy(unixPath, home);
        strcat(unixPath,"/");
        strcat(unixPath, path);
      }
      else
        strcpy(unixPath, path);
    }
    for (x=0;x<strlen(unixPath);x++)
      if (unixPath[x]=='\\') unixPath[x]='/';
    return TRUE;
}