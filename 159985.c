static void DecodeFPDelta(unsigned char *input, unsigned char *output, int cols,
                          int channels, int bytesPerSample)
{
  DecodeDeltaBytes(input, cols * bytesPerSample, channels);
  int32_t rowIncrement = cols * channels;

  if (bytesPerSample == 2)
  {

#if LibRawBigEndian
    const unsigned char *input0 = input;
    const unsigned char *input1 = input + rowIncrement;
#else
    const unsigned char *input1 = input;
    const unsigned char *input0 = input + rowIncrement;
#endif
    for (int col = 0; col < rowIncrement; ++col)
    {
      output[0] = input0[col];
      output[1] = input1[col];
      output += 2;
    }
  }
  else if (bytesPerSample == 3)
  {
    const unsigned char *input0 = input;
    const unsigned char *input1 = input + rowIncrement;
    const unsigned char *input2 = input + rowIncrement * 2;
    for (int col = 0; col < rowIncrement; ++col)
    {
      output[0] = input0[col];
      output[1] = input1[col];
      output[2] = input2[col];
      output += 3;
    }
  }
  else
  {
#if LibRawBigEndian
    const unsigned char *input0 = input;
    const unsigned char *input1 = input + rowIncrement;
    const unsigned char *input2 = input + rowIncrement * 2;
    const unsigned char *input3 = input + rowIncrement * 3;
#else
    const unsigned char *input3 = input;
    const unsigned char *input2 = input + rowIncrement;
    const unsigned char *input1 = input + rowIncrement * 2;
    const unsigned char *input0 = input + rowIncrement * 3;
#endif
    for (int col = 0; col < rowIncrement; ++col)
    {
      output[0] = input0[col];
      output[1] = input1[col];
      output[2] = input2[col];
      output[3] = input3[col];
      output += 4;
    }
  }
}