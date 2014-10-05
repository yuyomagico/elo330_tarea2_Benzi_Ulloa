Tarea2_elo330
=============

CSA: corrector de saturación de audio. Es un programa que permite saturar un archivo de audio, y/o restaurar archivos saturados.

Formato de ejecucion del programa:

csa   <archivo_de_audio_original>  <ganancia> <offset> [p]

Si el parámetro [p] es ingresado, csa reproduce el archivo de audio original completo, luego la versión saturada y finalmente la corregida.

Notas para Windows:

El programa utiliza sox.exe, por lo que se requiere tenerlo en el path o en el directorio del programa. El archivo ya esta en el repositorio.


Notas para Linux:

El programa utiliza "aplay", puede instalarse con $apt-get install aplay $yum install aplay
