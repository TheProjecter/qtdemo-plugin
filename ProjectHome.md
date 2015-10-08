[QtDemo](http://doc.trolltech.com/4.6/qtdemo.html) plugin is a plugin for [QtCreator](http://qt.nokia.com/products/developer-tools) IDE. With this plugin, Qt demos and examples can be previewed in [QtCreator](http://qt.nokia.com/products/developer-tools) IDE directly. And these projects can be opened in the IDE also.

# How to install #

  * [Download](http://code.google.com/p/qtdemo-plugin/downloads/list) QtDemoPlugin-xxx.zip, and unpack it into QtCreator's home directory(QTCREATOR\_HOME).
  * Configure Qt SDK home path for QtDemo plugin.
    * Open QtDemo plugin's property file in an editor:   QTCREATOR\_HOME/bin/QtDemo.properties.
    * Edit QT\_SDK\_HOME, let it point to your current Qt SDK home path.
  * Start [QtCreator](http://qt.nokia.com/products/developer-tools).

> Note: QTCREATOR\_HOME is the folder where your [QtCreator](http://qt.nokia.com/products/developer-tools) IDE is installed.

# How to compile #

  * Checkout qtdemo plugin's source code
  * Copy qtdemo folder to QTCREATOR\_SRC\_HOME/src/plugins/
  * Copy plugins.pro to QTCREATOR\_SRC\_HOME/src/plugins/ to overwrite the original one.

> Note: QTCREATOR\_SRC\_HOME is the root folder of QtCreator's source code tree.

# Screen snapshot #

  * Home screen

> ![http://qtdemo-plugin.googlecode.com/files/qtdemo_plugin2.png](http://qtdemo-plugin.googlecode.com/files/qtdemo_plugin2.png)

  * Open project

> ![http://qtdemo-plugin.googlecode.com/files/qtdemo_plugin_open_project2.png](http://qtdemo-plugin.googlecode.com/files/qtdemo_plugin_open_project2.png)