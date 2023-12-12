## GreenSense: Sistema Inteligente de Monitoreo de Variables Ambientales para Plantas

Se desarrolla un sistema de monitoreo de variables ambientales para plantas que mide la humedad en el sustrato y la temperatura ambiente, permitiendo tomar decisiones sobre el riego de las plantas. Además, se utiliza una Raspberry Pi como controlador central para recibir las señales de los sensores y enviar las señales al actuador encargado del riego. Se establece una comunicación a través de una red inalámbrica con un servidor, donde se envía información de los datos medidos. Estos datos son procesados en un dashboard de métricas utilizando el stack Grafana, permitiendo acciones al actuador a través de un aplicativo web y notificaciones automatizadas a Telegram sobre alertas del sistema.

Requerimientos tecnicos:

| Nombre | Versión |
| --- | --- |
| React | 18 |
| Node | 16.20.2 |
| Axios | 1.6.0 |
| http://socket.io/ | 1.0.0 |

Credenciales de red:

```jsx
ssid: green-sense
password: holahola
```

Como ejecutar el proyecto en el front

```bash
npm run start
```

Como ejecutar el proyecto en el back

```bash
node app.js
```
