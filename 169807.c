ldns_bget_keyword_data(ldns_buffer *b, const char *keyword, const char *k_del, char
*data, const char *d_del, size_t data_limit)
{
       /* we assume: keyword|sep|data */
       char *fkeyword;
       ssize_t i;

       if(strlen(keyword) >= LDNS_MAX_KEYWORDLEN)
               return -1;
       fkeyword = LDNS_XMALLOC(char, LDNS_MAX_KEYWORDLEN);
       if(!fkeyword)
               return -1; /* out of memory */

       i = ldns_bget_token(b, fkeyword, k_del, data_limit);
       if(i==0 || i==-1) {
               LDNS_FREE(fkeyword);
               return -1; /* nothing read */
       }

       /* case??? */
       if (strncmp(fkeyword, keyword, strlen(keyword)) == 0) {
               LDNS_FREE(fkeyword);
               /* whee, the match! */
               /* retrieve it's data */
               i = ldns_bget_token(b, data, d_del, 0);
               return i;
       } else {
               LDNS_FREE(fkeyword);
               return -1;
       }
}