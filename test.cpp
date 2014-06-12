#include"cyclewriter.h"
#include<stdio.h>
#include<unistd.h>

// Usage:
//
// ./test base_file_name rollover duration limit do_cycle consider delay count 
//
// Where:
// 		base_file_name - The base file name to use
// 		rollover - A numberic value in MB of when to rollover (may be 0 to disable)
// 		duration - A specific amount of time to rollover (may be 0 to disable)
// 		limit - a specific limit on the number of files (may be 0)
// 		do_cycle - either 0 to stop after limit or 1 to continue
//
// 		consider - the number of bytes to consider each round
// 		delay - the delay between rounds in microseconds
// 		count - the total number of rounds
int main(int argc, char**argv)
{
	if(argc != 9) {
		printf("%d arguments given. This is wrong. Read the source code for usage", argc);
		exit(0);
	}
	
	string basename = argv[1];
	int rollover = atoi(argv[2]);
	int duration = atoi(argv[3]);
	int limit = atoi(argv[4]);
	bool cycle = !!atoi(argv[5]);

	int consider = atoi(argv[6]);
	int delay = atoi(argv[7]);
	int count = atoi(argv[8]);

	int rounds = 1;

	bool bQuit = false;
	int ret_files = 0;
	int rounds_between_recommend = 0;

	printf("{\n");
	printf("\t\"base\":\"%s\",\n", basename.c_str());
	printf("\t\"rollover\":%d,\n", rollover);
	printf("\t\"duration\":%d,\n", duration);
	printf("\t\"limit\":%d,\n", limit);
	printf("\t\"cycle\":%s,\n", cycle?"true":"false");
	printf("\t\"consider\":%d,\n", consider);
	printf("\t\"delay\":%d,\n", delay);
	printf("\t\"count\":%d,\n", count);

	string fname = "";
	cycle_writer *cyc = new cycle_writer();

	cyc->setup(
		basename.c_str(),
		rollover,
		duration,
		limit,
		cycle
	);


	cycle_writer::conclusion ret;

	while(rounds != count) 
	{
		ret = cyc->consider(consider);
		
		if(fname == "") 
		{
			fname = cyc->get_current_file_name();
			ret_files++;

			printf("\t\"initial\": \"%s\",\n", fname.c_str());
			printf("\t\"entries\": [\n");
		}

		if(ret == cycle_writer::SAMEFILE) 
		{
			rounds_between_recommend++;
		}

		if(ret == cycle_writer::NEWFILE) 
		{
			fname = cyc->get_current_file_name();
			ret_files++;

			printf("\t\t{");
			printf(" \"rounds\": %d,", rounds_between_recommend);
			printf(" \"reason\": \"%s\",", cyc->m_last_reason.c_str());
			printf(" \"file\": \"%s\"", fname.c_str());
			printf(" },\n");

			rounds_between_recommend = 0;
		}

		if(ret == cycle_writer::DOQUIT) 
		{
			printf("\t\t{");
			printf(" \"rounds\": %d,", rounds_between_recommend);
			printf(" \"reason\": \"%s\"", cyc->m_last_reason.c_str());
			printf(" }\n");
			printf("\t],\n");
			bQuit = true;
			break;
		}

		fflush(0);
		usleep(delay);
		rounds++;
	}

	if(!bQuit) {
			printf("\t\t{");
			printf(" \"rounds\": %d,", rounds_between_recommend);
			printf(" \"reason\": \"Count reached\"");
			printf(" }\n");
			printf("\t],\n");
	}
	printf("\t\"recommended_total\": %d", ret_files);
	printf("\n}\n");
}
