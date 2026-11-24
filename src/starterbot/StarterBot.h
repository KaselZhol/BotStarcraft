#pragma once
#include <vector>
#include "MapTools.h"

#include <BWAPI.h>
#include "Mina.h"
#include <map>

class StarterBot
{
    MapTools m_mapTools;
	std::vector<mina> minas;
	// mapas de seguimiento de los frames de viaje y pico de cada unidad recolectora (pregunta 2)
    std::map<int, int> framesViajeUnidad;
    std::map<int, int> framesPicoUnidad;
public:

    StarterBot();

    // helper functions to get you started with bot programming and learn the API
    void sendIdleWorkersToMinerals();
    void trainAdditionalWorkers();
    void buildAdditionalSupply();
    void drawDebugInformation();
	//Monitoreo para equipo ESPORT
	void monitorearTelemetriaEquipo();
    // functions that are triggered by various BWAPI events from main.cpp
	void onStart();
	void onFrame();
	void onEnd(bool isWinner);
	void onUnitDestroy(BWAPI::Unit unit);
	void onUnitMorph(BWAPI::Unit unit);
	void onSendText(std::string text);
	void onUnitCreate(BWAPI::Unit unit);
	void onUnitComplete(BWAPI::Unit unit);
	void onUnitShow(BWAPI::Unit unit);
	void onUnitHide(BWAPI::Unit unit);
	void onUnitRenegade(BWAPI::Unit unit);
	//Agregado para actualizar y dibujar las minas
	void actualizarYDibujarMinas();
	//para guardar los datos de las minas en un archivo csv
	void guardarResultadosCSV();
	
};