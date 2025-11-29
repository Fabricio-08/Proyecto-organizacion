#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <ctime>

using namespace std;


struct Contacto {
    string nombre;
    string telefono;
    string email;

    Contacto(string n, string t, string e) : nombre(n), telefono(t), email(e) {}
};


struct NodoLista {
    Contacto contacto;
    NodoLista* siguiente;

    NodoLista(Contacto c) : contacto(c), siguiente(nullptr) {}
};


struct NodoArbol {
    Contacto contacto;
    NodoArbol* izquierdo;
    NodoArbol* derecho;

    NodoArbol(Contacto c) : contacto(c), izquierdo(nullptr), derecho(nullptr) {}
};

class GestorContactos {
private:

    vector<Contacto> contactos;


    NodoLista* listaEliminados;


    stack<string> historial;


    queue<Contacto> contactosPendientes;


    NodoArbol* arbolContactos;

public:
    GestorContactos() : listaEliminados(nullptr), arbolContactos(nullptr) {}

    ~GestorContactos() {

        while (listaEliminados != nullptr) {
            NodoLista* temp = listaEliminados;
            listaEliminados = listaEliminados->siguiente;
            delete temp;
        }


        liberarArbol(arbolContactos);
    }


    void agregarContacto(string nombre, string telefono, string email) {
        Contacto nuevoContacto(nombre, telefono, email);
        contactos.push_back(nuevoContacto);


        string operacion = "AGREGADO: " + nombre + " - " + telefono;
        historial.push(operacion);


        arbolContactos = insertarEnArbol(arbolContactos, nuevoContacto);

        cout << "Contacto agregado exitosamente!\n";
    }


    void listarContactos() {
        if (contactos.empty()) {
            cout << "No hay contactos registrados.\n";
            return;
        }


        ordenarPorNombre();

        cout << "\n=== LISTA DE CONTACTOS (Ordenados por nombre) ===\n";
        for (size_t i = 0; i < contactos.size(); i++) {
            cout << i + 1 << ". Nombre: " << contactos[i].nombre
                << " | Telefono: " << contactos[i].telefono
                << " | Email: " << contactos[i].email << endl;
        }
        cout << "Total: " << contactos.size() << " contactos\n";
    }


    void buscarContacto(string criterio) {
        cout << "\n=== RESULTADOS DE BUSQUEDA ===\n";


        cout << "Busqueda secuencial:\n";
        bool encontrado = false;
        for (size_t i = 0; i < contactos.size(); i++) {
            if (contactos[i].nombre.find(criterio) != string::npos ||
                contactos[i].telefono.find(criterio) != string::npos) {
                cout << " - " << contactos[i].nombre << " | "
                    << contactos[i].telefono << " | "
                    << contactos[i].email << endl;
                encontrado = true;
            }
        }

        if (!encontrado) {
            cout << " No se encontraron contactos con ese criterio.\n";
        }


        cout << "\nBusqueda binaria (por nombre):\n";
        ordenarPorNombre();
        int indice = busquedaBinaria(criterio);
        if (indice != -1) {
            cout << " - " << contactos[indice].nombre << " | "
                << contactos[indice].telefono << " | "
                << contactos[indice].email << endl;
        }
        else {
            cout << " No se encontro el contacto con busqueda binaria.\n";
        }
    }


    void eliminarContacto(int indice) {
        if (indice < 0 || indice >= (int)contactos.size()) {
            cout << "Indice invalido.\n";
            return;
        }


        Contacto eliminado = contactos[indice];
        NodoLista* nuevoNodo = new NodoLista(eliminado);
        nuevoNodo->siguiente = listaEliminados;
        listaEliminados = nuevoNodo;


        contactos.erase(contactos.begin() + indice);


        liberarArbol(arbolContactos);
        arbolContactos = nullptr;
        for (const auto& contacto : contactos) {
            arbolContactos = insertarEnArbol(arbolContactos, contacto);
        }


        string operacion = "ELIMINADO: " + eliminado.nombre + " - " + eliminado.telefono;
        historial.push(operacion);

        cout << "Contacto eliminado exitosamente!\n";
    }


    void mostrarEliminados() {
        if (listaEliminados == nullptr) {
            cout << "No hay contactos eliminados.\n";
            return;
        }

        cout << "\n=== CONTACTOS ELIMINADOS (Lista Enlazada) ===\n";
        NodoLista* actual = listaEliminados;
        int contador = 1;
        while (actual != nullptr) {
            cout << contador << ". " << actual->contacto.nombre
                << " | " << actual->contacto.telefono
                << " | " << actual->contacto.email << endl;
            actual = actual->siguiente;
            contador++;
        }
    }


