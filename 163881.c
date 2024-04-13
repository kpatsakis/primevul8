int main(int argc, char* argv[])
{
	InitUtils("jpgicc");

	HandleSwitches(argc, argv);

	if ((argc - xoptind) != 2) {
		Help(0);
	}

	OpenInput(argv[xoptind]);
	OpenOutput(argv[xoptind+1]);

	TransformImage(cInpProf, cOutProf);


	if (Verbose) { fprintf(stdout, "\n"); fflush(stdout); }

	Done();

	return 0;
}