ldns_fget_keyword_data_l(FILE *f, const char *keyword, const char *k_del, char *data,
               const char *d_del, size_t data_limit, int *line_nr)
{
       /* we assume: keyword|sep|data */
       char *fkeyword;
       ssize_t i;

       if(strlen(keyword) >= LDNS_MAX_KEYWORDLEN)
               return -1;
       fkeyword = LDNS_XMALLOC(char, LDNS_MAX_KEYWORDLEN);
       if(!fkeyword)
               return -1;

       i = ldns_fget_token(f, fkeyword, k_del, LDNS_MAX_KEYWORDLEN);
       if(i==0 || i==-1) {
               LDNS_FREE(fkeyword);
               return -1;
       }

       /* case??? i instead of strlen? */
       if (strncmp(fkeyword, keyword, LDNS_MAX_KEYWORDLEN - 1) == 0) {
               /* whee! */
               /* printf("%s\n%s\n", "Matching keyword", fkeyword); */
               i = ldns_fget_token_l(f, data, d_del, data_limit, line_nr);
               LDNS_FREE(fkeyword);
               return i;
       } else {
               /*printf("no match for %s (read: %s)\n", keyword, fkeyword);*/
               LDNS_FREE(fkeyword);
               return -1;
       }
}