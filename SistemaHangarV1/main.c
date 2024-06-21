#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <windows.h>
#include "pila.h"

#define TAMANIOFIL 40 // Cantidad de filas
#define TAMANIOCOL 80 // Cantidad de columnas
#define VACIO '.' // Digito para representar el espacio vacío
#define ESTELA ','
#define COLISION 'X' // Digito de la colision
#define ABAJO 30 // '\x1E'
#define ARRIBA 31 // '\x1F'
#define DERECHA 16 // '\x1F'
#define IZQUIERDA 17 // '\x1F'
#define MAX_COLORES 15 // Maximo número de colores diferentes

// --------------------------- ESTRUCTURAS ----------------------------------

typedef struct
{
    int hora;
    int minuto;
    int segundo;
} Hora;

typedef struct
{
    int dia;
    int mes;
    int anio;
} Fecha;

typedef struct
{
    int id;
    int idUsuario;
    Hora hora;
    Fecha fecha;
} Registro;

typedef struct
{
    char dni[10];
    char nombreApellido[30];
    char genero;
    int anioNacimiento;
} Persona;

typedef struct
{
    int id;
    char nombreUsuario[30];
    char contrasenia[30];
    int rol;
    Persona persona;
    int alta;
} Usuario;

typedef struct
{
    int id;
    char numLicencia[15];
    int horasVuelo;
    char rango[30];
    Persona persona;
    int alta;
} Piloto;

typedef struct
{
    int id;
    char nombre[30];
    int numeracion;
    char modelo[30];
    char aerolinea[30];
    int capacidadPasajeros;
    int vuelosRealizados;
    int combustibleMaximo; // maximo 1500
    int combustibleActual;
    int idPiloto; // -1 Sin Piloto en el avion
    int alta; // 1 alta 0 baja
} Avion;

typedef struct {
    int x;
    int y;
    int direccion; // 0 arriba 1 derecha 2 abajo 3 izquierda
    char icono;
    int color;
    char estado[20];
    int idAvion;
} UbicacionAvion;

// ------------------ VARIABLES GLOBALES -----------------

const char archivoPersonas[] = "personas.bin";
const char archivoUsuarios[] = "usuarios.bin";
const char archivoRegistros[] = "registros.bin";
const char archivoPilotos[] = "pilotos.bin";
const char archivoAviones[] = "aviones.bin";
const char archivoHangar[] = "hangar.bin";

// Variable Global necesaria para identificar el usuario que este logueado actualmente
Usuario* usuarioAutenticado = NULL;

// --------------------------- PROTOTIPADOS ----------------------------------
void login();
void menuLogin();
void subMenuLoginRegistrarUsuario();
// ------------------- MENUS -----------------
void menuInvitado();
void menuOperador();
// --------------
void subMenuHangar();
void subMenuHangarBetaRadar();
// --------------
void subMenuTaller();
void subMenuTallerCombustible();
void subMenuTallerVerAviones();
void subMenuTallerAgregarAvion();
void subMenuTallerModificarAvion();
void subMenuTallerDarAltaBaja();
// --------------
void subMenuCuartel();
void subMenuCuartelVerPilotos();
void subMenuCuartelAgregarPiloto();
void subMenuCuartelModificarPiloto();
void subMenuCuartelDarAltaBaja();
// --------------
void menuAdmin();
void subMenuAdminModificarUsuario();
// ---------------------- Extras ------------------------------
void mostrarDialogo(char texto[]);
void mostrarBarraEspera();
void mostrarAsciiAvion();
void mostrarAsciiHangar();
void mostrarAsciiCuartel();
void mostrarAsciiSistemaHangar();
int anioActual();
Fecha fechaActual();
Hora horaActual();
int cantidadRegistros(char archivo[], int unidad);
int cantidadAvionesDisponiblesHangar();
void ascenderPiloto(Piloto *piloto);
void convertirMinusculas(char *cadena);
/// ---------------------- Funciones de ordenamiento ----------------------
int pasarPilotosDisponiblesParaHangarDeArchivoArreglo(Piloto arreglo[], int dimension);
int pasarPilotosDeArchivoArreglo(Piloto pilotos[], int dimension);
int buscarPosMenorPilotoNombre (Piloto pilotos[], int pos, int validos);
void intercambioSeleccionPilotos(Piloto pilotos[], int posMen, int i);
void ordenarPorSeleccionArregloPilotos (int validos,  Piloto pilotos[]);
void mostrarArregloPilotos(Piloto pilotos[], int validos);
int pasarHangarDeArchivoArreglo (Avion arreglo[], int dimension);
int pasarAvionesDisponiblesParaHangarDeArchivoArreglo(Avion arreglo[], int dimension);
int pasarAvionesDeArchivoArreglo (Avion arreglo[], int dimension);
int pasarAvionesAltaDeArchivoArreglo (Avion arreglo[], int dimension);
void insertarOrdenarAviones(Avion aviones[], int i, int u);
void ordenarPorInsercionAvionesConbustibleActual(Avion arreglo[], int validos);
void mostrarArregloAviones(Avion arreglo[], int validos);
void mostrarArregloHangar(Avion arreglo[], int validos);
int buscarPosMenorAvionNombre (Avion aviones[], int pos, int validos);
void intercambioSeleccionAvionesNombres(Avion aviones[], int posMen, int i);
void ordenarPorSeleccionArregloAvionesNombre (int validos,  Avion aviones[]);
// ------------------- Validaciones -------------------------------
int validarLongitud(char palabra[], int minimo, int maximo);
int validarDni(char dni[]);
int validarAdmin(char user[]);
int validarUsuario(char usuario[]);
int validarNumLicencia(char licencia[]);
int validarExistenciaAvionDentroHangar(int idAvion);
int validarExistenciaPilotoDentroHangar(int idPiloto);
int validarHangarListoParaVolar();
int validarTodoHangarFueraDelRadar(UbicacionAvion aviones[], int numAviones);
// --------------------- CREATE --------------------------------
Registro crearRegistro(int idUsuario);
void registrarRegistro(Registro registro);
Usuario crearAdmin(char user[], char pass[]);
void registrarAdmin();
Persona crearPersona();
void registrarPersona(Persona persona);
Usuario crearUsuario(Persona persona);
void registrarUsuario(Usuario usuario);
Piloto crearPiloto(Persona persona);
void registrarPiloto(Piloto piloto);
Avion crearAvion();
void registrarAvion(Avion avion);
// ----------------------- READ --------------------------------
void registrarAvion(Avion avion);
Usuario getUsuario(int id);
Piloto getPiloto(int id);
Piloto getPilotoHangar(int id);
Avion getAvion(int id);
Avion getAvionHangar(int id);
void mostrarRegistro(Registro registro);
void cargarUltimoRegistroRecursivo(Pila *pila, FILE *archi, int *contador);
void cargarPilaRegistros(Pila *pilaRegistros);
void mostrarRegistros();
void mostrarPersona(Persona p);
void mostrarUsuario(Usuario u);
void mostrarUsuarios();
void mostrarAvion(Avion avion);
void mostrarAvionHangar(Avion avion);
void mostrarAviones();
void mostrarAvionesDinamicos(Avion **aviones, int validos);
void mostrarAvionesAlfabeticamente();
void mostrarAvionesPorCombustible();
void mostrarAvionesDadosAlta();
void mostrarAvionesDadosBaja();
void mostrarPiloto(Piloto  p);
void mostrarPilotos();
void mostrarPilotosDinamicos(Piloto **pilotos, int validos);
void mostrarPilotosAlfabeticamente();
void mostrarPilotosDadosAlta();
void mostrarPilotosDadosBaja();
int contieneSubcadena(char *totalCadena, char *subcadena);
void consultaAvionAgregarLista(Avion **avionesDinamicos, int *validos, Avion avion);
void consultarAvionesPorNombre();
void consultaPilotoAgregarLista(Piloto **pilotosDinamicos, int *validos, Piloto piloto);
void consultarPilotosPorNombre();
// ------------------------- UPDATE ---------------------------------
Piloto modificarPiloto(Piloto pilot);
Avion modificarAvion(Avion avion);
void modificarRolUsuario();
Usuario modificarUsuario(Usuario usuario);
void modificarUsuarioAutenticado();
void setUsuario (Usuario u);
void setAvion (Avion a);
void setAvionHangar(Avion a);
void setPiloto (Piloto p);
void mostrarPilotoEnAvion(Avion  a);
void mostrarPilotosEnAviones(Avion arreglo[], int validos);
void mostrarHangar(char hangar[]);
int pasarPilotosAltaDeArchivoArreglo(Piloto arreglo[],char archivo[]);
void cargarCombustible(char archivo[]);
int buscarAvionEnArreglo(Avion a, Avion arreglo[], int validos);
int combustibleOptimo(Avion a);
int buscarPilotoEnArreglo(int idPiloto, Avion arreglo[], int validos);
void mostrarPilotosDadosDeAlta(char archivo[]);
void agregarAvionAHangar(Avion a);
// -------------------------------- DELETE -------------------------------
void modificarAltaUsuario(int id);
void modificarAltaAvion(int opcion);
void modificarAltaPiloto(int opcion);

/////////PROTOTIPADO////////////////////////////////////
void imprimirRadarYMenu(char matriz[TAMANIOFIL][TAMANIOCOL], UbicacionAvion* aviones, int numeroDeAviones);
int moverAviones(UbicacionAvion* aviones, int numeroDeAviones, char matriz[TAMANIOFIL][TAMANIOCOL]);
void inicializarMatriz(char matriz[TAMANIOFIL][TAMANIOCOL], UbicacionAvion* aviones, int numeroDeAviones, int* colores);
void generarColoresUnicos(int* colores, int numColores);
void gotoxy(int x, int y);

/// -------------------- MAIN --------------------

int main()
{
    menuLogin();
    return 0;
}

/// ---------------------------- MENUS -------------------------------

void login()
{
    FILE* archi;
    archi = fopen(archivoUsuarios,"rb");
    if (archi != NULL)
    {
        char username[30], password[30];
        system("cls");
        textcolor(9);
        mostrarDialogo("Inicio de Sesion");
        textcolor(15);
        printf("\nIngrese su nombre de usuario => ");
        fflush(stdin);
        gets(username);
        printf("\nIngrese su contrase%ca => ",164);
        fflush(stdin);
        gets(password);
        Usuario u;
        int encontrado = 0;
        while (fread(&u,sizeof(Usuario),1,archi) > 0 && encontrado != 1)
        {
            // Compara cada usuario con el nombre ingresado
            if (strcmpi(u.nombreUsuario,username) == 0)
            {
                encontrado = 1;
                if (strcmp(u.contrasenia,password) == 0)
                {
                    if (u.alta != 1)
                    {
                        textcolor(12);
                        mostrarDialogo("Tu usuario fue vetado del sistema");
                        textcolor(15);
                        printf("\n\n");
                        system("pause");
                    }
                    else
                    {
                        // Asigna memoria para el usuario y copia los datos
                        if (usuarioAutenticado == NULL)
                        {
                            usuarioAutenticado = (Usuario*) malloc(sizeof(Usuario));
                        }
                        if (usuarioAutenticado != NULL)
                        {
                            *usuarioAutenticado = u;
                        }
                        Registro r = crearRegistro(u.id);
                        registrarRegistro(r);
                    }
                }
                else
                {
                    textcolor(12);
                    mostrarDialogo("Contrasenia incorrecta");
                    textcolor(15);
                    /*
                    printf("\n\n");
                    system("pause");
                    */
                }
            }
        }
        if (encontrado == 0)
        {
            textcolor(12);
            mostrarDialogo("Usuario no encontrado");
            textcolor(15);
            /*
            printf("\n\n");
            system("pause");
            */
        }
        fclose(archi);
    }
}

void menuLogin()
{
    int aux, salir = 0;
    char opcion = 'x';

    while (salir != 1)
    {
        system("cls");
        textcolor(9);
        mostrarAsciiSistemaHangar();
        textcolor(15);
        printf("\n____________________________________________________________________________\n");
        printf("\n1. Iniciar sesion\n2. Registrarse\n\n");
        textcolor(9);
        printf("0. Finalizar Sistema");
        textcolor(15);
        printf("\n\nIngrese opcion => ");
        fflush(stdin);
        scanf("%c",&opcion);
        switch (opcion)
        {
        case '1':
            login();
            if (usuarioAutenticado != NULL)
            {
                if (usuarioAutenticado->rol == 0)
                {
                    menuInvitado();
                }
                else if (usuarioAutenticado->rol == 1)
                {
                    menuOperador();
                }
                else if (usuarioAutenticado->rol == 2)
                {
                    menuAdmin();
                }
                else
                {
                    textcolor(12);
                    mostrarDialogo("Error en los permisos");
                    textcolor(15);
                    printf("\n\n");
                    system("pause");
                }
            }
            else
            {
                textcolor(12);
                mostrarDialogo("No se pudo iniciar sesion");
                textcolor(15);
                printf("\n\n");
                system("pause");
            }
            break;
        case '2':
            subMenuLoginRegistrarUsuario();
            break;
        //case '9':
            //registrarAdmin();
            //break;
        case '0':
            salir = 1;
            textcolor(9);
            mostrarDialogo("Finalizando Sistema");
            textcolor(15);
            printf("\n Equipo de Desarrollo:");
            textcolor(6);
            printf("\n Daniel Herrera\n Diego Groppo\n Leonel Benavides\n Nacho Gonzalez");
            textcolor(15);
            printf("\n\n");
            break;
        default:
            textcolor(12);
            mostrarDialogo("Opcion invalida");
            textcolor(15);
            printf("\n\n");
            system("pause");
        }
    }
}

void subMenuLoginRegistrarUsuario()
{
    Persona p = crearPersona();
    Usuario u = crearUsuario(p);
    registrarPersona(p);
    registrarUsuario(u);
    textcolor(10);
    mostrarDialogo("Registro Usuario Finalizado");
    textcolor(15);
    printf("\n\n");
    system("pause");
}

// --------------------------------

void menuInvitado()
{
    int logout = 0;
    char opcion = 'x';

    while (logout != 1)
    {
        system("cls");
        mostrarDialogo("Menu Invitado");
        printf("\n\n1. Ver todos los Aeroplanos disponibles\n2. Ver todos los Pilotos disponibles\n3. Modificar mi perfil\n\n0. Cerrar Sesion");
        printf("\n\nIngrese opcion => ");
        fflush(stdin);
        scanf("%c",&opcion);
        switch (opcion)
        {
        case '1':
            mostrarAvionesDadosAlta();
            break;
        case '2':
            mostrarPilotosDadosAlta();
            break;
        case '3':
            modificarUsuarioAutenticado();
            break;
        case '0':
            logout = 1;
            usuarioAutenticado = NULL;
            textcolor(9);
            mostrarDialogo("Cerrando Sesion: Invitado");
            textcolor(15);
            printf("\n\n");
            system("pause");
            break;
        default:
            textcolor(12);
            mostrarDialogo("Opcion invalida");
            textcolor(15);
            printf("\n\n");
            system("pause");
        }
    }
}

// --------------------------------

void menuOperador()
{
    // Lineas necesarias para consumir combustible ya que el Radar no lo hace correctamente
    // Consumir combustible de 6 aviones
    Avion a0 = getAvion(0);
    Avion a1 = getAvion(1);
    Avion a2 = getAvion(2);
    Avion a3 = getAvion(3);
    Avion a4 = getAvion(4);
    Avion a5 = getAvion(5);
    a0.combustibleActual = 1079;
    a1.combustibleActual = 50;
    a2.combustibleActual = 550;
    a3.combustibleActual = 700;
    a4.combustibleActual = 450;
    a5.combustibleActual = 100;
    setAvion(a0);
    setAvion(a1);
    setAvion(a2);
    setAvion(a3);
    setAvion(a4);
    setAvion(a5);

    int aux = 0, id, logout = 0;
    char opcion = 'x';
    while (logout != 1)
    {
        system("cls");
        textcolor(9);
        mostrarDialogo("Menu Operador");
        textcolor(15);
        printf("\n\n1. Menu Hangar de Despegue\n2. Menu Taller de Aviones\n3. Menu Cuartel de Pilotos\n\n4. Modificar mi Perfil");
        textcolor(9);
        printf("\n\n0. Cerrar Sesion");
        textcolor(15);
        printf("\n\nIngrese opcion => ");
        fflush(stdin);
        scanf("%c",&opcion);
        switch (opcion)
        {
        case '1':
            subMenuHangar();
            break;
        case '2':
            subMenuTaller();
            break;
        case '3':
            subMenuCuartel();
            break;
        case '4':
            modificarUsuarioAutenticado();
            break;
        case '0':
            logout = 1;
            usuarioAutenticado = NULL;
            textcolor(9);
            mostrarDialogo("Cerrando Sesion: Operador");
            textcolor(15);
            printf("\n\n");
            system("pause");
            system("cls");
            break;
        default:
            textcolor(12);
            mostrarDialogo("Opcion invalida");
            textcolor(15);
            printf("\n\n");
            system("pause");
        }
    }
}

