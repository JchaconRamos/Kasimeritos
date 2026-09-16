#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>

using namespace std;

// ============================================================
// DECLARACIONES ADELANTADAS
// ============================================================

class ProcesoProduccion;


// ============================================================
// INTERFAZ ESTADO
// ============================================================

class Estado
{
public:

    virtual ~Estado() = default;

    // Método polimórfico para avanzar al siguiente estado
    virtual void avanzar(ProcesoProduccion& proceso) = 0;

    // Devuelve el nombre del estado
    virtual string obtenerNombre() const = 0;
};


// ============================================================
// CLASE MATERIA PRIMA
// ============================================================

class MateriaPrima
{
private:

    string tipoMadera;
    string calidad;
    double cantidad;

public:

    MateriaPrima(string tipo, string calidadMateria, double cantidadMateria)
        : tipoMadera(tipo),
        calidad(calidadMateria),
        cantidad(cantidadMateria)
    {}

    string getTipoMadera() const
    {
        return tipoMadera;
    }

    string getCalidad() const
    {
        return calidad;
    }

    double getCantidad() const
    {
        return cantidad;
    }

    void mostrarInformacion() const
    {
        cout << "\n--- INFORMACION DE LA MATERIA PRIMA ---\n";
        cout << "Tipo de madera: " << tipoMadera << endl;
        cout << "Calidad: " << calidad << endl;
        cout << "Cantidad: " << cantidad << " unidades\n";
    }
};


// ============================================================
// CLASE PROCESO PRODUCCION
// ============================================================

class ProcesoProduccion
{
private:

    unique_ptr<Estado> estadoActual;

    MateriaPrima materiaPrima;

    vector<string> historial;

public:

    ProcesoProduccion(MateriaPrima materia)
        : materiaPrima(materia)
    {
        // El proceso comienza en Recepción
        estadoActual = nullptr;
    }

    // Cambiar el estado actual
    void cambiarEstado(unique_ptr<Estado> nuevoEstado);

    // Obtener nombre del estado actual
    string obtenerEstadoActual() const;

    // Avanzar en el proceso
    void avanzar();

    // Obtener materia prima
    MateriaPrima& obtenerMateriaPrima()
    {
        return materiaPrima;
    }

    // Registrar eventos
    void registrarEvento(string evento)
    {
        historial.push_back(evento);
    }

    // Mostrar historial
    void mostrarHistorial() const;

    // Guardar historial en archivo
    void guardarHistorial();

    // Establecer estado inicial
    void establecerEstadoInicial(unique_ptr<Estado> estado);

    // Obtener estado
    Estado* obtenerEstado() const
    {
        return estadoActual.get();
    }
};


// ============================================================
// DECLARACIONES DE LOS ESTADOS
// ============================================================

class Recepcion;
class InspeccionCalidad;
class CorteDimensionado;
class SecadoTratamiento;
class Almacenado;
class DistribuidoProduccion;
class Rechazado;
class Reproceso;


// ============================================================
// ESTADO RECEPCION
// ============================================================

class Recepcion : public Estado
{
public:

    void avanzar(ProcesoProduccion& proceso) override;

    string obtenerNombre() const override
    {
        return "Recepcion de Materia Prima";
    }
};


// ============================================================
// ESTADO INSPECCION DE CALIDAD
// ============================================================

class InspeccionCalidad : public Estado
{
private:

    bool inspeccionRealizada = false;
    bool materiaApta = false;

public:

    void avanzar(ProcesoProduccion& proceso) override;

    string obtenerNombre() const override
    {
        return "Inspeccion de Calidad";
    }

    void establecerResultado(bool apta)
    {
        inspeccionRealizada = true;
        materiaApta = apta;
    }
};


// ============================================================
// ESTADO CORTE Y DIMENSIONADO
// ============================================================

class CorteDimensionado : public Estado
{
public:

