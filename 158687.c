show_db_version(FILE * f)
{
#ifdef DB_VERSION_STRING
DEBUG(D_any)
  {
  fprintf(f, "Library version: BDB: Compile: %s\n", DB_VERSION_STRING);
  fprintf(f, "                      Runtime: %s\n",
    db_version(NULL, NULL, NULL));
  }
else
  fprintf(f, "Berkeley DB: %s\n", DB_VERSION_STRING);

#elif defined(BTREEVERSION) && defined(HASHVERSION)
  #ifdef USE_DB
  fprintf(f, "Probably Berkeley DB version 1.8x (native mode)\n");
  #else
  fprintf(f, "Probably Berkeley DB version 1.8x (compatibility mode)\n");
  #endif

#elif defined(_DBM_RDONLY) || defined(dbm_dirfno)
fprintf(f, "Probably ndbm\n");
#elif defined(USE_TDB)
fprintf(f, "Using tdb\n");
#else
  #ifdef USE_GDBM
  fprintf(f, "Probably GDBM (native mode)\n");
  #else
  fprintf(f, "Probably GDBM (compatibility mode)\n");
  #endif
#endif
}