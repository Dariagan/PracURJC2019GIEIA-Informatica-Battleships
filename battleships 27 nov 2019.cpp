#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iomanip>    

using namespace std;

struct propiedades_barco {//se crea el tipo de struct "propiedades_barco", que contiene todas los tipos de propiedades que puede tener un barco; las coordenadas X e Y de cada una de sus cinco (o menos) casillas, la longitud del barco, su orientacion en el tablero y si está colocado o no en este.

    int Xcasilla[5], Ycasilla[5], longitud;
    bool orientacion, puesto_casillero;
};

void CrearBarcos(propiedades_barco(&arraybarcos)[20][2]);

void PosicionarBarcos(int(&array_posicion)[26][26][2], string elapodo, propiedades_barco(&arraybarcos)[20][2], int dimensionXcasillero, int dimensionYcasillero, bool jugador);

bool Combate(int(&array_posicion)[26][26][2], string elapodo[2], int dimensionXcasillero, int dimensionYcasillero, propiedades_barco(&arraybarcos)[20][2]);

void MostradorCasillero(int casillero[26][26][2], int dimensionXcasillero, int dimensionYcasillero, bool jugador);
void MostradorCasilleroCHAR(char casillero[26][26][2], int dimensionXcasillero, int dimensionYcasillero, bool jugador);

int ContadorBarcos(propiedades_barco arraybarcos[20][2], int tipobarco_acontar, bool jugador);

//no utilizé ninguna variable global.

int main() {
	
    int i;

    string apodos[2];//se rellenan las dos posiciones del array "apodos" con el apodo de cada jugador
    for (i = 0; i < 2; i++) {
        cout << "\nJugador " << i + 1 << ", por favor introduzca su apodo: ";
        cin >> apodos[i];
        if (apodos[i].size() < 3){
        	
        	cout << "\nEl minimo de caracteres es tres.\n";
        	i--;
		}
    }

    while (1){

        cout << "\n-----------------------------------------Bienvenidos al juego de hundir la flota-----------------------------------------\n\n";

        int array_casillerodoble[26][26][2];//casillero dual (una matriz 7x7 para cada jugador). Cada dimension: eje x, eje y, jugadores. No registra los barcos para facilitar la visualizacion del tablero mediante un bucle "for", al no tener que involucrar las dimensiones del barco que es y sus respectivas casillas que ocupa.

        for (int i = 0; i < 26; i++){
            for (int j = 0; j < 26; j++){
                for (int k = 0; k < 2; k++){
                    array_casillerodoble[i][j][k] = 0; // se vacía el casillero de los barcos
                }
            }
        }

        propiedades_barco arraybarcos[20][2];
        for (i = 0; i < 20; i++) {
            for (int j = 0; j < 2; j++){
                for (int k = 0; k < 5; k++){
                    arraybarcos[i][j].longitud = -100;
                    arraybarcos[i][j].puesto_casillero = false;// al empezar, todos los barcos por "default" no estan puestos en el casillero.
                    arraybarcos[i][j].Xcasilla[k] = -2;
                    arraybarcos[i][j].Ycasilla[k] = -2;
                }
            }
        }

        CrearBarcos(arraybarcos);//se llama a la funcion que crea todos los barcos

        //se empieza a crear el casillero
        int dimensionXcasillero, dimensionYcasillero;

        for (;;) {

            cout << "\nQue dimensiones del casillero quieren? Escriba la dimension horizontal [espacio] dimension vertical\n";
            cout << "Ejemplos: 10 10, 5 6, 26 5. EL MINIMO ES CINCO Y EL MAXIMO ES VEINTISEIS\n";
            cin >> dimensionXcasillero >> dimensionYcasillero;//el casillero toma el tamaño horizontal y vertical que quieran los jugadores

            if ((dimensionXcasillero > 26 || dimensionYcasillero > 26) || (dimensionXcasillero < 5 || dimensionYcasillero < 5)) {
                //si el casillero es demasiado grande o demasiado pequeño tal que no quepan los barcos muy grandes, se le obliga al jugador reintroducirla. El programa produce otra iteracion de este bucle 'for' infinito
                cout << "\nHas introducido una dimension demasiado pequeña o grande\n";
                continue;
            }
            else {
                break;//si el jugador introduce unas dimensiones sensatas del casillero, el programa escapa del bucle "for" infinito y prosigue.
            }
        }

        PosicionarBarcos(array_casillerodoble, apodos[0], arraybarcos, dimensionXcasillero, dimensionYcasillero, 0);
        PosicionarBarcos(array_casillerodoble, apodos[1], arraybarcos, dimensionXcasillero, dimensionYcasillero, 1);

        bool ganador(Combate(array_casillerodoble, apodos, dimensionXcasillero, dimensionYcasillero, arraybarcos));

        cout << endl << apodos[!ganador] << ", quieres la revancha? Escribe 'si' si quieres jugar otra partida. Si quieres cerrar el juego, escribe cualquier cosa: ";

        string respuesta;
        cin >> respuesta;

        if (respuesta == "si" || respuesta == "SI"){

            continue;//se reinicia el juego, puede ser que tenga errores por utilizar variables ya inicializadas en la partida anterior, pero no estoy seguro.
        }
        else {
            exit(0);
        }
    }
}


