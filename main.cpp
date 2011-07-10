#ifdef _WIN32
#include <iostream>
#include <cstdlib>
#include <process.h>
#include <string>

using namespace std;

#else

#endif

int main(int argc,char**argv)
{
#ifdef _WIN32
	char *fullPath=_fullpath(NULL,argv[0],0);
	char *driveLetter=new char[_MAX_DRIVE];
	char *path=new char[_MAX_DIR];
	_splitpath(fullPath,driveLetter,path,NULL,NULL);
	free(fullPath);
#ifdef _M_X64
	char *bits="amd64";
#else
	char *bits=getenv("PROCESSOR_ARCHITECTURE");
	if(!bits)
		bits="\"PROCESSOR_ARCHITECTURE is empty\"";
	else if(strcmp(bits,"")==0)
		bits="\"PROCESSOR_ARCHITECTURE is empty\"";
#endif

	string workingCopy=driveLetter;
	workingCopy+=path;

	delete [] driveLetter;
	delete [] path;

	_spawnlp(_P_WAIT,"java","java","-cp",(string("\"")+workingCopy+"lib\\UpdateManager.jar\"").c_str(),"org.dndp.UpdateManager.Launcher",(string("\"")+workingCopy+"\\\"").c_str(),bits,NULL);

	return 0;
#else

#endif
}