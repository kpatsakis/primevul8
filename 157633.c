bidi_string_reverse (
  FriBidiChar *str,
  const FriBidiStrIndex len
)
{
  FriBidiStrIndex i;

  fribidi_assert (str);

  for (i = 0; i < len / 2; i++)
    {
      FriBidiChar tmp = str[i];
      str[i] = str[len - 1 - i];
      str[len - 1 - i] = tmp;
    }
}