void CrearBarcos(propiedades_barco(&arraybarcos)[20][2]) {//esta funcion crea todos los barcos y los coloca ordenadamente en el array "arraybarcos", pero no les asigna una posicion en el casillero

    int i, numero_bpequenos, numero_bmedianos, numero_bgrandes, numero_bmuygrandes, numero_btotales;

    for (;;) {

        cout << "\nCuantos barcos pequenos, medianos, grandes, muy grandes quieren en juego? (Cada tipo ocupa respectivamente dos, tres, cuatro y cinco casillas).\n";
        cout << "El formato para introducirlo es: [cantidad de pequenos][espacio][cantidad de medianos][espacio][cantidad de grandes][espacio][cantidad de muy grandes]\n";
        cout << "Ejemplos: 7 5 2 0, 9 3 2 1, 9 4 0 0, 0 3 3 0, etc. AVISO: el total maximo de barcos es veinte.\n";

        //los jugadores eligen cuantos barcos de cada tipo quieren que hayan en juego (los dos jugadores reciben la misma cantidad de cada tipo de barco)

        cin >> numero_bpequenos >> numero_bmedianos >> numero_bgrandes >> numero_bmuygrandes;//se introduce la cantidad de cada tipo de barco en su respectiva variable

        numero_btotales = numero_bpequenos + numero_bmedianos + numero_bgrandes + numero_bmuygrandes; //se hace el sumatorio de todos los barcos que hay, y si sobrepasa veinte, el programa te hace reintroducirlos

        if (numero_btotales > 20 || numero_btotales < 1) {

            cout << "El programa no admite una cantidad de barcos totales mayor a veinte ni menor a uno.\n";
            continue; //el programa no permite al usuario salir de este bucle "for" infinito hasta que haya puesto una cantidad de barcos sensata
        }
        else {

            break; //el programa sale del bucle "for" si el usuario introdujo una cantidad de barcos permitida.
        }
    }
    //lo siguiente parece muy complejo, pero en realidad lo unico que se hace es rellenar el array "arraybarcos" con cada barco existente. Se rellena ordenadamente sin dejar huecos vacios o rellenando posiciones ya rellenadas.
    for (i = 0; i < numero_bpequenos; i++) {

        arraybarcos[i][0].longitud = 2; //se rellenan los barcos pequeños de longitud dos para el jugador 0
        arraybarcos[i][1].longitud = 2; //se rellenan los barcos pequeños de longitud dos para el jugador 1
    }
    for (i = 0; i < numero_bmedianos; i++) {

        arraybarcos[i + numero_bpequenos][0].longitud = 3; //se rellenan los barcos medianos de longitud tres para el jugador 0
        arraybarcos[i + numero_bpequenos][1].longitud = 3; //se rellenan los barcos medianos de longitud tres para el jugador 1
    }
    for (i = 0; i < numero_bgrandes; i++) {

        arraybarcos[i + numero_bpequenos + numero_bmedianos][0].longitud = 4; //se rellenan los barcos grandes de longitud cuatro para el jugador 0
        arraybarcos[i + numero_bpequenos + numero_bmedianos][1].longitud = 4; //se rellenan los barcos grandes de longitud cuatro para el jugador 1
    }
    for (i = 0; i < numero_bmuygrandes; i++) {

        arraybarcos[i + numero_bpequenos + numero_bmedianos + numero_bgrandes][0].longitud = 5; //se rellenan los barcos muy grandes de longitud cinco para el jugador 0
        arraybarcos[i + numero_bpequenos + numero_bmedianos + numero_bgrandes][1].longitud = 5; //se rellenan los barcos muy grandes de longitud cinco para el jugador 1
    }

    return;//se cierra la funcion de crear barcos para ambos jugadores
}


