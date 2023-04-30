//Archivo:safebridge.cpp
//Autor: Alejandra Cabrera Ruiz A01704463
//Autor: Salvador Rodriguez Paredes A01704562
//Actividad 5.5 Problemas de sincronización 2
//Run program: g++ -pthread safebridge.cpp -o app
//             ./app

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

using namespace std;

//Define cuantos vehiculos van a cruzar el puente 
//y define cuantos vehiculos pueden estar en el puente, 3 en este caso

const int MAX_VEHICLES = 10;
const int MAX_ON_BRIDGE = 3;

//Define la variable inicial que guardará los vehiculos en el puente

int vehicles_on_bridge = 0;

//Solo utilizamos un semáforo, el cual es bridge_semaphore.
//Este se usa para garantizar que no haya más de 3 vehículos 
// y el puente no colapse. Esta variable esta definida en MAX_ON_BRIDGE.
// usamos la funcion sem_wait() que bloquea el semáforo hasta que se libere
// el semáforo y el contador se decremente. La función sem_post() hace lo 
// contrario al incrementar el contador del semáforo, liberando un recurso.
// Estas funciones están definidas en la libreria <semaphore.h>

sem_t bridge_semaphore;


void arrive_bridge(int id, int direction) {
    cout << "Vehicle " << id << " arrived on bridge\n";
    while (vehicles_on_bridge > 0 && (direction == 0) != (vehicles_on_bridge < 0)) {
        usleep(50000); // Esperar un tiempo para evitar bloqueos
    }
    vehicles_on_bridge += direction;
}

//Como lo marca el problema esta funcion solo imprime la información
// de los vehículos que cruzan el puente y usleep simula el 
// tiempo que tarda en cruzar el puente

void cross_bridge(int id, int direction) {
    cout << "Vehicle " << id << " crossing bridge\n";
    usleep(50000); 
}

void exit_bridge(int id, int direction, int* vehicle_data) {
    vehicles_on_bridge -= direction;
    cout << "Vehicle " << id << " exited bridge\n";
    // Libera el semáforo
    sem_post(&bridge_semaphore); 
    delete vehicle_data;
}

void* one_vehicle(void* arg) {
    //Recibe el vehiculo que es un arreglo y asigna a cada variable su información, id y dirección
    int* vehicle_data = new int[2];
    vehicle_data[0] = ((int*) arg)[0];
    vehicle_data[1] = ((int*) arg)[1];
    int id = vehicle_data[0];
    int direction = vehicle_data[1];

    // Espera a que sea seguro cruzar el puente, es decir haya menos de 3 vehiculos en el
    sem_wait(&bridge_semaphore); 
    arrive_bridge(id, direction);
    cross_bridge(id, direction);
    exit_bridge(id, direction, vehicle_data);

    pthread_exit(NULL);
}

int main() {
    sem_init(&bridge_semaphore, 0, MAX_ON_BRIDGE);

    pthread_t vehicles[MAX_VEHICLES];
    //Define los datos de los vehiculos y la dirección norte o sur
    int vehicle_data[MAX_VEHICLES][2] = {{1, 1}, {2, -1}, {3, 1}, {4, -1}, {5, 1}, {6, 1}, {7, -1}, {8, -1}, {9, 1}, {10, -1}};

    //Crea un thread para cada vehiculo de acuerdo al maximo de vehículos definido en un inicio
    for (int i = 0; i < MAX_VEHICLES; i++) {
        pthread_create(&vehicles[i], NULL, one_vehicle, (void*) vehicle_data[i]);
    }

    for (int i = 0; i < MAX_VEHICLES; i++) {
        pthread_join(vehicles[i], NULL);
    }

    //Destruye nuestro semáforo
    sem_destroy(&bridge_semaphore);
    return 0;
}