    void avanzar(ProcesoProduccion& proceso) override;

    string obtenerNombre() const override
    {
        return "Corte y Dimensionado";
    }
};


// ============================================================
// ESTADO SECADO Y TRATAMIENTO
// ============================================================

class SecadoTratamiento : public Estado
{
public:

    void avanzar(ProcesoProduccion& proceso) override;

    string obtenerNombre() const override
    {
        return "Secado y Tratamiento";
    }
};


// ============================================================
// ESTADO ALMACENADO
// ============================================================

class Almacenado : public Estado
{
public:

    void avanzar(ProcesoProduccion& proceso) override;

    string obtenerNombre() const override
    {
        return "Almacenado";
    }
};


// ============================================================
// ESTADO DISTRIBUIDO A PRODUCCION
// ============================================================

class DistribuidoProduccion : public Estado
{
public:

    void avanzar(ProcesoProduccion& proceso) override
    {
        cout << "\nLa materia prima ya fue distribuida a produccion.\n";
        cout << "El ciclo de produccion ha finalizado.\n";

        proceso.registrarEvento(
            "Intento de avanzar desde Distribuido a Produccion"
        );
    }

    string obtenerNombre() const override
    {
        return "Distribuido a Produccion";
    }
};


// ============================================================
// ESTADO RECHAZADO
// ============================================================

class Rechazado : public Estado
{
public:

    void avanzar(ProcesoProduccion& proceso) override
    {
        cout << "\nLa materia prima fue rechazada.\n";
        cout << "No puede continuar al proceso de produccion.\n";

        proceso.registrarEvento(
            "Intento de avanzar desde Rechazado"
        );
    }

    string obtenerNombre() const override
    {
        return "Rechazada";
    }
};


// ============================================================
// ESTADO REPROCESO
// ============================================================

class Reproceso : public Estado
{
public:

    void avanzar(ProcesoProduccion& proceso) override
    {
        cout << "\nSe realizara un reproceso de la materia prima.\n";
        cout << "La materia prima regresara a Recepcion.\n";

        proceso.registrarEvento(
            "Reproceso realizado. Regreso a Recepcion"
        );

        proceso.cambiarEstado(make_unique<Recepcion>());
    }

    string obtenerNombre() const override
    {
        return "Reproceso";
    }
};


// ============================================================
// IMPLEMENTACION DE PROCESO PRODUCCION
// ============================================================

void ProcesoProduccion::establecerEstadoInicial(
    unique_ptr<Estado> estado)
{
    estadoActual = move(estado);

    registrarEvento(
        "Inicio del proceso en: " + estadoActual->obtenerNombre()
    );
}


void ProcesoProduccion::cambiarEstado(
    unique_ptr<Estado> nuevoEstado)
{
    if (nuevoEstado == nullptr)
    {
        return;
    }

    string estadoAnterior = obtenerEstadoActual();
    string estadoNuevo = nuevoEstado->obtenerNombre();

    cout << "\nTRANSICION:\n";
    cout << estadoAnterior << " -> "
        << estadoNuevo << endl;

    registrarEvento(
        estadoAnterior + " -> " + estadoNuevo
    );

    estadoActual = move(nuevoEstado);
}


string ProcesoProduccion::obtenerEstadoActual() const
{
    if (estadoActual == nullptr)
    {
        return "Sin estado";
    }

    return estadoActual->obtenerNombre();
}


void ProcesoProduccion::avanzar()
{
    if (estadoActual == nullptr)
    {
        cout << "El proceso no tiene un estado inicial.\n";
        return;
    }

    estadoActual->avanzar(*this);
}


void ProcesoProduccion::mostrarHistorial() const
{
    cout << "\n=====================================\n";
    cout << "       HISTORIAL DEL PROCESO\n";
    cout << "=====================================\n";

    if (historial.empty())
    {
        cout << "No existen eventos registrados.\n";
        return;
    }

    for (size_t i = 0; i < historial.size(); i++)
    {
        cout << i + 1 << ". "
            << historial[i] << endl;
    }
}