void PosicionarBarcos(int(&array_posicion)[26][26][2], string elapodo, propiedades_barco(&arraybarcos)[20][2], int dimensionXcasillero, int dimensionYcasillero, bool jugador) {
    int i;

    cout << "\n\n-----------------------------------------DISPOSICION DE LOS BARCOS-----------------------------------------\n\n";


    cout << "\nLe toca a " << elapodo << " posicionar sus barcos; el otro jugador, NO MIRE \n\n";

    cout << "Este es su casillero, " << elapodo << ": ";
    MostradorCasillero(array_posicion, dimensionXcasillero, dimensionYcasillero, jugador);//se le muestra el casillero al jugador x, mediante una funcion independiente.
    cout << "Los numeros de arriba indican la columna, y las letras de la izquierda la fila.\n\n";

    int barco_elegido;// se crea la variable barco_elegido, que se va a utilizar posteriormente para manipular el array "array_posicion".
    for (;;) {

        cout << "Que barco quieres posicionar o reposicionar? (escribir el numero): \n";
        for (i = 0; i < ContadorBarcos(arraybarcos, 0, 0); i++) {
//se muestran todos los barcos disponibles que hay para posicionar, enumerados en orden para diferenciar barcos del mismo tipo
            cout << i + 1 << ": barco ";
            switch (arraybarcos[i][jugador].longitud) {

            case 2:
                cout << "pequeno " << i + 1;
                break;
            case 3:
                cout << "mediano " << (i + 1) - ContadorBarcos(arraybarcos, 2, 0);
                break;
            case 4:
                cout << "grande " << (i + 1) - (ContadorBarcos(arraybarcos, 2, 0) + (ContadorBarcos(arraybarcos, 3, 0)));
                break;
            case 5:
                cout << "muy grande " << (i + 1 - (ContadorBarcos(arraybarcos, 2, 0) + (ContadorBarcos(arraybarcos, 3, 0) + (ContadorBarcos(arraybarcos, 4, 0)))));
                break;
            }
            cout << endl;
        }// Aqui termina el mostrador de los barcos disponibles a posicionar.
        if (ContadorBarcos(arraybarcos, 1, jugador) == ContadorBarcos(arraybarcos, 0, 0)) {
//Si el numero de barcos posicionados es igual al numero de barcos totales, se le permite al jugador x terminar su preparación
            cout << "Ya has posicionado todos tus barcos disponibles, si quieres terminar tu preparacion, escribe '100'\n";
        }

        cin >> barco_elegido;
        barco_elegido--; // se le resta una unidad al int "barco_elegido" para que se adapte a las posiciones de los arrays, que empiezan en cero.

        if (barco_elegido == 99 && ContadorBarcos(arraybarcos, 1, jugador) == ContadorBarcos(arraybarcos, 0, 0)) {
//si el jugador introduce un 99 (100 - 1) y tiene disponible la opción de terminar su preparación
            return;
        }
        if (barco_elegido < 0 || barco_elegido > ContadorBarcos(arraybarcos, 0, 0)) {
//si el jugador elige un barco negativo o mayor al numero de barcos disponibles, el programa le obliga a elegir otro.
            cout << "Has elegido un barco inexistente.\n\n";
            continue;
        }

        MostradorCasillero(array_posicion, dimensionXcasillero, dimensionYcasillero, jugador);

        cout << "\nDonde lo quieres poner y con que orientacion?, escribe [numero de la columna][espacio][letra de la fila][espacio][H ó V] para elegir la posicion y la orientacion del barco.\n";
        cout << "Ejemplos: 3 A V, 4 D H, 8 C V  9 J V. La 'H' significa que se va a poner horizontalmente y la 'V' significa que se va a poner verticalmente.\n";
        cout << "Si quieres cambiar otro barco, escribe '10 * *'\n";



        while (1) {

        introduccion_posicion:

            int coordenadaXintroducida;
            char letra_introducida;
            char letra_deorientacionintroducida;


            cin >> coordenadaXintroducida >> letra_introducida >> letra_deorientacionintroducida;
            if (coordenadaXintroducida == 10 && letra_introducida == '*' && letra_deorientacionintroducida == '*') {
			//si se escribió "10 * *", el programa vuelve a la pantalla de selección de barco a mover.
                break;
            }
                                //si se escribió una H o V cursiva, esta se vuelva mayúscula para no causar problemas
            bool orientacionintroducida = toupper(letra_deorientacionintroducida) - 'H';// la letra introducida 'H' ó 'V' se vuelve un 0 o 1 booleano al restarse el valor de 'H'
            

            arraybarcos[barco_elegido][jugador].orientacion = orientacionintroducida;
            

            if (letra_introducida >= 'a' && letra_introducida <= 'z') {

                letra_introducida = toupper(letra_introducida);
            }
            int coordenadaYintroducida = int(letra_introducida - 'A');
			//se le resta 'A' a la letra-coordenada para hacer que la coordenada tenga una base numérica en vez de alfabética-ascii. De esta forma es más facil de operar.

            if (--coordenadaXintroducida < 0 || coordenadaXintroducida > dimensionXcasillero || coordenadaYintroducida < 0 || coordenadaYintroducida > dimensionYcasillero) {

                cout << "Has introducido una orientacion o posicion incorrecta, o esta ultima esta fuera de rango del casillero. Por favor, introduce una posicion y orientacion correctamente:";
                continue;
            }

            //este "if" borra todas las casillas ocupadas anteriormente por un barco que acabó de ser movido ahora mismo, así no hay numeros '1' donde en realidad no hay nada
            if (arraybarcos[barco_elegido][jugador].puesto_casillero == true) {

                for (i = 0; i < arraybarcos[barco_elegido][jugador].longitud; i++) {

                    (array_posicion)[arraybarcos[barco_elegido][jugador].Xcasilla[i]][arraybarcos[barco_elegido][jugador].Ycasilla[i]][jugador] = 0;
                }
            }

            bool limite_alcanzado = false;
            int tempX[5]; 
            int tempY[5];

            for (i = 0; i < arraybarcos[barco_elegido][jugador].longitud; i++) {

                tempX[i] = arraybarcos[barco_elegido][jugador].Xcasilla[i]; //se guarda los datos numericos de las coordenadas previas antes de reemplazar las coordenadas por las nuevas coordenadas...
                tempY[i] = arraybarcos[barco_elegido][jugador].Ycasilla[i];//...de esta forma, si se cancela el movimiento, se puede revertir el cambio poniendole al barco de vuelta las coordenadas anteriores.
            }

            for (i = 0; i < arraybarcos[barco_elegido][jugador].longitud; i++) {

                if (arraybarcos[barco_elegido][jugador].orientacion == 0) {//si el barco está puesto horizontalmente, se hace esto

                    arraybarcos[barco_elegido][jugador].Ycasilla[i] = coordenadaYintroducida;//si está puesto horizontalmente el barco, la coordenada Y de las i casillas no varía

                    if (limite_alcanzado == false) {

                        arraybarcos[barco_elegido][jugador].Xcasilla[i] = coordenadaXintroducida + i;//si no se alcanza el borde derecho del tablero, se rellena de izquierda a derecha.
                    }
                    else {
                    	
                        arraybarcos[barco_elegido][jugador].Xcasilla[i] = coordenadaXintroducida - i;//si se alcanzó el borde derecho del tablero, se vuelve a rellenar pero de derecha a izquierda en vez de izquierda a derecha.
                    }

                    if (arraybarcos[barco_elegido][jugador].Xcasilla[i] == dimensionXcasillero && limite_alcanzado == false) { //si se llega al borde derecho del tablero...
                    	
                        i = -1; //...se reinicia el bucle "for"
                        limite_alcanzado = true;//y esto se pone con valor "true" para que no se vuelva a meter el programa en este "if" y ejecute el procedimiento si se alcanzó el borde derecho del tablero
                        continue; // la "i" se vuelve cero de vuelta
                    }

                }
                else if (arraybarcos[barco_elegido][jugador].orientacion == 1) {//si el barco está puesto verticalmente, se hace esto

                    arraybarcos[barco_elegido][jugador].Xcasilla[i] = coordenadaXintroducida;//si está puesto verticalmente el barco, la coordenada X de las i casillas no varía

                    if (limite_alcanzado == false) {

                        arraybarcos[barco_elegido][jugador].Ycasilla[i] = coordenadaYintroducida + i;//si no se alcanza el borde inferior del tablero, se rellena de arriba a abajo
                    }
                    else {
                    	
                        arraybarcos[barco_elegido][jugador].Ycasilla[i] = coordenadaYintroducida - i;//si se alcanzó el borde inferior del tablero, se vuelve a rellenar pero de abajo a arriba en vez de arriba a abajo.
                    }

                    if (arraybarcos[barco_elegido][jugador].Ycasilla[i] == dimensionYcasillero && limite_alcanzado == false) {//si se llega al borde inferior del tablero...
                    	
                        i = -1;//...se reinicia el bucle "for"
                        limite_alcanzado = true;//y esto se pone con valor "true" para que no se vuelva a meter el programa en este "if" y ejecute el procedimiento si se alcanzó el limite inferior del tablero
                        continue;// la "i" se vuelve cero de vuelta
                    }
                }
            }

            for (i = 0; i < arraybarcos[barco_elegido][jugador].longitud; i++) {

                if ((array_posicion)[arraybarcos[barco_elegido][jugador].Xcasilla[i]][arraybarcos[barco_elegido][jugador].Ycasilla[i]][jugador] == 1) {
// si se detecta que una de las casillas que se quieren ocupar con el barco que se quiere poner ya está rellena, el programa te avisa y te obliga a elegir otra posicion o poner otro barco.
                    cout << "Has elegido una posicion que implicaria ocupar casillas de otro barco ya puesto\nSi quieres cambiar otro barco, escribe '10 * *'. Si quieres elegir otra posicion, escribe otra posicion y orientacion: ";

                    for (i = 0; i < arraybarcos[barco_elegido][jugador].longitud && arraybarcos[barco_elegido][jugador].puesto_casillero == true; i++) {
//si el barco que se quería poner ya estaba puesto en otro lugar previamente, el programa revierte sus coordenadas a sus coordenadas anteriores y lo deja donde estaba.
                        arraybarcos[barco_elegido][jugador].Xcasilla[i] = tempX[i];
                        arraybarcos[barco_elegido][jugador].Ycasilla[i] = tempY[i];
                        
                    }

                    goto introduccion_posicion;//aquí utilizé un "goto" porque no había otra alternativa, si utilizo un "break" se van a producir las acciones siguientes, cosa que no quiero.
                }
            }//si se consigue cruzar este "for" detector de casillas ya ocupadas, se rellenan todas las casillas comprobadas con numeros '1'
            for (i = 0; i < arraybarcos[barco_elegido][jugador].longitud; i++) {

                (array_posicion)[arraybarcos[barco_elegido][jugador].Xcasilla[i]][arraybarcos[barco_elegido][jugador].Ycasilla[i]][jugador] = 1;
            }

            arraybarcos[barco_elegido][jugador].puesto_casillero = true;//el barco elegido se define como puesto en el casillero
            MostradorCasillero(array_posicion, dimensionXcasillero, dimensionYcasillero, jugador);//se muestra el casillero para ver cómo quedó el nuevo barco puesto 
            break;//escape de este bucle "while" que se encarga de posicionar el barco elegido.
        }

    }
}


