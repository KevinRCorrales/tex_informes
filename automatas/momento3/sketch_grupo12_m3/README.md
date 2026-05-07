# Compilar usando Arduino CLI

Instala el programa desde:

[https://github.com/arduino/arduino-cli/releases/latest](https://github.com/arduino/arduino-cli/releases/latest)

Abre una terminal en un editor basado en Code-OSS (VSCode, VSCodium, Cursor, etc), y ejecuta:

```
cd automatas/momento3 && arduino-cli compile --fqbn arduino:avr:uno --output-dir sketch_grupo12_m3/build sketch_grupo12_m3
```

Esto genera los archivos necesarios para el wokwi.toml

Abre el diagrams.json con el visor de Wokwi e inicia la simulación (tener instalada la extensión desde el marketplace del editor).