void ProcesoProduccion::guardarHistorial()
{
    ofstream archivo("historial_produccion.txt");

    if (!archivo)
    {
        cout << "No se pudo crear el archivo.\n";
        return;
    }

    archivo << "HISTORIAL DEL PROCESO DE PRODUCCION\n";
    archivo << "===================================\n\n";

    archivo << "Materia prima: "
        << materiaPrima.getTipoMadera() << endl;

    archivo << "Calidad: "
        << materiaPrima.getCalidad() << endl;

    archivo << "Cantidad: "
        << materiaPrima.getCantidad() << endl;

    archivo << "\nEVENTOS:\n";

    for (size_t i = 0; i < historial.size(); i++)
    {
        archivo << i + 1 << ". "
            << historial[i] << endl;
    }

    archivo.close();

    cout << "\nHistorial guardado correctamente en:\n";
    cout << "historial_produccion.txt\n";
}


// ============================================================
// IMPLEMENTACION DE RECEPCION
// ============================================================

void Recepcion::avanzar(ProcesoProduccion& proceso)
{
    cout << "\nLa materia prima fue recibida correctamente.\n";
    cout << "Enviando a inspeccion de calidad...\n";

    proceso.cambiarEstado(
        make_unique<InspeccionCalidad>()
    );
}


// ============================================================
// IMPLEMENTACION DE INSPECCION
// ============================================================

void InspeccionCalidad::avanzar(
    ProcesoProduccion& proceso)
{
    cout << "\n--- INSPECCION DE CALIDAD ---\n";

    cout << "Materia prima: "
        << proceso.obtenerMateriaPrima().getTipoMadera()
        << endl;

    cout << "Calidad registrada: "
        << proceso.obtenerMateriaPrima().getCalidad()
        << endl;

    cout << "\nSeleccione resultado:\n";
    cout << "1. Materia prima apta\n";
    cout << "2. Materia prima rechazada\n";
    cout << "3. Reproceso\n";
    cout << "Seleccione una opcion: ";

    int opcion;
    cin >> opcion;

    switch (opcion)
    {
    case 1:

        cout << "\nLa materia prima cumple con los "
            << "estandares de calidad.\n";

        proceso.registrarEvento(
            "Inspeccion aprobada"
        );

        proceso.cambiarEstado(
            make_unique<CorteDimensionado>()
        );

        break;


    case 2:

        cout << "\nLa materia prima NO cumple "
            << "con los estandares.\n";

        proceso.registrarEvento(
            "Inspeccion rechazada"
        );

        proceso.cambiarEstado(
            make_unique<Rechazado>()
        );

        break;


    case 3:

        cout << "\nLa materia prima necesita "
            << "ajustes menores.\n";

        proceso.registrarEvento(
            "Materia prima enviada a reproceso"
        );

        proceso.cambiarEstado(
            make_unique<Reproceso>()
        );

        break;


    default:

        cout << "\nOpcion invalida.\n";
        cout << "La materia prima permanece en "
            << "Inspeccion de Calidad.\n";

        proceso.registrarEvento(
            "Opcion invalida en inspeccion"
        );

        break;
    }
}


// ============================================================
// IMPLEMENTACION CORTE Y DIMENSIONADO
// ============================================================

void CorteDimensionado::avanzar(
    ProcesoProduccion& proceso)
{
    cout << "\nRealizando corte y dimensionado...\n";

    cout << "La madera ha sido cortada segun "
        << "las medidas establecidas.\n";

    proceso.cambiarEstado(
        make_unique<SecadoTratamiento>()
    );
}


// ============================================================
// IMPLEMENTACION SECADO Y TRATAMIENTO
// ============================================================

