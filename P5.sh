#!/bin/bash

# sysinfo - Un script que informa del estado del sistema

##### Constantes

TITLE="Información del sistema para $HOSTNAME"
df=df
RIGHT_NOW=$(date +"%x %r%Z")
TIME_STAMP="Actualizada el $RIGHT_NOW por $USER"

##### Estilos

TEXT_BOLD=$(tput bold)

TEXT_GREEN=$(tput setaf 2)
TEXT_RESET=$(tput sgr0)

##### Funciones

system_info()
{
   # Función de stub temporal
   echo "función system_info"
}


show_uptime()
{
   # Función de stub temporal
   echo "función show_uptime"
}


drive_space()
{
   # Función de stub temporal
   echo "función drive_space"
   echo "--Estas son las particiones:----------------"
   df
}


home_space()
{
    # Función de stub temporal
   echo "función home_space"
   if  [ $(whoami) != root ]
  then 
     du -h ~/ | sort -nr
     du -s ~/
     echo
     if  [ $USER != $LOGNAME ]
     then
       echo "Los usuario no coinciden"
       echo "USER: $USER"
       echo "LOGNAME: $LOGNAME"
     else
       echo "Los usuarios si coinciden"
     fi
  else 
    # du -h /home | sort -nr
    # du -s /home
    #Bucle for para ver los directorios de /home
    for filename in /home/*; do
      fn=$(basename "$filename")
      printf "%-10s %-10s %-13s %s\n" "Directorio" "Subdirectorios" "Usado" "Directorio"
      directorios=$(($(find $filename -maxdepth 1 | wc -l)-1))
      subdirectorios=$(find $filename/*/* -maxdepth 1 | wc -l)
      printf "%-10d %-14d %s\n" "$directorios" "$subdirectorios" "$(du -s $filename)"  
      #du -s $filename: tamaño usado por el directorio
      #find $filename/*/* -maxdepth 1 -type d | wc -l: numero directorios
      #El otro find cuenta numero de archivos y subdirectorios
    done
  fi
  
  echo
  echo Numero de procesos: 
  ps | wc -l
}

error_exit()
{
        echo "$1" 1>&2
        exit 1
}

check()
{
	if [ -e $(which lsof) ] && [ -e $(which df) ] && [ -e $(which du) ];
	then 
	     echo "Todos los comandos en orden"	

	else
	     error_exit "Falta algun comando"
        fi

}

open_files()
{
	printf "Numero de archivos abiertos\n"
	printf "%-20s %s\n" "Usuario" "Nº archivos abiertos"
      #Usamos cut -d... para encontrar una lista de TODOS los usuarios.
      #Usamos comando lsof -u para ver que archivos estan abiertos.
	for i in $(cut -d: -f1 /etc/passwd) ;do
	  printf "%-20s %d\n" "$i" "$(lsof -u $i | wc -l)"
	done
}

ps_()
{
	for process in $(ps -u root | tr -s " " "_" | cut -d " " -f 2,4) ;do
		tiempo=$(echo $process | cut -d'_' -f 4)
		if [ $tiempo != "00:00:00" ] && [ $tiempo != "00:00:01" ] ;then
			echo $process
		fi
	done
}

##### Programa principal

if [ "$1" == "--help"  ]; then
    echo "Ejecute con el siguiente formato: $0"
    exit 0
  fi


cat << _EOF_

$TEXT_BOLD$TITLE$TEXT_RESET



$(check)

$(system_info)

$(show_uptime)

$(drive_space)

$(home_space)

$(open_files)

$(ps_)

$TEXT_GREEN$TIME_STAMP$TEXT_RESET

_EOF_
