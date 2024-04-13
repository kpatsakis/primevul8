transport_check_waiting(const uschar *transport_name, const uschar *hostname,
  int local_message_max, uschar *new_message_id, BOOL *more, oicf oicf_func, void *oicf_data)
{
dbdata_wait *host_record;
int host_length;
open_db dbblock;
open_db *dbm_file;

int         i;
struct stat statbuf;

*more = FALSE;

DEBUG(D_transport)
  {
  debug_printf("transport_check_waiting entered\n");
  debug_printf("  sequence=%d local_max=%d global_max=%d\n",
    continue_sequence, local_message_max, connection_max_messages);
  }

/* Do nothing if we have hit the maximum number that can be send down one
connection. */

if (connection_max_messages >= 0) local_message_max = connection_max_messages;
if (local_message_max > 0 && continue_sequence >= local_message_max)
  {
  DEBUG(D_transport)
    debug_printf("max messages for one connection reached: returning\n");
  return FALSE;
  }

/* Open the waiting information database. */

if (!(dbm_file = dbfn_open(string_sprintf("wait-%.200s", transport_name),
			  O_RDWR, &dbblock, TRUE)))
  return FALSE;

/* See if there is a record for this host; if not, there's nothing to do. */

if (!(host_record = dbfn_read(dbm_file, hostname)))
  {
  dbfn_close(dbm_file);
  DEBUG(D_transport) debug_printf("no messages waiting for %s\n", hostname);
  return FALSE;
  }

/* If the data in the record looks corrupt, just log something and
don't try to use it. */

if (host_record->count > WAIT_NAME_MAX)
  {
  dbfn_close(dbm_file);
  log_write(0, LOG_MAIN|LOG_PANIC, "smtp-wait database entry for %s has bad "
    "count=%d (max=%d)", hostname, host_record->count, WAIT_NAME_MAX);
  return FALSE;
  }

/* Scan the message ids in the record from the end towards the beginning,
until one is found for which a spool file actually exists. If the record gets
emptied, delete it and continue with any continuation records that may exist.
*/

/* For Bug 1141, I refactored this major portion of the routine, it is risky
but the 1 off will remain without it.  This code now allows me to SKIP over
a message I do not want to send out on this run.  */

host_length = host_record->count * MESSAGE_ID_LENGTH;

while (1)
  {
  msgq_t      *msgq;
  int         msgq_count = 0;
  int         msgq_actual = 0;
  BOOL        bFound = FALSE;
  BOOL        bContinuation = FALSE;

  /* create an array to read entire message queue into memory for processing  */

  msgq = store_malloc(sizeof(msgq_t) * host_record->count);
  msgq_count = host_record->count;
  msgq_actual = msgq_count;

  for (i = 0; i < host_record->count; ++i)
    {
    msgq[i].bKeep = TRUE;

    Ustrncpy(msgq[i].message_id, host_record->text + (i * MESSAGE_ID_LENGTH),
      MESSAGE_ID_LENGTH);
    msgq[i].message_id[MESSAGE_ID_LENGTH] = 0;
    }

  /* first thing remove current message id if it exists */

  for (i = 0; i < msgq_count; ++i)
    if (Ustrcmp(msgq[i].message_id, message_id) == 0)
      {
      msgq[i].bKeep = FALSE;
      break;
      }

  /* now find the next acceptable message_id */

  for (i = msgq_count - 1; i >= 0; --i) if (msgq[i].bKeep)
    {
    uschar subdir[2];

    subdir[0] = split_spool_directory ? msgq[i].message_id[5] : 0;
    subdir[1] = 0;

    if (Ustat(spool_fname(US"input", subdir, msgq[i].message_id, US"-D"),
	      &statbuf) != 0)
      msgq[i].bKeep = FALSE;
    else if (!oicf_func || oicf_func(msgq[i].message_id, oicf_data))
      {
      Ustrcpy(new_message_id, msgq[i].message_id);
      msgq[i].bKeep = FALSE;
      bFound = TRUE;
      break;
      }
    }

  /* re-count */
  for (msgq_actual = 0, i = 0; i < msgq_count; ++i)
    if (msgq[i].bKeep)
      msgq_actual++;

  /* reassemble the host record, based on removed message ids, from in
  memory queue  */

  if (msgq_actual <= 0)
    {
    host_length = 0;
    host_record->count = 0;
    }
  else
    {
    host_length = msgq_actual * MESSAGE_ID_LENGTH;
    host_record->count = msgq_actual;

    if (msgq_actual < msgq_count)
      {
      int new_count;
      for (new_count = 0, i = 0; i < msgq_count; ++i)
	if (msgq[i].bKeep)
	  Ustrncpy(&host_record->text[new_count++ * MESSAGE_ID_LENGTH],
	    msgq[i].message_id, MESSAGE_ID_LENGTH);

      host_record->text[new_count * MESSAGE_ID_LENGTH] = 0;
      }
    }

  /* Check for a continuation record. */

  while (host_length <= 0)
    {
    int i;
    dbdata_wait * newr = NULL;
    uschar buffer[256];

    /* Search for a continuation */

    for (i = host_record->sequence - 1; i >= 0 && !newr; i--)
      {
      sprintf(CS buffer, "%.200s:%d", hostname, i);
      newr = dbfn_read(dbm_file, buffer);
      }

    /* If no continuation, delete the current and break the loop */

    if (!newr)
      {
      dbfn_delete(dbm_file, hostname);
      break;
      }

    /* Else replace the current with the continuation */

    dbfn_delete(dbm_file, buffer);
    host_record = newr;
    host_length = host_record->count * MESSAGE_ID_LENGTH;

    bContinuation = TRUE;
    }

  if (bFound)		/* Usual exit from main loop */
    {
    store_free (msgq);
    break;
    }

  /* If host_length <= 0 we have emptied a record and not found a good message,
  and there are no continuation records. Otherwise there is a continuation
  record to process. */

  if (host_length <= 0)
    {
    dbfn_close(dbm_file);
    DEBUG(D_transport) debug_printf("waiting messages already delivered\n");
    return FALSE;
    }

  /* we were not able to find an acceptable message, nor was there a
   * continuation record.  So bug out, outer logic will clean this up.
   */

  if (!bContinuation)
    {
    Ustrcpy(new_message_id, message_id);
    dbfn_close(dbm_file);
    return FALSE;
    }

  store_free(msgq);
  }		/* we need to process a continuation record */

/* Control gets here when an existing message has been encountered; its
id is in new_message_id, and host_length is the revised length of the
host record. If it is zero, the record has been removed. Update the
record if required, close the database, and return TRUE. */

if (host_length > 0)
  {
  host_record->count = host_length/MESSAGE_ID_LENGTH;

  dbfn_write(dbm_file, hostname, host_record, (int)sizeof(dbdata_wait) + host_length);
  *more = TRUE;
  }

dbfn_close(dbm_file);
return TRUE;
}