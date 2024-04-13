void sdb_edit(procinfo *pi)
{
  char * filename = omStrDup("/tmp/sdXXXXXX");
  int f=mkstemp(filename);
  if (f==-1)
  {
    Print("cannot open %s\n",filename);
    omFree(filename);
    return;
  }
  if (pi->language!= LANG_SINGULAR)
  {
    Print("cannot edit type %d\n",pi->language);
    close(f);
    f=NULL;
  }
  else
  {
    const char *editor=getenv("EDITOR");
    if (editor==NULL)
      editor=getenv("VISUAL");
    if (editor==NULL)
      editor="vi";
    editor=omStrDup(editor);

    if (pi->data.s.body==NULL)
    {
      iiGetLibProcBuffer(pi);
      if (pi->data.s.body==NULL)
      {
        PrintS("cannot get the procedure body\n");
        close(f);
        si_unlink(filename);
        omFree(filename);
        return;
      }
    }

    write(f,pi->data.s.body,strlen(pi->data.s.body));
    close(f);

    int pid=fork();
    if (pid!=0)
    {
      si_wait(&pid);
    }
    else if(pid==0)
    {
      if (strchr(editor,' ')==NULL)
      {
        execlp(editor,editor,filename,NULL);
        Print("cannot exec %s\n",editor);
      }
      else
      {
        char *p=(char *)omAlloc(strlen(editor)+strlen(filename)+2);
        sprintf(p,"%s %s",editor,filename);
        system(p);
      }
      exit(0);
    }
    else
    {
      PrintS("cannot fork\n");
    }

    FILE* fp=fopen(filename,"r");
    if (fp==NULL)
    {
      Print("cannot read from %s\n",filename);
    }
    else
    {
      fseek(fp,0L,SEEK_END);
      long len=ftell(fp);
      fseek(fp,0L,SEEK_SET);

      omFree((ADDRESS)pi->data.s.body);
      pi->data.s.body=(char *)omAlloc((int)len+1);
      myfread( pi->data.s.body, len, 1, fp);
      pi->data.s.body[len]='\0';
      fclose(fp);
    }
  }
  si_unlink(filename);
  omFree(filename);
}