    void mostrarHistorial() {
        if (historial.empty()) {
            cout << "No hay operaciones en el historial.\n";
            return;
        }

        cout << "\n=== HISTORIAL DE OPERACIONES (Ultimas primero) ===\n";
        stack<string> temp = historial;
        int contador = 1;

        while (!temp.empty()) {
            cout << contador << ". " << temp.top() << endl;
            temp.pop();
            contador++;
        }
    }


    void encolarContactoPendiente(string nombre, string telefono, string email) {
        Contacto pendiente(nombre, telefono, email);
        contactosPendientes.push(pendiente);
        cout << "Contacto pendiente agregado a la cola.\n";
    }

    void mostrarContactosPendientes() {
        if (contactosPendientes.empty()) {
            cout << "No hay contactos pendientes en la cola.\n";
            return;
        }

        cout << "\n=== CONTACTOS PENDIENTES (Cola) ===\n";
        queue<Contacto> temp = contactosPendientes;
        int contador = 1;

        while (!temp.empty()) {
            Contacto pendiente = temp.front();
            cout << contador << ". " << pendiente.nombre
                << " | " << pendiente.telefono
                << " | " << pendiente.email << endl;
            temp.pop();
            contador++;
        }
    }

    void procesarContactosPendientes() {
        if (contactosPendientes.empty()) {
            cout << "No hay contactos pendientes en la cola.\n";
            return;
        }

        cout << "\n=== PROCESANDO CONTACTOS PENDIENTES ===\n";
        int procesados = 0;

        while (!contactosPendientes.empty()) {
            Contacto pendiente = contactosPendientes.front();
            agregarContacto(pendiente.nombre, pendiente.telefono, pendiente.email);
            contactosPendientes.pop();
            procesados++;
        }

        cout << "Se procesaron " << procesados << " contactos pendientes.\n";
    }


    void mostrarArbolInorden() {
        cout << "\n=== CONTACTOS EN ARBOL (Recorrido Inorden) ===\n";
        if (arbolContactos == nullptr) {
            cout << "El arbol esta vacio.\n";
            return;
        }
        inorden(arbolContactos);
    }

    void mostrarArbolPreorden() {
        cout << "\n=== CONTACTOS EN ARBOL (Recorrido Preorden) ===\n";
        if (arbolContactos == nullptr) {
            cout << "El arbol esta vacio.\n";
            return;
        }
        preorden(arbolContactos);
    }


    void listarPorTelefono() {
        if (contactos.empty()) {
            cout << "No hay contactos registrados.\n";
            return;
        }

        ordenarPorTelefono();

        cout << "\n=== LISTA DE CONTACTOS (Ordenados por telefono) ===\n";
        for (size_t i = 0; i < contactos.size(); i++) {
            cout << i + 1 << ". Telefono: " << contactos[i].telefono
                << " | Nombre: " << contactos[i].nombre
                << " | Email: " << contactos[i].email << endl;
        }
    }

private:

    void ordenarPorNombre() {
        for (size_t i = 0; i < contactos.size() - 1; i++) {
            for (size_t j = 0; j < contactos.size() - i - 1; j++) {
                if (contactos[j].nombre > contactos[j + 1].nombre) {
                    swap(contactos[j], contactos[j + 1]);
                }
            }
        }
    }


    void ordenarPorTelefono() {
        for (size_t i = 0; i < contactos.size() - 1; i++) {
            size_t min_idx = i;
            for (size_t j = i + 1; j < contactos.size(); j++) {
                if (contactos[j].telefono < contactos[min_idx].telefono) {
                    min_idx = j;
                }
            }
            if (min_idx != i) {
                swap(contactos[i], contactos[min_idx]);
            }
        }
    }


    int busquedaBinaria(string nombre) {
        int izquierda = 0;
        int derecha = contactos.size() - 1;

        while (izquierda <= derecha) {
            int medio = izquierda + (derecha - izquierda) / 2;

            if (contactos[medio].nombre == nombre) {
                return medio;
            }

            if (contactos[medio].nombre < nombre) {
                izquierda = medio + 1;
            }
            else {
                derecha = medio - 1;
            }
        }

        return -1;
    }


