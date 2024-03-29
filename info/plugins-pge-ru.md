## Графические плагины

Игра может запускать полноэкранные эффекты из .dll/.so файлов.

Видео-демка:\
[![HPW: Плагины на графику](https://img.youtube.com/vi/i_2XOWrNE4U/0.jpg)](https://youtu.be/i_2XOWrNE4U "HPW: Плагины на графику")

**путь к плагинам: build/plugin/effect/**

[API для плагинов](../src/plugin/graphic-effect/hpw-plugin-effect.h) позволяет указать какие параметры эффекта можно настраивать и они будут видны в настройках игры. Нет никаких ограничений на функционал эффекта - вы можете писать всё что можно написать на C/C++.

Так же все параметры сохраняются для каждого эффекта и загружаются обратно при его выборе.
Конфиги хранятся в **build/config.yml->plugin->graphic**

Для создания плагина, создайте разделяемую библиотеку и подключите в неё хедеры для API, затем пропишите функции **plugin_init, plugin_apply и plugin_finalize** с интерфейсом в стиле Си (Для этого сразу пишите на чистом Си, либо добавьте перед функциями **extern "C"**, если используете C++).
[см. Пример плагина на Си](../src/plugin/graphic-effect/example/brightness.c).
