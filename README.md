# TFG

### INSTRUCCIONES
Para compilar el contenido de este repositorio, es necesario ademas descargar sus dependencias,
que pueden ser encontradas en https://mega.nz/file/4w8z3RJb#cvpN1rASrgOUKqUwt-TL2P2QTu8EhVzuhbrNig33JZ0.
Una vez descargadas, deben descomprimirse dentro del directorio ProyectoTFG, y copiar los archivos
Dependencies/assimp/lib/assimp-vc141-mt.dll y Dependencies/assimp/lib/assimp-vc141-mtd.dll a la carpeta
ProyectoTFG/exes/OpenGL/. Por ultimo, se debe asegurar que en las propiedades del proyecto Main, en el apartado
Depuracion, el directorio de trabajo este establecido como $(TargetDir) en todas las configuraciones.

Si se quiere probar la aplicacion sin necesidad de compilar el contenido del repositorio, se puede acceder
a los ejecutables a traves del siguiente enlace: https://mega.nz/file/QhFyWAzQ#z-_7sd72g_HBL-6BAN8PLVbWTc4KMF-fDYRD_DShtP0.


### INSTRUCTIONS
In order to compile the content of this repository, it is needed to download its dependencies,
which can be found in https://mega.nz/file/4w8z3RJb#cvpN1rASrgOUKqUwt-TL2P2QTu8EhVzuhbrNig33JZ0.
Once downloaded, they must be decompressed into the ProyectoTFG directory, and you must copy the
Dependencies/assimp/lib/assimp-vc141-mt.dll and Dependencies/assimp/lib/assimp-vc141-mtd.dll files into the
ProyectoTFG/exes/OpenGL/ folder. Finally, you must check if in the Main project properties, in the Debug section,
the working directory is set to $(TargetDir) in all the configurations.

If you want to run the application without compiling the content of the repository, executables can be accessed through
the next link: https://mega.nz/file/QhFyWAzQ#z-_7sd72g_HBL-6BAN8PLVbWTc4KMF-fDYRD_DShtP0.

### Resumen
Aplicación interactiva para renderizar terrenos fractales, utilizando el algoritmo Ray Marching, y modelos basados en mallas de vértices, basado en la tubería clásica de renderizado. Se incorpora también un movimiento físico del modelo sobre las superficies generadas, mediante el cálculo de puntos de colisión entre ellos. Además, la aplicación permite utilizar como API de programación gráfica OpenGL y Vulkan, con GLSL como shading language.

### Abstract
Interactive application in order to render fractal terrains through the Ray Marching algorithm and vertex mesh models using the classic rendering pipeline. Collision points between them can be calculated, allowing a simple, physical movement of the model upon the created surfaces. Furthermore, the application lets OpenGL and Vulkan to be used, with GLSL as shading language.