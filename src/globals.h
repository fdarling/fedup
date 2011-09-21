#ifndef FEDUP_GLOBALS_H
#define FEDUP_GLOBALS_H

namespace fedup {

enum OpenResult
{
	OpenSucceeded,
	OpenDoesntExist,
	OpenAccessDenied,
	OpenReadError
};

enum SaveResult
{
	SaveNothingToSave,
	SaveSucceeded,
	SaveDirectoryDoesntExist,
	SaveAccessDenied,
	SaveWriteError
};

} // namespace fedup

#endif
