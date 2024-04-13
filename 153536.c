AuthBasicCred(struct http_auth *ha, Str uname, Str pw, ParsedURL *pu,
	      HRequest *hr, FormList *request)
{
    Str s = Strdup(uname);
    Strcat_char(s, ':');
    Strcat(s, pw);
    return Strnew_m_charp("Basic ", encodeB(s->ptr)->ptr, NULL);
}