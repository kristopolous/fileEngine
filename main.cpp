#include "engine.h"
#include<stdio.h>
#include<unistd.h>

int main()
{
	string fname;
	CycleWriter *cyc = new CycleWriter();

	cyc->setup(
		"file%G%r",
		1,
		1,
		12,
		false
	);

/*
	cyc->m_baseFileName = "file%G%r";
	cyc->m_durationSeconds = 1;
	cyc->m_rolloverMB = 1;
	cyc->m_fileLimit = 12;
	cyc->m_doCycle = false;
*/

	CycleWriter::Conclusion ret;
	while(1) 
	{
		ret = cyc->consider(10000);
		
		if(ret == CycleWriter::SAMEFILE) 
		{
			printf(".");
		}
		if(ret == CycleWriter::NEWFILE) 
		{
			fname = cyc->getCurrentFileName();
			printf("\n[%s] New File: %s\n", 
				cyc->m_lastReason.c_str(),
				fname.c_str()
			);
		}
		if(ret == CycleWriter::DOQUIT) 
		{
			printf("\n[%s] Quiting\n", 
				cyc->m_lastReason.c_str()
			);
			exit(0);
		}
		fflush(0);
		usleep(1000);
	}
}
