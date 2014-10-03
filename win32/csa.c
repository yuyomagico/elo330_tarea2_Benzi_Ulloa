#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef _WIN32
	const char* OS_VERSION="Win32";
	const char* PLAY_COMMAND = "sox -r 20000 -b 16 -L -e signed-integer -t raw SaludoAudacity.raw -d";
#elif __APPLE__
    const char* OS_VERSION="Apple";
#elif __linux
    const char* OS_VERSION="Linux";
	//const char* PLAY_COMMAND = "aplay --format=S16_LE -t raw SaludoAudacity.raw";
#endif

int main(int argc, const char* argv[]){
	printf("OS: %s\n", OS_VERSION);
	//execl(".\\sox\\sox.exe",0);
	//execl(".\\sox\\sox.exe", "-r 8000", "-b 16", "-L", "-e signed-integer", "-t"," raw", "SaludoAudacity.raw", "-d" , 0);
	//int a = execl(".\\sox\\sox.exe", PLAY_COMMAND , 0);
	
	
	if(argc != 4 && argc != 5){
		printf("Sintaxis: csa   <archivo_de_audio_original>  <ganancia> <offset> [p]");
	}
	else if(argc == 4){
		printf("modo 1");
	}
	else{
		printf("Modo de reproduccion triple\n");
		FILE* stream = popen(PLAY_COMMAND, "r");
		short int nums[20];
		
		fread(nums, sizeof(int), 20, stream);
		
		int i;
		for(i=0; i<20; i++){
			printf("num[%i]: %d\n", i, nums[i]);
		}
		pclose(stream);
	}
	exit(0);
}
