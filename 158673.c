random_number(int limit)
{
if (limit < 1)
  return 0;
if (random_seed == 0)
  {
  if (f.running_in_test_harness) random_seed = 42; else
    {
    int p = (int)getpid();
    random_seed = (int)time(NULL) ^ ((p << 16) | p);
    }
  }
random_seed = 1103515245 * random_seed + 12345;
return (unsigned int)(random_seed >> 16) % limit;
}