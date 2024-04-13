string_timesince(struct timeval * then)
{
struct timeval diff;

timesince(&diff, then);
return string_timediff(&diff);
}