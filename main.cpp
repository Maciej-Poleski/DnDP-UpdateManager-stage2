#ifdef _WIN32
#include <iostream>
#include <cstdlib>
#include <process.h>
#include <string>
#include <direct.h>

using namespace std;

#else
/* Żądam X/Open System Interface */
#define _XOPEN_SOURCE 700
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <libgen.h>
#include <spawn.h>
#include <sys/wait.h>

/* Sprawdzam, czy moje żądanie zostanie spełnione */
#if _XOPEN_VERSION != 700
#error Ten kompilator nie jest zgodny z XSI SUSv4
#error Nie mogę kontynuować
#endif

/* Sprawdzam, czy posix_spawn() jest dostępna */
#if !defined(_POSIX_SPAWN)
#error Ten kompilator nie obsługuje posix_spawn()
#error Nie mogę kontynuować
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

	printf("Przeprowadzanie aktualizacji... ");
	fflush(stdout);
	if(_spawnlp(_P_WAIT,"java","java","-cp",(string("\"")+workingCopy+"lib\\UpdateManager.jar\"").c_str(),"org.dndp.UpdateManager.Launcher",(string("\"")+workingCopy+"\\\"").c_str(),bits,NULL))
		printf("Niepowodzenie\n");
	else
		printf("Gotowe!\n");
	fflush(stdout);

	_chdir(workingCopy.c_str());
	_spawnlp(_P_DETACH,"java","java","-classpath","lib\\commands-3.6.0.jar;lib\\common-3.6.0.jar;lib\\db4o-full-java5-7.12.jar;lib\\dndc-0.2-SNAPSHOT.jar;lib\\engine-0.2-SNAPSHOT.jar;lib\\jface-3.6.0.jar;lib\\swt.jar","org.dndp.dndc.client.Main",NULL);
	
	return 0;	// To nigdy się nie wydarzy!
#else
	char * path=realpath(argv[0],NULL);
	char *tmppath=dirname(path);
	strcpy(path,tmppath);
	const char* updateManager_jar="/lib/UpdateManager.jar";
	char *jar=(char*)malloc(strlen(path)+strlen(updateManager_jar)+1);
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


        free(jar);
        free(path);

	return 0;
#endif
}