// --------------------------------

void subMenuHangar()
{
    int volver = 0;
    char opcion = 'x';

    while (volver != 1)
    {
        system("cls");
        mostrarAsciiHangar();
        textcolor(9);
        mostrarDialogo("Menu Hangar de Despegue");
        textcolor(15);
        printf("\n\n1. Asignar Avion y Piloto al Hangar de Despegue\n2. Retirar todos los Aviones del Hangar\n3. Mostrar Hangar");
        textcolor(10);
        printf("\n\n4. (BETA) Ejecutar Radar y Despuegue");
        textcolor(9);
        printf("\n\n0. Volver");
        textcolor(15);
        printf("\n\nIngrese opcion => ");
        fflush(stdin);
        scanf("%c",&opcion);
        switch (opcion)
        {
        case '1':
            subMenuHangarAsignarAvion();
            break;
        case '2':
            subMenuHangarLimpiarHangar();
            break;
        case '3':
            subMenuHangarVerHangar();
            break;
        case '4':
            subMenuHangarBetaRadar();
            system("pause");
            break;
        case '0':
            volver = 1;
            textcolor(9);
            mostrarDialogo("Volviendo al menu principal");
            textcolor(15);
            printf("\n\n");
            system("pause");
            break;
        default:
            textcolor(12);
            mostrarDialogo("Opcion invalida");
            textcolor(15);
            printf("\n\n");
            system("pause");
        }
    }
}

void subMenuHangarAsignarAvion()
{
    int cantAvionesDisp = cantidadAvionesDisponiblesHangar();
    int cantPilotosDisp = cantidadPilotosDisponiblesHangar();
    int cantAvionesHangar = cantidadRegistros(archivoHangar,sizeof(Avion));

    if(cantAvionesDisp == 0)
    {
        textcolor(12);
        mostrarDialogo("No existen Aviones disponibles para asignar");
        textcolor(15);
        printf("\n\n");
        system("pause");
    }
    else if (cantPilotosDisp == 0)
    {
        textcolor(12);
        mostrarDialogo("No existen Pilotos disponibles para asignar");
        textcolor(15);
        printf("\n\n");
        system("pause");
    }
    else
    {
        if (cantAvionesHangar >= 15)
        {
            textcolor(12);
            mostrarDialogo("El Hangar se encuentra en su capacidad maxima");
            textcolor(15);
            printf("\n\n");
            system("pause");
        }
        else
        {
            int dimensionP = cantidadRegistros(archivoPilotos,sizeof(Piloto));
            int dimensionA = cantidadRegistros(archivoAviones,sizeof(Avion));
            Piloto arregloP[dimensionP];
            Avion arregloA[dimensionA];
            int validosP = 0;
            int validosA = 0;
            int idPiloto = 0;
            int idAvion = 0;
            // ---------------------------------------------------------------------------------------------
            validosA = pasarAvionesDisponiblesParaHangarDeArchivoArreglo(arregloA, dimensionA);
            mostrarArregloAviones(arregloA,validosA);
            int posicionA = -1;
            do
            {
                printf("\n Seleccione la ");
                textcolor(6);
                printf("ID del Avion");
                textcolor(15);
                printf(" que desea asignar => ");
                scanf("%d",&idAvion);

                // ------ Buscar la posicion del Avion dentro del arreglo segun su ID
                for (int i=0; i<validosA && posicionA == -1; i++) {
                    if (arregloA[i].id == idAvion) {
                        posicionA = i;
                    }
                }
                if (posicionA == -1) {
                    textcolor(12);
                    mostrarDialogo("El Avion no se encuentra habilitado para elegir");
                    textcolor(15);
                } else {
                    if (arregloA[posicionA].alta < 1)
                    {
                        textcolor(12);
                        printf("\n El Avion se encuentra dado de baja..\n");
                        textcolor(15);
                    }
                    if (arregloA[posicionA].combustibleActual < 300)
                    {
                        textcolor(12);
                        printf("\n El Avion no posee combustible suficiente..\n");
                        textcolor(15);
                    }
                    if (validarExistenciaAvionDentroHangar(idAvion) != 0) {
                        textcolor(12);
                        printf("\n El Avion ya se encuentra asignado dentro del Hangar..\n");
                        textcolor(15);
                    }
                }

            }
            while(posicionA == -1 || posicionA < 0 || posicionA > validosA || arregloA[posicionA].alta < 1 || validarExistenciaAvionDentroHangar(idAvion) != 0 || arregloA[posicionA].combustibleActual < 300);
            // ---------------------------------------------------------------------------------------------
            validosP = pasarPilotosDisponiblesParaHangarDeArchivoArreglo(arregloP, dimensionP);
            mostrarArregloPilotos(arregloP,validosP);
            int posicionP = -1;
            do
            {
                printf("\n Seleccione la ");
                textcolor(6);
                printf("ID del Piloto");
                textcolor(15);
                printf(" que desea asignar => ");
                scanf("%d",&idPiloto);

                // ------ Buscar la posicion del Piloto dentro del arreglo segun su ID
                for (int i=0; i<validosP && posicionP == -1; i++) {
                    if (arregloP[i].id == idPiloto) {
                        posicionP = i;
                    }
                }
                if (posicionP == -1) {
                    textcolor(12);
                    mostrarDialogo("El Piloto no se encuentra habilitado para elegir");
                    textcolor(15);
                } else {
                    if (arregloP[posicionP].alta < 1)
                    {
                        textcolor(12);
                        printf("\n El Piloto no se encuentra disponible..");
                        textcolor(15);
                    }
                    if (validarExistenciaPilotoDentroHangar(idPiloto) != 0) {
                        textcolor(12);
                        printf("\n El Piloto ya se encuentra asignado dentro del Hangar..\n");
                        textcolor(15);
                    }
                }
            }
            while(posicionP == -1 || posicionP < 0 || posicionP > validosP || arregloP[posicionP].alta < 1 || validarExistenciaPilotoDentroHangar(idPiloto) != 0);

            // Asignando piloto al avion
            Avion avion = arregloA[posicionA];
            avion.idPiloto = idPiloto;

            agregarAvionAHangar(avion);
        }
    }

}

void subMenuHangarLimpiarHangar()
{
    int dimension = cantidadRegistros(archivoHangar,sizeof(Avion));
    if (dimension < 1) {
        textcolor(12);
        mostrarDialogo("No hay Aviones para retirar dentro del Hangar");
        textcolor(15);
        printf("\n\n");
        system("pause");
    } else {
        textcolor(9);
        mostrarBarraEspera();
        textcolor(15);
        FILE* buffer;
        buffer = fopen(archivoHangar,"wb");
        if (buffer != NULL) {
            // El archivo se ha abierto en modo "wb", lo cual lo trunca a 0 bytes
            fclose(buffer);
        }
        textcolor(10);
        mostrarDialogo("Se han retirado todos los Aviones del Hangar con exito");
        textcolor(15);
        printf("\n\n");
        system("pause");
    }
}

void subMenuHangarVerHangar()
{
    int dim = cantidadRegistros(archivoHangar,sizeof(Avion));
    if (dim == 0) {
        textcolor(12);
        mostrarDialogo("No se encuentran Aviones dentro del Hangar");
        textcolor(15);
        printf("\n\n");
        system("pause");
    } else {
        Avion arreglo[dim];
        int validos = 0;
        validos = pasarHangarDeArchivoArreglo(arreglo,dim);
        mostrarArregloHangar(arreglo,validos);
    }
}

void subMenuHangarBetaRadar ()
{
    if (validarHangarListoParaVolar() > 0) {
        textcolor(12);
        mostrarDialogo("Uno o mas Aviones requieren Combustible");
        textcolor(9);
        mostrarDialogo("Visite el Taller de Aviones");
        textcolor(15);
    } else {
        srand(time(NULL));
        char matriz[TAMANIOFIL][TAMANIOCOL]; // 40 / 80

        int numeroDeAviones = cantidadRegistros(archivoHangar,sizeof(Avion));

        // ----------------------------------------------------------------------------------

        UbicacionAvion aviones[numeroDeAviones];

        // Generar colores distintos
        int colores[MAX_COLORES]; // 15
        generarColoresUnicos(colores, MAX_COLORES);

        //Inicializar matriz
        inicializarMatriz(matriz, aviones, numeroDeAviones, colores);

        // Bucle principal
        do
        {
            system("cls");

            imprimirRadarYMenu(matriz, aviones, numeroDeAviones);

            moverAviones(aviones, numeroDeAviones, matriz);

            Sleep(650);  // 1000 Espera un segundo para que se actualice el radar

            if (validarTodoHangarFueraDelRadar(aviones,numeroDeAviones) > 0) // Si ya estan fuera, que se muevan 1 casillero mas
            {
                system("cls");
                imprimirRadarYMenu(matriz, aviones, numeroDeAviones);
                moverAviones(aviones, numeroDeAviones, matriz);

                Sleep(650);
            }
        }
        while (validarTodoHangarFueraDelRadar(aviones,numeroDeAviones) < 1); // Repetir mientras NO ESTEN afuera

        /// ---------------- LUEGO DE VOLAR EN EL RADAR, SE HACE UNA COMPROBACION PARA MODIFICAR LOS AVIONES Y PILOTOS ----------------------------------

        // Comprobacion final para modificar los Aviones en el archivoAviones
        for (int i=0; i<numeroDeAviones; i++) {

            Avion a = getAvionHangar(aviones[i].idAvion); // Trae el avion desde el Hangar

            if (strcmpi(aviones[i].estado,"Colisionado") == 0) { /// Si el Avion resulto Colisionado ...

                if (a.id != -1) { // Corrobora que se haya traido un Avion valido (ID != -1)
                    // -------------- AVION HANGAR ----------------
                    a.combustibleActual -= 150; // -150 de combustible por viaje interrumpido
                    if (a.combustibleActual < 0) {
                        a.combustibleActual = 0;
                    }
                    a.alta = 0; // Si colisiona el avion se da de baja
                    setAvionHangar(a); // Actualiza la informacion en el archivo de Hangar
                    // --------------- PILOTO ---------------
                    Piloto p = getPiloto(a.idPiloto);
                    if (p.id != -1) { // Corrobora que se haya traido un Avion valido (ID != -1)
                        p.alta = 0; // Si colisiona el piloto se da de baja
                        setPiloto(p); // Actualiza la informacion en el archivo de Pilotos
                    }
                    // -------------- AVION ----------------
                    a.idPiloto = -1; // Le retira el piloto para asignarlo al archivo de Aviones
                    setAvion(a); // Actualiza la informacion en el archivo de Aviones
                }

            } else if (strcmpi(aviones[i].estado,"Viaje Finalizado") == 0) { /// VIAJE CON EXITO

                if (a.id != -1) { // Corrobora que se haya traido un Avion valido (ID != -1)
                    // -------------- AVION HANGAR ----------------
                    a.combustibleActual -= 300; // -300 de combustible por viaje finalizado
                    if (a.combustibleActual < 0) {
                        a.combustibleActual = 0;
                    }
                    a.vuelosRealizados += 1;  // +1 viaje realizado
                    setAvionHangar(a); // Actualiza la informacion en el archivo de Hangar
                    // --------------- PILOTO ---------------
                    Piloto p = getPiloto(a.idPiloto);
                    if (p.id != -1) { // Corrobora que se haya traido un Avion valido (ID != -1)
                        p.horasVuelo += 7; // +7 horas de vuelo por viaje finalizado con exito
                        setPiloto(p); // Actualiza la informacion en el archivo de Pilotos
                    }
                    // -------------- AVION ----------------
                    a.idPiloto = -1; // Le retira el piloto para asignarlo al archivo de Aviones
                    setAvion(a); // Actualiza la informacion en el archivo de Aviones
                }

            }
        }
    }
}

// --------------------------------

void subMenuTaller()
{
    Avion avion;
    int volver = 0;
    char opcion = 'x';
    while (volver != 1)
    {
        system("cls");
        mostrarAsciiAvion();
        textcolor(9);
        mostrarDialogo("Menu Taller de Aviones");
        textcolor(15);
        printf("\n1. Recargar Combustible\n2. Ver aeroplanos\n3. Agregar avion\n4. Modificar avion\n5. Dar de alta/baja\n\n");
        textcolor(9);
        printf("0. Volver");
        textcolor(15);
        printf("\n\nIngrese opcion => ");
        fflush(stdin);
        scanf("%c",&opcion);
        switch (opcion)
        {
        case '1':
            subMenuTallerCombustible();
            break;
        case '2':
            subMenuTallerVerAviones();
            break;
        case '3':
            subMenuTallerAgregarAvion();
            break;
        case '4':
            subMenuTallerModificarAvion();
            break;
        case '5':
            subMenuTallerDarAltaBaja();
            break;
        case '0':
            volver = 1;
            textcolor(9);
            mostrarDialogo("Volviendo al menu principal");
            textcolor(15);
            printf("\n\n");
            system("pause");
            break;
        default:
            textcolor(12);
            mostrarDialogo("Opcion invalida");
            textcolor(15);
            printf("\n\n");
            system("pause");
        }
    }
}

void subMenuTallerCombustible()
{
    mostrarAvionesPorCombustible();
    textcolor(9);
    mostrarDialogo("Preparando combustible suficiente en el Taller de Aviones");
    textcolor(15);
    printf("\n\n");
    system("pause");
    int validosA = 0, validosH = 0, i = 0;
    int dimensionA = cantidadRegistros(archivoAviones,sizeof(Avion));
    int dimensionH = cantidadRegistros(archivoHangar,sizeof(Avion));
    Avion arregloA[dimensionA];
    Avion arregloH[dimensionH];
    validosA = pasarAvionesDeArchivoArreglo(arregloA, dimensionA);
    validosH = pasarHangarDeArchivoArreglo(arregloH, dimensionH);
    ordenarPorInsercionAvionesConbustibleActual(arregloA,validosA);
    int cargasRealizadas = 0;
    // Cargan los aviones del Hangar, sino ocurre errores ya que el avion existe tanto DENTRO del archioAviones y DENTRO del archivoHangar
    for (int i=0; i<validosH; i++)
    {
        if(arregloH[i].combustibleActual < 1500)
        {
            arregloH[i].combustibleActual = 1500;
            setAvionHangar(arregloH[i]); // sobreescribe el avion con el combustible lleno
        }
    }
    // Cargan los aviones del archivoAviones
    for (int i=0; i<validosA; i++)
    {
        if(arregloA[i].combustibleActual < 1500)
        {
            arregloA[i].combustibleActual = 1500;
            setAvion(arregloA[i]); // sobreescribe el avion con el combustible lleno
            cargasRealizadas ++;
        }
    }
    if (cargasRealizadas == 0)
    {
        textcolor(9);
        mostrarDialogo("Todos los aviones tienen el tanque lleno");
        textcolor(15);
        printf("\n\n");
        system("pause");
    }
    else
    {
        textcolor(9);
        //for (int i=0; i<cargasRealizadas; i++) {
        mostrarBarraEspera();
        //}
        textcolor(10);
        mostrarDialogo("Se han llenado todos los tanques de combustible");
        textcolor(15);
        printf("\n\n");
        system("pause");
    }
}

void subMenuTallerVerAviones()
{
    int volver = 0;
    char opcion = 'x';

    while (volver != 1)
    {
        system("cls");
        textcolor(9);
        mostrarDialogo("Menu Taller de Aviones -> (Ver aeroplanos)");
        textcolor(15);
        printf("\n\n1. Buscar Avion/es por Nombre\n2. Ver todos los aeroplanos\n3. Ver aviones alfabeticamente\n4. Ver aeroplanos disponibles\n5. Ver aeroplanos de baja\n\n");
        textcolor(9);
        printf("0. Volver");
        textcolor(15);
        printf("\n\nIngrese opcion => ");
        fflush(stdin);
        scanf("%c",&opcion);
        switch (opcion)
        {
        case '1':
            consultarAvionesPorNombre();
            break;
        case '2':
            mostrarAviones();
            break;
        case '3':
            mostrarAvionesAlfabeticamente();
            break;
        case '4':
            mostrarAvionesDadosAlta();
            break;
        case '5':
            mostrarAvionesDadosBaja();
            break;
        case '0':
            volver = 1;
            textcolor(9);
            mostrarDialogo("Volviendo al Menu Taller de Aviones");
            textcolor(15);
            printf("\n\n");
            system("pause");
            break;
        default:
            textcolor(12);
            mostrarDialogo("Opcion invalida");
            textcolor(15);
            printf("\n\n");
            system("pause");
        }
    }
}

void subMenuTallerAgregarAvion()
{
    Avion a = crearAvion();
    registrarAvion(a);
    textcolor(10);
    mostrarDialogo("Registro Avion Finalizado");
    textcolor(15);
    printf("\n\n");
    system("pause");
}

