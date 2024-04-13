show_whats_supported(FILE * fp)
{
auth_info * authi;

DEBUG(D_any) {} else show_db_version(fp);

fprintf(fp, "Support for:");
#ifdef SUPPORT_CRYPTEQ
  fprintf(fp, " crypteq");
#endif
#if HAVE_ICONV
  fprintf(fp, " iconv()");
#endif
#if HAVE_IPV6
  fprintf(fp, " IPv6");
#endif
#ifdef HAVE_SETCLASSRESOURCES
  fprintf(fp, " use_setclassresources");
#endif
#ifdef SUPPORT_PAM
  fprintf(fp, " PAM");
#endif
#ifdef EXIM_PERL
  fprintf(fp, " Perl");
#endif
#ifdef EXPAND_DLFUNC
  fprintf(fp, " Expand_dlfunc");
#endif
#ifdef USE_TCP_WRAPPERS
  fprintf(fp, " TCPwrappers");
#endif
#ifdef SUPPORT_TLS
# ifdef USE_GNUTLS
  fprintf(fp, " GnuTLS");
# else
  fprintf(fp, " OpenSSL");
# endif
#endif
#ifdef SUPPORT_TRANSLATE_IP_ADDRESS
  fprintf(fp, " translate_ip_address");
#endif
#ifdef SUPPORT_MOVE_FROZEN_MESSAGES
  fprintf(fp, " move_frozen_messages");
#endif
#ifdef WITH_CONTENT_SCAN
  fprintf(fp, " Content_Scanning");
#endif
#ifdef SUPPORT_DANE
  fprintf(fp, " DANE");
#endif
#ifndef DISABLE_DKIM
  fprintf(fp, " DKIM");
#endif
#ifndef DISABLE_DNSSEC
  fprintf(fp, " DNSSEC");
#endif
#ifndef DISABLE_EVENT
  fprintf(fp, " Event");
#endif
#ifdef SUPPORT_I18N
  fprintf(fp, " I18N");
#endif
#ifndef DISABLE_OCSP
  fprintf(fp, " OCSP");
#endif
#ifndef DISABLE_PRDR
  fprintf(fp, " PRDR");
#endif
#ifdef SUPPORT_PROXY
  fprintf(fp, " PROXY");
#endif
#ifdef SUPPORT_SOCKS
  fprintf(fp, " SOCKS");
#endif
#ifdef SUPPORT_SPF
  fprintf(fp, " SPF");
#endif
#ifdef TCP_FASTOPEN
  deliver_init();
  if (f.tcp_fastopen_ok) fprintf(fp, " TCP_Fast_Open");
#endif
#ifdef EXPERIMENTAL_LMDB
  fprintf(fp, " Experimental_LMDB");
#endif
#ifdef EXPERIMENTAL_QUEUEFILE
  fprintf(fp, " Experimental_QUEUEFILE");
#endif
#ifdef EXPERIMENTAL_SRS
  fprintf(fp, " Experimental_SRS");
#endif
#ifdef EXPERIMENTAL_ARC
  fprintf(fp, " Experimental_ARC");
#endif
#ifdef EXPERIMENTAL_BRIGHTMAIL
  fprintf(fp, " Experimental_Brightmail");
#endif
#ifdef EXPERIMENTAL_DCC
  fprintf(fp, " Experimental_DCC");
#endif
#ifdef EXPERIMENTAL_DMARC
  fprintf(fp, " Experimental_DMARC");
#endif
#ifdef EXPERIMENTAL_DSN_INFO
  fprintf(fp, " Experimental_DSN_info");
#endif
#ifdef EXPERIMENTAL_REQUIRETLS
  fprintf(fp, " Experimental_REQUIRETLS");
#endif
#ifdef EXPERIMENTAL_PIPE_CONNECT
  fprintf(fp, " Experimental_PIPE_CONNECT");
#endif
fprintf(fp, "\n");

fprintf(fp, "Lookups (built-in):");
#if defined(LOOKUP_LSEARCH) && LOOKUP_LSEARCH!=2
  fprintf(fp, " lsearch wildlsearch nwildlsearch iplsearch");
#endif
#if defined(LOOKUP_CDB) && LOOKUP_CDB!=2
  fprintf(fp, " cdb");
#endif
#if defined(LOOKUP_DBM) && LOOKUP_DBM!=2
  fprintf(fp, " dbm dbmjz dbmnz");
#endif
#if defined(LOOKUP_DNSDB) && LOOKUP_DNSDB!=2
  fprintf(fp, " dnsdb");
#endif
#if defined(LOOKUP_DSEARCH) && LOOKUP_DSEARCH!=2
  fprintf(fp, " dsearch");
#endif
#if defined(LOOKUP_IBASE) && LOOKUP_IBASE!=2
  fprintf(fp, " ibase");
#endif
#if defined(LOOKUP_LDAP) && LOOKUP_LDAP!=2
  fprintf(fp, " ldap ldapdn ldapm");
#endif
#ifdef EXPERIMENTAL_LMDB
  fprintf(fp, " lmdb");
#endif
#if defined(LOOKUP_MYSQL) && LOOKUP_MYSQL!=2
  fprintf(fp, " mysql");
#endif
#if defined(LOOKUP_NIS) && LOOKUP_NIS!=2
  fprintf(fp, " nis nis0");
#endif
#if defined(LOOKUP_NISPLUS) && LOOKUP_NISPLUS!=2
  fprintf(fp, " nisplus");
#endif
#if defined(LOOKUP_ORACLE) && LOOKUP_ORACLE!=2
  fprintf(fp, " oracle");
#endif
#if defined(LOOKUP_PASSWD) && LOOKUP_PASSWD!=2
  fprintf(fp, " passwd");
#endif
#if defined(LOOKUP_PGSQL) && LOOKUP_PGSQL!=2
  fprintf(fp, " pgsql");
#endif
#if defined(LOOKUP_REDIS) && LOOKUP_REDIS!=2
  fprintf(fp, " redis");
#endif
#if defined(LOOKUP_SQLITE) && LOOKUP_SQLITE!=2
  fprintf(fp, " sqlite");
#endif
#if defined(LOOKUP_TESTDB) && LOOKUP_TESTDB!=2
  fprintf(fp, " testdb");
#endif
#if defined(LOOKUP_WHOSON) && LOOKUP_WHOSON!=2
  fprintf(fp, " whoson");
#endif
fprintf(fp, "\n");

auth_show_supported(fp);
route_show_supported(fp);
transport_show_supported(fp);

#ifdef WITH_CONTENT_SCAN
malware_show_supported(fp);
#endif

if (fixed_never_users[0] > 0)
  {
  int i;
  fprintf(fp, "Fixed never_users: ");
  for (i = 1; i <= (int)fixed_never_users[0] - 1; i++)
    fprintf(fp, "%d:", (unsigned int)fixed_never_users[i]);
  fprintf(fp, "%d\n", (unsigned int)fixed_never_users[i]);
  }

fprintf(fp, "Configure owner: %d:%d\n", config_uid, config_gid);

fprintf(fp, "Size of off_t: " SIZE_T_FMT "\n", sizeof(off_t));

/* Everything else is details which are only worth reporting when debugging.
Perhaps the tls_version_report should move into this too. */
DEBUG(D_any) do {

  int i;

/* clang defines __GNUC__ (at least, for me) so test for it first */
#if defined(__clang__)
  fprintf(fp, "Compiler: CLang [%s]\n", __clang_version__);
#elif defined(__GNUC__)
  fprintf(fp, "Compiler: GCC [%s]\n",
# ifdef __VERSION__
      __VERSION__
# else
      "? unknown version ?"
# endif
      );
#else
  fprintf(fp, "Compiler: <unknown>\n");
#endif

#if defined(__GLIBC__) && !defined(__UCLIBC__)
  fprintf(fp, "Library version: Glibc: Compile: %d.%d\n",
	       	__GLIBC__, __GLIBC_MINOR__);
  if (__GLIBC_PREREQ(2, 1))
    fprintf(fp, "                        Runtime: %s\n",
	       	gnu_get_libc_version());
#endif

show_db_version(fp);

#ifdef SUPPORT_TLS
  tls_version_report(fp);
#endif
#ifdef SUPPORT_I18N
  utf8_version_report(fp);
#endif

  for (authi = auths_available; *authi->driver_name != '\0'; ++authi)
    if (authi->version_report)
      (*authi->version_report)(fp);

  /* PCRE_PRERELEASE is either defined and empty or a bare sequence of
  characters; unless it's an ancient version of PCRE in which case it
  is not defined. */
#ifndef PCRE_PRERELEASE
# define PCRE_PRERELEASE
#endif
#define QUOTE(X) #X
#define EXPAND_AND_QUOTE(X) QUOTE(X)
  fprintf(fp, "Library version: PCRE: Compile: %d.%d%s\n"
             "                       Runtime: %s\n",
          PCRE_MAJOR, PCRE_MINOR,
          EXPAND_AND_QUOTE(PCRE_PRERELEASE) "",
          pcre_version());
#undef QUOTE
#undef EXPAND_AND_QUOTE

  init_lookup_list();
  for (i = 0; i < lookup_list_count; i++)
    if (lookup_list[i]->version_report)
      lookup_list[i]->version_report(fp);

#ifdef WHITELIST_D_MACROS
  fprintf(fp, "WHITELIST_D_MACROS: \"%s\"\n", WHITELIST_D_MACROS);
#else
  fprintf(fp, "WHITELIST_D_MACROS unset\n");
#endif
#ifdef TRUSTED_CONFIG_LIST
  fprintf(fp, "TRUSTED_CONFIG_LIST: \"%s\"\n", TRUSTED_CONFIG_LIST);
#else
  fprintf(fp, "TRUSTED_CONFIG_LIST unset\n");
#endif

} while (0);
}