# Obeliks
Внешняя компонента 1С для выполнения звонков из 1С. АТС - Asterisk. Используется ARI REST API

В проекте использована библиотека Microsoft CppRestSDK. Для установки использовалась vcpkg.
Ликовку с библиотеками нужно делать только статическую иначе 1С компоненту не загрузит!

vcpkg install cpprestsdk:x86-windows-static для win32
vcpkg install cpprestsdk:x64-windows-static для x64

при установке также будут установлены доп. библиотеки.