void subMenuTallerModificarAvion()
{
    textcolor(9);
    mostrarDialogo("Menu Taller de Aviones -> (Modificar avion)");
    textcolor(15);
    FILE *archi;
    archi = fopen(archivoAviones, "r+b");
    char salir = 'n';
    int avionBuscado;
    Avion avion;
    if (archi != NULL)
    {
        do
        {
            mostrarAviones();
            printf("\n\nIngrese la ID del avion a modificar => ");
            scanf("%d", &avionBuscado);
            int encontrado = 0;
            while (fread(&avion, sizeof(Avion), 1, archi) > 0)
            {
                if (avionBuscado == avion.id)
                {
                    encontrado = 1;

                    avion = modificarAvion(avion); /// <------- MENU MODIFICAR AVION

                    // Modifica el archivo de Aviones
                    fseek(archi, sizeof(Avion)*(-1), SEEK_CUR);
                    fwrite(&avion, sizeof(Avion), 1, archi);
                    fflush(archi);
                    // Modifica el archivo de Hangar
                    Avion aH = getAvionHangar(avion.id);
                    avion.idPiloto = aH.idPiloto;
                    setAvionHangar(avion);

                    system("cls");
                    textcolor(10);
                    mostrarDialogo("Avion modificado");
                    textcolor(15);
                    mostrarAvion(avion);
                }
            }

            if (!encontrado)
            {
                textcolor(12);
                mostrarDialogo("No se encontro ningun avion con ese ID");
                textcolor(15);
            }

            printf("\n\nDesea guardar los cambios y salir? (S = Salir) => ");
            fflush(stdin);
            scanf("%c", &salir);
            salir = tolower(salir); // lo pasa a minusculas
            system("cls");
            rewind(archi);
        }
        while (salir != 's');
        fclose(archi);
    }
    else
    {
        textcolor(12);
        mostrarDialogo("No se ha podido abrir el archivo de Aviones");
        textcolor(15);
        printf("\n\n");
        system("pause");
    }

}

void subMenuTallerDarAltaBaja()
{
    int id;
    mostrarAviones();
    printf("\n\nIngrese ID del Avion => ");
    scanf("%d",&id);
    // ------------------------------------------------------------
    Avion av = getAvion(id);
    if (av.alta > 0)
    {
        av.alta = 0; // Si esta de alta, se lo da de baja
    }
    else
    {
        av.alta = 1; // Si esta de baja, se lo da de alta
    }
    setAvion(av);
    // ------------------------------------------------------------
    Avion aH = getAvionHangar(id);
    if (aH.id != -1) { // Si encontro el avion dentro del hangar
        if (aH.alta > 0)
        {
            aH.alta = 0; // Si esta de alta, se lo da de baja
        }
        else
        {
            aH.alta = 1; // Si esta de baja, se lo da de alta
        }
        setAvionHangar(aH);
    }
    // ------------------------------------------------------------
    if (av.alta > 0)
    {
        textcolor(10);
        mostrarDialogo("Avion dado de alta");
        textcolor(15);
    }
    else
    {
        textcolor(12);
        mostrarDialogo("Avion dado de baja");
        textcolor(15);
    }
    printf("\n\n");
    system("pause");
}

// --------------------------------

void subMenuCuartel()
{
    Persona pe;
    Piloto pi;
    int volver = 0;
    char opcion = 'x';

    while (volver != 1)
    {
        system("cls");
        mostrarAsciiCuartel();
        textcolor(9);
        mostrarDialogo("Menu Cuartel de Pilotos");
        textcolor(15);
        printf("\n\n1. Ver pilotos\n2. Agregar piloto\n3. Modificar piloto\n4. Dar de baja/alta\n\n");
        textcolor(9);
        printf("0. Volver");
        textcolor(15);
        printf("\n\nIngrese opcion => ");
        fflush(stdin);
        scanf("%c",&opcion);
        switch (opcion)
        {
        case '1':
            subMenuCuartelVerPilotos();
            break;
        case '2':
            subMenuCuartelAgregarPiloto();
            break;
        case '3':
            subMenuCuartelModificarPiloto();
            break;
        case '4':
            subMenuCuartelDarAltaBaja();
            break;
        case '0':
            volver = 1;
            textcolor(9);
            mostrarDialogo("Volviendo al menu principal");
            textcolor(15);
            printf("\n\n");
            system("pause");
            break;
        default:
            textcolor(12);
            mostrarDialogo("Opcion invalida");
            textcolor(15);
            printf("\n\n");
            system("pause");
        }
    }
}

void subMenuCuartelVerPilotos()
{
    int volver = 0;
    char opcion = 'x';

    while (volver != 1)
    {
        system("cls");
        textcolor(9);
        mostrarDialogo("Menu Cuartel de Pilotos -> (Ver pilotos)");
        textcolor(15);
        printf("\n\n1. Buscar Piloto/s por Nombre\n2. Ver todos los pilotos\n3. Ver pilotos alfabeticamente\n4. Ver pilotos disponibles\n5. Ver pilotos de baja\n\n");
        textcolor(9);
        printf("0. Volver");
        textcolor(15);
        printf("\n\nIngrese opcion => ");
        fflush(stdin);
        scanf("%c",&opcion);
        switch (opcion)
        {
        case '1':
            consultarPilotosPorNombre();
            break;
        case '2':
            mostrarPilotos();
            break;
        case '3':
            mostrarPilotosAlfabeticamente();
            break;
        case '4':
            mostrarPilotosDadosAlta();
            break;
        case '5':
            mostrarPilotosDadosBaja();
            break;
        case '0':
            volver = 1;
            textcolor(9);
            mostrarDialogo("Volviendo al Menu Cuartel de Pilotos");
            textcolor(15);
            printf("\n\n");
            system("pause");
            break;
        default:
            textcolor(12);
            mostrarDialogo("Opcion invalida");
            textcolor(15);
            printf("\n\n");
            system("pause");
        }
    }
}

void subMenuCuartelAgregarPiloto()
{
    Persona pe = crearPersona();
    Piloto pi = crearPiloto(pe);
    registrarPersona(pe);
    registrarPiloto(pi);
    textcolor(10);
    mostrarDialogo("Registro Piloto Finalizado");
    textcolor(15);
    printf("\n\n");
    system("pause");
}

void subMenuCuartelModificarPiloto()
{
    FILE *archi;
    archi = fopen(archivoPilotos, "r+b");
    char salir = 'n';
    int pilotoBuscado;
    Piloto piloto;
    if (archi != NULL)
    {
        do
        {
            mostrarPilotos();
            printf("\n\nIngrese la ID del piloto a modificar => ");
            scanf("%d", &pilotoBuscado);
            int encontrado = 0;
            while (fread(&piloto, sizeof(Piloto), 1, archi) > 0)
            {
                if (pilotoBuscado == piloto.id)
                {
                    encontrado = 1;
                    piloto = modificarPiloto(piloto); /// <------ MENU MODIFICAR PILOTO
                    fseek(archi, sizeof(Piloto)*(-1), SEEK_CUR);
                    fwrite(&piloto, sizeof(Piloto), 1, archi);
                    fflush(archi);
                    system("cls");
                    textcolor(10);
                    mostrarDialogo("Piloto Modificado");
                    textcolor(15);
                    mostrarPiloto(piloto);
                }
            }

            if (!encontrado)
            {
                textcolor(12);
                mostrarDialogo("No se encontro ningun piloto con ese ID");
                textcolor(15);
            }

            printf("\n\nDesea guardar los cambios y salir? (S = Salir) => ");
            fflush(stdin);
            scanf("%c", &salir);
            salir = tolower(salir); // lo pasa a minusculas
            rewind(archi);
        }
        while (salir != 's');
        fclose(archi);
    }
    else
    {
        textcolor(12);
        mostrarDialogo("No se ha podido abrir el archivo de Pilotos");
        textcolor(15);
        printf("\n\n");
        system("pause");
    }

}

void subMenuCuartelDarAltaBaja()
{
    int id;
    mostrarPilotos();
    printf("\n\nIngrese ID del Piloto => ");
    scanf("%d",&id);
    // ------------------------------------------------------------
    Piloto pi = getPiloto(id);
    if (pi.alta > 0)
    {
        pi.alta = 0; // Si esta de alta, se lo da de baja
    }
    else
    {
        pi.alta = 1; // Si esta de baja, se lo da de alta
    }
    setPiloto(pi);
    // ------------------------------------------------------------
    if (pi.alta > 0)
    {
        textcolor(10);
        mostrarDialogo("Piloto dado de alta");
        textcolor(15);
    }
    else
    {
        textcolor(12);
        mostrarDialogo("Piloto dado de baja");
        textcolor(15);
    }
    printf("\n\n");
    system("pause");
}

// --------------------------------

void menuAdmin()
{
    int aux = 0, id, logout = 0;
    char opcion = 'x';

    while (logout != 1)
    {
        system("cls");
        textcolor(6);
        mostrarDialogo("Menu Administrador");
        textcolor(15);
        printf("\n1. Listar usuarios\n2. Ver ultimos Login\n3. Dar permisos a un usuario\n4. Modificar un usuario\n5. Dar de baja/alta un usuario\n6. Modificar mi Perfil\n\n");
        textcolor(6);
        printf("0. Cerrar Sesion");
        textcolor(15);
        printf("\n\nIngrese opcion => ");
        fflush(stdin);
        scanf("%c",&opcion);
        switch (opcion)
        {
        case '1':
            mostrarUsuarios();
            break;
        case '2':
            mostrarRegistros();
            break;
        case '3':
            modificarRolUsuario();
            break;
        case '4':
            subMenuAdminModificarUsuario();
            break;
        case '5':
            mostrarUsuarios();
            printf("\n\nIngrese ID => ");
            scanf("%d",&id);
            modificarAltaUsuario(id);
            break;
        case '6':
            modificarUsuarioAutenticado();
            break;
        case '0':
            logout = 1;
            usuarioAutenticado = NULL;
            textcolor(9);
            mostrarDialogo("Cerrando Sesion: Administrador");
            textcolor(15);
            printf("\n\n");
            system("pause");
            break;
        default:
            textcolor(12);
            mostrarDialogo("Opcion invalida");
            textcolor(15);
            printf("\n\n");
            system("pause");
        }
    }
}

void subMenuAdminModificarUsuario()
{
	FILE *archi;
	archi = fopen(archivoUsuarios, "r+b");
	char salir = 'n';
	int usuarioBuscado;
	Usuario usuario;
	if (archi != NULL)
	{
  	  do
  	  {
  		  mostrarUsuarios();
  		  printf("\n\nIngrese la ID del Usuario a modificar => ");
  		  scanf("%d", &usuarioBuscado);
  		  int encontrado = 0;
  		  while (fread(&usuario, sizeof(Usuario), 1, archi) > 0)
  		  {
  			  if (usuarioBuscado == usuario.id)
  			  {
      			  encontrado = 1;
      			  usuario = modificarUsuario(usuario); /// <------ MENU MODIFICAR usuario
      			  fseek(archi, sizeof(Usuario)*(-1), SEEK_CUR);
      			  fwrite(&usuario, sizeof(Usuario), 1, archi);
      			  fflush(archi);
      			  system("cls");
      			  textcolor(10);
      			  mostrarDialogo("Usuario Modificado");
      			  textcolor(15);
      			  mostrarUsuario(usuario);
  			  }
  		  }

  		  if (!encontrado)
  		  {
  			  textcolor(12);
  			  mostrarDialogo("No se encontro ningun Usuario con ese ID");
  			  textcolor(15);
  		  }

  		  printf("\n\nDesea salir de la modificacion de usuarios? (S = Salir) => ");
  		  fflush(stdin);
  		  scanf("%c", &salir);
  		  salir = tolower(salir); // lo pasa a minusculas
  		  rewind(archi);
  	  }
  	  while (salir != 's');
  	  fclose(archi);
	}
	else
	{
  	  textcolor(12);
  	  mostrarDialogo("No se ha podido abrir el archivo de Usuarios");
  	  textcolor(15);
  	  printf("\n\n");
  	  system("pause");
	}

}

/// =============================================================== Extras ==============================================================================

// 0=BLACK / 1=BLUE / 2=GREEN / 3=CYAN / 4=RED / 5=MAGENTA / 6=BROWN / 7=LIGHTGRAY / 8=DARKGRAY / 9=LIGHTBLUE
// 10=LIGHTGREEN / 11=LIGHTCYAN / 12=LIGHTRED / 13=LIGHTMAGENTA / 14=YELLOW / 15=WHITE
// Funcion para cambiar los colores del texto por consola
void textcolor (int color)
{
    static int __BACKGROUND;
    HANDLE h = GetStdHandle ( STD_OUTPUT_HANDLE );
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    GetConsoleScreenBufferInfo(h, &csbiInfo);
    SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE),
                             color + (__BACKGROUND << 4));
}

// Función para mostrar textos dentro de un marco
void mostrarDialogo(char texto[])
{
    char aux[100];
    // ----------------------
    strcpy(aux, texto);
    strcat(aux, " |");
    char dialogo[100] = "| ";
    strcat(dialogo, aux);
    // ----------------------
    int longitud = strlen(dialogo);
    // ----------------------
    printf("\n");
    for (int i=0; i<longitud; i++)
    {
        printf("-");
    }
    printf("\n");
    puts(dialogo);
    for (int i=0; i<longitud; i++)
    {
        printf("-");
    }
    printf("\n");
}

// Funcion para mostrar una barra de espera
void mostrarBarraEspera()
{
    system("cls");
    mostrarDialogo("  |   |   |   |   |  ");
    sleep(1);
    system("cls");
    mostrarDialogo("* |   |   |   |   |  ");
    sleep(1);
    system("cls");
    mostrarDialogo("* | * |   |   |   |  ");
    sleep(1);
    system("cls");
    mostrarDialogo("* | * | * |   |   |  ");
    sleep(1);
    system("cls");
    mostrarDialogo("* | * | * | * |   |  ");
    sleep(1);
    system("cls");
    mostrarDialogo("* | * | * | * | * |  ");
    sleep(1);
    system("cls");
    mostrarDialogo("* | * | * | * | * | *");
    sleep(1);
}

void mostrarAsciiHangar()
{
    textcolor(9);
    printf("\n%c%c%c%c%c%c%c%c%c%c%c\n", 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 219);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 176, 176, 176, 176, 176, 176, 176, 176, 220, 220, 219, 220, 220);
    printf("%c%c%c%c",176, 176, 176, 176);
    textcolor(6);
    printf("%c%c%c",223, 223, 223);
    textcolor(9);
    printf("%c%c%c%c%c%c%c", 219, 219, 223, 223, 223, 219, 219);
    textcolor(6);
    printf("%c%c%c\n", 223, 223, 223);
    textcolor(6);
    printf("%c%c%c%c%c%c%c", 220, 220, 220, 220, 220, 220, 220);
    textcolor(9);
    printf("%c%c%c%c%c%c%c", 219, 219, 219, 219, 219, 219, 219);
    textcolor(6);
    printf("%c%c%c%c%c%c%c\n",220, 220, 220, 220, 220, 220, 220);
    textcolor(9);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 176, 176, 219, 220, 219, 176, 176, 223, 219, 219, 220, 219, 219, 223, 176, 176, 219, 220, 219, 176);
    textcolor(15);
}

/*
░░░░░░░░▒▒▒▒▒░░░░░▄█░░
▒▒▒▒─▄▄▄▄▄▄▄▄▄▄▄▄▄██░░
▒▒─▄████████████████▄░
░▄█▄██▄█▄█▄█▄█▄██▀▀▒░░
▒░▀████████████▀▒▒▒░░░
░▒▒▒▒░░▀▀████▄─░░░▒▒▒░
▒▒▒▒▒▒▒▒▒▒▒▀▀██▄▒▒▒▒▒▒
*/
//░ 176 ▒ 177 ▄ 220 ▀ 223 █ 219 ─ 196

void mostrarAsciiAvion() {
    textcolor(3);
    printf("\n");
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 176, 176, 176, 176, 176, 176, 176, 176, 177, 177, 177, 177, 177, 176, 176, 176, 176, 176, 176, 176, 176, 176, 176, 220, 219, 219, 176);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 177, 177, 177, 177, 176, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 220, 219, 219, 219, 219, 176);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 177, 177, 177, 220, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 223, 176);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 176, 220, 219, 220, 219, 219, 220, 219, 220, 219, 220, 219, 220, 219, 220, 219, 219, 219, 219, 219, 219, 219, 223, 223, 177, 176, 176);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 177, 176, 223, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 223, 223, 223, 223, 223, 223, 177, 177, 177, 176, 176, 176);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 176, 177, 177, 177, 177, 176, 176, 223, 223, 219, 219, 219, 219, 219, 196, 176, 176, 176, 176, 176, 176, 176, 176, 177, 177, 177, 176);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 177, 177, 177, 177, 177, 177, 177, 177, 177, 177, 177, 223, 223, 219, 219, 220, 177, 177, 177, 177, 177, 177, 177, 177, 177, 177, 177);
    textcolor(15);
}

