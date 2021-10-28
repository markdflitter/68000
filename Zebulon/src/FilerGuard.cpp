#include "../private_include/FilerGuard.h"
#include "../private_include/Filer.h"


using namespace mdf;
using namespace std;

namespace Zebulon
{

FilerGuard::FilerGuard (Filer* filer) : m_filer (filer), m_modified (false)
{
}

FilerGuard::~FilerGuard ()
{
	if (m_modified) m_filer->save (); 
}

int FilerGuard::format (int diskSize)
{
	m_modified = true;
	return m_filer->format (diskSize);
}

bool FilerGuard::createFile (const std::string& name, unsigned long initialSize, bool contiguous)
{
	bool result = m_filer->createFile (name, initialSize, contiguous);
	if (result) m_modified = true;
	return result;
}

bool FilerGuard::deleteFile (const std::string& name)
{
	bool result = m_filer->deleteFile (name);
	if (result) m_modified = true;
	return result;
}

bool FilerGuard::rightsizeFile (FileEntry::Ptr fileEntry, unsigned long totalBlocks)
{
	bool result = m_filer->rightsizeFile (fileEntry, totalBlocks);
	if (result)	m_modified = true;
	return result;
}

bool FilerGuard::boot (unsigned int slot, const std::string& filename, unsigned int loadAddress, unsigned int startAddress, unsigned int length)
{
	bool result = m_filer->boot (slot, filename, loadAddress, startAddress, length);
	if (result) m_modified = true;
	return result;
}

void FilerGuard::markModified ()
{
	m_modified = true;
}

}
