#pragma once
#include <vector>
#include <BWAPI.h>

class mina{
	private:
	int id;
	int cantidad;
	//contar los frames desde la ultima extraccion
	int cantidadDeFrames;

	//registro de los ultimos 20 conteos 
	std::vector<int> historial;
	//posicion de las zonas de minas cristales
	BWAPI::Position posicion;
    
	public:

	mina(int i, int c, BWAPI::Position p);
    void actualizar(int cantidadActual);
    int getId() const;
    BWAPI::Position getPosicion() const;
    int getFramesActuales() const;
    int getUltimoConteo() const;
    double getPromedio() const;
    
	
};
