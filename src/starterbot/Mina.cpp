#include "Mina.h"

mina::mina(int i, int c, BWAPI::Position p): id(i), cantidad(c), cantidadDeFrames(0), posicion(p){}

//metodo de actulizacion para los 20 ultimos frames de extraccion
void mina::actualizar(int cantidadActual)
    {
        if (cantidadActual < cantidad)
        {
            historial.push_back(cantidadDeFrames);
            if (historial.size() > 20)
            {
                historial.erase(historial.begin());
            }
            cantidad = cantidadActual;
            cantidadDeFrames = 0;
        }
        else
        {
            cantidadDeFrames++;
        }
}

//getters de consulta para leer la informacion
int mina::getId() const { 
	return id; 
}
BWAPI::Position mina::getPosicion() const {
	 return posicion;
}
int mina::getFramesActuales() const {
	return cantidadDeFrames; 
}

int mina::getUltimoConteo() const
{
    return historial.empty() ? 0 : historial.back();
}

// consulta del promedio dentro del historial
double mina::getPromedio() const
{
    if (historial.empty()) return 0.0;
    double suma = 0.0;
    for (int conteo : historial)
    {
        suma += conteo;
    }
    return suma / historial.size();
}
