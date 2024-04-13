UFhalfclose(URLFile *f)
{
    switch (f->scheme) {
    case SCM_FTP:
	closeFTP();
	break;
#ifdef USE_NNTP
    case SCM_NEWS:
    case SCM_NNTP:
	closeNews();
	break;
#endif
    default:
	UFclose(f);
	break;
    }
}