    NodoArbol* insertarEnArbol(NodoArbol* nodo, Contacto contacto) {
        if (nodo == nullptr) {
            return new NodoArbol(contacto);
        }

        if (contacto.nombre < nodo->contacto.nombre) {
            nodo->izquierdo = insertarEnArbol(nodo->izquierdo, contacto);
        }
        else {
            nodo->derecho = insertarEnArbol(nodo->derecho, contacto);
        }

        return nodo;
    }

    void inorden(NodoArbol* nodo) {
        if (nodo != nullptr) {
            inorden(nodo->izquierdo);
            cout << "Nombre: " << nodo->contacto.nombre << " | Telefono: "
                << nodo->contacto.telefono << " | Email: "
                << nodo->contacto.email << endl;
            inorden(nodo->derecho);
        }
    }

    void preorden(NodoArbol* nodo) {
        if (nodo != nullptr) {
            cout << "Nombre: " << nodo->contacto.nombre << " | Telefono: "
                << nodo->contacto.telefono << " | Email: "
                << nodo->contacto.email << endl;
            preorden(nodo->izquierdo);
            preorden(nodo->derecho);
        }
    }

    void liberarArbol(NodoArbol* nodo) {
        if (nodo != nullptr) {
            liberarArbol(nodo->izquierdo);
            liberarArbol(nodo->derecho);
            delete nodo;
        }
    }
};


void mostrarMenu() {
    cout << "\n" << string(50, '=') << endl;
    cout << "GESTOR DE AGENDA DE CONTACTOS" << endl;
    cout << string(50, '=') << endl;
    cout << "1.  Agregar contacto" << endl;
    cout << "2.  Listar contactos (por nombre)" << endl;
    cout << "3.  Buscar contacto" << endl;
    cout << "4.  Eliminar contacto" << endl;
    cout << "5.  Mostrar historial de operaciones" << endl;
    cout << "6.  Contactos pendientes" << endl;   
    cout << "7.  Recorridos del arbol" << endl;
    cout << "8.  Listar por telefono" << endl;
    cout << "9.  Mostrar eliminados" << endl;
    cout << "0.  Salir" << endl;
    cout << string(50, '=') << endl;
    cout << "Seleccione una opcion: ";
}


int main() {
    GestorContactos gestor;
    int opcion;

    do {
        mostrarMenu();
        cin >> opcion;
        cin.ignore();

        switch (opcion) {
        case 1: {
            string nombre, telefono, email;
            cout << "Nombre: ";
            getline(cin, nombre);
            cout << "Telefono: ";
            getline(cin, telefono);
            cout << "Email: ";
            getline(cin, email);
            gestor.agregarContacto(nombre, telefono, email);
            break;
        }
        case 2:
            gestor.listarContactos();
            break;
        case 3: {
            string criterio;
            cout << "Ingrese nombre o telefono a buscar: ";
            getline(cin, criterio);
            gestor.buscarContacto(criterio);
            break;
        }
        case 4: {
            int indice;
            cout << "Ingrese el numero del contacto a eliminar: ";
            cin >> indice;
            gestor.eliminarContacto(indice - 1);
            break;
        }
        case 5:
            gestor.mostrarHistorial();
            break;
        case 6: {
            string nombre, telefono, email;
            cout << "Nombre del contacto pendiente: ";
            getline(cin, nombre);
            cout << "Telefono: ";
            getline(cin, telefono);
            cout << "Email: ";
            getline(cin, email);
            gestor.encolarContactoPendiente(nombre, telefono, email);
            break;
        }
        case 7:
            gestor.mostrarContactosPendientes();
            break;
        case 8:
            gestor.procesarContactosPendientes();
            break;
        case 9: {
            int subopcion;
            cout << "\n=== RECORRIDOS DEL ARBOL ===\n";
            cout << "1. Mostrar recorrido Inorden\n";
            cout << "2. Mostrar recorrido Preorden\n";
            cout << "Seleccione una opcion: ";
            cin >> subopcion;
            cin.ignore();

            if (subopcion == 1) {
                gestor.mostrarArbolInorden();
            }
            else if (subopcion == 2) {
                gestor.mostrarArbolPreorden();
            }
            else {
                cout << "Opcion invalida.\n";
            }
            break;
        }
        case 10:
            gestor.listarPorTelefono();
            break;
        case 11:
            gestor.mostrarEliminados();
            break;

        case 0:
            cout << "¡Hasta luego! Gracias por usar el gestor.\n";
            break;
        default:
            cout << "Opcion invalida. Intente nuevamente.\n";
        }

        if (opcion != 0) {
            cout << "\nPresione Enter para continuar...";
            cin.get();
        }

    } while (opcion != 0);

    return 0;
}