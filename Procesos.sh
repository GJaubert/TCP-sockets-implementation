#!/bin/bash

Titulo="Información deseada para $HOSTNAME"
Date_hour=$(date +"%x %r%Z")
actualizacion="Actualizada el $RIGHT_NOW por $USER"

online=0
offline=0
uid=0
reverse=0
killer=0

#Funcion error error_exit con salida 1
error_exit()
{
        echo "$1" 1>&2
        exit 1
}

#Chequeamos que esten los comandos mas importantes
check()
{
        if [ -e $(which lsof) ] && [ -e $(which ps) ] && [ -e $(which sort) ] [ -e $(which cut) ] && [ -e $(which wc) ] && [ -e $(which who) ];
        then
             echo "Todos los comandos en orden"

        else
             error_exit "Falta algun comando"
        fi

}

for argv in "$@"
  do
     case $argv in
	 "-ON"|"--online")
 		 online=1;;
	 "-OFF"|"--offline")
		 offline=1;;
	 "-U"|"--uid")
		 uid=1;;
	 "-R"|"--reverse")
		 reverse=1;;
	 "-K"|"--kill")
		 killer=1
		 shift
		 N=$(echo $@ | cut -d ' ' -f 2)  
		 break;;
	 "--help")
	 echo "./Nombre_del_script [OPCION]"
	 echo "Opciones disponibles: -ON -OFF -U -R -K N (Usar -K solo al final)"
	 exit 1;;
	 *)
	  error_exit "Opcion no valida, use --help para más ayuda";;	 
     esac
  done 

lista=$(ps -eo user --no-headers| sort -u)

#Print de solo la cabecera
printf "%-20s %-10s %-10s %-20s %-20s %-20s %-20s\n\n" "User" "UID" "GID" "Mas CPU consumida" "Proceso mas antiguo" "Fichero abiertos" "Procesos del usuario"

Imprimir(){ 
	for usuario in $1
    	  do
	    printf "%-20s %-10s %-10s %-20s" "$usuario" "$(id -u $usuario 2>/dev/null)" "$(id -g $usuario 2>/dev/null)" "$(ps -u $usuario -o pid,time --sort=-pcpu --no-headers | head -n 1)"
            printf "%-21s %-20s %-20s\n" "$(ps -u $usuario -o pid,etime --no-headers --sort=etime | tail -n 1)" "$(lsof -u $usuario 2>/dev/null | wc -l)" "$(ps -u $usuario --no-header | wc -l)"
          done
}


if [ $online == 1 ]; then
     	listaonline=$(who | cut -d ' ' -f 1)
     	Imprimir "$listaonline"
fi

if [ $uid == 1  ];then
	orden=uid
else
	orden=user
fi

if [ $reverse == 1 ];then
	orden=-$orden
fi

listamod=$(ps -eo user --no-headers --sort=$orden | uniq)

if [ $offline == 1 ]; then
	for i in $listamod
	  do
	    for j in $(who | cut -d ' ' -f 1)
	      do 
	        if [ $i == $j  ];then
                  listaoffline=${listamod//$j}
	        fi
              done  
          done
	  Imprimir "$listaoffline"
elif [ $online == 0  ];then
  	Imprimir "$listamod"
fi	

if [ $killer == 1  ];then
  	for i in $(ps -A -o pid --no-header)
	  do
		  if [ $(lsof -p $i 2>/dev/null | wc -l) -gt $N ];then
    		    echo pid: $i y numero de archivos abiertos: $(lsof -p $i | wc -l)
		    #kill -9 $i 2>/dev/null
	    fi
	  done
fi