void mostrarAsciiSistemaHangar()
{
    printf("\n"
           "    ____  _     _                         _   _                               \n"
           "   / ___|(_)___| |_ ___ _ __ ___   __ _  | | | | __ _ _ __   __ _  __ _ _ __  \n"
           "   \\___ \\| / __| __/ _ \\ '_ ` _ \\ / _` | | |_| |/ _` | '_ \\ / _` |/ _` | '__| \n"
           "    ___) | \\__ \\ ||  __/ | | | | | (_| | |  _  | (_| | | | | (_| | (_| | |    \n"
           "   |____/|_|___/\\__\\___|_| |_| |_|\\__,_| |_| |_|\\__,_|_| |_|\\__, |\\__,_|_|    \n"
           "                                                            |___/             \n");
}


void mostrarAsciiCuartel()
{
    // Colores
    int border_color = 10;
    int text_color = 14;
    int background_color = 7;

    // Dibujo de la chapa militar
    textcolor(border_color);
    printf("\n");
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
           220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220,220);
    //printf("%c                                     %c\n", 219, 219);
    printf("%c   ", 219);
    textcolor(text_color);
    printf("%c%c   %c%c        %c%c    %c%c    %c%c", 178, 178, 178, 178, 178, 178, 178, 178, 178, 178);
    textcolor(border_color);
    printf("    %c\n", 219);
    printf("%c    ", 219);
    textcolor(text_color);
    printf("%c%c   %c%c        %c%c    %c%c    %c%c", 178, 178, 178, 178, 178, 178, 178, 178, 178, 178);
    textcolor(border_color);
    printf("   %c\n", 219);
    printf("%c     ", 219);
    textcolor(text_color);
    printf("%c%c   %c%c", 178, 178, 178, 178);
    textcolor(background_color);
    printf("   %c    ", 158);
    textcolor(text_color);
    printf("%c%c    %c%c    %c%c", 178, 178, 178, 178, 178, 178);
    textcolor(border_color);
    printf("  %c\n", 219);
    printf("%c    ", 219);
    textcolor(text_color);
    printf("%c%c   %c%c        %c%c    %c%c    %c%c", 178, 178, 178, 178, 178, 178, 178, 178, 178, 178);
    textcolor(border_color);
    printf("   %c\n", 219);
    printf("%c   ", 219);
    textcolor(text_color);
    printf("%c%c   %c%c        %c%c    %c%c    %c%c", 178, 178, 178, 178, 178, 178, 178, 178, 178, 178);
    textcolor(border_color);
    printf("    %c\n", 219);
    //printf("%c                                     %c\n", 219, 219);
    printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
           223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223, 223 ,223,223);
    textcolor(15); // Restablecer color predeterminado
}

// Funcion para obtener el año actual
int anioActual()
{
    // Obtener la hora actual
    time_t tiempo_actual;
    time(&tiempo_actual);
    // Convertir a estructura tm
    struct tm *tiempo_local = localtime(&tiempo_actual);
    // Obtener el anio
    int anio_actual = tiempo_local->tm_year + 1900;  // tm_year es el numero de anios desde 1900
    return anio_actual;
}

// Funcion para obtener la hora actual
Hora horaActual()
{
    time_t tiempo_actual;
    time(&tiempo_actual);
    struct tm *tiempo_local = localtime(&tiempo_actual);

    Hora hora_actual;
    hora_actual.hora = tiempo_local->tm_hour;
    hora_actual.minuto = tiempo_local->tm_min;
    hora_actual.segundo = tiempo_local->tm_sec;

    return hora_actual;
}

// Funcion para obtener la fecha actual
Fecha fechaActual()
{
    time_t tiempo_actual;
    time(&tiempo_actual);
    struct tm *tiempo_local = localtime(&tiempo_actual);

    Fecha fecha_actual;
    fecha_actual.dia = tiempo_local->tm_mday;
    fecha_actual.mes = tiempo_local->tm_mon + 1; // tm_mon es el número de meses desde enero (0-11)
    fecha_actual.anio = tiempo_local->tm_year + 1900; // tm_year es el número de años desde 1900

    return fecha_actual;
}

// Función para obtener la cantidad de registros dentro de un archivo
int cantidadRegistros(char archivo[], int unidad)
{
    int cantidad = 0;
    FILE* archi;
    archi = fopen(archivo,"rb");
    if (archi != NULL)
    {
        fseek(archi,0,2);
        cantidad = ftell(archi);
        cantidad /= unidad;
        fclose(archi);
    }
    return cantidad;
}

///Obtiene la cantidad de aviones disponibles para Asignar al Hangar
int cantidadAvionesDisponiblesHangar()
{
    FILE *archi;
    archi = fopen(archivoAviones,"rb");
    int i=0;
    Avion avion;
    if(archi != NULL)
    {
        while(fread(&avion, sizeof(Avion), 1, archi) > 0)
        {
            if(avion.alta > 0 && avion.idPiloto == -1 && avion.combustibleActual >= 300 && validarExistenciaAvionDentroHangar(avion.id) < 1)
            {
                i++;
            }
        }
        fclose(archi);
    }
    return i;
}

///Obtiene la cantidad de Pilotos disponibles para Asignar al Hangar
int cantidadPilotosDisponiblesHangar()
{
    FILE *archi;
    archi = fopen(archivoPilotos,"rb");
    int i=0;
    Piloto piloto;
    if(archi != NULL)
    {
        while(fread(&piloto, sizeof(Piloto), 1, archi) > 0)
        {
            if(piloto.alta > 0 && validarExistenciaPilotoDentroHangar(piloto.id) < 1)
            {
                i++;
            }
        }
        fclose(archi);
    }
    return i;
}

// Función para asignar el rango de un piloto segun sus horas de vuelo
void ascenderPiloto (Piloto *piloto)
{
    if (piloto->horasVuelo >= 500)
    {
        strcpy(piloto->rango,"Comandante");
    }
    else if (piloto->horasVuelo >= 400)
    {
        strcpy(piloto->rango,"Segundo Comandante");
    }
    else if (piloto->horasVuelo >= 300)
    {
        strcpy(piloto->rango,"Tercer Comandante");
    }
    else if (piloto->horasVuelo >= 200)
    {
        strcpy(piloto->rango,"Capitan");
    }
    else if (piloto->horasVuelo >= 150)
    {
        strcpy(piloto->rango,"Segundo Capitan");
    }
    else if (piloto->horasVuelo >= 100)
    {
        strcpy(piloto->rango,"Tercer Capitan");
    }
    else if (piloto->horasVuelo >= 80)
    {
        strcpy(piloto->rango,"Primer Oficial");
    }
    else if (piloto->horasVuelo >= 50)
    {
        strcpy(piloto->rango,"Segundo Oficial");
    }
    else if (piloto->horasVuelo >= 30)
    {
        strcpy(piloto->rango,"Piloto Instrumental");
    }
    else if (piloto->horasVuelo < 30)
    {
        strcpy(piloto->rango,"Alumno Piloto");
    }
}

// Convierte una cadena completamente en una cadena de letras minusculas
void convertirMinusculas(char *cadena)
{
    for (int i = 0; cadena[i] != '\0'; i++)
    {
        cadena[i] = tolower(cadena[i]);
    }
}

/// ---------------------- Funciones de ordenamiento ----------------------

//----------------Pilotos-------------------

int pasarPilotosDisponiblesParaHangarDeArchivoArreglo(Piloto arreglo[], int dimension)
{
    FILE * archi;
    archi = fopen(archivoPilotos, "rb");
    Piloto p;
    int i = 0;
    if(archi != NULL)
    {
        while(fread(&p,sizeof(Piloto),1,archi) > 0)
        {
            if (i <= dimension) {
                if (p.alta > 0 && validarExistenciaPilotoDentroHangar(p.id) != 1)   // Solo si el piloto esta de alta (disponible) se agrega al arreglo
                {
                    arreglo[i] = p;
                    i++;
                }
            }
        }
        fclose(archi);
    }
    return i;
}

int pasarPilotosDeArchivoArreglo(Piloto pilotos[], int dimension)
{
    int i = 0;
    FILE * archi;
    archi = fopen(archivoPilotos, "rb");
    Piloto p;
    if(archi != NULL)
    {
        while(fread(&p,sizeof(Piloto),1,archi) > 0)
        {
            if (i < dimension) {
                pilotos[i] = p;
                i++;
            }
        }
        fclose(archi);
    }
    return i; // retorna los validos
}

int buscarPosMenorPilotoNombre (Piloto pilotos[], int pos, int validos)
{
    int posmenor = pos;
    char palabraMenor[20];
    strcpy (palabraMenor, pilotos[pos].persona.nombreApellido);
    int i;

    for (i=pos+1; i<validos; i++)
    {
        if ( strcmpi (palabraMenor, pilotos[i].persona.nombreApellido) >0 )
        {
            strcpy (palabraMenor, pilotos[i].persona.nombreApellido);
            posmenor=i;
        }
    }
    return posmenor;
}

void intercambioSeleccionPilotos(Piloto pilotos[], int posMen, int i)
{
    Piloto aux;
    aux = pilotos[posMen];
    pilotos[posMen] = pilotos[i];
    pilotos[i] = aux;
}

void ordenarPorSeleccionArregloPilotos (int validos,  Piloto pilotos[])
{
    int posMen;
    for (int i=0; i<validos; i++)
    {
        posMen = buscarPosMenorPilotoNombre (pilotos, i, validos);
        intercambioSeleccionPilotos (pilotos, posMen, i);
    }
}

void mostrarArregloPilotos(Piloto pilotos[], int validos)
{
    for(int i = 0; i< validos; i++)
    {
        mostrarPiloto(pilotos[i]);
    }
    printf("\n\n");
    system("pause");
}

//----------------Aviones-------------------

int pasarHangarDeArchivoArreglo (Avion arreglo[], int dimension)
{
    FILE* archi;
    archi = fopen(archivoHangar, "rb");
    Avion a;
    int i = 0;
    if(archi != NULL)
    {
        while(fread(&a, sizeof(Avion), 1, archi) > 0)
        {
            if (i < dimension) {
                arreglo[i] = a;
                i ++;
            }
        }
        fclose(archi);
    }
    return i; // validos
}

int pasarAvionesDisponiblesParaHangarDeArchivoArreglo(Avion arreglo[], int dimension)
{
    FILE * archi;
    archi = fopen(archivoAviones, "rb");
    Avion a;
    int i = 0;
    if(archi != NULL)
    {
        while(fread(&a,sizeof(Avion),1,archi) > 0)
        {
            if (a.alta > 0 && a.idPiloto == -1 && a.combustibleActual >= 300 && validarExistenciaAvionDentroHangar(a.id) < 1)   // Solo si el avion esta disponible para asignar al Hangar se agrega al arreglo
            {
                if (i < dimension)
                {
                    arreglo[i] = a;
                    i++;
                }
            }
        }
        fclose(archi);
    }
    return i;
}

int pasarAvionesDeArchivoArreglo (Avion arreglo[], int dimension)
{
    FILE* archi;
    archi = fopen(archivoAviones, "rb");
    Avion a;
    int i = 0;
    if(archi != NULL)
    {
        while(fread(&a, sizeof(Avion), 1, archi) > 0 && i < dimension)
        {
            arreglo[i] = a;
            i ++;
        }
        fclose(archi);
    }
    return i; // validos
}

int pasarAvionesAltaDeArchivoArreglo (Avion arreglo[], int dimension)
{
    FILE* archi;
    archi = fopen(archivoAviones, "rb");
    Avion a;
    int i = 0;
    if(archi != NULL)
    {
        while(fread(&a, sizeof(Avion), 1, archi) > 0)
        {
            if (a.alta > 0 && i < dimension)
            {
                arreglo[i] = a;
                i ++;
            }
        }
        fclose(archi);
    }
    return i; // validos
}

void insertarOrdenarAviones(Avion aviones[], int i, int u)
{
    Avion aux;
    aux = aviones[u];
    aviones[u] = aviones[i];
    aviones[i] = aux;
}

void ordenarPorInsercionAvionesConbustibleActual(Avion arreglo[], int validos)
{
    int i = 0, u = 0;
    while(u < validos)
    {
        i = u + 1;
        while(i < validos)
        {
            if(arreglo[i].combustibleActual < arreglo[u].combustibleActual)
            {
                insertarOrdenarAviones(arreglo, i, u);
            }
            i++;
        }
        u++;
    }
}

void mostrarArregloAviones(Avion arreglo[], int validos)
{
    for(int i = 0; i< validos; i++)
    {
        mostrarAvion(arreglo[i]);
    }
    printf("\n\n");
    system("pause");
}

void mostrarArregloHangar(Avion arreglo[], int validos)
{
    for(int i=0; i<validos; i++)
    {
        mostrarAvionHangar(arreglo[i]);
    }
    printf("\n\n");
    system("pause");
}

int buscarPosMenorAvionNombre (Avion aviones[], int pos, int validos)
{
    int posmenor = pos;
    char palabraMenor[20];
    strcpy (palabraMenor, aviones[pos].nombre);
    int i;

    for (i=pos+1; i<validos; i++)
    {
        if ( strcmpi (palabraMenor, aviones[i].nombre) >0 )
        {
            strcpy (palabraMenor, aviones[i].nombre);
            posmenor=i;
        }
    }
    return posmenor;
}

void intercambioSeleccionAvionesNombres(Avion aviones[], int posMen, int i)
{
    Avion aux;
    aux = aviones[posMen];
    aviones[posMen] = aviones[i];
    aviones[i] = aux;
}

void ordenarPorSeleccionArregloAvionesNombre (int validos,  Avion aviones[])
{
    int posMen;
    for (int i=0; i<validos; i++)
    {
        posMen = buscarPosMenorAvionNombre(aviones, i, validos);
        intercambioSeleccionAvionesNombres(aviones, posMen, i);
    }
}

/// ---------------------- Validaciones --------------------------

// Valida la longitud de un texto, su minimo y su maximo, 0 exito, 1 error
int validarLongitud(char palabra[], int minimo, int maximo)
{
    int error = 0;
    if (strlen(palabra) < minimo)
    {
        textcolor(12);
        mostrarDialogo("El dato no cumple con la minima cantidad de caracteres");
        textcolor(15);
        error = 1;
    }
    else if (strlen(palabra) > maximo)
    {
        textcolor(12);
        mostrarDialogo("El dato supera la capacidad maxima de caracteres");
        textcolor(15);
        error = 1;
    }
    else
    {
        error = 0;
    }
    return error;
}

// Valida los DNI de persona y verifica que no haya repetidos, 0 exito, 1 dni repetido
int validarDni(char dni[])
{
    FILE* archi;
    archi = fopen(archivoPersonas,"rb");
    // ----------------------------------------
    int existe = 0;
    Persona p;
    while (existe < 1 && fread(&p,sizeof(Persona),1,archi) > 0)
    {
        if (strcmpi(p.dni,dni) == 0)
        {
            textcolor(12);
            mostrarDialogo("El DNI ingresado ya fue registrado");
            textcolor(15);
            existe ++;
        }
    }
    return existe; // 0 dni valido / 1 dni repetido
}

// Valida y verifica que no haya un admin repetido, 0 exito, 1 admin repetido
int validarAdmin(char user[])
{
    FILE* archi;
    archi = fopen(archivoUsuarios,"rb");
    // ----------------------------------------
    int existe = 0;
    Usuario u;
    while (existe < 1 && fread(&u,sizeof(Usuario),1,archi) > 0)
    {
        if (strcmpi(u.nombreUsuario,user) == 0)
        {
            textcolor(12);
            mostrarDialogo("El Administrador ya esta registrado");
            textcolor(15);
            existe ++;
            system("pause");
        }
    }
    return existe; // 0 usuario valido, 1 usuario repetido
}

// Valida los nombres de usuario y verifica que no haya repetidos, 0 exito, 1 nombre usuario repetido
int validarUsuario(char usuario[])
{
    FILE* archi;
    archi = fopen(archivoUsuarios,"rb");
    // ----------------------------------------
    int existe = 0;
    Usuario u;
    while (existe < 1 && fread(&u,sizeof(Usuario),1,archi) > 0)
    {
        if (strcmpi(u.nombreUsuario,usuario) == 0)
        {
            textcolor(12);
            mostrarDialogo("El nombre de usuario ingresado ya fue registrado");
            textcolor(15);
            existe ++;
        }
    }
    return existe; // 0 usuario valido / 1 usuario repetido
}

