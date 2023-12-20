#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iomanip>    

using namespace std;

struct BoatProperties {//se crea el tipo de struct "BoatProperties", que contiene todas los tipos de propiedades que puede tener un barco; las coordenadas X e Y de cada una de sus cinco (o menos) casillas, la length del barco, su orientacion en el tablero y si está colocado o no en este.

    int x_pos[5], y_pos[5], length;
    bool orientation, placed;
};

void createBoats(BoatProperties(&boats_array)[20][2]);

void placeBoats(int(&array_posicion)[26][26][2], string nick, BoatProperties(&boats_array)[20][2], int size_x, int size_y, bool player);

bool startMatch(int(&array_posicion)[26][26][2], string nick[2], int size_x, int size_y, BoatProperties(&boats_array)[20][2]);

void showGrid(int casillero[26][26][2], int size_x, int size_y, bool player);
void ShowGrid2(char casillero[26][26][2], int size_x, int size_y, bool player);

int countBoats(BoatProperties boats_array[20][2], int type_of_boat_to_count, bool player);

//no utilizé ninguna variable global.

int main() {
	
    int i;

    string apodos[2];//se rellenan las dos posiciones del array "apodos" con el apodo de cada player
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

        int array_casillerodoble[26][26][2];//casillero dual (una matriz 7x7 para cada player). Cada dimension: eje x, eje y, jugadores. No registra los barcos para facilitar la visualizacion del tablero mediante un bucle "for", al no tener que involucrar las dimensiones del barco que es y sus respectivas casillas que ocupa.

        for (int i = 0; i < 26; i++){
            for (int j = 0; j < 26; j++){
                for (int k = 0; k < 2; k++){
                    array_casillerodoble[i][j][k] = 0; // se vacía el casillero de los barcos
                }
            }
        }

        BoatProperties boats_array[20][2];
        for (i = 0; i < 20; i++) {
            for (int j = 0; j < 2; j++){
                for (int k = 0; k < 5; k++){
                    boats_array[i][j].length = -100;
                    boats_array[i][j].placed = false;// al empezar, todos los barcos por "default" no estan puestos en el casillero.
                    boats_array[i][j].x_pos[k] = -2;
                    boats_array[i][j].y_pos[k] = -2;
                }
            }
        }

        createBoats(boats_array);//se llama a la funcion que crea todos los barcos

        //se empieza a crear el casillero
        int size_x, size_y;

        for (;;) {

            cout << "\nQue dimensiones del casillero quieren? Escriba la dimension horizontal [espacio] dimension vertical\n";
            cout << "Ejemplos: 10 10, 5 6, 26 5. EL MINIMO ES CINCO Y EL MAXIMO ES VEINTISEIS\n";
            cin >> size_x >> size_y;//el casillero toma el tamaño horizontal y vertical que quieran los jugadores

            if ((size_x > 26 || size_y > 26) || (size_x < 5 || size_y < 5)) {
                //si el casillero es demasiado grande o demasiado pequeño tal que no quepan los barcos muy grandes, se le obliga al player reintroducirla. El programa produce otra iteracion de este bucle 'for' infinito
                cout << "\nHas introducido una dimension demasiado pequeña o grande\n";
                continue;
            }
            else break;//si el player introduce unas dimensiones sensatas del casillero, el programa escapa del bucle "for" infinito y prosigue.  
        }

        placeBoats(array_casillerodoble, apodos[0], boats_array, size_x, size_y, 0);
        placeBoats(array_casillerodoble, apodos[1], boats_array, size_x, size_y, 1);

        bool ganador(startMatch(array_casillerodoble, apodos, size_x, size_y, boats_array));

        cout << endl << apodos[!ganador] << ", quieres la revancha? Escribe 'si' si quieres jugar otra partida. Si quieres cerrar el juego, escribe cualquier cosa: ";

        string respuesta;
        cin >> respuesta;

        if (respuesta != "si" && respuesta != "SI") exit(0); 
    }
}