bool Combate(int(&array_posicion)[26][26][2], string elapodo[2], int dimensionXcasillero, int dimensionYcasillero, propiedades_barco(&arraybarcos)[20][2]) {
    int i, j;
    int contador_aciertos[2], contador_fallos[2], contador_tiros[2];
    
    for (i = 0; i < 2; i++) {//se vuelven cero los contadores de aciertos y fallos al empezar, para que sea posible sumarles unos.
        contador_aciertos[i] = 0;
        contador_fallos[i] = 0;
    }
    
    srand(time(NULL));
    bool jugador_enturno = rand() % 1; //se elige aleatoriamente al jugador que va a empezar.
    cout << "Le toca al jugador " << jugador_enturno + 1 << " empezar, quien seria: " << elapodo[jugador_enturno] << endl; 
    
    char casillero_registrodeataques[26][26][2];//se crea el casillero de registro de ataques exitosos y fallados sobre el enemigo
    for (i = 0; i < 26; i++) {
        for (j = 0; j < 26; j++) {
            casillero_registrodeataques[i][j][0] = '?';//se vuelven todas las casillas de este casillero signos de interrogación ya que no se atacó ninguna casilla y no se sabe lo que hay ahí.
            casillero_registrodeataques[i][j][1] = '?';
        }
    }

    while (1) {

        cout << "Tu casillero actualmente, " << elapodo[jugador_enturno] << ": ";
        MostradorCasillero(array_posicion, dimensionXcasillero, dimensionYcasillero, jugador_enturno);//simplemente se muestra el casillero normal que muestra tu propio casillero para ver que has perdido...
		//...cada vez que vuelve a ser tu turno, tras el ataque del enemigo.
        cout << "El casillero de tus ataques al enemigo:";//se muestra el casillero mencionado anteriormente, de los signos de preguntas. Este registra los ataques hechos al enemigo.
        MostradorCasilleroCHAR(casillero_registrodeataques, dimensionXcasillero, dimensionYcasillero, jugador_enturno);


        for (;;) {

            cout << "Que casilla del enemigo (" << elapodo[!jugador_enturno] << ") quieres atacar? ";
            int coordenadaXintroducida;
            char letra_introducida;
            cin >> coordenadaXintroducida >> letra_introducida;
            coordenadaXintroducida--;//se le resta uno para adecuarse a la posicion [0] de los arrays 

            if (letra_introducida >= 'a' && letra_introducida <= 'z') {

                letra_introducida = toupper(letra_introducida);//se vuelve mayuscula la letra si es minuscula
            }
            int posXatacada = coordenadaXintroducida, posYatacada = int(letra_introducida - 'A');//finalmente se asignan las coordenadas de la posición atacada

            if (posXatacada < 0 || posXatacada > dimensionXcasillero || posYatacada < 0 || posYatacada > dimensionYcasillero) {//si está mal se repite el "for"

                cout << "Has introducido una posicion incorrecta o fuera de rango";
                continue;
            }

            if ((array_posicion)[posXatacada][posYatacada][!jugador_enturno] == 1) {// si hay un '1' en la casilla del enemigo atacada...

                (array_posicion)[posXatacada][posYatacada][!jugador_enturno] = 9;//... se vuelve un '9'
                contador_aciertos[jugador_enturno]++;//...se suma uno al contador de aciertos
                cout << "\n\nAL ATACAR LA POSICION " << coordenadaXintroducida + 1 << " " << letra_introducida << ", HAS DADO EN EL BLANCO.";
                
                casillero_registrodeataques[posXatacada][posYatacada][jugador_enturno] = 'X';//se marca una 'X' en la posicion correspondiente del casillero-registro de ataques al enemigo
                MostradorCasilleroCHAR(casillero_registrodeataques, dimensionXcasillero, dimensionYcasillero, jugador_enturno);// se muestra este mismo casillero, el '?' fue reemplazado por una 'X'

                int barcoatacado = 0;
                for (i = 0; i < 20; i++) {//este bucle 'for' chequea barco por barco del array "array_barcos"...
                    for (j = 0; j < arraybarcos[i][!jugador_enturno].longitud; j++) {

                        if (arraybarcos[i][!jugador_enturno].Xcasilla[j] == posXatacada && arraybarcos[i][!jugador_enturno].Ycasilla[j] == posYatacada) {//... si fue este el atacado.
							// tras cruzar el "if" que detecta si el barco que encontró el bucle "for" es el que fue atacado...
                            arraybarcos[i][!jugador_enturno].Xcasilla[j] = -1;//la casilla correspondiente destruida tomo como coordenada '-1' para diferenciarlas de las casillas no destruidas de los barcos...
                            arraybarcos[i][!jugador_enturno].Ycasilla[j] = -1;//... al tener estas ultimas coordenadas normales 
                            barcoatacado = i;
                        }
                    }
                }
                
                int contadorcasillasdestruidas = 0;
                for (i = 0; i < arraybarcos[barcoatacado][!jugador_enturno].longitud; i++) {

                    if (arraybarcos[barcoatacado][!jugador_enturno].Xcasilla[i] == -1) {

                        contadorcasillasdestruidas++;//se cuenta el numero de casillas destruidas de el barco atacado.
                    }
                }
                if (contadorcasillasdestruidas == arraybarcos[barcoatacado][!jugador_enturno].longitud) {
					//si el numero de casillas destruidas es igual a la longitud del barco atacado, este se detecta como destruido y se le avisa al jugador atacante.
                    cout << "\nIMPORTANTE: Con ese ataque has hundido un barco enemigo en su totalidad.";
                }

                break;//se finaliza el turno
            }
            else if ((array_posicion)[posXatacada][posYatacada][!jugador_enturno] == 9) {// si hay un '9' en la casilla atacada por el jugador, se le permite atacar otra casilla.

                cout << "\n\nHas elegido una casilla que ya habías atacado previamente\n";
                continue;//se "reinicia" el turno
            }
            else {// si hay un '0' en la casilla atacada...
            	
                casillero_registrodeataques[posXatacada][posYatacada][jugador_enturno] = '/';// el casillero de registro de ataques vuelve a esa posición un '/' (que significa que no hay nada ahí)
                contador_fallos[jugador_enturno]++;//se suma uno al contador de tiros fallados
                cout << "\n\nDesafortunadamente, no hay nada ahi.";
                MostradorCasilleroCHAR(casillero_registrodeataques, dimensionXcasillero, dimensionYcasillero, jugador_enturno);//se le muestra al atacante su casillero de registro de ataques.
                break;
            }
        }

        if (ContadorBarcos(arraybarcos, -1, !jugador_enturno) == ContadorBarcos(arraybarcos, 0, !jugador_enturno)) {//esto es un detector de si se ganó la partida tras el último ataque del jugador.
			
			for (i=0;i<2;i++)
            contador_tiros[i] = contador_fallos[i] + contador_aciertos[i];	//el contador de tiros totales es igual a la suma de los tiros fallados y los tiros acertados.
			//todo lo siguiente simplemente muestra las estadísticas de cada jugador en la partida.
            cout << "\n\n------------------------------------------------------------------------------------------------\n";
            cout << "                               " << elapodo[jugador_enturno] << " HA GANADO LA PARTIDA!!!      \n";
            cout << "------------------------------------------------------------------------------------------------\n\n\n";
            cout << "Estadisticas de final de juego: \n\n";

			cout << "Estadisticas de " << elapodo[jugador_enturno] << ":\n\n";
			
            cout << "Numero de aciertos: " << contador_aciertos[jugador_enturno] << endl;
            cout << "Numero de fallos:   " << contador_fallos[jugador_enturno] << endl;
            cout << "Precision:          " << fixed << setprecision(1) << float(contador_aciertos[jugador_enturno])*100/float(contador_tiros[jugador_enturno]) << "%\n\n";
            
            
            cout << "Barcos perdidos en total:    " << ContadorBarcos(arraybarcos, -1, jugador_enturno) << endl;
            if (ContadorBarcos (arraybarcos, 2, 0) > 0)
                cout << "Barcos pequenos perdidos:    " << ContadorBarcos(arraybarcos, -2, jugador_enturno) << endl;
            if (ContadorBarcos (arraybarcos, 3, 0) > 0)
                cout << "Barcos medianos perdidos:    " << ContadorBarcos(arraybarcos, -3, jugador_enturno) << endl;
            if (ContadorBarcos (arraybarcos, 4, 0) > 0)
                cout << "Barcos grandes perdidos:     " << ContadorBarcos(arraybarcos, -4, jugador_enturno) << endl;
            if (ContadorBarcos (arraybarcos, 5, 0) > 0)
                cout << "Barcos muy grandes perdidos: " << ContadorBarcos(arraybarcos, -5, jugador_enturno) << endl;
                
                
            cout << "\n------------------------------------------------------------------------------------------------\n\n";

			
			cout << "Estadisticas de " << elapodo[!jugador_enturno] << ":\n\n";
			
            cout << "Numero de aciertos: " << contador_aciertos[!jugador_enturno] << endl;
            cout << "Numero de fallos:   " << contador_fallos[!jugador_enturno] << endl;
            cout << "Precision:          " << fixed <<setprecision(1) << float(contador_aciertos[!jugador_enturno])*100/float(contador_tiros[!jugador_enturno]) << "%\n\n";
            
            
            cout << "Barcos perdidos en total:    " << ContadorBarcos(arraybarcos, -1, !jugador_enturno) << endl;
            if (ContadorBarcos (arraybarcos, 2, 0) > 0)
                cout << "Barcos pequenos perdidos:    " << ContadorBarcos(arraybarcos, -2, !jugador_enturno) << endl;
            if (ContadorBarcos (arraybarcos, 3, 0) > 0)
                cout << "Barcos medianos perdidos:    " << ContadorBarcos(arraybarcos, -3, !jugador_enturno) << endl;
            if (ContadorBarcos (arraybarcos, 4, 0) > 0)
                cout << "Barcos grandes perdidos:     " << ContadorBarcos(arraybarcos, -4, !jugador_enturno) << endl;
            if (ContadorBarcos (arraybarcos, 5, 0) > 0)
                cout << "Barcos muy grandes perdidos: " << ContadorBarcos(arraybarcos, -5, !jugador_enturno) << endl;


            return jugador_enturno;//se devuelve al jugador ganador y se sale de esta función
        }


        jugador_enturno = !jugador_enturno;
//el jugador en turno se vuelve el jugador que no estaba en el turno, y después de los temporizadores se produce otra instancia de este bucle "while", siendo el jugador en turno nuevo el oponente del actual
        sleep(3);

        cout << "\n\n----------------------------------------------------------------------------------------\n\nHa finalizado el turno de " << elapodo[!jugador_enturno] << ", ahora empieza el turno de " << elapodo[jugador_enturno] << endl << endl;

        cout << "En tres segundos empieza el turno de " << elapodo[jugador_enturno] << endl;
        sleep(1);
        cout << "En dos segundos empieza el turno de " << elapodo[jugador_enturno] << endl;
        sleep(1);
        cout << "En un segundo empieza el turno de " << elapodo[jugador_enturno] << endl << endl;
        sleep(1);
    }
}


