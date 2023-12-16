#!/bin/bash

# Obtén la fecha del sistema en el formato deseado (YYYY-MM-DD)
fecha_del_dia=$(date +'%d-%m-%Y')

# Directorio raíz del repositorio
repositorio_dir="/w/Universidad/PI/Practicas-PI"

# Mensaje de commit personalizado
mensaje_commit="Actualizacion del repositorio en el día $fecha_del_dia de las prácticas de la Asignatura PI"

# Cambia al directorio del repositorio
cd "$repositorio_dir" || exit

# Agrega todos los cambios en el repositorio (incluyendo subdirectorios)
git add --all

# Realiza el commit con el mensaje personalizado
git commit -m "$mensaje_commit"

# Sube los cambios al repositorio remoto en la rama 'main' 
git push origin main