void createBoats(BoatProperties(&boats_array)[20][2]) {//esta funcion crea todos los barcos y los coloca ordenadamente en el array "boats_array", pero no les asigna una posicion en el casillero

    int i, small_boats_count, med_boats_count, big_boats_count, huge_boats_count, numero_btotales;

    for (;;) {

        cout << "\nCuantos barcos pequenos, medianos, grandes, muy grandes quieren en juego? (Cada tipo ocupa respectivamente dos, tres, cuatro y cinco casillas).\n";
        cout << "El formato para introducirlo es: [cantidad de pequenos][espacio][cantidad de medianos][espacio][cantidad de grandes][espacio][cantidad de muy grandes]\n";
        cout << "Ejemplos: 7 5 2 0, 9 3 2 1, 9 4 0 0, 0 3 3 0, etc. AVISO: el total maximo de barcos es veinte.\n";

        //los jugadores eligen cuantos barcos de cada tipo quieren que hayan en juego (los dos jugadores reciben la misma cantidad de cada tipo de barco)

        cin >> small_boats_count >> med_boats_count >> big_boats_count >> huge_boats_count;//se introduce la cantidad de cada tipo de barco en su respectiva variable

        numero_btotales = small_boats_count + med_boats_count + big_boats_count + huge_boats_count; //se hace el sumatorio de todos los barcos que hay, y si sobrepasa veinte, el programa te hace reintroducirlos

        if (numero_btotales > 20 || numero_btotales < 1) {

            cout << "El programa no admite una cantidad de barcos totales mayor a veinte ni menor a uno.\n";
            continue; //el programa no permite al usuario salir de este bucle "for" infinito hasta que haya puesto una cantidad de barcos sensata
        }
        else {

            break; //el programa sale del bucle "for" si el usuario introdujo una cantidad de barcos permitida.
        }
    }
    //lo siguiente parece muy complejo, pero en realidad lo unico que se hace es rellenar el array "boats_array" con cada barco existente. Se rellena ordenadamente sin dejar huecos vacios o rellenando posiciones ya rellenadas.
    for (i = 0; i < small_boats_count; i++) {

        boats_array[i][0].length = 2; //se rellenan los barcos pequeños de length dos para el player 0
        boats_array[i][1].length = 2; //se rellenan los barcos pequeños de length dos para el player 1
    }
    for (i = 0; i < med_boats_count; i++) {

        boats_array[i + small_boats_count][0].length = 3; //se rellenan los barcos medianos de length tres para el player 0
        boats_array[i + small_boats_count][1].length = 3; //se rellenan los barcos medianos de length tres para el player 1
    }
    for (i = 0; i < big_boats_count; i++) {

        boats_array[i + small_boats_count + med_boats_count][0].length = 4; //se rellenan los barcos grandes de length cuatro para el player 0
        boats_array[i + small_boats_count + med_boats_count][1].length = 4; //se rellenan los barcos grandes de length cuatro para el player 1
    }
    for (i = 0; i < huge_boats_count; i++) {

        boats_array[i + small_boats_count + med_boats_count + big_boats_count][0].length = 5; //se rellenan los barcos muy grandes de length cinco para el player 0
        boats_array[i + small_boats_count + med_boats_count + big_boats_count][1].length = 5; //se rellenan los barcos muy grandes de length cinco para el player 1
    }

    return;//se cierra la funcion de crear barcos para ambos jugadores
}