// Valida los numeros de licencia de un piloto y verifica que no haya repetidos, 0 exito, 1 licencia repetida
int validarNumLicencia(char licencia[])
{
    FILE* archi;
    archi = fopen(archivoPilotos,"rb");
    // ----------------------------------------
    int existe = 0;
    Piloto p;
    while (existe < 1 && fread(&p,sizeof(Piloto),1,archi) > 0)
    {
        if (strcmpi(p.numLicencia,licencia) == 0)
        {
            textcolor(12);
            mostrarDialogo("El numero de licencia ingresado ya fue registrado");
            textcolor(15);
            existe ++;
        }
    }
    return existe; // 0 licencia valida / 1 licencia repetida
}

// Valida la existencia de un avion dentro del Hangar, 0 avion disponible , 1 avion ya dentro del hangar
int validarExistenciaAvionDentroHangar(int idAvion)
{
    FILE* archi;
    archi = fopen(archivoHangar,"rb");
    // ----------------------------------------
    int existe = 0;
    Avion a;
    while (existe < 1 && fread(&a,sizeof(Avion),1,archi) > 0)
    {
        if (a.id == idAvion)
        {
            existe ++;
        }
    }
    return existe; // 0 avion disponible para asignar / 1 avion ya dentro del hangar
}

// Valida la existencia de un piloto dentro del Hangar, 0 piloto disponible , 1 piloto ya dentro del hangar
int validarExistenciaPilotoDentroHangar(int idPiloto)
{
    FILE* archi;
    archi = fopen(archivoHangar,"rb");
    // ----------------------------------------
    int existe = 0;
    Avion a;
    while (existe < 1 && fread(&a,sizeof(Avion),1,archi) > 0)
    {
        if (a.idPiloto == idPiloto)
        {
            existe ++;
        }
    }
    return existe; // 0 piloto disponible para asignar / 1 piloto ya dentro del hangar
}

// 0 todos los aviones estan listos / 1 uno o mas aviones requieren algo
int validarHangarListoParaVolar() {
    FILE* archi;
    archi = fopen(archivoHangar,"rb");
    // ----------------------------------------
    int faltante = 0;
    Avion a;
    while (faltante < 1 && fread(&a,sizeof(Avion),1,archi) > 0)
    {
        if (a.combustibleActual < 300) /// AQUI SE COMPRUEBA QUE EL AVION TENGA SUFICIENTE COMBUSTIBLE
            ///DEBERIA VALIDARSE EL ALTA DEL PILOTO Y DEL AVION EN UN FUTURO
        {
            faltante ++;
        }
    }
    return faltante; // 0 todos los aviones estan listos / 1 uno o mas aviones requieren algo
}

// 1 TODOS LOS AVIONES terminaron su vuelo / 0 NO TERMINARON vuelo
int validarTodoHangarFueraDelRadar(UbicacionAvion aviones[], int numAviones)
{
    int todosFuera = 0; // bandera binaria
    int cantidadFuera = 0; // contador
    for (int i=0; i<numAviones; i++)
    {
        if (strcmpi(aviones[i].estado,"Viaje Finalizado") == 0 || strcmpi(aviones[i].estado,"Colisionado") == 0 )
        {
            cantidadFuera ++;
        }
    }
    if (cantidadFuera == numAviones)
    {
        todosFuera = 1;
    }
    return todosFuera;
}

/// ____________________________________________________________ (CREATE) __________________________________________________________________________________

Registro crearRegistro(int idUsuario)
{
    Registro registro;
    registro.id = cantidadRegistros(archivoRegistros,sizeof(Registro));
    registro.idUsuario = idUsuario;
    registro.fecha = fechaActual();
    registro.hora = horaActual();
    return registro;
}

void registrarRegistro(Registro registro)
{
    FILE *archi;
    archi = fopen(archivoRegistros,"ab");
    if (archi != NULL)
    {
        fwrite(&registro,sizeof(Registro),1,archi);
        fclose(archi);
    }
    else
    {
        textcolor(12);
        mostrarDialogo("Error: No se ha podido abrir el archivo de Registros");
        textcolor(15);
        printf("\n\n");
        system("pause");
    }
}

Usuario crearAdmin(char user[], char pass[])
{
    Persona p;
    strcpy(p.dni,"0");
    strcpy(p.nombreApellido,"ADMIN");
    p.genero = 'O';
    p.anioNacimiento = 1968;
    Usuario u;
    u.id = cantidadRegistros(archivoUsuarios,sizeof(Usuario));
    strcpy(u.nombreUsuario,user);
    strcpy(u.contrasenia,pass);
    u.persona = p;
    u.rol = 2;
    u.alta = 1;
    return u;
}

/// Funcion PRINCIPAL para la creacion del ADMIN
void registrarAdmin()
{
    FILE* archi;
    archi = fopen(archivoUsuarios,"ab");
    if (archi != NULL)
    {
        Usuario admin;
        char user[30] = "admin"; /// Establece que el nombre de usuario sea:  admin
        char pass[30] = "admin"; /// Establece que la contraseña sea:   	  admin
        int existe = validarAdmin(user);
        if (existe != 1)
        {
            // Si no existe un user 'admin' creado, crea uno nuevo
            admin = crearAdmin(user,pass);
            fwrite(&admin,sizeof(Usuario),1,archi);
        }
        fclose(archi);
    }
}

Persona crearPersona()
{
    char aux[100], dni[10], nombreApellido[30], genero = 'n';
    int anioNacimiento = 0;
    textcolor(9);
    mostrarDialogo("Nueva Persona");
    textcolor(15);
    do
    {
        printf("\nIngrese DNI (1-9 caracteres) => ");
        fflush(stdin);
        gets(aux);
    }
    while (validarLongitud(aux,1,9) > 0 || validarDni(aux) > 0);
    strcpy(dni,aux);
    do
    {
        printf("\nIngrese nombre y apellido (5-29 caracteres) => ");
        fflush(stdin);
        gets(aux);
    }
    while (validarLongitud(aux,5,29) > 0);
    strcpy(nombreApellido,aux);
    do
    {
        printf("\nIngrese genero (F/M/O) => ");
        fflush(stdin);
        scanf("%c",&genero);
        genero = tolower(genero);
        if (genero != 'f' && genero != 'm' && genero != 'o')
        {
            textcolor(12);
            printf("\nOpcion no valida\n");
            textcolor(15);
        }
    }
    while (genero != 'f' && genero != 'm' && genero != 'o');
    do
    {
        printf("\nIngrese a%co de nacimiento => ",164);
        scanf("%d",&anioNacimiento);
        if (anioNacimiento < 1900 || anioNacimiento > (anioActual() - 18))
        {
            textcolor(12);
            printf("\nA%co invalido\n",164);
            textcolor(15);
        }
    }
    while (anioNacimiento < 1900 || anioNacimiento > (anioActual() - 18));
    // -------------- Encapsulacion -------------
    Persona persona;
    strcpy(persona.dni,dni);
    strcpy(persona.nombreApellido,nombreApellido);
    persona.genero = genero;
    persona.anioNacimiento = anioNacimiento;
    // ------------------------------------------
    return persona;
}

void registrarPersona(Persona persona)
{
    FILE* archi;
    archi = fopen(archivoPersonas,"ab");
    if (archi != NULL)
    {
        fwrite(&persona,sizeof(Persona),1,archi);
        fclose(archi);
    }
    else
    {
        textcolor(12);
        mostrarDialogo("Error: No se ha podido abrir el archivo de Personas");
        textcolor(15);
    }
}

Usuario crearUsuario(Persona persona)
{
    char aux[100], nombreUsuario[30], contrasenia[30];
    textcolor(9);
    mostrarDialogo("Nuevo Usuario");
    textcolor(15);
    do
    {
        printf("\nIngrese nombre de usuario (3-29 caracteres) => ");
        fflush(stdin);
        gets(aux);
    }
    while (validarLongitud(aux,3,29) > 0 || validarUsuario(aux) > 0);
    strcpy(nombreUsuario,aux);
    int bandera = 0;
    do
    {
        do
        {
            printf("\nIngrese contrase%ca (3-29 caracteres) => ",164);
            fflush(stdin);
            gets(contrasenia);
        }
        while (validarLongitud(contrasenia,3,29) > 0);
        printf("\nConfirme contrase%ca => ",164);
        gets(aux);
        if (strcmp(contrasenia,aux) == 0)
        {
            bandera = 1; // contraseña correcta
        }
        else
        {
            textcolor(12);
            printf("\nLas contrasenias no coinciden...\n");
            textcolor(15);
        }
    }
    while(bandera < 1);

    // --------------------------------------
    Usuario usuario;
    usuario.id = cantidadRegistros(archivoUsuarios,sizeof(Usuario));
    strcpy(usuario.nombreUsuario,nombreUsuario);
    strcpy(usuario.contrasenia,contrasenia);
    usuario.persona = persona;
    usuario.rol = 0; // 0 invitado 1 operador 2 admin
    usuario.alta = 1; // 1 alta 0 baja
    // --------------------------------------
    return usuario;
}

void registrarUsuario(Usuario usuario)
{
    FILE* archi;
    archi = fopen(archivoUsuarios,"ab");
    if (archi != NULL)
    {
        fwrite(&usuario,sizeof(Usuario),1,archi);
        fclose(archi);
        // --------------------------------------
        Registro registro = crearRegistro(usuario.id);
        registrarRegistro(registro); // Registra la hora y fecha en la que se hizo el registro
    }
    else
    {
        textcolor(12);
        mostrarDialogo("Error: No se ha podido abrir el archivo de Usuarios");
        textcolor(15);
    }
}

Piloto crearPiloto(Persona persona)
{
    char aux[100], numLicencia[15];
    int horasVuelo = -1;
    textcolor(9);
    mostrarDialogo("Nuevo Piloto");
    textcolor(15);
    do
    {
        printf("\nIngrese Numero de Licencia (3-14 caracteres) => ");
        fflush(stdin);
        gets(aux);
    }
    while (validarLongitud(aux,3,14) > 0 || validarNumLicencia(aux) > 0);
    strcpy(numLicencia,aux);
    do
    {
        printf("\nIngrese Horas de Vuelo realizadas => ");
        scanf("%d",&horasVuelo);
    }
    while (horasVuelo < 0);
    // --------------------------------------
    Piloto piloto;
    piloto.id = cantidadRegistros(archivoPilotos,sizeof(Piloto));
    strcpy(piloto.numLicencia,numLicencia);
    piloto.horasVuelo = horasVuelo;
    ascenderPiloto(&piloto); // asigna rango al piloto
    piloto.persona = persona;
    piloto.alta = 1; // 1 alta 0 baja
    // --------------------------------------
    return piloto;
}

void registrarPiloto(Piloto piloto)
{
    FILE* archi;
    archi = fopen(archivoPilotos,"ab");
    if (archi != NULL)
    {
        fwrite(&piloto,sizeof(Piloto),1,archi);
        fclose(archi);
    }
    else
    {
        textcolor(12);
        mostrarDialogo("Error: No se ha podido abrir el archivo de Pilotos");
        textcolor(15);
    }
}

Avion crearAvion()
{
    char aux[100], nombre[30], modelo[30], aerolinea[30];
    int numeracion, vuelosRealizados, capPasajeros, combustibleMax;
    textcolor(9);
    mostrarDialogo("Nuevo Avion");
    textcolor(15);
    do
    {
        printf("\nIngrese el nombre Alias del avion (3-29 caracteres) => ");
        fflush(stdin);
        gets(aux);
    }
    while (validarLongitud(aux,3,29) > 0);
    strcpy(nombre,aux);
    do
    {
        printf("\nIngrese la numeracion Alias del avion (0-999)=> ");
        fflush(stdin);
        scanf("%d",&numeracion);
    }
    while(numeracion > 999 || numeracion < 0);
    do
    {
        printf("\nIngrese modelo del avion (3-29 caracteres) => ");
        fflush(stdin);
        gets(aux);
    }
    while(validarLongitud(aux,3,29) > 0);
    strcpy(modelo,aux);
    do
    {
        printf("\nIngrese aerolinea del avion (3-29 caracteres) => ");
        fflush(stdin);
        gets(aux);
    }
    while(validarLongitud(aux,3,29) > 0);
    strcpy(aerolinea,aux);
    do
    {
        printf("\nIngrese la capadidad maxima de pasajeros (max. 200) => ");
        scanf("%d",&capPasajeros);
    }
    while (capPasajeros < 0 || capPasajeros > 200);
    do
    {
        printf("\nIngrese la cantidad de vuelos realizados por el avion => ");
        scanf("%d",&vuelosRealizados);
    }
    while (vuelosRealizados < 0);
    // --------------------------------------
    Avion avion;
    avion.id = cantidadRegistros(archivoAviones,sizeof(Avion));
    strcpy(avion.nombre,nombre);
    avion.numeracion = numeracion;
    strcpy(avion.modelo,modelo);
    strcpy(avion.aerolinea,aerolinea);
    avion.capacidadPasajeros = capPasajeros;
    avion.vuelosRealizados = vuelosRealizados;
    avion.combustibleMaximo = 1500;
    avion.combustibleActual = 1500;
    avion.idPiloto = -1; // -1 Sin Piloto Asignado
    avion.alta = 1; // 1 alta 0 baja
    // --------------------------------------
    return avion;
}

void registrarAvion(Avion avion)
{
    FILE* archi;
    archi = fopen(archivoAviones,"ab");
    if (archi != NULL)
    {
        fwrite(&avion,sizeof(Avion),1,archi);
        fclose(archi);
    }
    else
    {
        textcolor(12);
        mostrarDialogo("Error: No se ha podido abrir el archivo de Aviones");
        textcolor(15);
    }
}

/// ___________________________________ (READ) ___________________________________

// Retorna un Registro segun la ID, si no lo encuentra retorna un Registro con ID -1
Registro getRegistro(int id)
{
    FILE *archi;
    archi = fopen(archivoRegistros,"rb");
    Registro r;
    if (archi != NULL)
    {
        int encontrado = 0;
        while (encontrado < 1 && fread(&r,sizeof(Registro),1,archi) > 0)
        {
            if (r.id == id)
            {
                encontrado ++;
            }
        }
        if (encontrado < 1) {
            r.id = -1;
        }
        fclose(archi);
    }
    return r; // SI NO SE PUDO ABRIR EL ARCHIVO, id = -1;
}

// Retorna un Usuario segun la ID, si no lo encuentra retorna un Usuario con ID -1
Usuario getUsuario(int id)
{
    FILE *archi;
    archi = fopen(archivoUsuarios,"rb");
    Usuario u;
    if (archi != NULL)
    {
        int encontrado = 0;
        while (encontrado < 1 && fread(&u,sizeof(Usuario),1,archi) > 0)
        {
            if (u.id == id)
            {
                encontrado ++;
            }
        }
        if (encontrado < 1) {
            u.id = -1;
        }
        fclose(archi);
    }
    return u; // SI NO SE ENCUENTRA id = -1;
}

// Retorna un Piloto segun la ID, si no lo encuentra retorna un Piloto con ID -1
Piloto getPiloto(int id)
{
    FILE *archi;
    archi = fopen(archivoPilotos,"rb");
    Piloto p;
    if (archi != NULL)
    {
        int encontrado = 0;
        while (encontrado < 1 && fread(&p,sizeof(Piloto),1,archi) > 0)
        {
            if (p.id == id)
            {
                encontrado ++;
            }
        }
        if (encontrado < 1) {
            p.id = -1;
        }
        fclose(archi);
    }
    return p; // SI NO SE ENCONTRO id = -1;
}

// Retorna un Piloto segun la ID, si no lo encuentra retorna un Piloto con ID -1
Piloto getPilotoHangar(int id)
{
    FILE *archi;
    archi = fopen(archivoHangar,"rb");
    Avion a;
    Piloto p;
    if (archi != NULL)
    {
        int encontrado = 0;
        while (encontrado < 1 && fread(&a,sizeof(Avion),1,archi) > 0)
        {
            if (a.idPiloto == id)
            {
                encontrado ++;
                p = getPiloto(a.idPiloto);
            }
        }
        if (encontrado < 1) {
            p.id = -1;
        }
        fclose(archi);
    }
    return p; // SI NO SE ENCONTRO, id = -1;
}

// Retorna un Avion segun la ID, si no lo encuentra retorna un Avion con ID -1
Avion getAvion(int id)
{
    FILE *archi;
    archi = fopen(archivoAviones,"rb");
    Avion a;
    if (archi != NULL)
    {
        int encontrado = 0;
        while (encontrado < 1 && fread(&a,sizeof(Avion),1,archi) > 0)
        {
            if (a.id == id)
            {
                encontrado ++;
            }
        }
        if (encontrado < 1) {
            a.id = -1;
        }
        fclose(archi);
    }
    return a; // SI NO SE ENCONTRO, id = -1;
}

// SI NO SE ENCONTRO, Avion id = -1;
Avion getAvionHangar(int id)
{
    FILE *archi;
    archi = fopen(archivoHangar,"rb");
    Avion a;
    if (archi != NULL)
    {
        int encontrado = 0;
        while (encontrado < 1 && fread(&a,sizeof(Avion),1,archi) > 0)
        {
            if (a.id == id)
            {
                encontrado ++;
            }
        }
        if (encontrado < 1){
            a.id = -1;
        }
        fclose(archi);
    }
    return a; // SI NO SE ENCONTRO id = -1;
}

