#include "engine.h"

using namespace std;

CycleWriter::CycleWriter() :
    m_baseFileName(""),
    m_byteCount(0),
    m_doCycle(false),
    m_duration(0),
    m_fileCountTotal(0),
    m_fileIndex(0),
    m_fileLimit(0),
    m_lastTime(0),
    m_bFirstConsider(false),
    m_rolloverMB(0)
{
	// null terminate the first
	// character of the limit format
	// to say that we want things to
	// be created when we consider the 
	// next file.
	m_limitFormat[0] = 0;
}

bool CycleWriter::setup(
  string baseFileName,
  int rolloverMB,
  int duration,
  int fileLimit,
  bool doCycle
) 
{
  if(m_bFirstConsider) 
  {
    return false;
  }
  m_baseFileName = baseFileName;
  m_rolloverMB = rolloverMB;
  m_duration = duration;
  m_fileLimit = fileLimit;
  m_doCycle = doCycle;

  return true;
}

string CycleWriter::getCurrentFileName() 
{
	return m_lastFileName;
}

//
// nextFile doesn't return the file pointer
// instead it possibly assigns a new value to
// pFile.  This means that the consider()
// can run through both possible logic blocks
// in one go.  This isn't needed now, but it's
// a conscious forward-design decision.
//
CycleWriter::Conclusion CycleWriter::nextFile() 
{
	// If we were told to only write a limited number
	// of files
	if (m_fileLimit > 0) 
	{
    // if We are not cycling and the total number
    // of files written to disk exceeds the maximum
    // number that we are supposed to write
    if(!m_doCycle && m_fileCountTotal >= m_fileLimit) 
    {
				// thenwWe've reached our capture limit
				// and have been instructed not to
				// cycle in a ring, so we return a 
				// DOQUIT signaling the end of the capture.
				//
        m_lastReason = "Maximum Number of Capture Files Written";

				return DOQUIT;
    }

    // Otherwise, we see if our current index exceeds
    // our limit
		if(m_fileIndex >= m_fileLimit) 
		{
      // in which case we reset the index back to zero
      // the fileIndex will by definition always be equal
      // to or less then the fileCountTotal so we don't
      // need to check for the cycle here because it will
      // be caught above and return out of the function.
			m_fileIndex = 0;
		}
	}
	
	// If we've made it here then we need to
	// do a new file name.
	//
	// The consider() block will move the
	// m_lastTime forward if needed so we
	// can trust that the value we have at
	// this point is valid.
	//
	// our file is base + number
	// when applicable.
	
	if (m_duration > 0) 
  {
		// This means that we need to run strftime
		// over the base name.  We'll just allocate
		// a good static 400 bytes here for the file
		// name ... 260 is usually considered max path,
		// the edge cases where it isn't is really 
		// outside the scope of this comment block 
		// ... really.
		const size_t our_sz = 400;
		size_t their_sz;
		char fName[our_sz];
		const struct tm*our_time = localtime(&m_lastTime);

		their_sz = strftime(
			fName,
			our_sz,
			m_baseFileName.c_str(),
			our_time
		);

		if(their_sz == 0) 
		{
			// TODO: we failed ...
		}

		// We now have our "base" filename
		m_lastFileName = fName;
	} 
	else 
	{
		// we aren't using the duration flag
		// so we don't have to run the stftime
		// function ... our job is really easy!
		m_lastFileName = m_baseFileName; 
	}

	// If we've specified to limit the number of 
	// bytes per file and we have made it here,
	// that means that we should append the 
	// current value of m_fileIndex (which was dealt
	// with above) on to the end of m_lastFileName
	// which we created from above.
	if (m_fileLimit > 0) 
	{
		// We haven't created our format for
		// the file numbering yet, so we do it now.
		if(m_limitFormat[0] == 0) 
		{
			int 
				// This is going to be the maximum numbr of
				// decimal digits we need.
				sz = 0,

				// a temporary copy of the file limit for
				// our exploitation and fun.
				our_fileLimit = m_fileLimit;

			// Just divide by 10 until we get zero.
			// It's really not bad.
			while(our_fileLimit > 0) 
      {
				sz ++;
				our_fileLimit /= 10;
			}

			// Now we can construct our format which will
			// actually be put inside another sprintf later on
			snprintf(
				m_limitFormat,

				sizeof(m_limitFormat),

				// double % => one percent
				// so it's really
				// %% 0(%d) d
				// Which will yield something like
				//
				// %05d
				//
				"%%0%dd",

				sz
			);
		}	

		// now that we have our format string, we can take
		// our base file name and append the current number 
		// to it.  We need to allocate a new char for that
		// first. This is a number, we'll set aside 22 bytes
		// for it just to prepare for the 45th century AD
		// when that kind of storage comes free with every
		// cereal box.
		char index[22];

		snprintf(
			index,
			sizeof(index),
			m_limitFormat,
			m_fileIndex
		);

		// Now we can tack the index string value on
		// to our base.
		m_lastFileName += index;
	}

  // Increment the total number of files
	m_fileCountTotal ++;

  // And the current index.
  m_fileIndex ++;

	// We've recommended a new file.
	return NEWFILE;
}

CycleWriter::Conclusion CycleWriter::consider(long byteCount) 
{
  // This is for any routine which restricts 
  // execution after an initial consider 
  m_bFirstConsider = true;

	m_byteCount += byteCount;

	if (m_duration > 0) 
  {
		// If this is our first consideration, we set the timer up.
		if(m_lastTime == 0) 
    {
			m_lastTime = time(0);
		}

		// If the current time is more than the last,
		// this is beyond our permissable limit
		if(time(0) - m_lastTime >= m_duration) 
    {
			// reset the last time to now.
			m_lastTime = time(0);

			// reset our file numbering back to 0.
			// tcpdump doesn't do this necessarily and
			// it's pretty confusing.
			m_fileIndex = 0;

			// Also bring back our byteCount since this
			// will be a new file to consider.
			m_byteCount = byteCount;

      // Set the last reason
      m_lastReason = "Maximum Time Reached";

			// This will be the basis for the next file we create.
			return nextFile();
		}
	}

	if (m_rolloverMB > 0) 
  {
		// If we've rolled over the amount of bytes we
		// are supposed to write to a specific file then
		if(m_byteCount > m_rolloverMB * 1024 * 1024) 
    {
			// we reset the counter back to the amount
			// we considered to write this time
			m_byteCount = byteCount;

      // Set the last reason
      m_lastReason = "Maximum File Size Reached";

			// and return our next file pointer
			return nextFile();
		}
	}

	// None of our limits were hit, so we can return
	// the previous file pointer.
	return SAMEFILE;
}
