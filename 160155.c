void LibRaw::PentaxISO(ushort c)
{
  int code[] = {3,    4,    5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
                15,   16,   17,  18,  19,  20,  21,  22,  23,  24,  25,  26,
                27,   28,   29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
                39,   40,   41,  42,  43,  44,  45,  50,  100, 200, 400, 800,
                1600, 3200, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267,
                268,  269,  270, 271, 272, 273, 274, 275, 276, 277, 278};
  double value[] = {
      50,     64,     80,     100,    125,    160,    200,    250,    320,
      400,    500,    640,    800,    1000,   1250,   1600,   2000,   2500,
      3200,   4000,   5000,   6400,   8000,   10000,  12800,  16000,  20000,
      25600,  32000,  40000,  51200,  64000,  80000,  102400, 128000, 160000,
      204800, 258000, 325000, 409600, 516000, 650000, 819200, 50,     100,
      200,    400,    800,    1600,   3200,   50,     70,     100,    140,
      200,    280,    400,    560,    800,    1100,   1600,   2200,   3200,
      4500,   6400,   9000,   12800,  18000,  25600,  36000,  51200};
#define numel (sizeof(code) / sizeof(code[0]))
  int i;
  for (i = 0; i < numel; i++)
  {
    if (code[i] == c)
    {
      iso_speed = value[i];
      return;
    }
  }
  if (i == numel)
    iso_speed = 65535.0f;
}