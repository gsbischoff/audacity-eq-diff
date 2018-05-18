#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
CountNewLines(char *filename)
{
	// Open input
	FILE *Input = fopen(filename, "r");

	if(Input)
	{
		unsigned int count = 0;
		char c;
		while((c = fgetc(Input)) != EOF)
		{
			if(c == '\n')
				count++;
		}
		return(count);
	}
	return(-1);
}

int
main(int argc, char **argv)
{
	if(argc < 3)
		return(0);

	// Read in the two files
	FILE *InputA = fopen(argv[1], "r"); int errA = errno;
	FILE *InputB = fopen(argv[2], "r"); int errB = errno;

	if(InputA && InputB)
	{
		// Attempt to read...
		int LineCountA = CountNewLines(argv[1]);
		int LineCountB = CountNewLines(argv[2]);

		if(LineCountA != LineCountB)
		{
			puts("Files don't have the same number of lines!\n");
			return(0);
		}

		double frequencies[LineCountA][2];	// [index of frequency in list][0: freq (Hz), 1: level (dB)]

		// Format: ignore first line
		while(fgetc(InputA) != '\n');
		while(fgetc(InputB) != '\n');

		// lines [freq]   [level]
		char buf_a[64], buf_b[64];

		unsigned int count = 0;

		while(fgets(buf_a, 64, InputA) 
		   && fgets(buf_b, 64, InputB))
		{
			char *end;

			// Parse the two doubles
			double freq_a = strtod(buf_a, &end);
			double levl_a = strtod(end, 0);

			double freq_b = strtod(buf_b, &end);
			double levl_b = strtod(end, 0);

			if(freq_a == freq_b && count < LineCountA)
			{
				frequencies[count][0] = freq_a;
				frequencies[count][1] = levl_a - levl_b;

				count++;
			}
			else
			{
				printf("Frequencies %f and %f on line %d do not match!\n", freq_a, freq_b, count);
				return(0);
			}
		}

		// Write an XML file for this setting
		FILE *Output = fopen(argc == 4 ? argv[3] : "output.XML", "w");

		fprintf(Output, "<equalizationeffect>\n"
						"\t<curve name=\"%s\">\n", argv[2]);

		for(int i = 0; i < count; ++i)
		{
			fprintf(Output, "\t\t<point f=\"%.12f\" d=\"%.12f\"/>\n", 
					frequencies[i][0], frequencies[i][1]);
		}

		fprintf(Output, "\t</curve>\n"
						"</equalizationeffect>");

		fflush(Output);
		fclose(Output);
	}
	else
	{
		if(!InputA)
			printf("Error opening file \"%s\": %s\n", argv[1], strerror(errA));

		if(!InputB)
			printf("Error opening file \"%s\": %s\n", argv[2], strerror(errB));
	}
	return(0);
}