void placeBoats(int(&array_posicion)[26][26][2], string nick, BoatProperties(&boats_array)[20][2], int size_x, int size_y, bool player) {
    int i;

    cout << "\n\n-----------------------------------------DISPOSICION DE LOS BARCOS-----------------------------------------\n\n";

    cout << "\nLe toca a " << nick << " posicionar sus barcos; el otro player, NO MIRE \n\n";

    cout << "Este es su casillero, " << nick << ": ";
    showGrid(array_posicion, size_x, size_y, player);//se le muestra el casillero al player x, mediante una funcion independiente.
    cout << "Los numeros de arriba indican la columna, y las letras de la izquierda la fila.\n\n";

    int barco_elegido;// se crea la variable barco_elegido, que se va a utilizar posteriormente para manipular el array "array_posicion".
    for (;;) {

        cout << "Que barco quieres posicionar o reposicionar? (escribir el numero): \n";
        for (i = 0; i < countBoats(boats_array, 0, 0); i++) {
//se muestran todos los barcos disponibles que hay para posicionar, enumerados en orden para diferenciar barcos del mismo tipo
            cout << i + 1 << ": barco ";
            switch (boats_array[i][player].length) {

            case 2:
                cout << "pequeno " << i + 1;
                break;
            case 3:
                cout << "mediano " << (i + 1) - countBoats(boats_array, 2, 0);
                break;
            case 4:
                cout << "grande " << (i + 1) - (countBoats(boats_array, 2, 0) + (countBoats(boats_array, 3, 0)));
                break;
            case 5:
                cout << "muy grande " << (i + 1 - (countBoats(boats_array, 2, 0) + (countBoats(boats_array, 3, 0) + (countBoats(boats_array, 4, 0)))));
                break;
            }
            cout << endl;
        }// Aqui termina el mostrador de los barcos disponibles a posicionar.
        if (countBoats(boats_array, 1, player) == countBoats(boats_array, 0, 0)) {
//Si el numero de barcos posicionados es igual al numero de barcos totales, se le permite al player x terminar su preparación
            cout << "Ya has posicionado todos tus barcos disponibles, si quieres terminar tu preparacion, escribe '100'\n";
        }

        cin >> barco_elegido;
        barco_elegido--; // se le resta una unidad al int "barco_elegido" para que se adapte a las posiciones de los arrays, que empiezan en cero.

        if (barco_elegido == 99 && countBoats(boats_array, 1, player) == countBoats(boats_array, 0, 0)) {
//si el player introduce un 99 (100 - 1) y tiene disponible la opción de terminar su preparación
            return;
        }
        if (barco_elegido < 0 || barco_elegido > countBoats(boats_array, 0, 0)) {
//si el player elige un barco negativo o mayor al numero de barcos disponibles, el programa le obliga a elegir otro.
            cout << "Has elegido un barco inexistente.\n\n";
            continue;
        }

        showGrid(array_posicion, size_x, size_y, player);

        cout << "\nDonde lo quieres poner y con que orientation?, escribe [numero de la columna][espacio][letra de la fila][espacio][H ó V] para elegir la posicion y la orientation del barco.\n";
        cout << "Ejemplos: 3 A V, 4 D H, 8 C V  9 J V. La 'H' significa que se va a poner horizontalmente y la 'V' significa que se va a poner verticalmente.\n";
        cout << "Si quieres cambiar otro barco, escribe '10 * *'\n";

        while (1) {

        introduccion_posicion:

            int input_num_coord;
            char input_letter_coord;
            char letra_deorientacionintroducida;


            cin >> input_num_coord >> input_letter_coord >> letra_deorientacionintroducida;
            if (input_num_coord == 10 && input_letter_coord == '*' && letra_deorientacionintroducida == '*') {
			//si se escribió "10 * *", el programa vuelve a la pantalla de selección de barco a mover.
                break;
            }
                                //si se escribió una H o V cursiva, esta se vuelva mayúscula para no causar problemas
            bool orientacionintroducida = toupper(letra_deorientacionintroducida) - 'H';// la letra introducida 'H' ó 'V' se vuelve un 0 o 1 booleano al restarse el valor de 'H'
            

            boats_array[barco_elegido][player].orientation = orientacionintroducida;
            

            if (input_letter_coord >= 'a' && input_letter_coord <= 'z') {

                input_letter_coord = toupper(input_letter_coord);
            }
            int coordenadaYintroducida = int(input_letter_coord - 'A');
			//se le resta 'A' a la letra-coordenada para hacer que la coordenada tenga una base numérica en vez de alfabética-ascii. De esta forma es más facil de operar.

            if (--input_num_coord < 0 || input_num_coord > size_x || coordenadaYintroducida < 0 || coordenadaYintroducida > size_y) {

                cout << "Has introducido una orientation o posicion incorrecta, o esta ultima esta fuera de rango del casillero. Por favor, introduce una posicion y orientation correctamente:";
                continue;
            }

            //este "if" borra todas las casillas ocupadas anteriormente por un barco que acabó de ser movido ahora mismo, así no hay numeros '1' donde en realidad no hay nada
            if (boats_array[barco_elegido][player].placed == true) {

                for (i = 0; i < boats_array[barco_elegido][player].length; i++) {

                    (array_posicion)[boats_array[barco_elegido][player].x_pos[i]][boats_array[barco_elegido][player].y_pos[i]][player] = 0;
                }
            }

            bool grid_bounds_reached = false;
            int temp_x[5]; 
            int temp_y[5];

            for (i = 0; i < boats_array[barco_elegido][player].length; i++) {

                temp_x[i] = boats_array[barco_elegido][player].x_pos[i]; //se guarda los datos numericos de las coordenadas previas antes de reemplazar las coordenadas por las nuevas coordenadas...
                temp_y[i] = boats_array[barco_elegido][player].y_pos[i];//...de esta forma, si se cancela el movimiento, se puede revertir el cambio poniendole al barco de vuelta las coordenadas anteriores.
            }

            for (i = 0; i < boats_array[barco_elegido][player].length; i++) {

                if (boats_array[barco_elegido][player].orientation == 0) {//si el barco está puesto horizontalmente, se hace esto

                    boats_array[barco_elegido][player].y_pos[i] = coordenadaYintroducida;//si está puesto horizontalmente el barco, la coordenada Y de las i casillas no varía

                    if ( ! grid_bounds_reached ) 
                        boats_array[barco_elegido][player].x_pos[i] = input_num_coord + i;//si no se alcanza el borde derecho del tablero, se rellena de izquierda a derecha.    
                    else 
                        boats_array[barco_elegido][player].x_pos[i] = input_num_coord - i;//si se alcanzó el borde derecho del tablero, se vuelve a rellenar pero de derecha a izquierda en vez de izquierda a derecha.
                    

                    if (boats_array[barco_elegido][player].x_pos[i] == size_x && grid_bounds_reached == false) { //si se llega al borde derecho del tablero...      	
                        i = -1; //...se reinicia el bucle "for"
                        grid_bounds_reached = true;//y esto se pone con valor "true" para que no se vuelva a meter el programa en este "if" y ejecute el procedimiento si se alcanzó el borde derecho del tablero
                        continue; // la "i" se vuelve cero de vuelta
                    }

                }
                else if (boats_array[barco_elegido][player].orientation == 1) {//si el barco está puesto verticalmente, se hace esto

                    boats_array[barco_elegido][player].x_pos[i] = input_num_coord;//si está puesto verticalmente el barco, la coordenada X de las i casillas no varía

                    if (grid_bounds_reached == false) {

                        boats_array[barco_elegido][player].y_pos[i] = coordenadaYintroducida + i;//si no se alcanza el borde inferior del tablero, se rellena de arriba a abajo
                    }
                    else {
                    	
                        boats_array[barco_elegido][player].y_pos[i] = coordenadaYintroducida - i;//si se alcanzó el borde inferior del tablero, se vuelve a rellenar pero de abajo a arriba en vez de arriba a abajo.
                    }

                    if (boats_array[barco_elegido][player].y_pos[i] == size_y && grid_bounds_reached == false) {//si se llega al borde inferior del tablero...
                    	
                        i = -1;//...se reinicia el bucle "for"
                        grid_bounds_reached = true;//y esto se pone con valor "true" para que no se vuelva a meter el programa en este "if" y ejecute el procedimiento si se alcanzó el limite inferior del tablero
                        continue;// la "i" se vuelve cero de vuelta
                    }
                }
            }

            for (i = 0; i < boats_array[barco_elegido][player].length; i++) {

                if ((array_posicion)[boats_array[barco_elegido][player].x_pos[i]][boats_array[barco_elegido][player].y_pos[i]][player] == 1) {
// si se detecta que una de las casillas que se quieren ocupar con el barco que se quiere poner ya está rellena, el programa te avisa y te obliga a elegir otra posicion o poner otro barco.
                    cout << "Has elegido una posicion que implicaria ocupar casillas de otro barco ya puesto\nSi quieres cambiar otro barco, escribe '10 * *'. Si quieres elegir otra posicion, escribe otra posicion y orientation: ";

                    for (i = 0; i < boats_array[barco_elegido][player].length && boats_array[barco_elegido][player].placed == true; i++) {
//si el barco que se quería poner ya estaba puesto en otro lugar previamente, el programa revierte sus coordenadas a sus coordenadas anteriores y lo deja donde estaba.
                        boats_array[barco_elegido][player].x_pos[i] = temp_x[i];
                        boats_array[barco_elegido][player].y_pos[i] = temp_y[i];
                        
                    }

                    goto introduccion_posicion;//aquí utilizé un "goto" porque no había otra alternativa, si utilizo un "break" se van a producir las acciones siguientes, cosa que no quiero.
                }
            }//si se consigue cruzar este "for" detector de casillas ya ocupadas, se rellenan todas las casillas comprobadas con numeros '1'
            for (i = 0; i < boats_array[barco_elegido][player].length; i++) {

                (array_posicion)[boats_array[barco_elegido][player].x_pos[i]][boats_array[barco_elegido][player].y_pos[i]][player] = 1;
            }

            boats_array[barco_elegido][player].placed = true;//el barco elegido se define como puesto en el casillero
            showGrid(array_posicion, size_x, size_y, player);//se muestra el casillero para ver cómo quedó el nuevo barco puesto 
            break;//escape de este bucle "while" que se encarga de posicionar el barco elegido.
        }

    }
}