// ------------------------------------------------------------------------------

void mostrarRegistro(Registro registro)
{
    Usuario u = getUsuario(registro.idUsuario);
    textcolor(6);
    printf("\n   %d  ", registro.id);
    textcolor(15);
    printf("	%d	", u.id);
    printf("   (%s)   ", u.nombreUsuario);
    printf(" [%d", registro.fecha.dia);
    printf("/%d", registro.fecha.mes);
    printf("/%d] ", registro.fecha.anio);
    printf(" [%d", registro.hora.hora);
    printf(":%d", registro.hora.minuto);
    printf(":%d] ", registro.hora.segundo);

    return(registro);
}

void cargarUltimoRegistroRecursivo(Pila *pila, FILE *archi, int *contador)
{
    Registro r;
    if (fread(&r,sizeof(Registro),1,archi) > 0)   // condicion de corte = solucion trivial
    {
        cargarUltimoRegistroRecursivo(pila,archi,contador);
        if (*contador < 50)
        {
            apilar(pila,r.id);
            int aux = *contador;
            *contador = (aux + 1);
        }
    }
}

void cargarPilaRegistros(Pila *pilaRegistros)
{
    FILE *archi;
    Registro registro;
    archi=fopen(archivoRegistros, "rb");
    if(archi!=NULL)
    {
        int contador = 0;
        cargarUltimoRegistroRecursivo(pilaRegistros,archi,&contador);
        fclose(archi);
    }
}

void mostrarRegistros()
{
    Pila pilaRegistros;
    inicpila(&pilaRegistros);
    cargarPilaRegistros(&pilaRegistros);
    textcolor(6);
    mostrarDialogo(" ID | idUsuario |  Usuario  |    Fecha   |    Hora  ");
    textcolor(15);
    Registro r;
    int id;
    while (!pilavacia(&pilaRegistros))
    {
        id = desapilar(&pilaRegistros);
        r = getRegistro(id);
        if (r.id != -1)
        {
            mostrarRegistro(r);
        }
        else
        {
            textcolor(12);
            printf("\nRegistro no encontrado.");
            textcolor(15);
        }
    }
    printf("\n\n");
    system("pause");
}

void mostrarPersona(Persona p)
{
    printf ("\n Nombre: %s",p.nombreApellido);
    printf ("\n DNI: %s",p.dni);
    printf ("\n Genero: %c",toupper(p.genero));
    printf ("\n Edad = %d a%cos",anioActual()-p.anioNacimiento,164);
}

void mostrarUsuario(Usuario u)
{
    printf("\n______________________");
    textcolor(6);
    printf ("\n ID: %d",u.id);
    textcolor(15);
    printf ("\n Usuario: %s",u.nombreUsuario);
    // printf ("Contrasenia: %s",u.contrasenia);
    mostrarPersona(u.persona);
    printf ("\n Rol: %d ",u.rol);
    if (u.rol == 0)
    {
        printf("(Invitado)");
    }
    else if (u.rol == 1)
    {
        textcolor(9);
        printf("(Operador)");
        textcolor(15);
    }
    else if (u.rol == 2)
    {
        textcolor(6);
        printf("(Administrador)");
        textcolor(15);
    }
    if (u.alta > 0)
    {
        textcolor(10);
    }
    else
    {
        textcolor(12);
    }
    printf ("\n Alta = %d",u.alta);
    textcolor(15);
    printf("\n______________________");
}

void mostrarUsuarios()
{
    FILE* archi;
    archi = fopen(archivoUsuarios,"rb");
    if (archi != NULL)
    {
        Usuario u;
        while (fread(&u,sizeof(Usuario),1,archi) > 0)
        {
            mostrarUsuario(u);
        }
        fclose(archi);
        printf("\n\n");
        system("pause");
    }
    else
    {
        textcolor(12);
        mostrarDialogo("No se ha podido abrir el archivo de Usuarios");
        textcolor(15);
        printf("\n\n");
        system("pause");
    }
}

void mostrarAvion(Avion avion)
{
    printf("\n______________________");
    textcolor(6);
    printf("\n ID: %d", avion.id);
    textcolor(15);
    printf("\n Nombre: %s", avion.nombre);
    printf("\n Numeracion: %d", avion.numeracion);
    printf("\n Modelo: %s", avion.modelo);
    printf("\n Aerolinea: %s", avion.aerolinea);
    printf("\n Capacidad pasajeros: %d", avion.capacidadPasajeros);
    printf("\n Vuelos realizados: %d", avion.vuelosRealizados);
    //printf("Combustible MAX: %d\n", avion.combustibleMaximo);
    float porcentaje = (((float)avion.combustibleActual) / ((float)avion.combustibleMaximo)) * 100;
    printf("\n Combustible actual: %.2f%%", porcentaje);
    if (avion.alta > 0)
    {
        textcolor(10);
    }
    else
    {
        textcolor(12);
    }
    printf("\n Alta: %d", avion.alta);
    textcolor(15);
}

void mostrarAvionHangar(Avion avion)
{
    Piloto piloto;
    printf("\n__________ [Plataforma Avion] ___________");
    textcolor(6);
    printf("\n ID Avion: %d", avion.id);
    textcolor(15);
    printf("\n Avion: %s-%d", avion.nombre, avion.numeracion);
    printf("\n Aerolinea: %s", avion.aerolinea);
    printf("\n Vuelos realizados: %d", avion.vuelosRealizados);
    //printf("Combustible MAX: %d\n", avion.combustibleMaximo);
    float porcentaje = (((float)avion.combustibleActual) / ((float)avion.combustibleMaximo)) * 100;
    if (porcentaje > (float) 65)
    {
        textcolor(10);
    }
    else if (porcentaje > (float) 35)
    {
        textcolor(14);
    }
    else
    {
        textcolor(12);
    }
    printf("\n Combustible: %.2f%%", porcentaje);
    textcolor(15);
    printf("\n => Alta: ");
    if (avion.alta < 1) {
        textcolor(12);
    } else {
        textcolor(10);
    }
    printf("%d",avion.alta);
    textcolor(15);
    printf("\n---------- [Piloto del Avion] -----------");
    if (avion.idPiloto == -1) {
        printf("\n => ID Piloto: %d",avion.idPiloto);
        printf("\n => Piloto: ");
        textcolor(12);
        printf("Sin Piloto Asignado");
        textcolor(15);
    } else {
        piloto = getPiloto(avion.idPiloto);
        textcolor(6);
        printf("\n => ID Piloto: %d",piloto.id);
        textcolor(15);
        printf("\n => Piloto: %s",piloto.persona.nombreApellido);
        printf("\n => Rango: %s", piloto.rango);
        printf("\n => Horas de Vuelo: %d", piloto.horasVuelo);
        printf("\n => Alta: ");
        if (piloto.alta < 1) {
            textcolor(12);
        } else {
            textcolor(10);
        }
        printf("%d",piloto.alta);
        textcolor(15);
    }
}

void mostrarAviones()
{
    FILE *archi;
    archi = fopen(archivoAviones,"rb");
    Avion avion;
    if(archi!=NULL)
    {
        while(fread(&avion, sizeof(Avion), 1, archi) > 0)
        {
            mostrarAvion(avion);
        }
        fclose(archi);
        printf("\n\n");
        system("pause");
    }
}

void mostrarAvionesDinamicos(Avion **aviones, int validos)
{
    for (int i=0; i<validos; i++)
    {
        mostrarAvion((*aviones)[i]);
    }
    printf("\n\n");
    system("pause");
}

void mostrarAvionesAlfabeticamente()
{
    int dimension = cantidadRegistros(archivoAviones,sizeof(Avion));
    Avion aviones[dimension];
    int validos = 0;
    validos = pasarAvionesDeArchivoArreglo(aviones, dimension);
    ordenarPorSeleccionArregloAvionesNombre (validos, aviones);
    // --------
    mostrarArregloAviones(aviones,validos);
}

void mostrarAvionesPorCombustible()
{
    // ordenar arreglo
    int dimension = cantidadRegistros(archivoAviones,sizeof(Avion));
    Avion aviones[dimension];
    int validos = 0;
    validos = pasarAvionesAltaDeArchivoArreglo(aviones,archivoAviones);
    ordenarPorInsercionAvionesConbustibleActual(aviones,validos);
    // -- mostrar arreglo
    Avion a;
    float combustiblePromedio;
    for (int i=0; i<validos; i++)
    {
        a = aviones[i];
        combustiblePromedio = (float) (((float) a.combustibleActual / (float) a.combustibleMaximo) *100);
        // ----- mostrar avion -----
        textcolor(6);
        printf("\n| ID: %d |",a.id);
        if (combustiblePromedio > 59)
        {
            textcolor(10); // verde claro
        }
        else if (combustiblePromedio > 35)
        {
            textcolor(14); // amarillo
        }
        else
        {
            textcolor(12); // rojo claro
        }
        printf(" Combustible: %.2f%% ",combustiblePromedio);
        textcolor(15);
        printf("| %s-%d | %s | %s |",a.nombre,a.numeracion,a.modelo,a.aerolinea);
    }
    //mostrarArregloAviones(aviones,validos);
}

void mostrarAvionesDadosAlta()
{
    FILE *archi;
    Avion avion;
    archi = fopen(archivoAviones, "rb");

    if (archi != NULL)
    {
        while (fread(&avion, sizeof(Avion), 1, archi) > 0)
        {
            if (avion.alta == 1)
            {
                mostrarAvion(avion);
            }
        }
        fclose(archi);
        printf("\n\n");
        system("pause");
    }
}

void mostrarAvionesDadosBaja()
{
    FILE *archi;
    Avion avion;
    archi = fopen(archivoAviones, "rb");

    if (archi != NULL)
    {
        while (fread(&avion, sizeof(Avion), 1, archi) > 0)
        {
            if (avion.alta == 0)
            {
                mostrarAvion(avion);
            }
        }
        fclose(archi);
        printf("\n\n");
        system("pause");
    }
}

void mostrarPiloto(Piloto  p)
{
    printf("\n______________________");
    textcolor(6);
    printf("\n ID: %d", p.id);
    textcolor(15);
    printf("\n Nombre y apellido: %s", p.persona.nombreApellido);
    printf("\n DNI: %s", p.persona.dni);
    printf("\n Edad: %d a%cos", (anioActual() - p.persona.anioNacimiento), 164);
    printf("\n Genero: %c", p.persona.genero);
    printf("\n Numero de licencia: %s", p.numLicencia);
    printf("\n Horas de vuelo: %d", p.horasVuelo);
    printf("\n Rango: %s", p.rango);
    if (p.alta > 0)
    {
        textcolor(10);
    }
    else
    {
        textcolor(12);
    }
    printf("\n Alta: %d", p.alta);
    textcolor(15);
}

void mostrarPilotos()
{
    FILE *archi;
    archi = fopen(archivoPilotos,"rb");
    Piloto piloto;
    if(archi!=NULL)
    {
        while(fread(&piloto, sizeof(Piloto), 1, archi) > 0)
        {
            mostrarPiloto(piloto);
        }
        fclose(archi);
        printf("\n\n");
        system("pause");
    }
}

void mostrarPilotosDinamicos(Piloto **pilotos, int validos)
{
    for (int i=0; i<validos; i++)
    {
        mostrarPiloto((*pilotos)[i]);
    }
    printf("\n\n");
    system("pause");
}

void mostrarPilotosAlfabeticamente()
{
    int dimension = cantidadRegistros(archivoPilotos,sizeof(Piloto));
    Piloto pilotos[dimension];
    int validos = 0;
    validos = pasarPilotosDeArchivoArreglo(pilotos,dimension);
    ordenarPorSeleccionArregloPilotos(validos,pilotos);
    // --------
    mostrarArregloPilotos(pilotos,validos);
}

void mostrarPilotosDadosAlta()
{
    FILE *archi;
    Piloto pilot;
    archi=fopen(archivoPilotos, "rb");
    if(archi!=NULL)
    {
        while(fread(&pilot, sizeof(Piloto), 1, archi) > 0)
        {
            if (pilot.alta == 1)
            {
                //void mostrarPilotoEnAvion(Avion  p)
                mostrarPiloto(pilot);
            }
        }
        fclose(archi);
        printf("\n\n");
        system("pause");
    }
}

void mostrarPilotosDadosBaja()
{
    FILE *archi;
    Piloto pilot;
    archi=fopen(archivoPilotos, "rb");
    if(archi!=NULL)
    {
        while(fread(&pilot, sizeof(Piloto), 1, archi) > 0)
        {
            if (pilot.alta == 0)
            {
                mostrarPiloto(pilot);
            }
        }
        fclose(archi);
        printf("\n\n");
        system("pause");
    }
}

// Devuelve 1 (verdadero) si subcadena se encuentra dentro de totalCadena, y 0 (falso) en caso contrario.
int contieneSubcadena(char *totalCadena, char *subcadena)
{
    convertirMinusculas(totalCadena);
    convertirMinusculas(subcadena);
    return strstr(totalCadena, subcadena) != NULL; // 1 verdadero / 0 falso
}

// Funcion que recibe un arreglo dinamico, y añade un dato extra al arreglo dinamico
void consultaAvionAgregarLista(Avion **avionesDinamicos, int *validos, Avion avion)
{
    Avion *buffer = (Avion*) realloc(*avionesDinamicos,(*validos + 1)*sizeof(Avion)); // Añade un espacio extra
    if (buffer == NULL)
    {
        fprintf(stderr, "Error al redimensionar la memoria\n");
        return;
    }
    *avionesDinamicos = buffer;
    (*avionesDinamicos)[*validos] = avion;
    (*validos)++;
}

void consultarAvionesPorNombre()
{
    int dim = cantidadRegistros(archivoAviones,sizeof(Avion));
    Avion aviones[dim];
    pasarAvionesDeArchivoArreglo(aviones,dim);
    // -----------
    char nombre[40];
    do
    {
        printf("\n\n Busqueda Avion (Nombre) => ");
        fflush(stdin);
        gets(nombre);
    }
    while(validarLongitud(nombre,1,29));
    // -----------
    int validos = 0;
    Avion* avionesDinamicos = (Avion*) malloc(0); /// Crea el arreglo dinamico con 0 posiciones
    char nombreCompleto[30];
    for (int i=0; i<dim; i++)
    {
        strcpy(nombreCompleto, aviones[i].nombre);
        if (contieneSubcadena(nombreCompleto, nombre) > 0)   /// Si la palabra ingresada esta dentro del Nombre alias del Avion
        {
            consultaAvionAgregarLista(&avionesDinamicos, &validos, aviones[i]); /// Entonces añade 1 posicion, y registra el Avion al arreglo dinamico
        }
    }
    mostrarAvionesDinamicos(&avionesDinamicos, validos);
    free(avionesDinamicos); // Libera la memoria utilizada por el arreglo después de usarla
}

// Funcion que recibe un arreglo dinamico, y añade un dato extra al arreglo dinamico
void consultaPilotoAgregarLista(Piloto **pilotosDinamicos, int *validos, Piloto piloto)
{
    Piloto *buffer = (Piloto*) realloc(*pilotosDinamicos,(*validos + 1)*sizeof(Piloto)); // Añade un espacio extra
    if (buffer == NULL)
    {
        fprintf(stderr, "Error al redimensionar la memoria\n");
        return;
    }
    *pilotosDinamicos = buffer;
    (*pilotosDinamicos)[*validos] = piloto;
    (*validos)++;
}

void consultarPilotosPorNombre()
{
    int dim = cantidadRegistros(archivoPilotos,sizeof(Piloto));
    Piloto pilotos[dim];
    pasarPilotosDeArchivoArreglo(pilotos,dim);
    // -----------
    char nombre[40];
    do
    {
        printf("\n\n Busqueda Piloto (Nombre Alias) => ");
        fflush(stdin);
        gets(nombre);
    }
    while(validarLongitud(nombre,1,29));
    // -----------
    int validos = 0;
    Piloto *pilotosDinamicos = (Piloto*) malloc(0); /// Crea el arreglo dinamico con 0 posiciones
    char nombreApellido[30];
    for (int i=0; i<dim; i++)
    {
        strcpy(nombreApellido, pilotos[i].persona.nombreApellido);
        if (contieneSubcadena(nombreApellido, nombre) > 0)   /// Si la palabra ingresada esta dentro del Nombre alias del Avion
        {
            consultaPilotoAgregarLista(&pilotosDinamicos, &validos, pilotos[i]); /// Entonces añade 1 posicion, y registra el Avion al arreglo dinamico
        }
    }
    mostrarPilotosDinamicos(&pilotosDinamicos, validos);
    free(pilotosDinamicos); // Libera la memoria utilizada por el arreglo después de usarla
}

