# Informes TeX/LaTeX

## Desarrollo

Si no sabes crear un Pull Request usa: [Issue para solicitudes de contenido](https://github.com/KevinRCorrales/tex_informes/issues/new?template=solicitud_contenido.yml)

Para reportar errores de compilación usa: [Issue para errores (bugs)](https://github.com/KevinRCorrales/tex_informes/issues/new?template=error_compilacion.yml)

## Objetivo

Repositorio donde almaceno informes en TeX/LaTeX con código versionado para una colaboración más productiva con mis compañeros.

Todo informe se basará en la plantilla: [plantilla](IEEE-conference-template-062824.tex)

## Obtener el código fuente y proponer cambios

Esto es por si no sabes hacer un Pull Request en GitHub:

Usando una terminal, clona el repositorio y entra al directorio:

```
git clone https://github.com/KevinRCorrales/tex_informes.git
cd tex_informes
```

Realiza tus cambios en un editor basado en VSCode y cuando los tengas listos ejecuta:

```
git diff
```

Copia la salida de ese comando y pegala en el issue enlazado más arriba.

Cuando se incorporen tus cambios, sincroniza tu copia local:

```
git restore .
git pull
```

## Probar cambios

Comprueba que tus cambios compilan usando TeXLive que es el más ideal para un entorno completo:

```
pdflatex <ruta al archivo .tex>
```

Abre el PDF en tu visor preferido y verifica.
