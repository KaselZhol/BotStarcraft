#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"
#include <fstream>
#include<string>
StarterBot::StarterBot()
{
    
}

// Called when the bot starts!
void StarterBot::onStart()
{
    
    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(0);
    BWAPI::Broodwar->setFrameSkip(0);
    
    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    // Call MapTools OnStart
    m_mapTools.onStart();

    //asignar las minas de cristal determinadas por BWAPI a la lista de minas, con los datos de id, cantidad y posicion
    //se usa cantidad (1500) y no getResources() porque las minas ocultas mostraran 0 y no se podra calcular el promedio de extraccion
    for (auto& mineral : BWAPI::Broodwar->getStaticMinerals()){
        minas.push_back(mina(mineral->getID(), 1500, mineral->getPosition()));
    }
}



// Called on each frame of the game
void StarterBot::onFrame()
{
    // Update our MapTools information
    m_mapTools.onFrame();

    // Send our idle workers to mine minerals so they don't just stand there
    sendIdleWorkersToMinerals();

    // Train more workers so we can gather more income
    trainAdditionalWorkers();

    // Build more supply if we are going to run out soon
    buildAdditionalSupply();

    // Draw unit health bars, which brood war unfortunately does not do
    Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();
    
    // Ejecutar la actualizacion y dibujo de las minas en cada frame
    actualizarYDibujarMinas();
    //monitoreo del equipo 
    monitorearTelemetriaEquipo();
    //ventana de 3 minutos 
    if (BWAPI::Broodwar->getFrameCount() == 720)
    {
        guardarResultadosCSV();
        BWAPI::Broodwar->printf("3 minutos completados. Datos guardados en CSV.");
        BWAPI::Broodwar->pauseGame(); // sale de la partida automaticamente
    }
}

// Send our idle workers to mine minerals so they don't just stand there
void StarterBot::sendIdleWorkersToMinerals()
{
    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    const BWAPI::Unitset& visibles = BWAPI::Broodwar->getMinerals();
    for (auto& unit : myUnits)
    {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle())
        {
            // Buscamos un mineral que ningun otro trabajador nuestro este usando
            for (auto& mineral : visibles)
            {
                bool ocupado = false;
                for (auto& otro : myUnits)
                {
                    if (otro->getType().isWorker() &&
                        (otro->getOrderTarget() == mineral || otro->getTarget() == mineral))
                    {
                        ocupado = true;
                        break;
                    }
                }

                if (!ocupado)
                {
                    unit->rightClick(mineral);
                    break; // Este trabajador ya tiene su mina asignada
                }
            }

            /* Esto es para que el bot busque la mina mas cercana a cada trabajador y haga click derecho sobre ella para que empiece a extraer,
            pero no es necesario porque BWAPI hace esto automaticamente

            // Get the closest mineral to this worker unit
                BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

            // If a valid mineral was found, right click it with the unit in order to start harvesting
                if (closestMineral) { unit->rightClick(closestMineral); }    
            
            */ 
            
        }
    }
}

// Train more workers so we can gather more income
void StarterBot::trainAdditionalWorkers()
{
    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    const int workersWanted = 9;
    const int workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    if (workersOwned < workersWanted)
    {
        // get the unit pointer to my depot
        const BWAPI::Unit myDepot = Tools::GetDepot();

        // if we have a valid depot unit and it's currently not training something, train a worker
        // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
        if (myDepot && !myDepot->isTraining()) { myDepot->train(workerType); }
    }
}

// Build more supply if we are going to run out soon
void StarterBot::buildAdditionalSupply()
{
    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply >= 2) { return; }

    // Otherwise, we are going to build a supply provider
    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    const bool startedBuilding = Tools::BuildBuilding(supplyProviderType);
    if (startedBuilding)
    {
        BWAPI::Broodwar->printf("Started Building %s", supplyProviderType.getName().c_str());
    }
}

// Draw some relevent information to the screen to help us debug the bot
void StarterBot::drawDebugInformation()
{
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), "Hello, World!\n");
    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
}

// Called whenever the game ends and tells you if you won or not
void StarterBot::onEnd(bool isWinner)
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
}

// Called whenever a unit is destroyed, with a pointer to the unit
void StarterBot::onUnitDestroy(BWAPI::Unit unit)
{
	
}

// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void StarterBot::onUnitMorph(BWAPI::Unit unit)
{
	
}

// Called whenever a text is sent to the game by a user
void StarterBot::onSendText(std::string text) 
{ 
    if (text == "/map")
    {
        m_mapTools.toggleDraw();
    }
}

// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible, 
// so this will trigger when you issue the build command for most units
void StarterBot::onUnitCreate(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit finished construction, with a pointer to the unit
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{
	
}

// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void StarterBot::onUnitShow(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit gets hidden, with a pointer to the destroyed unit
// This is usually triggered when units enter the fog of war and are no longer visible
void StarterBot::onUnitHide(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit switches player control
// This usually happens when a dark archon takes control of a unit
void StarterBot::onUnitRenegade(BWAPI::Unit unit)
{ 
	
}
void StarterBot::actualizarYDibujarMinas(){
    // recorremos los minerales que el bot puede ver en ese instante
    for (auto& mineral : BWAPI::Broodwar->getMinerals())
    {
        // buscamos su objeto correspondiente en nuestro vector por ID
        for (auto& m : minas)
        {
            if (m.getId() == mineral->getID())
            {
                // Actualizamos contador de frames
                m.actualizar(mineral->getResources());

                // Obtenemos posicion
                BWAPI::Position pos = m.getPosicion();

                // Dibujamos ultimo conteo y hallamos su promedio (2 decimales) 
                BWAPI::Broodwar->drawTextMap(pos.x - 45, pos.y - 15,
                    "Ult: %d\nProm: %.2f\nAct: %d",
                    m.getUltimoConteo(),
                    m.getPromedio(),
                    m.getFramesActuales());
                
                break; //siguiente
            }
        }
    }
}
void StarterBot::guardarResultadosCSV()
{
    std::string carpeta = "../resultados";

    // Incluye el nombre de la raza automaticamente en el archivo (Terran, Protoss o Zerg)
    std::string raza = BWAPI::Broodwar->self()->getRace().getName();
    int mitadX = BWAPI::Broodwar->mapWidth() * 16;
    
    // Detecta automaticamente si tu base principal esta a la izquierda o derecha
    BWAPI::Unit depot = Tools::GetDepot();
    std::string ladoBase = (depot && depot->getPosition().x < mitadX) ? "Izquierda": "Derecha";

    std::ofstream archivo(carpeta + "/resultados_" + raza + "_" + ladoBase + ".csv");    if (archivo.is_open())
    {
        archivo << "ID_Mina,Pos_X,Pos_Y,Lado,Ultimo_Conteo,Promedio_Frames\n";
        
        int mitadX = BWAPI::Broodwar->mapWidth() * 16;

        for (const auto& m : minas)
        {
            if (m.getPromedio() > 0.0)
            {
                BWAPI::Position pos = m.getPosicion();
                std::string lado = (pos.x < mitadX) ? "Izquierda" : "Derecha";

                archivo << m.getId() << ","
                        << pos.x << ","
                        << pos.y << ","
                        << lado << ","
                        << m.getUltimoConteo() << ","
                        << m.getPromedio() << "\n";
            }
        }
        archivo.close();
    }
}

void StarterBot::monitorearTelemetriaEquipo()
{
    int trabajadoresActivos = 0;
    int mineralesCercanos = 0;

    // 1. Contar cuantos minerales activos hay visibles en la zona
    for (auto& mineral : BWAPI::Broodwar->getMinerals()){
        if (mineral->getResources() > 0)
        {
            mineralesCercanos++;
        }
    }

    // 2. Auditar el ciclo de trabajo de cada unidad recolectora
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
        if (!unit->getType().isWorker()) continue;

        int id = unit->getID();
        bool estaExtrayendo = unit->isGatheringMinerals() && !unit->isMoving();
        bool estaViajando = unit->isMoving() || unit->isCarryingMinerals();

        if (estaExtrayendo){
            framesPicoUnidad[id]++;
            trabajadoresActivos++;
        } else if (estaViajando){
            framesViajeUnidad[id]++;
            trabajadoresActivos++;
        }

        int totalFrames = framesPicoUnidad[id] + framesViajeUnidad[id];
        if (totalFrames > 0){
            double pctViaje = (100.0 * framesViajeUnidad[id]) / totalFrames;
            double pctPico = (100.0 * framesPicoUnidad[id]) / totalFrames;

            // Imprime en verde sobre cada trabajador su proporcion Viaje vs Picado
            BWAPI::Broodwar->drawTextMap(unit->getPosition().x - 35, unit->getPosition().y - 22,
                "\x07Viaje: %.0f%% | Pico: %.0f%%",
                pctViaje,
                pctPico);
        }
    }
    // 3. Calcular indicadores macroeconomicos para el panel superior
    double ratioSaturacion = (mineralesCercanos > 0)
        ? static_cast<double>(trabajadoresActivos) / mineralesCercanos
        : 0.0;

    const char* estadoBase = "SUB-SATURADA";
    if (ratioSaturacion >= 1.0 && ratioSaturacion <= 2.0) estadoBase = "OPTIMA";
    else if (ratioSaturacion > 2.0) estadoBase = "SOBRE-SATURADA";

    int mineralesAcumulados = BWAPI::Broodwar->self()->gatheredMinerals();
    double minutos = BWAPI::Broodwar->getFrameCount() / 1440.0; // 24 FPS * 60 segundos
    double mpm = (minutos > 0.0) ? (mineralesAcumulados / minutos) : 0.0;

    // Panel fijo de telemetria en la esquina superior izquierda
    BWAPI::Broodwar->drawTextScreen(10, 25,
        "\x04[TELEMETRIA DE RENDIMIENTO]\n"
        "\x07Saturacion: %d/%d (%.2fx) - %s\n"
        "\x07Recolectado: %d min | Tasa: %.1f M/min",
        trabajadoresActivos, mineralesCercanos, ratioSaturacion, estadoBase,
        mineralesAcumulados, mpm);
}