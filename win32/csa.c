#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <math.h>

#define CMD_LENGTH 300

/* OS Detection*/
#ifdef _WIN32
	const char* OS_VERSION="Win32";
	char* PLAY_COMMAND = "sox -r 20000 -b 16 -L -e signed-integer -t raw ";
#elif __APPLE__
    const char* OS_VERSION="Apple";
#elif __linux
    const char* OS_VERSION="Linux";
	char* PLAY_COMMAND = "aplay --format=S16_LE -t raw ";
#endif

void check_data(short int*, short int*, float, int);

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
		/* Check de archivo valido y se obtiene el tamanio */
		struct stat st;
		int filesize;
		char g_file[50]="g_", r_file[50]="r_";
		
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
			filesize = filesize/2;
			strcat(g_file, argv[1]);
			strcat(r_file, argv[1]);
		}
		
		/* Se concatena el comando */
		printf("Modo de reproduccion triple\n");
		
		/* Comandos para cada la resproduccion de cada archivo */
		char play_command[CMD_LENGTH];
		char play_command_g[CMD_LENGTH];
		char play_command_r[CMD_LENGTH];
		strcpy(play_command, PLAY_COMMAND);
		strcat(play_command, argv[1]);
		strcpy(play_command_g, PLAY_COMMAND);
		strcat(play_command_g, g_file);
		strcpy(play_command_r, PLAY_COMMAND);
		strcat(play_command_r, r_file);
		
		if(strcmp(OS_VERSION,"Win32") == 0){
			strcat(play_command, " -d");
			strcat(play_command_g, " -d");
			strcat(play_command_r, " -d");
		}
		
		/* Se muestra el comando y el tamanio del archivo */
		printf("Play command: %s\n", play_command);
		printf("File size Bytes: %d Words: %d\n", filesize*2, filesize);
		
		
		/* Se lee el archivo de audio de entrada*/
		short int orig_data[filesize];		
		short int gain_data[filesize];	
		short int rest_data[filesize];
		
		/* Abertura de archivos, en modo binario, lectura y escritura segun corresponda */
		FILE* input_file = fopen(argv[1], "rb");
		FILE* gain_file = fopen(g_file, "wb");
		FILE* rest_file = fopen(r_file, "wb");
		
		fread(orig_data, sizeof(short), filesize, input_file);
		fclose(input_file);
		
		int i;
		float gain;
		int offset;
		
		/* Aqui falta el chequeo de valides de los datos pero filo. */
		sscanf(argv[2], "%f", &gain);
		sscanf(argv[3], "%d", &offset);
		offset = offset % (filesize);
		
		printf("Ganancia: %f Offset: %d\n", gain, offset);
		
		/* Se realiza el ajuste de ganancias */
		for(i=0; i<filesize; i++){
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
		fwrite(gain_data, sizeof(short), filesize, gain_file);
		fclose(gain_file);
		
		char plot_str[CMD_LENGTH];
		sprintf(plot_str, "gnuplot -persist -e \"plot  \\\"gain.dat\\\" using 1:2 title '%s'\"",g_file);
		FILE* plotter = popen(plot_str, "w");
		//pclose(plotter);
		//fprintf(gain_data, sizeof(short), filesize, gain_file);
		
		/* Se Analiza el archivo con ganancia */
		check_data(gain_data, rest_data, gain, filesize);
		
		/* Se guarda la data restaurada en el archivo correspondiente */
		fwrite(rest_data, sizeof(short), filesize, rest_file);
		fclose(rest_file);
		
		input_file = fopen("gain.dat", "w");
		gain_file = fopen("gain.dat", "w");
		rest_file = fopen("gain.dat", "w");
		/* Se hace .dat*/
		fprintf(input_file, "# time\t orig\n");
		fprintf(gain_file, "# time\t gain\n");
		fprintf(rest_file, "# time\t rest\n");
		for(i=0; i<filesize; i++){
			fprintf(input_file, "%d\t%d\n", i, orig_data[i]);
			fprintf(gain_file, "%d\t%d\n", i, gain_data[i]);
			fprintf(rest_file, "%d\t%d\n", i, rest_data[i]);
		}
		fclose(input_file);
		fclose(gain_file);
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

void check_data(short int *gain_data, short int *rest_data, float gain,  int data_length){
	printf("Analizando archivo...\n");
	
	/* Parametros del polinomio de interpolacion, valores polinomio interpolacion, archivo temporal, comando fit y comando calc*/
	float A,B,C,D,E;
	char tmpName [L_tmpnam];
	char fit[CMD_LENGTH], eval[CMD_LENGTH];
	tmpnam (tmpName);
	
	int i;
	if(gain != 0){
		/* Copio a rest_data, gain_data/gain*/
		for(i=0; i < data_length; i++){
			rest_data[i] = (short int) (((float)gain_data[i])/gain);
		}
		/* Valores para realizar interpolacion */
		short int X[4], Y[4];
		for(i=0; i < data_length; i++){
			/* Chequeo saturacion y no estar en los extremos */
			if((gain_data[i] == 32767 || gain_data[i] == -32768) && i < data_length - 2 && i > 1){
				short int sat_type;
				unsigned int sat_start, sat_end;
				
				sat_type = gain_data[i];
				
				sat_start = i;
				X[0] = i-1;
				X[1] = i;
				Y[0] = rest_data[i-1];
				Y[1] = rest_data[i];
				
				/* Busco todos los puntos saturados hasta encontrar */
				while(gain_data[i] == sat_type && i < data_length - 2){
					i++;
				}
				
				sat_end = i-1;
				X[2] = i-1;
				X[3] = i;
				Y[2] = rest_data[i-1];
				Y[3] = rest_data[i];
				
				/* Solo considerar saturaciones mayores a 2 puntos*/
				if(sat_end - sat_start > 1){
					printf("X: [%d,%d,%d,%d], Y: [%d,%d,%d,%d]\n", X[0], X[1], X[2], X[3], Y[0], Y[1], Y[2], Y[3]);
					/* Polyfit, parseando salida (borra todo lo que salga antes de "RESULT") y guardandola en archivo */
										
					/* Se reparan todos los puntos afectados en los datos a restaurar*/
					char to_eval[9*(sat_end - sat_start + 1)];
					char num[7];
					float result[sat_end - sat_start + 1];
					int j;
					sprintf(to_eval, "");
					
					for(j=sat_start; j<=sat_end; j++){
						if(j != sat_end)
							sprintf(num, "%d,", j);
						else
							sprintf(num, "%d", j);
						strcat(to_eval, num);
					}
					
					sprintf(fit, "octave --eval p=polyfit([%d,%d,%d,%d],[%d,%d,%d,%d],%d);RESULT=polyval(p,[%s]) | sed -n -e '/RESULT =/,${p}' |  sed 's/ \\+/,/g'> %s", X[0], X[1], X[2], X[3], Y[0], Y[1], Y[2], Y[3], 2, to_eval, tmpName);
					FILE* octave = popen(fit, "w");
					pclose(octave);
					
					FILE* output = fopen(tmpName, "r");
					fscanf(output, "RESULT,=\n\n");
					
					int k = 0;
					for(k=0; k< sat_end - sat_start + 1; k++){
						//result[k] = atoi((char*) output);
						fscanf(output, ",%f", &(result[k]));
					}
					fclose(output);
					
					//printf("P = %.2fx^4 %.2fx^3 %.2fx^2 %.2fx %.2f\nUBICACION: %d SATURACIONES: %d\n", A, B, C, D, E, sat_start*2, (sat_end - sat_start + 1));
					for(j=sat_start; j<=sat_end; j++){
						if(result[j-sat_start] > 32767 || result[j-sat_start] < -32768){
							printf("Dato (%d,%.2f) saturado\n", j, result[j-sat_start]);
							rest_data[j] = rest_data[j-1];
						}
						else{
							rest_data[j] = result[j-sat_start];
							printf("Nuevo dato (%d, %d)\n", j, rest_data[j]);
						}
					}
				}
			}
		}
	}
	else{
		printf("Error: gain = 0");
		exit(1);
	}
	
	//remove(tmpName);
	
	printf("Archivo restaurado.\n");
}
