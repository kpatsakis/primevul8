transport_update_waiting(host_item *hostlist, uschar *tpname)
{
const uschar *prevname = US"";
host_item *host;
open_db dbblock;
open_db *dbm_file;

DEBUG(D_transport) debug_printf("updating wait-%s database\n", tpname);

/* Open the database for this transport */

if (!(dbm_file = dbfn_open(string_sprintf("wait-%.200s", tpname),
		      O_RDWR, &dbblock, TRUE)))
  return;

/* Scan the list of hosts for which this message is waiting, and ensure
that the message id is in each host record. */

for (host = hostlist; host; host = host->next)
  {
  BOOL already = FALSE;
  dbdata_wait *host_record;
  uschar *s;
  int i, host_length;
  uschar buffer[256];

  /* Skip if this is the same host as we just processed; otherwise remember
  the name for next time. */

  if (Ustrcmp(prevname, host->name) == 0) continue;
  prevname = host->name;

  /* Look up the host record; if there isn't one, make an empty one. */

  if (!(host_record = dbfn_read(dbm_file, host->name)))
    {
    host_record = store_get(sizeof(dbdata_wait) + MESSAGE_ID_LENGTH);
    host_record->count = host_record->sequence = 0;
    }

  /* Compute the current length */

  host_length = host_record->count * MESSAGE_ID_LENGTH;

  /* Search the record to see if the current message is already in it. */

  for (s = host_record->text; s < host_record->text + host_length;
       s += MESSAGE_ID_LENGTH)
    if (Ustrncmp(s, message_id, MESSAGE_ID_LENGTH) == 0)
      { already = TRUE; break; }

  /* If we haven't found this message in the main record, search any
  continuation records that exist. */

  for (i = host_record->sequence - 1; i >= 0 && !already; i--)
    {
    dbdata_wait *cont;
    sprintf(CS buffer, "%.200s:%d", host->name, i);
    if ((cont = dbfn_read(dbm_file, buffer)))
      {
      int clen = cont->count * MESSAGE_ID_LENGTH;
      for (s = cont->text; s < cont->text + clen; s += MESSAGE_ID_LENGTH)
        if (Ustrncmp(s, message_id, MESSAGE_ID_LENGTH) == 0)
          { already = TRUE; break; }
      }
    }

  /* If this message is already in a record, no need to update. */

  if (already)
    {
    DEBUG(D_transport) debug_printf("already listed for %s\n", host->name);
    continue;
    }


  /* If this record is full, write it out with a new name constructed
  from the sequence number, increase the sequence number, and empty
  the record. */

  if (host_record->count >= WAIT_NAME_MAX)
    {
    sprintf(CS buffer, "%.200s:%d", host->name, host_record->sequence);
    dbfn_write(dbm_file, buffer, host_record, sizeof(dbdata_wait) + host_length);
    host_record->sequence++;
    host_record->count = 0;
    host_length = 0;
    }

  /* If this record is not full, increase the size of the record to
  allow for one new message id. */

  else
    {
    dbdata_wait *newr =
      store_get(sizeof(dbdata_wait) + host_length + MESSAGE_ID_LENGTH);
    memcpy(newr, host_record, sizeof(dbdata_wait) + host_length);
    host_record = newr;
    }

  /* Now add the new name on the end */

  memcpy(host_record->text + host_length, message_id, MESSAGE_ID_LENGTH);
  host_record->count++;
  host_length += MESSAGE_ID_LENGTH;

  /* Update the database */

  dbfn_write(dbm_file, host->name, host_record, sizeof(dbdata_wait) + host_length);
  DEBUG(D_transport) debug_printf("added to list for %s\n", host->name);
  }

/* All now done */

dbfn_close(dbm_file);
}