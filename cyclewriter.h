#include<string>
#include<stdlib.h>
#include<stddef.h>
#include<time.h>
#include<stdio.h>

using namespace std;

class cycle_writer {
public:
	//
	// The conclusion is what consider() tells you to do
	// after you tell it how many more bytes to consider.
	// 
	// We don't deal directly with file pointers here to
	// keep the concerns separated.  This engine only advises
	// a course of action.
	//
	enum conclusion {

		// Continue to use the same file
		SAMEFILE,

		// Close the current file handle
		// and use the one in get_current_file_name()
		NEWFILE,

		// It's the end of the capture,
		// close the file handle and exit.
		DOQUIT
	};

	cycle_writer();
	~cycle_writer() {};

	//
	// Setup sets all the parameters of the 
	// engine in one go so you don't miss anything.
	//
	// Also, if the engine has already started 
	// (via a call to consider()), then this will
	// be locked down and return false.
	//
	bool setup(string base_file_name, int rollover_mb, int duration_seconds, int file_limit, bool do_cycle);
	
	//
	// Consider byte_count bytes at the current time
	// and tell us whether 
	//
	//  * a new file should be written,
	//  * we should use the same file, or 
	//  * we should quit.
	//
	// If we should use a new file, or should quit,
	// then m_last_reason will tell us why consider
	// thought so, and in the case of a new file,
	// get_current_file_name() will tell us the new
	// capture file name to use.
	// 
	cycle_writer::conclusion consider(long byte_count);

	//
	// The yields the current file name 
	// based on the input parameters and 
	// what has been past into consider.
	//
	string get_current_file_name();

	// Last reason for a new file
	string m_last_reason;

private:
	//
	// This will yield a new file if
	// needed or conclude that we need 
	// to exit.
	//
	cycle_writer::conclusion next_file();

	//
	// These are the variables that are set
	// to specify how the engine will work.
	// 
	// Use the setup() function to set them up.
	//
	// values <= 0 mean don't use the feature.
	// 
	
	// The base file name to write to
	string m_base_file_name; // = ""

	// Number of bytes before rolling over
	int m_rollover_mb; // = 0

	// Time in seconds between captures
	int m_duration_seconds; // = 0

	// Total number of allowed captures
	int m_file_limit; // = 0

	// Whether to cycle the names at all
	bool m_do_cycle; // = false

private:
	// Total number of bytes written
	int m_byte_count;

	// Last time of a capture
	time_t m_last_time;

	// Total number of files written
	int m_file_count_total;

	// Current index
	int m_file_index;

	//
	// This is the 0-left padded format
	// for creating file names. Since
	// we don't know what's what at first
	// we just leave this hanging.
	//
	char m_limit_format[6];

	// The last file name that
	// was created (mostly for debugging)
	string m_last_file_name;

	//
	// This is toggled to true the
	// first time consider is run ...
	// it will lock the setup() from 
	// being further run
	//
	bool m_first_consider;
};

