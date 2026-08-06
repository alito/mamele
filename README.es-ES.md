

# **mamele** #

El entorno de aprendizaje de MAME. Este es un fork del [repositorio principal de MAME](https://github.com/mamedev/mame) para permitir que agentes programados jueguen a los juegos compatibles con el emulador MAME. Si estás familiarizado con el [Arcade Learning Environment](http://www.arcadelearningenvironment.org/) entonces mamele es para MAME lo que el Arcade Learaning Environment es para Stella.

Para ver la documentación principal de mamele ve a [learning_environment/README.md](learning_environment/README.md)
El resto de este documento es el readme estándar de MAME.

# MAME


## ¿Qué es MAME?

MAME es un marco de emulación multifuncional.

El propósito de MAME es preservar décadas de historia del software. A medida que la tecnología electrónica sigue avanzando a gran velocidad, MAME evita que este importante software "vintage" se pierda y se olvide. Esto se logra documentando el hardware y su funcionamiento. El código fuente de MAME sirve como esta documentación. El hecho de que el software sea utilizable sirve principalmente para validar la precisión de la documentación (¿cómo más puedes probar que has recreado el hardware fielmente?). Con el tiempo, MAME (que originalmente significaba Multiple Arcade Machine Emulator) absorbió el proyecto hermano MESS (Multi Emulator Super System), por lo que MAME ahora documenta una amplia variedad de (mayormente vintage) computadoras, consolas de videojuegos y calculadoras, además de los videojuegos arcade que fueron su enfoque inicial.

## ¿Dónde puedo obtener más información?

* [Sitio web oficial del equipo de desarrollo de MAME](https://www.mamedev.org/) (incluye descargas binarias, wiki, foros y más)
* [MAME Testers](https://mametesters.org/) (rastreador oficial de errores de MAME)

### Comunidad

* [Foros de MAME en bannister.org](https://forums.bannister.org/ubbthreads.php?ubb=cfrm&c=5)
* [r/MAME](https://www.reddit.com/r/MAME/) en Reddit
* [Foros de MAMEWorld](https://www.mameworld.info/ubbthreads/)

## Desarrollo

![Alt](https://repobeats.axiom.co/api/embed/8461d8ae4630322dafc736fc25782de214b49630.svg "Repobeats analytics image")

### Estado de CI y escaneo de código

[![CI (Linux)](https://github.com/mamedev/mame/workflows/CI%20(Linux)/badge.svg)](https://github.com/mamedev/mame/actions/workflows/ci-linux.yml) [![CI (Windows](https://github.com/mamedev/mame/workflows/CI%20(Windows)/badge.svg)](https://github.com/mamedev/mame/actions/workflows/ci-windows.yml) [![CI (macOS)](https://github.com/mamedev/mame/workflows/CI%20(macOS)/badge.svg)](https://github.com/mamedev/mame/actions/workflows/ci-macos.yml) [![Compile UI translations](https://github.com/mamedev/mame/workflows/Compile%20UI%20translations/badge.svg)](https://github.com/mamedev/mame/actions/workflows/language.yml) [![Build documentation](https://github.com/mamedev/mame/workflows/Build%20documentation/badge.svg)](https://github.com/mamedev/mame/actions/workflows/docs.yml)  [![Coverity Scan Status](https://scan.coverity.com/projects/5727/badge.svg?flat=1)](https://scan.coverity.com/projects/mame-emulator)

### ¿Cómo compilarlo?

Si estás en un sistema tipo UNIX (incluidos Linux y macOS), podría ser tan sencillo como escribir

```
make
```

para una compilación completa,

```
make SUBTARGET=tiny
```

para una compilación que incluya un pequeño subconjunto de sistemas compatibles.

Consulta la página [Compilando MAME](http://docs.mamedev.org/initialsetup/compilingmame.html) en nuestro sitio de documentación para obtener más información, incluidos los prerequisitos para macOS y las distribuciones de Linux más populares.

Para versiones recientes de macOS necesitas instalar [Xcode](https://developer.apple.com/xcode/) incluidas las herramientas de línea de comandos y [SDL 2.0](https://github.com/libsdl-org/SDL/releases/latest).

Para usuarios de Windows, proporcionamos un [entorno de compilación](http://www.mamedev.org/tools/) listo para usar basado en MinGW-w64.

Las compilaciones con Visual Studio también son posibles, pero aún necesitas el [entorno de compilación](http://www.mamedev.org/tools/) basado en MinGW-w64.
Para generar los archivos de solución y proyecto simplemente ejecuta:

```
make vs2022
```
o usa este comando para compilarlo directamente con msbuild

```
make vs2022 MSBUILD=1
```

### Estándar de codificación

El código fuente de MAME debe visualizarse y editarse con tu editor configurado para usar cuatro espacios por tabulación. Las tabulaciones se usan para la indentación inicial de las líneas, utilizando una tabulación por nivel de indentación. Los espacios se usan para otra alineación dentro de una línea.

Algunas partes del código siguen el [estilo Allman](https://en.wikipedia.org/wiki/Indent_style#Allman_style); otras partes del código siguen el [estilo K&R](https://en.wikipedia.org/wiki/Indent_style#K.26R_style) -- dependiendo principalmente de quién escribió la versión original. **Sobre todo, sé consistente con lo que modificas y mantén los cambios de espacios en blanco al mínimo al modificar el código existente.** Para código nuevo, la mayoría tiende a preferir el estilo Allman, así que si no te importa demasiado, usa ese.

Todos los colaboradores deben agregar un encabezado estándar para la información de licencia (en archivos nuevos) o informarnos sus deseos sobre cuál de las siguientes licencias prefieren para su código: la licencia [BSD-3-Clause](http://opensource.org/licenses/BSD-3-Clause), la [LGPL-2.1](http://opensource.org/licenses/LGPL-2.1) o la [GPL-2.0](http://opensource.org/licenses/GPL-2.0).

Consulta las [Directrices de codificación C++](https://docs.mamedev.org/contributing/cxx.html) más específicas en nuestro sitio web de documentación.

## Licencia

El proyecto MAME en su totalidad se pone a disposición bajo los términos de la
[Licencia Pública General de GNU, versión 2](http://opensource.org/licenses/GPL-2.0)
o posterior (GPL-2.0+), ya que contiene código disponible bajo múltiples
licencias compatibles con GPL. Una gran mayoría de los archivos de código fuente (más del 90%
incluidos los archivos centrales) se ponen a disposición bajo los términos de la
[Licencia BSD de 3 cláusulas](http://opensource.org/licenses/BSD-3-Clause), y
alentamos a los nuevos colaboradores a hacer sus contribuciones disponibles bajo los
términos de esta licencia.

Ten en cuenta que MAME es una marca registrada de Gregory Ember, y se requiere
permiso para utilizar el nombre, logotipo o marca verbal "MAME".

<a href="http://opensource.org/licenses/GPL-2.0" target="_blank">
<img align="right" width="100" src="https://opensource.org/wp-content/uploads/2009/06/OSIApproved.svg">
</a>

    Copyright (c) 1997-2026  MAMEdev y colaboradores

    Este programa es software libre; puedes redistribuirlo y/o modificarlo
    bajo los términos de la Licencia Pública General de GNU versión 2, según se indica en
    docs/legal/GPL-2.0.

    Este programa se distribuye con la esperanza de que sea útil, pero SIN
    NINGUNA GARANTÍA; incluso sin la garantía implícita de COMERCIABILIDAD o
    IDONEIDAD PARA UN PROPÓSITO PARTICULAR. Consulta la Licencia Pública General de GNU para
    más detalles.

Consulta [COPYING](COPYING) para obtener más detalles.