/// ___________________________________ (UPDATE) ___________________________________

Piloto modificarPiloto(Piloto pilot)
{
    int num, volver = 0;
    char aux[100], letra, opcion = 'x';

    do {
        system("cls");
        mostrarPiloto(pilot);
        printf("\n\n1. Nombre y Apellido");
        printf("\n2. A%co de nacimiento",164);
        printf("\n3. Numero de DNI");
        printf("\n4. Identidad de Genero");
        printf("\n5. Numero de Licencia");
        printf("\n6. Horas de vuelo");
        textcolor(10);
        printf("\n\n0. Volver y Guardar");
        textcolor(15);
        printf("\n\nIngrese que apartado desea modificar => ");
        fflush(stdin);
        scanf("%c", &opcion);

        switch(opcion)
        {
        case '1':
            do
            {
                printf("\nIngrese el nuevo Nombre y Apellido (5-29 caracteres) => ");
                fflush(stdin);
                gets(aux);
            }
            while (validarLongitud(aux,5,29) > 0);
            strcpy(pilot.persona.nombreApellido,aux);
            break;
        case '2':
            do
            {
                printf("\nIngrese el nuevo A%co de Nacimiento => ",164);
                scanf("%d",&num);
                if (num < 1900 || num > (anioActual() - 18))
                {
                    textcolor(12);
                    printf("\nA%co invalido ...\n",164);
                    textcolor(15);
                }
            }
            while (num < 1900 || num > (anioActual() - 18));
            pilot.persona.anioNacimiento = num;
            break;
        case '3':
            do
            {
                printf("\nIngrese el nuevo DNI (1-9 caracteres) => ");
                fflush(stdin);
                gets(aux);
            }
            while (validarLongitud(aux,1,9) > 0 || validarDni(aux) > 0);
            strcpy(pilot.persona.dni,aux);
            break;
        case '4':
            do
            {
                printf("\nIngrese el nuevo Genero (F/M/O) => ");
                fflush(stdin);
                scanf("%c",&letra);
                letra = tolower(letra);
                if (letra != 'f' && letra != 'm' && letra != 'o')
                {
                    textcolor(12);
                    printf("\nOpcion no valida\n");
                    textcolor(15);
                }
            }
            while (letra != 'f' && letra != 'm' && letra != 'o');
            pilot.persona.genero = letra;
            break;
        case '5':
            do
            {
                printf("\nIngrese el nuevo Numero de Licencia (3-14 caracteres) => ");
                fflush(stdin);
                gets(aux);
            }
            while (validarLongitud(aux,3,14) > 0 || validarNumLicencia(aux) > 0);
            strcpy(pilot.numLicencia,aux);
            break;
        case '6':
            do
            {
                printf("\nIngrese las nuevas Horas de Vuelo => ");
                scanf("%d",&num);
            }
            while (num < 0);
            pilot.horasVuelo = num;
            ascenderPiloto(&pilot);
            break;
        case '0':
            volver = 1;
            break;
        default:
            textcolor(12);
            mostrarDialogo("Opcion Invalida");
            textcolor(15);
        }

    } while (volver != 1);

    ///________________________

    return pilot;
}

Avion modificarAvion(Avion avion)
{
    int num, volver = 0;
    char aux[100], opcion = 'x';
    do
    {
        system("cls");
        mostrarAvion(avion);
        printf("\n\n1. Nombre Alias");
        printf("\n2. Numeracion Alias");
        printf("\n3. Modelo");
        printf("\n4. Aerolinea");
        printf("\n5. Vuelos realizados");
        printf("\n6. Capacidad pasajeros");
        textcolor(10);
        printf("\n\n0. Volver y Guardar");
        textcolor(15);
        printf("\n\nIngrese que apartado desea modificar => ");
        fflush(stdin);
        scanf("%c", &opcion);
        switch(opcion)
        {
        case '1':
            do
            {
                printf("\nIngrese el nuevo Nombre Alias (3-29 caracteres) => ");
                fflush(stdin);
                gets(aux);
            }
            while (validarLongitud(aux,3,29) > 0);
            strcpy(avion.nombre,aux);
            break;
        case '2':
            do
            {
                printf("\nIngrese la nueva Numeracion Alias (0-999) => ");
                scanf("%d",&num);
            }
            while(num > 999 || num < 0);
            avion.numeracion = num;
            break;
        case '3':
            do
            {
                printf("\nIngrese el nuevo Modelo (3-29 caracteres) => ");
                fflush(stdin);
                gets(aux);
            }
            while(validarLongitud(aux,3,29) > 0);
            strcpy(avion.modelo,aux);
            break;
        case '4':
            do
            {
                printf("\nIngrese la nueva Aerolinea (3-29 caracteres) => ");
                fflush(stdin);
                gets(aux);
            }
            while(validarLongitud(aux,3,29) > 0);
            strcpy(avion.aerolinea,aux);
            break;
        case '5':
            do
            {
                printf("\nIngrese la nueva cantidad de Vuelos Realizados => ");
                scanf("%d",&num);
            }
            while (num < 0);
            avion.vuelosRealizados = num;
            break;
        case '6':
            do
            {
                printf("\nIngrese la nueva Capacidad max. de Pasajeros (1-200) => ");
                scanf("%d",&num);
            }
            while (num < 1 || num > 200);
            avion.capacidadPasajeros = num;
            break;
        case '0':
            volver = 1;
            break;
        default:
            textcolor(12);
            mostrarDialogo("Opcion Invalida");
            textcolor(15);
            system("pause");
            printf("\n\n");
        }
    }
    while (volver != 1);
    return avion;
}

void modificarRolUsuario()
{
    FILE* archi;
    archi = fopen(archivoUsuarios,"r+b");
    if (archi != NULL)
    {
        Usuario u;
        int id, aux;
        mostrarUsuarios();
        printf("\n\nIngrese ID => ");
        scanf("%d",&id);
        printf("\nQue permisos desea otorgar?");
        printf(" 0(Invitado)");
        textcolor(3);
        printf(" 1(Operador)");
        textcolor(6);
        printf(" 2(Admin)");
        textcolor(15);
        do
        {
            printf("\n\nIngrese rol => ");
            scanf("%d",&aux);
        }
        while (aux < 0 || aux > 2);
        // Se posiciona en el usuario a modificar y lo modifica
        fseek(archi,sizeof(Usuario)*id,0);
        fread(&u,sizeof(Usuario),1,archi);
        u.rol = aux;
        // Vuelve a la posicion 0
        rewind(archi);
        // Se posiciona en el usuario a modificar y lo sobreescribe
        fseek(archi,sizeof(Usuario)*id,0);
        fwrite(&u,sizeof(Usuario),1,archi);
        fclose(archi);
        textcolor(10);
        mostrarDialogo("Permisos modificados con exito");
        textcolor(15);
        system("pause");
    }
}

Usuario modificarUsuario(Usuario usuario)
{
    int num;
    char aux[100], auxPass[100], letra, opcion = 'x';
    int semaforo, volver = -1;
    system("cls");
    do
    {
        ///_____
        mostrarUsuario(usuario);
        printf("\n\n1. Nombre y Apellido");
        printf("\n2. A%co de nacimiento",164);
        printf("\n3. Numero de DNI");
        printf("\n4. Genero");
        printf("\n5. Nombre de Usuario");
        printf("\n6. Contrase%ca", 164);
        textcolor(10);
        printf("\n\n0. Guardar y Volver");
        textcolor(15);
        printf("\n\nIngrese que apartado desea modificar => ");
        fflush(stdin);
        scanf("%c", &opcion);

        switch(opcion)
        {
        case '1':
            do
            {
                printf("\nIngrese el nuevo nombre y apellido (5-29 caracteres) => ");
                fflush(stdin);
                gets(aux);
            }
            while (validarLongitud(aux,5,29) > 0);
            strcpy(usuario.persona.nombreApellido,aux);
            break;
        case '2':
            do
            {
                printf("\nIngrese el nuevo a%co de nacimiento => ",164);
                scanf("%d",&num);
                if (num < 1900 || num > (anioActual() - 18))
                {
                    textcolor(12);
                    printf("\nA%co invalido ...\n",164);
                    textcolor(15);
                }
            }
            while (num < 1900 || num > (anioActual() -18 ));
            usuario.persona.anioNacimiento = num;
            break;
        case '3':
            do
            {
                printf("\nIngrese el nuevo DNI (1-9 caracteres) => ");
                fflush(stdin);
                gets(aux);
            }
            while (validarLongitud(aux,1,9) > 0 || validarDni(aux) > 0);
            strcpy(usuario.persona.dni,aux);
            break;
        case '4':
            do
            {
                printf("\nIngrese el nuevo genero (F/M/O) => ");
                fflush(stdin);
                scanf("%c",&letra);
                letra = tolower(letra);
                if (letra != 'f' && letra != 'm' && letra != 'o')
                {
                    textcolor(12);
                    printf("\nOpcion no valida\n");
                    textcolor(15);
                }
            }
            while (letra != 'f' && letra != 'm' && letra != 'o');
            usuario.persona.genero = letra;
            break;
        case '5':
            do
                {
                    printf("\n\nIngrese el nuevo nombre de Usuario (3-29 caracteres) => ");
                    fflush(stdin);
                    gets(aux);
                }
                while (validarLongitud(aux,3,29) > 0 || validarUsuario(aux) > 0);
                strcpy(usuario.nombreUsuario,aux);
            break;
        case '6':

            semaforo = 0; // Confirmacion contraseña
                do
                {
                    do
                    {
                        printf("\n\nIngrese Contrase%ca (3-29 caracteres) => ",164);
                        fflush(stdin);
                        gets(aux);
                    }
                    while (validarLongitud(aux,3,29) > 0);
                    printf("\nConfirme Contrase%ca => ",164);
                    gets(auxPass);
                    if (strcmp(auxPass,aux) == 0)
                    {
                        semaforo = 1; // contraseña correcta
                    }
                    else
                    {
                        textcolor(12);
                        printf("\nLas Contrase%cas no coinciden...\n",164);
                        textcolor(15);
                    }
                    strcpy(usuario.contrasenia,aux);
                }
                while(semaforo < 1);

            break;
        case '0':
            volver = 1;
            break;
        default:
            textcolor(12);
            mostrarDialogo("Opcion invalida");
            textcolor(15);

        }
    }
    while (volver != 1);

    return usuario;
}

void modificarUsuarioAutenticado()
{
    FILE *archi;
    archi = fopen(archivoUsuarios,"r+b");
    Usuario u = *usuarioAutenticado;
    if (archi != NULL)
    {
        char letra, aux[100], pass[30], opcion = 'x';
        int anio, semaforo = 0, volver = 0;
        do
        {
            system("cls");
            if (u.rol == 1)
            {
                textcolor(9);
            }
            else if (u.rol == 2)
            {
                textcolor(6);
            }
            mostrarDialogo("Modificar mi Perfil");
            textcolor(15);

            printf("\n1. Nombre de Usuario\n2. Contrase%ca\n3. Numero de DNI\n4. Nombre y Apellido\n5. Identidad de Genero\n6. A%co de nacimiento\n\n",164,164);
            textcolor(10);
            printf("0. Guardar y Volver");
            textcolor(15);
            printf("\n\nIngrese opcion => ");
            fflush(stdin);
            scanf("%c",&opcion);
            switch (opcion)
            {
            case '1':
                do
                {
                    printf("\n\nIngrese Nombre de Usuario (3-29 caracteres) => ");
                    fflush(stdin);
                    gets(aux);
                }
                while (validarLongitud(aux,3,29) > 0 || validarUsuario(aux) > 0);
                strcpy(u.nombreUsuario,aux);
                break;
            case '2':
                semaforo = 0; // Confirmacion contraseña
                do
                {
                    do
                    {
                        printf("\n\nIngrese Contrase%ca (3-29 caracteres) => ",164);
                        fflush(stdin);
                        gets(pass);
                    }
                    while (validarLongitud(pass,3,29) > 0);
                    printf("\nConfirme Contrase%ca => ",164);
                    gets(aux);
                    if (strcmp(pass,aux) == 0)
                    {
                        semaforo = 1; // contraseña correcta
                    }
                    else
                    {
                        textcolor(12);
                        printf("\nLas Contrase%cas no coinciden...\n",164);
                        textcolor(15);
                    }
                    strcpy(u.contrasenia,pass);
                }
                while(semaforo < 1);

                break;
            case '3':
                do
                {
                    printf("\n\nIngrese Numero de DNI (1-9 caracteres) => ");
                    fflush(stdin);
                    gets(aux);
                }
                while (validarLongitud(aux,1,9) > 0 || validarDni(aux) > 0);
                strcpy(u.persona.dni,aux);
                break;
            case '4':
                do
                {
                    printf("\n\nIngrese Nombre y apellido (5-29 caracteres) => ");
                    fflush(stdin);
                    gets(aux);
                }
                while (validarLongitud(aux,5,29) > 0);
                strcpy(u.persona.nombreApellido,aux);
                break;
            case '5':
                do
                {
                    printf("\n\nIngrese Genero (F/M/O) => ");
                    fflush(stdin);
                    scanf("%c",&letra);
                    letra = tolower(letra);
                }
                while (letra != 'f' && letra != 'm' && letra != 'o');
                u.persona.genero = letra;
                break;
            case '6':
                do
                {
                    printf("\n\nModifique A%co de nacimiento => ",164);
                    scanf("%d",&anio);
                    if (anio < 1900 || anio > (anioActual() - 18))
                    {
                        textcolor(12);
                        printf("\nA%co invalido ...",164);
                        textcolor(15);
                    }
                }
                while (anio < 1900 || anio > (anioActual() - 18));
                u.persona.anioNacimiento = anio;
                break;
            case '0':
                printf("\n\nIngrese Contrase%ca actual para Confirmar cambios => ",164);
                fflush(stdin);
                gets(pass);
                if (strcmp(pass,u.contrasenia) == 0)
                {
                    setUsuario(u); // funcion que sobreescribe totalmente el Usuario
                    textcolor(10);
                    mostrarDialogo("Perfil modificado con exito");
                    textcolor(15);
                }
                else
                {
                    textcolor(12);
                    mostrarDialogo("Contrasenia incorrecta, omitiendo cambios");
                    textcolor(15);
                }
                volver = 1;
                printf("\n\n");
                system("pause");
                break;
            default:
                textcolor(12);
                mostrarDialogo("Opcion Invalida");
                textcolor(15);
                printf("\n\n");
                system("pause");
            }
        }
        while (volver != 1);
        fclose(archi);
    }
}

// Funcion que sobreescribe totalmente un Usuario (set = establecer)
void setUsuario (Usuario u)
{
    FILE *archi;
    archi = fopen(archivoUsuarios,"r+b");
    if (archi != NULL)
    {
        fseek(archi,sizeof(Usuario)*u.id,0);
        fwrite(&u,sizeof(Usuario),1,archi);
        fclose(archi);
    }
    else
    {
        textcolor(12);
        mostrarDialogo("No se ha podido abrir el archivo de Usuarios");
        textcolor(15);
    }
}

// Funcion que sobreescribe totalmente un Avion (set = establecer)
void setAvion (Avion a)
{
    FILE *archi;
    archi = fopen(archivoAviones,"r+b");
    if (archi != NULL)
    {
        fseek(archi,sizeof(Avion)*a.id,0);
        fwrite(&a,sizeof(Avion),1,archi);
        fclose(archi);
    }
    else
    {
        textcolor(12);
        mostrarDialogo("No se ha podido abrir el archivo de Aviones");
        textcolor(15);
    }
}

// Funcion que sobreescribe totalmente un Avion de Hangar (set = establecer)
void setAvionHangar (Avion a)
{
    FILE *archi;
    archi = fopen(archivoHangar,"r+b");
    if (archi != NULL)
    {
        Avion aux;
        int encontrado = 0;
        while (encontrado < 1 && fread(&aux,sizeof(Avion),1,archi) > 0) {
            if (aux.id == a.id) {
                encontrado ++;
                fseek(archi,sizeof(Avion)*-1,1);
                fwrite(&a,sizeof(Avion),1,archi);
            }
        }
        fclose(archi);
    }
    else
    {
        textcolor(12);
        mostrarDialogo("No se ha podido abrir el archivo de Hangar");
        textcolor(15);
    }
}

// Funcion que sobreescribe totalmente un Piloto (set = establecer)
void setPiloto (Piloto p)
{
    FILE *archi;
    archi = fopen(archivoPilotos,"r+b");
    if (archi != NULL)
    {
        fseek(archi,sizeof(Piloto)*p.id,0);
        ascenderPiloto(&p);
        fwrite(&p,sizeof(Piloto),1,archi);
        fclose(archi);
    }
    else
    {
        textcolor(12);
        mostrarDialogo("No se ha podido abrir el archivo de Pilotos");
        textcolor(15);
    }
}

