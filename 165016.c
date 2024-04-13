inline void SwapModule::runSwap<3, char>(const char *input, char *output, int count)
{
	for (int i=0; i<count; i++)
	{
		output[3*i] = input[3*i+2];
		output[3*i+1] = input[3*i+1];
		output[3*i+2] = input[3*i];
	}
}