bool startMatch(int(&array_posicion)[26][26][2], string nick[2], int size_x, int size_y, BoatProperties(&boats_array)[20][2]) {
    int i, j;
    int contador_aciertos[2], contador_fallos[2], contador_tiros[2];
    
    for (i = 0; i < 2; i++) {//se vuelven cero los contadores de aciertos y fallos al empezar, para que sea posible sumarles unos.
        contador_aciertos[i] = 0;
        contador_fallos[i] = 0;
    }
    
    srand(time(NULL));
    bool player_on_turn = rand() % 1; //se elige aleatoriamente al player que va a empezar.
    cout << "Le toca al player " << player_on_turn + 1 << " empezar, quien seria: " << nick[player_on_turn] << endl; 
    
    char casillero_registrodeataques[26][26][2];//se crea el casillero de registro de ataques exitosos y fallados sobre el enemigo
    for (i = 0; i < 26; i++) {
        for (j = 0; j < 26; j++) {
            casillero_registrodeataques[i][j][0] = '?';//se vuelven todas las casillas de este casillero signos de interrogación ya que no se atacó ninguna casilla y no se sabe lo que hay ahí.
            casillero_registrodeataques[i][j][1] = '?';
        }
    }

    while (1) {

        cout << "Tu casillero actualmente, " << nick[player_on_turn] << ": ";
        showGrid(array_posicion, size_x, size_y, player_on_turn);//simplemente se muestra el casillero normal que muestra tu propio casillero para ver que has perdido...
		//...cada vez que vuelve a ser tu turno, tras el ataque del enemigo.
        cout << "El casillero de tus ataques al enemigo:";//se muestra el casillero mencionado anteriormente, de los signos de preguntas. Este registra los ataques hechos al enemigo.
        ShowGrid2(casillero_registrodeataques, size_x, size_y, player_on_turn);


        for (;;) {

            cout << "Que casilla del enemigo (" << nick[!player_on_turn] << ") quieres atacar? ";
            int input_num_coord;
            char input_letter_coord;
            cin >> input_num_coord >> input_letter_coord;
            input_num_coord--;//se le resta uno para adecuarse a la posicion [0] de los arrays 

            if (input_letter_coord >= 'a' && input_letter_coord <= 'z') {

                input_letter_coord = toupper(input_letter_coord);//se vuelve mayuscula la letra si es minuscula
            }
            int posXatacada = input_num_coord, posYatacada = int(input_letter_coord - 'A');//finalmente se asignan las coordenadas de la posición atacada

            if (posXatacada < 0 || posXatacada > size_x || posYatacada < 0 || posYatacada > size_y) {//si está mal se repite el "for"

                cout << "Has introducido una posicion incorrecta o fuera de rango";
                continue;
            }

            if ((array_posicion)[posXatacada][posYatacada][!player_on_turn] == 1) {// si hay un '1' en la casilla del enemigo atacada...

                (array_posicion)[posXatacada][posYatacada][!player_on_turn] = 9;//... se vuelve un '9'
                contador_aciertos[player_on_turn]++;//...se suma uno al contador de aciertos
                cout << "\n\nAL ATACAR LA POSICION " << input_num_coord + 1 << " " << input_letter_coord << ", HAS DADO EN EL BLANCO.";
                
                casillero_registrodeataques[posXatacada][posYatacada][player_on_turn] = 'X';//se marca una 'X' en la posicion correspondiente del casillero-registro de ataques al enemigo
                ShowGrid2(casillero_registrodeataques, size_x, size_y, player_on_turn);// se muestra este mismo casillero, el '?' fue reemplazado por una 'X'

                int barcoatacado = 0;
                for (i = 0; i < 20; i++) {//este bucle 'for' chequea barco por barco del array "array_barcos"...
                    for (j = 0; j < boats_array[i][!player_on_turn].length; j++) {

                        if (boats_array[i][!player_on_turn].x_pos[j] == posXatacada && boats_array[i][!player_on_turn].y_pos[j] == posYatacada) {//... si fue este el atacado.
							// tras cruzar el "if" que detecta si el barco que encontró el bucle "for" es el que fue atacado...
                            boats_array[i][!player_on_turn].x_pos[j] = -1;//la casilla correspondiente destruida tomo como coordenada '-1' para diferenciarlas de las casillas no destruidas de los barcos...
                            boats_array[i][!player_on_turn].y_pos[j] = -1;//... al tener estas ultimas coordenadas normales 
                            barcoatacado = i;
                        }
                    }
                }
                
                int contadorcasillasdestruidas = 0;
                for (i = 0; i < boats_array[barcoatacado][!player_on_turn].length; i++) {

                    if (boats_array[barcoatacado][!player_on_turn].x_pos[i] == -1) {

                        contadorcasillasdestruidas++;//se cuenta el numero de casillas destruidas de el barco atacado.
                    }
                }
                if (contadorcasillasdestruidas == boats_array[barcoatacado][!player_on_turn].length) {
					//si el numero de casillas destruidas es igual a la length del barco atacado, este se detecta como destruido y se le avisa al player atacante.
                    cout << "\nIMPORTANTE: Con ese ataque has hundido un barco enemigo en su totalidad.";
                }

                break;//se finaliza el turno
            }
            else if ((array_posicion)[posXatacada][posYatacada][!player_on_turn] == 9) {// si hay un '9' en la casilla atacada por el player, se le permite atacar otra casilla.

                cout << "\n\nHas elegido una casilla que ya habías atacado previamente\n";
                continue;//se "reinicia" el turno
            }
            else {// si hay un '0' en la casilla atacada...
            	
                casillero_registrodeataques[posXatacada][posYatacada][player_on_turn] = '/';// el casillero de registro de ataques vuelve a esa posición un '/' (que significa que no hay nada ahí)
                contador_fallos[player_on_turn]++;//se suma uno al contador de tiros fallados
                cout << "\n\nDesafortunadamente, no hay nada ahi.";
                ShowGrid2(casillero_registrodeataques, size_x, size_y, player_on_turn);//se le muestra al atacante su casillero de registro de ataques.
                break;
            }
        }

        if (countBoats(boats_array, -1, !player_on_turn) == countBoats(boats_array, 0, !player_on_turn)) {//esto es un detector de si se ganó la partida tras el último ataque del player.
			
			for (i=0;i<2;i++)
            contador_tiros[i] = contador_fallos[i] + contador_aciertos[i];	//el contador de tiros totales es igual a la suma de los tiros fallados y los tiros acertados.
			//todo lo siguiente simplemente muestra las estadísticas de cada player en la partida.
            cout << "\n\n------------------------------------------------------------------------------------------------\n";
            cout << "                               " << nick[player_on_turn] << " HA GANADO LA PARTIDA!!!      \n";
            cout << "------------------------------------------------------------------------------------------------\n\n\n";
            cout << "Estadisticas de final de juego: \n\n";

			cout << "Estadisticas de " << nick[player_on_turn] << ":\n\n";
			
            cout << "Numero de aciertos: " << contador_aciertos[player_on_turn] << endl;
            cout << "Numero de fallos:   " << contador_fallos[player_on_turn] << endl;
            cout << "Precision:          " << fixed << setprecision(1) << float(contador_aciertos[player_on_turn])*100/float(contador_tiros[player_on_turn]) << "%\n\n";
            
            
            cout << "Barcos perdidos en total:    " << countBoats(boats_array, -1, player_on_turn) << endl;
            if (countBoats (boats_array, 2, 0) > 0)
                cout << "Barcos pequenos perdidos:    " << countBoats(boats_array, -2, player_on_turn) << endl;
            if (countBoats (boats_array, 3, 0) > 0)
                cout << "Barcos medianos perdidos:    " << countBoats(boats_array, -3, player_on_turn) << endl;
            if (countBoats (boats_array, 4, 0) > 0)
                cout << "Barcos grandes perdidos:     " << countBoats(boats_array, -4, player_on_turn) << endl;
            if (countBoats (boats_array, 5, 0) > 0)
                cout << "Barcos muy grandes perdidos: " << countBoats(boats_array, -5, player_on_turn) << endl;
                
                
            cout << "\n------------------------------------------------------------------------------------------------\n\n";

			
			cout << "Estadisticas de " << nick[!player_on_turn] << ":\n\n";
			
            cout << "Numero de aciertos: " << contador_aciertos[!player_on_turn] << endl;
            cout << "Numero de fallos:   " << contador_fallos[!player_on_turn] << endl;
            cout << "Precision:          " << fixed <<setprecision(1) << float(contador_aciertos[!player_on_turn])*100/float(contador_tiros[!player_on_turn]) << "%\n\n";
            
            
            cout << "Barcos perdidos en total:    " << countBoats(boats_array, -1, !player_on_turn) << endl;
            if (countBoats (boats_array, 2, 0) > 0)
                cout << "Barcos pequenos perdidos:    " << countBoats(boats_array, -2, !player_on_turn) << endl;
            if (countBoats (boats_array, 3, 0) > 0)
                cout << "Barcos medianos perdidos:    " << countBoats(boats_array, -3, !player_on_turn) << endl;
            if (countBoats (boats_array, 4, 0) > 0)
                cout << "Barcos grandes perdidos:     " << countBoats(boats_array, -4, !player_on_turn) << endl;
            if (countBoats (boats_array, 5, 0) > 0)
                cout << "Barcos muy grandes perdidos: " << countBoats(boats_array, -5, !player_on_turn) << endl;


            return player_on_turn;//se devuelve al player ganador y se sale de esta función
        }


        player_on_turn = !player_on_turn;
//el player en turno se vuelve el player que no estaba en el turno, y después de los temporizadores se produce otra instancia de este bucle "while", siendo el player en turno nuevo el oponente del actual
        sleep(3);

        cout << "\n\n----------------------------------------------------------------------------------------\n\nHa finalizado el turno de " << nick[!player_on_turn] << ", ahora empieza el turno de " << nick[player_on_turn] << endl << endl;

        cout << "En tres segundos empieza el turno de " << nick[player_on_turn] << endl;
        sleep(1);
        cout << "En dos segundos empieza el turno de " << nick[player_on_turn] << endl;
        sleep(1);
        cout << "En un segundo empieza el turno de " << nick[player_on_turn] << endl << endl;
        sleep(1);
    }
}


