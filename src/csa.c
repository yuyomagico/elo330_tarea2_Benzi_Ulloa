#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define CMD_LENGTH 300

/* OS Detection*/
#ifdef _WIN32
	const char* OS_VERSION="Win32";
	char* PLAY_COMMAND = "..\\win32\\sox\\sox -r 20000 -b 16 -L -e signed-integer -t raw %s -d";
#elif __APPLE__
    const char* OS_VERSION="Apple";
#error NOT SUPORTED YET
#elif __linux
    const char* OS_VERSION="Linux";
	char* PLAY_COMMAND = "aplay --format=S16_LE -t raw %s";
#endif

void check_data(short int*, short int*, int);

int main(int argc, const char* argv[]){
	printf("OS: %s\n", OS_VERSION);
	//execl(".\\sox\\sox.exe",0);
	//execl(".\\sox\\sox.exe", "-r 8000", "-b 16", "-L", "-e signed-integer", "-t"," raw", "SaludoAudacity.raw", "-d" , 0);
	//int a = execl(".\\sox\\sox.exe", PLAY_COMMAND , 0);
	
	
	if(argc != 4 && argc != 5){
		printf("Sintaxis: %s <archivo_de_audio_original> <ganancia> <offset> [p]\n", argv[0]);
	}
	else if(argc == 4){
		printf("modo 1");
	}
	else{
		/* Check de archivo valido y se obtiene el tamanio */
		struct stat st;
		int filesize;
		// MBT: Si prepones una extension al nombre del archivo, cuando uses un archivo dentro de una carpeta GG	
		char g_file[100], r_file[100];
		if(stat(argv[1], &st) != 0){
			printf("Invalid file\n");
			exit(1);
		}
		else{
			/* Se archivo era de tamanio impar de bytes, se ignora el ultimo byte*/
			filesize = st.st_size;
			if( filesize%2 != 0 ){
				filesize--;
			}
			sprintf(g_file,"%s%s",argv[1],"._g");
			sprintf(r_file,"%s%s",argv[1],"._r");
		}
		
		/* Se concatena el comando */
		printf("Modo de reproduccion triple\n");
		
		/* Comandos para cada la resproduccion de cada archivo */
		char play_command[CMD_LENGTH];
		char play_command_g[CMD_LENGTH];
		char play_command_r[CMD_LENGTH];
		// MBT: Para ordernar aplique sprintf en vez de strcpy y strcat
		sprintf(play_command, PLAY_COMMAND, argv[1]);		
		sprintf(play_command_g, PLAY_COMMAND, argv[1]);		
		sprintf(play_command_r, PLAY_COMMAND, argv[1]);		
		
		/* Se muestra el comando y el tamanio del archivo */
		printf("Play command: %s\n", play_command);
		printf("File size Bytes: %d Words: %d\n", filesize, filesize/2);
		
		/* Se lee el archivo de audio de entrada*/
		short int orig_data[filesize/2];		
		short int gain_data[filesize/2];	
		short int rest_data[filesize/2];
		
		/* Abertura de archivos, en modo binario, lectura y escritura segun corresponda */
		FILE* input_file = fopen(argv[1], "rb");
		
		fread(orig_data, sizeof(short), filesize/2, input_file);
		fclose(input_file);
		//fclose(rest_file);
		
		int i;
		float gain;
		sscanf(argv[2], "%f", &gain);
		printf("Ganancia: %f\n", gain);
		
		/* Se realiza el ajuste de ganancias */
		for(i=0; i<filesize/2; i++){
			if(orig_data[i] < 0 && orig_data[i]*gain < -32768){
				gain_data[i]=(short int)-32768;
			}
			else if(orig_data[i] > 0 && orig_data[i]*gain > 32767){
				gain_data[i]=(short int)32767;
			}
			else{
				gain_data[i]=(short int)orig_data[i]*gain;
			}
			//printf("%d.", gain_data[i]);
		}
		/* Se guardan los datos calculados en el archivo de ganancias */
		FILE* gain_file = fopen(g_file, "wb");
		if (gain_file == NULL)  {
			fprintf(stderr,"Error: couldn't create file %s.\n", g_file); 
			abort();
		}
		fwrite(gain_data, sizeof(short), filesize/2, gain_file);
		fclose(gain_file);
		
		/* Se Analiza el archivo con ganancia */
		check_data(gain_data, rest_data, filesize/2);
		
		/* Se guarda la data restaurada en el archivo correspondiente */
		FILE* rest_file = fopen(r_file, "wb");
		if (rest_file == NULL)  {
			fprintf(stderr,"Error: couldn't create file %s.\n", r_file); 
			abort();
		}
		fwrite(rest_data, sizeof(short), filesize/2, rest_file);
		fclose(rest_file);
		
		/* Se reproduce el archivo de audio de entrada, amplificado y restaurado */
		FILE* stream = popen(play_command, "r");
		pclose(stream);
		stream = popen(play_command_g, "r");
		pclose(stream);
		stream = popen(play_command_r, "r");
		pclose(stream);
	}
	exit(0);
}

void check_data(short int *gain_data, short int *rest_data, int data_length){
	printf("Archivo restaurado.\n");
}
