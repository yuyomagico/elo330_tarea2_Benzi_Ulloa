Tarea 2 Programación de Sistemas
================================

Nombre
------

**CSA**: corrector de saturación de audio. Es un programa que permite saturar un archivo de audio, y/o restaurar archivos saturados.

Sinopsis
--------

    csa "archivo_de_audio_original"  "ganancia" "offset" [p]

Descripción
-----------

Si el parámetro [p] es ingresado, csa reproduce el archivo de audio original completo, luego la versión saturada y finalmente la corregida.

### Notas para Windows:

El programa utiliza *sox.exe*, por lo que se requiere tenerlo en el path o en el directorio del programa. El archivo ya esta en el repositorio.

### Notas para Linux:

El programa utiliza *aplay*, el cual puede instalarse con el
administrador de paquetes propio de la distribucion.

#### Ubuntu

    $ sudo apt-get install aplay

#### Fedora

    $ yum install aplay

### Notas para computadores Macintosh (OS X)

Actualmente **CSA** no permite reproducir sonido en un equipo
Macintosh. Sin embargo se puede realizar la saturación y corrección del
archivo de audio sin problemas.

Compilando y Corriendo
----------------------

### Linux y OS X

En la carpeta raiz del repositorio ejecutar

	$ make && make run