void showGrid(int casillero[26][26][2], int size_x, int size_y, bool player) {//Esta funcion muestra el estado actual del casillero del player X

    int i, j;

    cout << "\n\n  "; //Esta sección escribe los numeros de arriba que indican la coordenada X de cierta columna.
    for (i = 0; i < size_x && i < 10; i++) {

        cout << " " << i + 1;
    }
    for (; i < size_x; i++) {//Cuando se llegan a numeros de dos digitos (10 11 12 13...) el programa escribe estos sin escribir un espacio en el medio (10111213...) para que no se descoordinen con la columna a la que se refieren

        cout << i + 1;
    }
    cout << "\n\n";

    char letra_fila = 'A';//Esta sección escribe las letras de la izquierda que indican la coordenada Y de cierta fila.
    for (i = 0; i < size_y; i++) {

        cout << letra_fila++ << ": ";

        for (j = 0; j < size_x; j++) {

            cout << casillero[j][i][player] << " ";
        }

        cout << endl;
    }
    cout << endl << endl;
    return;
}


void ShowGrid2(char casillero[26][26][2], int size_x, int size_y, bool player) {//Esta funcion muestra el estado actual del casillero del player X
    int i, j;
    cout << "\n\n  "; //Esta sección escribe los numeros de arriba que indican la coordenada X de cierta columna.
    for (i = 0; i < size_x && i < 10; i++) {
        cout << " " << i + 1;
    }
    for (; i < size_x; i++) {//Cuando se llegan a numeros de dos digitos (10 11 12 13...) el programa escribe estos sin escribir un espacio en el medio (10111213...) para que no se descoordinen con la columna a la que se refieren
        cout << i + 1;
    }
    cout << "\n\n";
    char letra_fila = 'A';//Esta sección escribe las letras de la izquierda que indican la coordenada Y de cierta fila.
    for (i = 0; i < size_y; i++) {
        cout << letra_fila++ << ": ";
        for (j = 0; j < size_x; j++) {
            cout << casillero[j][i][player] << " ";
        }
        cout << endl;
    }
    cout << endl << endl;
    return;
}


int countBoats(BoatProperties boats_array[20][2], int type_of_boat_to_count, bool player) {

    int i;

    int contador_bhundidosmuygrandes = 0;
    for (i = 0; i < 20; i++) {//contaduría de cada barco

        int hit_tiles = 0;
        for (int j = 0; j < boats_array[i][player].length &&  boats_array[i][player].length == abs(type_of_boat_to_count); j++) {//contaduría de cada casilla del barco "i"

            hit_tiles += boats_array[i][player].x_pos[j] == -1;
        }
        contador_bhundidosmuygrandes += hit_tiles == boats_array[i][player].length;
    }
    return contador_bhundidosmuygrandes ;


}



