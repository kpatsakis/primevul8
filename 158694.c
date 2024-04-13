acl_ratelimit(const uschar *arg, int where, uschar **log_msgptr)
{
double limit, period, count;
uschar *ss;
uschar *key = NULL;
uschar *unique = NULL;
int sep = '/';
BOOL leaky = FALSE, strict = FALSE, readonly = FALSE;
BOOL noupdate = FALSE, badacl = FALSE;
int mode = RATE_PER_WHAT;
int old_pool, rc;
tree_node **anchor, *t;
open_db dbblock, *dbm;
int dbdb_size;
dbdata_ratelimit *dbd;
dbdata_ratelimit_unique *dbdb;
struct timeval tv;

/* Parse the first two options and record their values in expansion
variables. These variables allow the configuration to have informative
error messages based on rate limits obtained from a table lookup. */

/* First is the maximum number of messages per period / maximum burst
size, which must be greater than or equal to zero. Zero is useful for
rate measurement as opposed to rate limiting. */

if (!(sender_rate_limit = string_nextinlist(&arg, &sep, NULL, 0)))
  return ratelimit_error(log_msgptr, "sender rate limit not set");

limit = Ustrtod(sender_rate_limit, &ss);
if      (tolower(*ss) == 'k') { limit *= 1024.0; ss++; }
else if (tolower(*ss) == 'm') { limit *= 1024.0*1024.0; ss++; }
else if (tolower(*ss) == 'g') { limit *= 1024.0*1024.0*1024.0; ss++; }

if (limit < 0.0 || *ss != '\0')
  return ratelimit_error(log_msgptr,
    "\"%s\" is not a positive number", sender_rate_limit);

/* Second is the rate measurement period / exponential smoothing time
constant. This must be strictly greater than zero, because zero leads to
run-time division errors. */

period = !(sender_rate_period = string_nextinlist(&arg, &sep, NULL, 0))
  ? -1.0 : readconf_readtime(sender_rate_period, 0, FALSE);
if (period <= 0.0)
  return ratelimit_error(log_msgptr,
    "\"%s\" is not a time value", sender_rate_period);

/* By default we are counting one of something, but the per_rcpt,
per_byte, and count options can change this. */

count = 1.0;

/* Parse the other options. */

while ((ss = string_nextinlist(&arg, &sep, big_buffer, big_buffer_size)))
  {
  if (strcmpic(ss, US"leaky") == 0) leaky = TRUE;
  else if (strcmpic(ss, US"strict") == 0) strict = TRUE;
  else if (strcmpic(ss, US"noupdate") == 0) noupdate = TRUE;
  else if (strcmpic(ss, US"readonly") == 0) readonly = TRUE;
  else if (strcmpic(ss, US"per_cmd") == 0) RATE_SET(mode, PER_CMD);
  else if (strcmpic(ss, US"per_conn") == 0)
    {
    RATE_SET(mode, PER_CONN);
    if (where == ACL_WHERE_NOTSMTP || where == ACL_WHERE_NOTSMTP_START)
      badacl = TRUE;
    }
  else if (strcmpic(ss, US"per_mail") == 0)
    {
    RATE_SET(mode, PER_MAIL);
    if (where > ACL_WHERE_NOTSMTP) badacl = TRUE;
    }
  else if (strcmpic(ss, US"per_rcpt") == 0)
    {
    /* If we are running in the RCPT ACL, then we'll count the recipients
    one by one, but if we are running when we have accumulated the whole
    list then we'll add them all in one batch. */
    if (where == ACL_WHERE_RCPT)
      RATE_SET(mode, PER_RCPT);
    else if (where >= ACL_WHERE_PREDATA && where <= ACL_WHERE_NOTSMTP)
      RATE_SET(mode, PER_ALLRCPTS), count = (double)recipients_count;
    else if (where == ACL_WHERE_MAIL || where > ACL_WHERE_NOTSMTP)
      RATE_SET(mode, PER_RCPT), badacl = TRUE;
    }
  else if (strcmpic(ss, US"per_byte") == 0)
    {
    /* If we have not yet received the message data and there was no SIZE
    declaration on the MAIL command, then it's safe to just use a value of
    zero and let the recorded rate decay as if nothing happened. */
    RATE_SET(mode, PER_MAIL);
    if (where > ACL_WHERE_NOTSMTP) badacl = TRUE;
    else count = message_size < 0 ? 0.0 : (double)message_size;
    }
  else if (strcmpic(ss, US"per_addr") == 0)
    {
    RATE_SET(mode, PER_RCPT);
    if (where != ACL_WHERE_RCPT) badacl = TRUE, unique = US"*";
    else unique = string_sprintf("%s@%s", deliver_localpart, deliver_domain);
    }
  else if (strncmpic(ss, US"count=", 6) == 0)
    {
    uschar *e;
    count = Ustrtod(ss+6, &e);
    if (count < 0.0 || *e != '\0')
      return ratelimit_error(log_msgptr, "\"%s\" is not a positive number", ss);
    }
  else if (strncmpic(ss, US"unique=", 7) == 0)
    unique = string_copy(ss + 7);
  else if (!key)
    key = string_copy(ss);
  else
    key = string_sprintf("%s/%s", key, ss);
  }

/* Sanity check. When the badacl flag is set the update mode must either
be readonly (which is the default if it is omitted) or, for backwards
compatibility, a combination of noupdate and strict or leaky. */

if (mode == RATE_PER_CLASH)
  return ratelimit_error(log_msgptr, "conflicting per_* options");
if (leaky + strict + readonly > 1)
  return ratelimit_error(log_msgptr, "conflicting update modes");
if (badacl && (leaky || strict) && !noupdate)
  return ratelimit_error(log_msgptr,
    "\"%s\" must not have /leaky or /strict option, or cannot be used in %s ACL",
    ratelimit_option_string[mode], acl_wherenames[where]);

/* Set the default values of any unset options. In readonly mode we
perform the rate computation without any increment so that its value
decays to eventually allow over-limit senders through. */

if (noupdate) readonly = TRUE, leaky = strict = FALSE;
if (badacl) readonly = TRUE;
if (readonly) count = 0.0;
if (!strict && !readonly) leaky = TRUE;
if (mode == RATE_PER_WHAT) mode = RATE_PER_MAIL;

/* Create the lookup key. If there is no explicit key, use sender_host_address.
If there is no sender_host_address (e.g. -bs or acl_not_smtp) then we simply
omit it. The smoothing constant (sender_rate_period) and the per_xxx options
are added to the key because they alter the meaning of the stored data. */

if (!key)
  key = !sender_host_address ? US"" : sender_host_address;

key = string_sprintf("%s/%s/%s%s",
  sender_rate_period,
  ratelimit_option_string[mode],
  unique == NULL ? "" : "unique/",
  key);

HDEBUG(D_acl)
  debug_printf_indent("ratelimit condition count=%.0f %.1f/%s\n", count, limit, key);

/* See if we have already computed the rate by looking in the relevant tree.
For per-connection rate limiting, store tree nodes and dbdata in the permanent
pool so that they survive across resets. In readonly mode we only remember the
result for the rest of this command in case a later command changes it. After
this bit of logic the code is independent of the per_* mode. */

old_pool = store_pool;

if (readonly)
  anchor = &ratelimiters_cmd;
else switch(mode)
  {
  case RATE_PER_CONN:
    anchor = &ratelimiters_conn;
    store_pool = POOL_PERM;
    break;
  case RATE_PER_BYTE:
  case RATE_PER_MAIL:
  case RATE_PER_ALLRCPTS:
    anchor = &ratelimiters_mail;
    break;
  case RATE_PER_ADDR:
  case RATE_PER_CMD:
  case RATE_PER_RCPT:
    anchor = &ratelimiters_cmd;
    break;
  default:
    anchor = NULL; /* silence an "unused" complaint */
    log_write(0, LOG_MAIN|LOG_PANIC_DIE,
      "internal ACL error: unknown ratelimit mode %d", mode);
    break;
  }

if ((t = tree_search(*anchor, key)))
  {
  dbd = t->data.ptr;
  /* The following few lines duplicate some of the code below. */
  rc = (dbd->rate < limit)? FAIL : OK;
  store_pool = old_pool;
  sender_rate = string_sprintf("%.1f", dbd->rate);
  HDEBUG(D_acl)
    debug_printf_indent("ratelimit found pre-computed rate %s\n", sender_rate);
  return rc;
  }

/* We aren't using a pre-computed rate, so get a previously recorded rate
from the database, which will be updated and written back if required. */

if (!(dbm = dbfn_open(US"ratelimit", O_RDWR, &dbblock, TRUE)))
  {
  store_pool = old_pool;
  sender_rate = NULL;
  HDEBUG(D_acl) debug_printf_indent("ratelimit database not available\n");
  *log_msgptr = US"ratelimit database not available";
  return DEFER;
  }
dbdb = dbfn_read_with_length(dbm, key, &dbdb_size);
dbd = NULL;

gettimeofday(&tv, NULL);

if (dbdb)
  {
  /* Locate the basic ratelimit block inside the DB data. */
  HDEBUG(D_acl) debug_printf_indent("ratelimit found key in database\n");
  dbd = &dbdb->dbd;

  /* Forget the old Bloom filter if it is too old, so that we count each
  repeating event once per period. We don't simply clear and re-use the old
  filter because we want its size to change if the limit changes. Note that
  we keep the dbd pointer for copying the rate into the new data block. */

  if(unique && tv.tv_sec > dbdb->bloom_epoch + period)
    {
    HDEBUG(D_acl) debug_printf_indent("ratelimit discarding old Bloom filter\n");
    dbdb = NULL;
    }

  /* Sanity check. */

  if(unique && dbdb_size < sizeof(*dbdb))
    {
    HDEBUG(D_acl) debug_printf_indent("ratelimit discarding undersize Bloom filter\n");
    dbdb = NULL;
    }
  }

/* Allocate a new data block if the database lookup failed
or the Bloom filter passed its age limit. */

if (!dbdb)
  {
  if (!unique)
    {
    /* No Bloom filter. This basic ratelimit block is initialized below. */
    HDEBUG(D_acl) debug_printf_indent("ratelimit creating new rate data block\n");
    dbdb_size = sizeof(*dbd);
    dbdb = store_get(dbdb_size);
    }
  else
    {
    int extra;
    HDEBUG(D_acl) debug_printf_indent("ratelimit creating new Bloom filter\n");

    /* See the long comment below for an explanation of the magic number 2.
    The filter has a minimum size in case the rate limit is very small;
    this is determined by the definition of dbdata_ratelimit_unique. */

    extra = (int)limit * 2 - sizeof(dbdb->bloom);
    if (extra < 0) extra = 0;
    dbdb_size = sizeof(*dbdb) + extra;
    dbdb = store_get(dbdb_size);
    dbdb->bloom_epoch = tv.tv_sec;
    dbdb->bloom_size = sizeof(dbdb->bloom) + extra;
    memset(dbdb->bloom, 0, dbdb->bloom_size);

    /* Preserve any basic ratelimit data (which is our longer-term memory)
    by copying it from the discarded block. */

    if (dbd)
      {
      dbdb->dbd = *dbd;
      dbd = &dbdb->dbd;
      }
    }
  }

/* If we are counting unique events, find out if this event is new or not.
If the client repeats the event during the current period then it should be
counted. We skip this code in readonly mode for efficiency, because any
changes to the filter will be discarded and because count is already set to
zero. */

if (unique && !readonly)
  {
  /* We identify unique events using a Bloom filter. (You can find my
  notes on Bloom filters at http://fanf.livejournal.com/81696.html)
  With the per_addr option, an "event" is a recipient address, though the
  user can use the unique option to define their own events. We only count
  an event if we have not seen it before.

  We size the filter according to the rate limit, which (in leaky mode)
  is the limit on the population of the filter. We allow 16 bits of space
  per entry (see the construction code above) and we set (up to) 8 of them
  when inserting an element (see the loop below). The probability of a false
  positive (an event we have not seen before but which we fail to count) is

    size    = limit * 16
    numhash = 8
    allzero = exp(-numhash * pop / size)
            = exp(-0.5 * pop / limit)
    fpr     = pow(1 - allzero, numhash)

  For senders at the limit the fpr is      0.06%    or  1 in 1700
  and for senders at half the limit it is  0.0006%  or  1 in 170000

  In strict mode the Bloom filter can fill up beyond the normal limit, in
  which case the false positive rate will rise. This means that the
  measured rate for very fast senders can bogusly drop off after a while.

  At twice the limit, the fpr is  2.5%  or  1 in 40
  At four times the limit, it is  31%   or  1 in 3.2

  It takes ln(pop/limit) periods for an over-limit burst of pop events to
  decay below the limit, and if this is more than one then the Bloom filter
  will be discarded before the decay gets that far. The false positive rate
  at this threshold is 9.3% or 1 in 10.7. */

  BOOL seen;
  unsigned n, hash, hinc;
  uschar md5sum[16];
  md5 md5info;

  /* Instead of using eight independent hash values, we combine two values
  using the formula h1 + n * h2. This does not harm the Bloom filter's
  performance, and means the amount of hash we need is independent of the
  number of bits we set in the filter. */

  md5_start(&md5info);
  md5_end(&md5info, unique, Ustrlen(unique), md5sum);
  hash = md5sum[0] | md5sum[1] << 8 | md5sum[2] << 16 | md5sum[3] << 24;
  hinc = md5sum[4] | md5sum[5] << 8 | md5sum[6] << 16 | md5sum[7] << 24;

  /* Scan the bits corresponding to this event. A zero bit means we have
  not seen it before. Ensure all bits are set to record this event. */

  HDEBUG(D_acl) debug_printf_indent("ratelimit checking uniqueness of %s\n", unique);

  seen = TRUE;
  for (n = 0; n < 8; n++, hash += hinc)
    {
    int bit = 1 << (hash % 8);
    int byte = (hash / 8) % dbdb->bloom_size;
    if ((dbdb->bloom[byte] & bit) == 0)
      {
      dbdb->bloom[byte] |= bit;
      seen = FALSE;
      }
    }

  /* If this event has occurred before, do not count it. */

  if (seen)
    {
    HDEBUG(D_acl) debug_printf_indent("ratelimit event found in Bloom filter\n");
    count = 0.0;
    }
  else
    HDEBUG(D_acl) debug_printf_indent("ratelimit event added to Bloom filter\n");
  }

/* If there was no previous ratelimit data block for this key, initialize
the new one, otherwise update the block from the database. The initial rate
is what would be computed by the code below for an infinite interval. */

if (!dbd)
  {
  HDEBUG(D_acl) debug_printf_indent("ratelimit initializing new key's rate data\n");
  dbd = &dbdb->dbd;
  dbd->time_stamp = tv.tv_sec;
  dbd->time_usec = tv.tv_usec;
  dbd->rate = count;
  }
else
  {
  /* The smoothed rate is computed using an exponentially weighted moving
  average adjusted for variable sampling intervals. The standard EWMA for
  a fixed sampling interval is:  f'(t) = (1 - a) * f(t) + a * f'(t - 1)
  where f() is the measured value and f'() is the smoothed value.

  Old data decays out of the smoothed value exponentially, such that data n
  samples old is multiplied by a^n. The exponential decay time constant p
  is defined such that data p samples old is multiplied by 1/e, which means
  that a = exp(-1/p). We can maintain the same time constant for a variable
  sampling interval i by using a = exp(-i/p).

  The rate we are measuring is messages per period, suitable for directly
  comparing with the limit. The average rate between now and the previous
  message is period / interval, which we feed into the EWMA as the sample.

  It turns out that the number of messages required for the smoothed rate
  to reach the limit when they are sent in a burst is equal to the limit.
  This can be seen by analysing the value of the smoothed rate after N
  messages sent at even intervals. Let k = (1 - a) * p/i

    rate_1 = (1 - a) * p/i + a * rate_0
           = k + a * rate_0
    rate_2 = k + a * rate_1
           = k + a * k + a^2 * rate_0
    rate_3 = k + a * k + a^2 * k + a^3 * rate_0
    rate_N = rate_0 * a^N + k * SUM(x=0..N-1)(a^x)
           = rate_0 * a^N + k * (1 - a^N) / (1 - a)
           = rate_0 * a^N + p/i * (1 - a^N)

  When N is large, a^N -> 0 so rate_N -> p/i as desired.

    rate_N = p/i + (rate_0 - p/i) * a^N
    a^N = (rate_N - p/i) / (rate_0 - p/i)
    N * -i/p = log((rate_N - p/i) / (rate_0 - p/i))
    N = p/i * log((rate_0 - p/i) / (rate_N - p/i))

  Numerical analysis of the above equation, setting the computed rate to
  increase from rate_0 = 0 to rate_N = limit, shows that for large sending
  rates, p/i, the number of messages N = limit. So limit serves as both the
  maximum rate measured in messages per period, and the maximum number of
  messages that can be sent in a fast burst. */

  double this_time = (double)tv.tv_sec
                   + (double)tv.tv_usec / 1000000.0;
  double prev_time = (double)dbd->time_stamp
                   + (double)dbd->time_usec / 1000000.0;

  /* We must avoid division by zero, and deal gracefully with the clock going
  backwards. If we blunder ahead when time is in reverse then the computed
  rate will be bogus. To be safe we clamp interval to a very small number. */

  double interval = this_time - prev_time <= 0.0 ? 1e-9
                  : this_time - prev_time;

  double i_over_p = interval / period;
  double a = exp(-i_over_p);

  /* Combine the instantaneous rate (period / interval) with the previous rate
  using the smoothing factor a. In order to measure sized events, multiply the
  instantaneous rate by the count of bytes or recipients etc. */

  dbd->time_stamp = tv.tv_sec;
  dbd->time_usec = tv.tv_usec;
  dbd->rate = (1 - a) * count / i_over_p + a * dbd->rate;

  /* When events are very widely spaced the computed rate tends towards zero.
  Although this is accurate it turns out not to be useful for our purposes,
  especially when the first event after a long silence is the start of a spam
  run. A more useful model is that the rate for an isolated event should be the
  size of the event per the period size, ignoring the lack of events outside
  the current period and regardless of where the event falls in the period. So,
  if the interval was so long that the calculated rate is unhelpfully small, we
  re-initialize the rate. In the absence of higher-rate bursts, the condition
  below is true if the interval is greater than the period. */

  if (dbd->rate < count) dbd->rate = count;
  }

/* Clients sending at the limit are considered to be over the limit.
This matters for edge cases such as a limit of zero, when the client
should be completely blocked. */

rc = dbd->rate < limit ? FAIL : OK;

/* Update the state if the rate is low or if we are being strict. If we
are in leaky mode and the sender's rate is too high, we do not update
the recorded rate in order to avoid an over-aggressive sender's retry
rate preventing them from getting any email through. If readonly is set,
neither leaky nor strict are set, so we do not do any updates. */

if ((rc == FAIL && leaky) || strict)
  {
  dbfn_write(dbm, key, dbdb, dbdb_size);
  HDEBUG(D_acl) debug_printf_indent("ratelimit db updated\n");
  }
else
  {
  HDEBUG(D_acl) debug_printf_indent("ratelimit db not updated: %s\n",
    readonly? "readonly mode" : "over the limit, but leaky");
  }

dbfn_close(dbm);

/* Store the result in the tree for future reference. */

t = store_get(sizeof(tree_node) + Ustrlen(key));
t->data.ptr = dbd;
Ustrcpy(t->name, key);
(void)tree_insertnode(anchor, t);

/* We create the formatted version of the sender's rate very late in
order to ensure that it is done using the correct storage pool. */

store_pool = old_pool;
sender_rate = string_sprintf("%.1f", dbd->rate);

HDEBUG(D_acl)
  debug_printf_indent("ratelimit computed rate %s\n", sender_rate);

return rc;
}