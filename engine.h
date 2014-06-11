#include<string>
#include<stdlib.h>
#include<stddef.h>
#include<time.h>
#include<stdio.h>

using namespace std;

class CycleWriter {
	public:
		// The conclusion is what
		// consider() tells you to do
		// after you tell it how many 
		// more bytes to consider
		enum Conclusion {
			// Continue to use the same file
			SAMEFILE,

			// Close the current file handle
			// and use the one in getCurrentFileName()
			NEWFILE,

			// It's the end of the capture,
			// close the file handle and exit.
			DOQUIT
		};

		// These are the variables that may be set
		// to specify how the engine will work.
		// 
		// They are public and you can set them
		// directly, or use the handy setup
		// function below to force yourself
		// to set them all
    //
	  // values <= 0 mean don't use them.
		
		// The base file name to write to
		string m_baseFileName; // = ""

		// Number of bytes before rolling over
		int m_rolloverMB; // = 0

		// Time in seconds between captures
		int m_duration; // = 0

		// Total number of allowed captures
		int m_fileLimit; // = 0

		// Whether to cycle the names at all
		bool m_doCycle; // = false
	
		// The setup allows you to set everything
		// in one go so you don't miss anything.
		//
		// Also, if the engine has already started 
		// (via a call to consider()), then this will
		// be locked down and return false.
		bool setup(
			string baseFileName,
			int rolloverMB,
			int duration,
			int fileLimit,
			bool doCycle
		);

		CycleWriter();
		~CycleWriter() {};
		
		CycleWriter::Conclusion consider(long byteCount);

		string getCurrentFileName();

		// Last reason for a new file
		string m_lastReason;

	private:
		// This will yield a new file if
		// needed or conclude that we need 
		// to exit.
		CycleWriter::Conclusion nextFile();

		// Total number of bytes written
		int m_byteCount;

		// Last time of a capture
		time_t m_lastTime;

		// Total number of files written
		int m_fileCountTotal;

		// Current index
		int m_fileIndex;

		// The file pointer that things
		// should be writing to
		int m_pFile;

		// This is the 0-left padded format
		// for creating file names. Since
		// we don't know what's what at first
		// we just leave this hanging.
		char m_limitFormat[6];

		// The last file name that
		// was created (mostly for debugging)
		string m_lastFileName;

		// This is toggled to true the
		// first time consider is run ...
		// it will lock the setup() from 
		// being further run
		bool m_bFirstConsider;
};

