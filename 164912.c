void ModuleState::addConvertIntToFloat(FormatCode input, FormatCode output)
{
	addModule(new ConvertIntToFloat(input, output));
}