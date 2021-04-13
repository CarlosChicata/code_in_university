Instrucciones
Compilar
1. Usar el comando make.

Conectar
1. Iniciar server(./server.exe <puerto>, ex: ./server.exe 1480)
2. Iniciar clientes(./client.exe)
3. Conectar clientes a server(Ingresar IP(De la pc que ejecuta: normalmente es10.0.2.15) y puerto del server)

Cada cliente
1. Enviar nickname(obligatorio para usar server)(>>nickname cli1)
2. Enviar mensajes(>>msg cli2 hola, como estas?)
3. Cerrar sesion(>>exit)

Comandos validos

1) login : la estructura es "login <nombre> <pass>". sirve para autenticar y asignar nombre a tu conecion en el servidor.
2) all : la estructura es "all". sirver apra ver todos los nombres de los clientes actuales. Si falta alguno falla automaticamente.
3) talk: la estructura es "talk <nombre de cliente> <mensaje>". envia un mensaje a una persona es especifico. da un error si no existe.
4) loud: la estructura es "loud <mensaje>". Envia un mensaje a todos los clientes. Si falta todos, te envia un error.
 