void SecadoTratamiento::avanzar(
    ProcesoProduccion& proceso)
{
    cout << "\nRealizando secado y tratamiento...\n";

    cout << "Se aplican los procesos necesarios "
        << "para controlar humedad y plagas.\n";

    proceso.cambiarEstado(
        make_unique<Almacenado>()
    );
}


// ============================================================
// IMPLEMENTACION ALMACENADO
// ============================================================

void Almacenado::avanzar(
    ProcesoProduccion& proceso)
{
    cout << "\nAlmacenando materia prima...\n";

    cout << "La materia prima esta lista "
        << "para ser utilizada.\n";

    proceso.cambiarEstado(
        make_unique<DistribuidoProduccion>()
    );
}


// ============================================================
// MENU PRINCIPAL
// ============================================================

void mostrarMenu()
{
    cout << "\n";
    cout << "============================================\n";
    cout << "     MAQUINA DE ESTADOS - PRODUCCION\n";
    cout << "============================================\n";

    cout << "1. Mostrar estado actual\n";
    cout << "2. Avanzar proceso\n";
    cout << "3. Mostrar informacion de materia prima\n";
    cout << "4. Mostrar historial\n";
    cout << "5. Guardar historial en archivo\n";
    cout << "6. Intentar transicion invalida\n";
    cout << "0. Salir\n";

    cout << "============================================\n";
    cout << "Seleccione una opcion: ";
}


// ============================================================
// FUNCION PARA PROBAR TRANSICIONES INVALIDAS
// ============================================================

void probarTransicionInvalida(
    ProcesoProduccion& proceso)
{
    cout << "\n=====================================\n";
    cout << "     PRUEBA DE TRANSICION INVALIDA\n";
    cout << "=====================================\n";

    cout << "Estado actual: "
        << proceso.obtenerEstadoActual()
        << endl;

    cout << "\nIntentando regresar directamente a "
        << "Recepcion...\n";

    cout << "TRANSICION INVALIDA RECHAZADA.\n";

    proceso.registrarEvento(
        "Transicion invalida rechazada"
    );
}


// ============================================================
// MAIN
// ============================================================

int main()
{
    // --------------------------------------------------------
    // CREAR MATERIA PRIMA
    // --------------------------------------------------------

    string tipo;
    string calidad;
    double cantidad;

    cout << "============================================\n";
    cout << "  SISTEMA DE CONTROL DE PRODUCCION\n";
    cout << "============================================\n";

    cout << "\nIngrese el tipo de madera: ";
    getline(cin, tipo);

    cout << "Ingrese la calidad: ";
    getline(cin, calidad);

    cout << "Ingrese la cantidad: ";
    cin >> cantidad;


    // --------------------------------------------------------
    // CREAR OBJETO PROCESO
    // --------------------------------------------------------

    MateriaPrima materia(
        tipo,
        calidad,
        cantidad
    );

    ProcesoProduccion proceso(materia);


    // --------------------------------------------------------
    // ESTADO INICIAL
    // --------------------------------------------------------

    proceso.establecerEstadoInicial(
        make_unique<Recepcion>()
    );


    // --------------------------------------------------------
    // MENU
    // --------------------------------------------------------

    int opcion;

    do
    {
        mostrarMenu();
        cin >> opcion;

        switch (opcion)
        {
        case 1:

            cout << "\nEstado actual: "
                << proceso.obtenerEstadoActual()
                << endl;

            break;


        case 2:

            proceso.avanzar();

            break;


        case 3:

            proceso.obtenerMateriaPrima()
                .mostrarInformacion();

            break;


        case 4:

            proceso.mostrarHistorial();

            break;


        case 5:

            proceso.guardarHistorial();

            break;


        case 6:

            probarTransicionInvalida(proceso);

            break;


        case 0:

            cout << "\nSaliendo del sistema...\n";

            break;


        default:

            cout << "\nOpcion invalida.\n";
        }

    } while (opcion != 0);


    return 0;
}