void MostradorCasillero(int casillero[26][26][2], int dimensionXcasillero, int dimensionYcasillero, bool jugador) {//Esta funcion muestra el estado actual del casillero del jugador X

    int i, j;

    cout << "\n\n  "; //Esta sección escribe los numeros de arriba que indican la coordenada X de cierta columna.
    for (i = 0; i < dimensionXcasillero && i < 10; i++) {

        cout << " " << i + 1;
    }
    for (; i < dimensionXcasillero; i++) {//Cuando se llegan a numeros de dos digitos (10 11 12 13...) el programa escribe estos sin escribir un espacio en el medio (10111213...) para que no se descoordinen con la columna a la que se refieren

        cout << i + 1;
    }
    cout << "\n\n";

    char letra_fila = 'A';//Esta sección escribe las letras de la izquierda que indican la coordenada Y de cierta fila.
    for (i = 0; i < dimensionYcasillero; i++) {

        cout << letra_fila++ << ": ";

        for (j = 0; j < dimensionXcasillero; j++) {

            cout << casillero[j][i][jugador] << " ";
        }

        cout << endl;
    }
    cout << endl << endl;
    return;
}


void MostradorCasilleroCHAR(char casillero[26][26][2], int dimensionXcasillero, int dimensionYcasillero, bool jugador) {//Esta funcion muestra el estado actual del casillero del jugador X
    int i, j;
    cout << "\n\n  "; //Esta sección escribe los numeros de arriba que indican la coordenada X de cierta columna.
    for (i = 0; i < dimensionXcasillero && i < 10; i++) {
        cout << " " << i + 1;
    }
    for (; i < dimensionXcasillero; i++) {//Cuando se llegan a numeros de dos digitos (10 11 12 13...) el programa escribe estos sin escribir un espacio en el medio (10111213...) para que no se descoordinen con la columna a la que se refieren
        cout << i + 1;
    }
    cout << "\n\n";
    char letra_fila = 'A';//Esta sección escribe las letras de la izquierda que indican la coordenada Y de cierta fila.
    for (i = 0; i < dimensionYcasillero; i++) {
        cout << letra_fila++ << ": ";
        for (j = 0; j < dimensionXcasillero; j++) {
            cout << casillero[j][i][jugador] << " ";
        }
        cout << endl;
    }
    cout << endl << endl;
    return;
}


