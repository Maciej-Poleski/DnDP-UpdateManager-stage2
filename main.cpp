#ifdef _WIN32
#include <iostream>
#include <cstdlib>
#include <process.h>
#include <string>

using namespace std;

#else
/* ¯¹dam X/Open System Interface */
#define _XOPEN_SOURCE 700
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <libgen.h>
#include <spawn.h>
#include <sys/wait.h>

/* Sprawdzam, czy moje ¿¹danie zostan¹ spe³nione */
#if _XOPEN_VERSION != 700
#error Ten kompilator nie jest zgodny z XSI SUSv4
#error Nie mogê kontynuowaæ
#endif

/* Sprawdzam, czy posix_spawn() jest dostêpna */
#if !defined(_POSIX_SPAWN)
#error Ten kompilator nie obs³uguje posix_spawn()
#error Nie mogê kontynuowaæ
#endif
#endif

int main(int argc,char**argv,char**envp)
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
	char * path=realpath(argv[0],NULL);
	char *tmppath=dirname(path);
	strcpy(path,tmppath);
	const char* updateManager_jar="/lib/UpdateManager.jar";
	char *jar=new char[strlen(path)+sizeof(updateManager_jar)];
	strcpy(jar,path);
	strcat(jar,updateManager_jar);

#ifdef __LP64__
	char* bits="x86_64";
#else
	char* bits="i386";
#endif

	pid_t proc1;
	char* const argv2[] =
	{		"java",
			"-cp",
			jar,
			"org.dndp.UpdateManager.Launcher",
			path,
			bits,
			NULL
	};

	if(posix_spawnp(&proc1, "java",NULL,NULL,argv2,envp))
	    return 3;

	int state;
	waitpid(proc1,&state,0);

	delete [] jar;

	return 0;
#endif
}