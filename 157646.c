index_array_reverse (
  FriBidiStrIndex *arr,
  const FriBidiStrIndex len
)
{
  FriBidiStrIndex i;

  fribidi_assert (arr);

  for (i = 0; i < len / 2; i++)
    {
      FriBidiStrIndex tmp = arr[i];
      arr[i] = arr[len - 1 - i];
      arr[len - 1 - i] = tmp;
    }
}