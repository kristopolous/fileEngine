#include"cycle_writer.h"
#include<stdio.h>
#include<unistd.h>

int main()
{
	string fname;
	cycle_writer *cyc = new cycle_writer();

	cyc->setup(
		"file%G%r",
		1,
		1,
		12,
		false
	);


	cycle_writer::conclusion ret;
	while(1) 
	{
		ret = cyc->consider(10000);
		
		if(ret == cycle_writer::SAMEFILE) 
		{
			printf(".");
		}

		if(ret == cycle_writer::NEWFILE) 
		{
			fname = cyc->get_current_file_name();

			printf("\n[%s] New File: %s\n", 
				cyc->m_last_reason.c_str(),
				fname.c_str()
			);
		}

		if(ret == cycle_writer::DOQUIT) 
		{
			printf("\n[%s] Quiting\n", cyc->m_last_reason.c_str());
			exit(0);
		}

		fflush(0);
		usleep(1000);
	}
}