int ContadorBarcos(propiedades_barco arraybarcos[20][2], int tipobarco_acontar, bool jugador) {

    int i;

    if (tipobarco_acontar == -5) {

        int contador_bhundidosmuygrandes = 0;
        for (i = 0; i < 20; i++) {//contaduría de cada barco

            int casillas_hundidas = 0;
            for (int j = 0; j < arraybarcos[i][jugador].longitud &&  arraybarcos[i][jugador].longitud == 5; j++) {//contaduría de cada casilla del barco "i"

                if (arraybarcos[i][jugador].Xcasilla[j] == -1) {

                    casillas_hundidas++;
                }
            }

            if (casillas_hundidas == arraybarcos[i][jugador].longitud) {

                contador_bhundidosmuygrandes ++;
            }
        }
        return contador_bhundidosmuygrandes ;
    }

    if (tipobarco_acontar == -4) {

        int contador_bhundidosgrandes = 0;
        for (i = 0; i < 20; i++) {//contaduría de cada barco

            int casillas_hundidas = 0;
            for (int j = 0; j < arraybarcos[i][jugador].longitud &&  arraybarcos[i][jugador].longitud == 4; j++) {//contaduría de cada casilla del barco "i"

                if (arraybarcos[i][jugador].Xcasilla[j] == -1) {

                    casillas_hundidas++;
                }
            }

            if (casillas_hundidas == arraybarcos[i][jugador].longitud) {

                contador_bhundidosgrandes ++;
            }
        }
        return contador_bhundidosgrandes ;
    }

    if (tipobarco_acontar == -3) {

        int contador_bhundidosmedianos = 0;
        for (i = 0; i < 20; i++) {//contaduría de cada barco

            int casillas_hundidas = 0;
            for (int j = 0; j < arraybarcos[i][jugador].longitud &&  arraybarcos[i][jugador].longitud == 3; j++) {//contaduría de cada casilla del barco "i"

                if (arraybarcos[i][jugador].Xcasilla[j] == -1) {

                    casillas_hundidas++;
                }
            }

            if (casillas_hundidas == arraybarcos[i][jugador].longitud) {

                contador_bhundidosmedianos++;
            }
        }
        return contador_bhundidosmedianos;
    }

    if (tipobarco_acontar == -2) {

        int contador_bhundidospequenos = 0;
        for (i = 0; i < 20; i++) {//contaduría de cada barco

            int casillas_hundidas = 0;
            for (int j = 0; j < arraybarcos[i][jugador].longitud &&  arraybarcos[i][jugador].longitud == 2; j++) {//contaduría de cada casilla del barco "i"

                if (arraybarcos[i][jugador].Xcasilla[j] == -1) {

                    casillas_hundidas++;
                }
            }

            if (casillas_hundidas == arraybarcos[i][jugador].longitud) {

                contador_bhundidospequenos++;
            }
        }
        return contador_bhundidospequenos;
    }
    if (tipobarco_acontar == -1) {

        int contador_bhundidostotales = 0;
        for (i = 0; i < 20; i++) {//contaduría de cada barco

            int casillas_hundidas = 0;
            for (int j = 0; j < arraybarcos[i][jugador].longitud; j++) {//contaduría de cada casilla del barco "i"

                if (arraybarcos[i][jugador].Xcasilla[j] == -1) {

                    casillas_hundidas++;
                }
            }

            if (casillas_hundidas == arraybarcos[i][jugador].longitud) {

                contador_bhundidostotales++;
            }
        }
        return contador_bhundidostotales;
    }
    if (tipobarco_acontar == 0) {

        int contador_btotales = 0;
        for (i = 0; i < 20; i++) {
            if (arraybarcos[i][0].longitud >= 2 && arraybarcos[i][0].longitud <= 5) {
                contador_btotales++;
            }
        }

        return contador_btotales;
    }
    if (tipobarco_acontar == 1) {

        int contador_bpuestos = 0;
        for (i = 0; i < 20; i++) {
            if (arraybarcos[i][jugador].puesto_casillero == true) {

                contador_bpuestos++;
            }
        }
        return contador_bpuestos;
    }
    if (tipobarco_acontar == 2) {

        int contador_bpequenos = 0;
        for (i = 0; i < 20; i++) {
            if (arraybarcos[i][0].longitud == 2) {

                contador_bpequenos++;
            }
        }
        return contador_bpequenos;
    }
    if (tipobarco_acontar == 3) {

        int contador_bmedianos = 0;
        for (i = 0; i < 20; i++) {
            if (arraybarcos[i][0].longitud == 3) {

                contador_bmedianos++;
            }
        }
        return contador_bmedianos;
    }
    if (tipobarco_acontar == 4) {

        int contador_bgrandes = 0;
        for (i = 0; i < 20; i++) {
            if (arraybarcos[i][0].longitud == 4) {

                contador_bgrandes++;
            }
        }
        return contador_bgrandes;
    }
    if (tipobarco_acontar == 5) {

        int contador_bmuygrandes = 0;
        for (i = 0; i < 20; i++) {
            if (arraybarcos[i][0].longitud == 5) {

                contador_bmuygrandes++;
            }
        }
        return contador_bmuygrandes;
    }
    return 0;
}



