void ModuleState::addConvertFloatToInt(FormatCode input, FormatCode output,
	const _PCMInfo &inputMapping, const _PCMInfo &outputMapping)
{
	addModule(new ConvertFloatToIntClip(input, output, inputMapping, outputMapping));
}