void mostrarPilotoEnAvion(Avion  a)
{
    if (a.idPiloto == -1)
    {
        textcolor(12);
        mostrarDialogo("No hay un piloto asignado en el avion");
        textcolor(15);
    }
    else
    {
        Piloto p = getPiloto(a.idPiloto);
        printf("\n______________________");
        textcolor(6);
        printf("\n ID: %d", p.id);
        textcolor(15);
        printf("\n Nombre y Apellido: %s", p.persona.nombreApellido);
        printf("\n DNI: %s\n", p.persona.dni);
        printf("\n Edad: %d a%cos", (anioActual() - p.persona.anioNacimiento), 164);
        printf("\n Genero: %c", p.persona.genero);
        printf("\n Numero de licencia: %s", p.numLicencia);
        printf("\n Horas de vuelo: %d", p.horasVuelo);
        printf("\n Rango: %s", p.rango);
        if (p.alta > 0)
        {
            textcolor(10);
        }
        else
        {
            textcolor(12);
        }
        printf("\n Alta: %d", p.alta);
        textcolor(15);
    }
}

void mostrarPilotosEnAviones(Avion arreglo[], int validos)
{
    for(int i = 0; i < validos; i++)
    {

        if(arreglo[i].idPiloto != -1)
        {
            mostrarPilotoEnAvion(arreglo[i]);
        }
    }
}

void mostrarHangar(char hangar[])
{
    Avion arreglo[20];
    int validos =0;

    validos = pasarAvionesAltaDeArchivoArreglo(arreglo,hangar);
    for(int i = 0; i < validos; i++)
    {
        printf("---------------------------------");
        printf("\n Avion:");
        mostrarAvion(arreglo[i]);
        printf("\n Piloto Del Avion:");
        mostrarPilotoEnAvion(arreglo[i]);

        printf("---------------------------------");
    }
}

///------------------------Funciones Axiliares----------------------

///--------------aviones-----------------

//  Valida que el avion NO ESTE dentro del archivo del Hangar
int buscarAvionEnArreglo(Avion a, Avion arreglo[], int validos)
{
    int flag = 0;
    for( int u = 0; u< validos; u++)
    {
        if ((a.id == arreglo[u].id) && 0 < combustibleOptimo(arreglo[u]))
        {
            flag = 1;
        }
    }
    return flag;
}

// Sirve para chequear que el combustible del avion no este en 0, porque si esta en 0 no  puede salir
int combustibleOptimo(Avion a)
{
    int optimo = 0;

    if (a.combustibleActual >= 300) // Consume 300 por vuelo
    {
        optimo = 1;
    }
    return optimo;
}

//  Valida que el piloto NO ESTE dentro del archivo del Hangar
int buscarPilotoEnArreglo(int idPiloto, Avion arreglo[], int validos)
{
    int flag = 0;
    for( int u = 0; u< validos; u++)
    {
        printf("%d", idPiloto);
        if (arreglo[u].idPiloto == idPiloto)
        {
            flag = 1;
        }
    }
    return flag;
}

///------------------------Filtrar Hangar---------------------------

// Muestra un arreglo de pilotos dados de alta
void mostrarPilotosDadosDeAlta(char archivo[])
{
    FILE *archi;
    Piloto  piloto;
    archi = fopen(archivo, "rb");

    if (archi != NULL)
    {
        while (fread(&piloto, sizeof(Piloto), 1, archi) > 0)
        {
            if (piloto.alta == 1)
            {
                mostrarPiloto(piloto);
            }
        }
        fclose(archi);
        printf("\n\n");
        system("pause");
    }
}

void agregarAvionAHangar(Avion a)
{
    FILE *archi;
    archi = fopen(archivoHangar, "ab");

    if (archi != NULL)
    {
        fwrite(&a, sizeof(Avion), 1, archi);
        fclose(archi);
        textcolor(10);
        mostrarDialogo("Avion agregado al Hangar con exito");
        textcolor(15);
        printf("\n\n");
        system("pause");
    }
}

/// ___________________________________ (DELETE) ___________________________________

void modificarAltaUsuario(int id)
{
    FILE* archi;
    archi = fopen(archivoUsuarios,"r+b");
    if (archi != NULL)
    {
        Usuario u;
        fseek(archi,sizeof(Usuario)*id,0);
        fread(&u,sizeof(Usuario),1,archi);
        if (u.alta > 0)
        {
            u.alta = 0; // Si estaba de alta, se le da de baja
        }
        else
        {
            u.alta = 1; // Si estaba de baja, se le da de alta
        }
        fseek(archi,sizeof(Usuario)*(-1),1);
        fwrite(&u,sizeof(Usuario),1,archi);
        fclose(archi);
        if (u.alta > 0)
        {
            textcolor(10);
            mostrarDialogo("Usuario dado de ALTA con exito");
            textcolor(15);
        }
        else
        {
            textcolor(10);
            mostrarDialogo("Usuario dado de BAJA con exito");
            textcolor(15);
        }

    }
}

void modificarAltaAvion(int opcion)
{
    Avion avion;
    FILE *archi;
    int avionBuscado;
    archi = fopen(archivoAviones, "r+b");
    if (archi != NULL)
    {

        if(opcion == 1)
        {
            mostrarAvionesDadosAlta();
            printf("\nIngrese la ID del avion a dar de baja => ");
            scanf("%d", &avionBuscado);
        }
        else if(opcion == 0)
        {
            mostrarAvionesDadosBaja();
            printf("\nIngrese la ID del avion a dar de alta => ");
            scanf("%d", &avionBuscado);
        }

        while (fread(&avion, sizeof(Avion), 1, archi) > 0)
        {

            if(avionBuscado==avion.id && avion.alta==0)
            {
                avion.alta=1;
                fseek(archi, sizeof(Avion)*(-1), SEEK_CUR);
                fwrite(&avion, sizeof(Avion), 1, archi);
                fflush(archi);
                textcolor(10);
                mostrarDialogo("Avion dado de ALTA con exito");
                textcolor(15);
                mostrarAvion(avion);
            }
            else if(avionBuscado==avion.id && avion.alta==1)
            {
                avion.alta=0;
                fseek(archi, sizeof(Avion)*(-1), SEEK_CUR);
                fwrite(&avion, sizeof(Avion), 1, archi);
                fflush(archi);
                textcolor(10);
                mostrarDialogo("Avion dado de BAJA con exito");
                textcolor(15);
                mostrarAvion(avion);
            }
        }
        fclose(archi);
    }
}

void modificarAltaPiloto(int opcion)
{
    Piloto piloto;
    FILE *archi;
    int pilotoBuscado;
    archi = fopen(archivoPilotos, "r+b");
    if (archi != NULL)
    {

        if(opcion == 1)
        {
            mostrarPilotosDadosAlta();
            printf("\nIngrese la ID del piloto a dar de baja => ");
            scanf("%d", &pilotoBuscado);
        }
        else if(opcion == 0)
        {
            mostrarPilotosDadosBaja();
            printf("\nIngrese la ID del piloto a dar de alta => ");
            scanf("%d", &pilotoBuscado);
        }


        while (fread(&piloto, sizeof(Piloto), 1, archi) > 0)
        {

            if(pilotoBuscado==piloto.id && piloto.alta==0)
            {
                piloto.alta=1;
                fseek(archi, sizeof(Piloto)*(-1), SEEK_CUR);
                fwrite(&piloto, sizeof(Piloto), 1, archi);
                fflush(archi);
                textcolor(10);
                mostrarDialogo("Piloto dado de ALTA con exito");
                textcolor(15);
                mostrarPiloto(piloto);
            }
            else if(pilotoBuscado==piloto.id && piloto.alta==1)
            {
                piloto.alta=0;
                fseek(archi, sizeof(Piloto)*(-1), SEEK_CUR);
                fwrite(&piloto, sizeof(Piloto), 1, archi);
                fflush(archi);
                textcolor(10);
                mostrarDialogo("Piloto dado de BAJA con exito");
                textcolor(15);
                mostrarPiloto(piloto);
            }
        }
        fclose(archi);
    }
}

// ----------------------------------------------------------------------------------------- RADAR

void gotoxy(int x, int y)
{
    HANDLE ventana;
    ventana = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD cordenadas;
    cordenadas.X = x;
    cordenadas.Y = y;

    SetConsoleCursorPosition(ventana, cordenadas);
}

void generarColoresUnicos(int* colores, int numColores)
{
    for (int i = 0; i < numColores; i++)
    {
        colores[i] = i + 1;
    }
    for (int i = numColores - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int temp = colores[i];
        colores[i] = colores[j];
        colores[j] = temp;
    }
}

void inicializarMatriz(char matriz[TAMANIOFIL][TAMANIOCOL], UbicacionAvion* aviones, int numeroDeAviones, int* colores)
{
    for (int i = 0; i < TAMANIOFIL; i++)
    {
        for (int j = 0; j < TAMANIOCOL; j++)
        {
            matriz[i][j] = VACIO; // .
        }
    }

    Avion avionesH[numeroDeAviones];
    pasarHangarDeArchivoArreglo(avionesH,numeroDeAviones);

    for (int i = 0; i < numeroDeAviones; i++)
    {
        int x, y;
        do
        {
            y = rand() % TAMANIOFIL; // 40
            x = rand() % TAMANIOCOL; // 80

        } while (matriz[y][x] != VACIO);

        aviones[i].x = x;
        aviones[i].y = y;

        aviones[i].direccion = rand() % 4 ; // Asigna una direccion para moverse

        aviones[i].color = colores[i % MAX_COLORES]; // Asigna un color

        if (aviones[i].direccion == 0)
        {
            aviones[i].icono = ARRIBA;
        }
        else if (aviones[i].direccion == 1)
        {
            aviones[i].icono = DERECHA;
        }
        else if (aviones[i].direccion == 2)
        {
            aviones[i].icono = ABAJO;
        }
        else if (aviones[i].direccion == 3)
        {
            aviones[i].icono = IZQUIERDA;
        }

        aviones[i].idAvion = avionesH[i].id;

        strcpy(aviones[i].estado, "En Vuelo");

        matriz[y][x] = aviones[i].icono;
    }
}

void imprimirRadarYMenu(char matriz[TAMANIOFIL][TAMANIOCOL], UbicacionAvion* aviones, int numeroDeAviones)
{
    // Imprimir borde superior
    textcolor(9);  // Azul
    gotoxy(35, 0); // Mover n° columnas a la derecha


    printf("%c", 219); // 219 BARRA SOLIDA
    for (int i = 0; i < TAMANIOCOL + 2; i++)
    {
        printf("%c", 219); // 219 BARRA SOLIDA
    }
    printf("%c\n", 219); // 219 BARRA SOLIDA


    // Imprimir radar y menu
    for (int i = 0; i < TAMANIOFIL; i++)
    {
        gotoxy(35, i + 1); // Mover 15 columnas a la derecha y una línea hacia abajo
        textcolor(9);  // Azul para el borde lateral izquierdo
        printf("%c%c", 219, 219); // 219 BARRA SOLIDA

        // Imprimir el radar
        for (int j = 0; j < TAMANIOCOL; j++)
        {
            char c = matriz[i][j];
            int color = 9;  // Azul para el espacio vacio
            if (c == VACIO) // .
            {
                color = 9;
            }
            else if (c == ESTELA) // ,
            {
                color = 14;
            }
            else if (c == COLISION) // X
            {
                color = 4;
            }
            else // Avion
            {
                for (int k = 0; k < numeroDeAviones; k++)
                {
                    if (aviones[k].y == i && aviones[k].x == j)
                    {
                        color = aviones[k].color;
                        break;
                    }
                }
            }
            textcolor(color);
            printf("%c", c);
        }
        textcolor(9);  // Azul para el borde lateral derecho
        printf("%c%c", 219, 219); // 219 BORDE BARRA SOLIDA

        // Imprimir el menu
        if (i < numeroDeAviones)
        {
            gotoxy(TAMANIOCOL + 4 + 35, i + 1); // Mover el cursor para alinear el menú 15 columnas a la derecha

            Avion a = getAvion(aviones[i].idAvion);

            textcolor(15);  // Blanco para el texto "Avion"
            printf(" Avion |");
            textcolor(aviones[i].color);  // Color del avión solo para el simbolo
            printf(" %c ", 220); // 220 CUADRITO MENU RADAR AVION DISPONIBLE
            textcolor(15);  // Blanco para el estado
            printf("| %s-%d |", a.nombre, a.numeracion);
            if (strcmpi(aviones[i].estado,"Viaje Finalizado") == 0) {
                textcolor(10);
            } else if(strcmpi(aviones[i].estado,"Colisionado") == 0) {
                textcolor(12);
            }
            printf(" %s", aviones[i].estado);
            textcolor(15);
            printf("\n");
        }
    }

    // Imprimir borde inferior
    gotoxy(35, TAMANIOFIL + 1); // Mover 15 columnas a la derecha y una línea hacia abajo
    textcolor(9);  // Azul
    printf("%c", 219);
    for (int i = 0; i < TAMANIOCOL + 2; i++)
    {
        printf("%c", 219);
    }
    printf("%c\n", 219);

    textcolor(15);  // Blanco distinto del default
}

int moverAviones(UbicacionAvion aviones[], int numeroDeAviones, char matriz[TAMANIOFIL][TAMANIOCOL])
{
    int colision = 0;
    for (int k = 0; k < numeroDeAviones; k++)
    {
        // Solo mover aviones que no estan colisionados ni fuera del radar
        if (strcmp(aviones[k].estado, "Colisionado") != 0 && strcmp(aviones[k].estado, "Viaje Finalizado") != 0) {

            int nuevoX = aviones[k].x;
            int nuevoY = aviones[k].y;

            int cono = rand() % 3;

            if (aviones[k].direccion == 0) {       // ARRIBA
                nuevoY ++;
                if (cono == 0) {             // NOROESTE
                    nuevoX --;
                } else if (cono == 1) {      // NORTE
                    nuevoX = nuevoX;
                } else if (cono == 2) {      // NORESTE
                    nuevoX ++;
                }
            } else if (aviones[k].direccion == 1) { // DERECHA
                nuevoX ++;
                if (cono == 0) {             // NORESTE
                    nuevoY ++;
                } else if (cono == 1) {      // ESTE
                    nuevoY = nuevoY;
                } else if (cono == 2) {      // SURESTE
                    nuevoY --;
                }
            } else if (aviones[k].direccion == 2) { // ABAJO
                nuevoY --;
                if (cono == 0) {             // SURESTE
                    nuevoX ++;
                } else if (cono == 1) {      // SUR
                    nuevoX = nuevoX;
                } else if (cono == 2) {      // SURORESTE
                    nuevoX --;
                }
            } else if (aviones[k].direccion == 3) { // IZQUIERDA
                nuevoX --;
                if (cono == 0) {             // SUROESTE
                    nuevoY --;
                } else if (cono == 1) {      // OESTE
                    nuevoY = nuevoY;
                } else if (cono == 2) {      // NOROESTE
                    nuevoY ++;
                }
            }

            // Marcar la posicion anterior del avion como estela solo si no es una colision
            if (matriz[aviones[k].y][aviones[k].x] != COLISION)
            {
                matriz[aviones[k].y][aviones[k].x] = ESTELA; // Cambio agregado
            }

            if (nuevoX >= 0 && nuevoX < TAMANIOCOL && nuevoY >= 0 && nuevoY < TAMANIOFIL)
            {
                if (matriz[nuevoY][nuevoX] == ARRIBA || matriz[nuevoY][nuevoX] == DERECHA || matriz[nuevoY][nuevoX] == ABAJO || matriz[nuevoY][nuevoX] == IZQUIERDA)
                {
                    matriz[nuevoY][nuevoX] = COLISION;

                    // El avion colisiona y se modifica
                    strcpy(aviones[k].estado, "Colisionado");

                    // Marcar el otro avion en la misma posicion como colisionado
                    for (int m = 0; m < numeroDeAviones; m++) {
                        if (aviones[m].x == nuevoX && aviones[m].y == nuevoY) {
                            strcpy(aviones[m].estado, "Colisionado");

                        }
                    }
                }
                else
                {
                    if (strcmp(aviones[k].estado, "Colisionado") != 0)
                    {
                        if (aviones[k].direccion == 0) {
                            aviones[k].icono = ARRIBA;

                        } else if (aviones[k].direccion == 1) {
                            aviones[k].icono = DERECHA;

                        } else if (aviones[k].direccion == 2) {
                            aviones[k].icono = ABAJO;

                        } else if (aviones[k].direccion == 3) {
                            aviones[k].icono = IZQUIERDA;
                        }
                        matriz[nuevoY][nuevoX] = aviones[k].icono;
                        aviones[k].x = nuevoX;
                        aviones[k].y = nuevoY;

                        strcpy(aviones[k].estado, "En Vuelo");
                    }
                }
            } else {
                strcpy(aviones[k].estado, "Viaje Finalizado");

            }
        }
    }
    return colision;
}
