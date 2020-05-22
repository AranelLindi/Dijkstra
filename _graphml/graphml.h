#ifndef GRAPHML_H
#define GRAPHML_H

// Standartbibliothek
#include <cstdlib> // Speicherplatzverwaltung
#include <fstream>  // IO Operationen
#include <vector> // XML-File lesen
#include <filesystem> // Dateien Existenz prüfen
#include <iostream> // Konsole Ausgabe (insb. Exception-Handling)

// eigener Code
#include "ml_container.h" // Container für GraphML
#include "../general/various.cpp" // copyCharArray

// Drittcode
#include "../rapidxml/rapidxml.hpp"


using namespace nsGraphML; // betrifft alle Objekte in ml_container.h


// Repräsentiert einen Graph nahe am GraphML-Standart (XML Definition um Graphen abzubilden).
class GraphML {
    char* id; // id des Graphen
    enum EdgeType edgedefault; // gerichtet/ungerichteter Graph
    struct Node* nodes; // Alle Knoten als Array
    struct Edge* edges; // Alle Kanten als Array
    struct Key* keys; // Alle Keys (Schlüssel/Eigenschaften) als Array

    uint8_t key_counter, node_counter, edge_counter; // Element counter
    uint8_t data_counter; // für jedes Element einzeln. Wird oft zurückgesetzt!

public:
    GraphML(const char* filepath); // Konstruktor
    ~GraphML(void); // Destruktor

    // Grundsätzliche Validierung des Graphen (nur bedingt logische Prüfung!)
    bool valid(void); // kann für Validierung des Graphen verwendet werden. Wirft Exceptions!

    // Anzahl key-Elemente
    uint8_t inline getkeys(void); 
    // Anzahl Knoten
    uint8_t inline getnodes(void);
    // Anzahl Kanten
    uint8_t inline getedges(void);

    // Liefert Adjazenzmatrix des Graphen
    const bool *const *const getAdjacencyMatrix(void);
    // Gibt die Adjazenzmatrix des Graphen frei
    void freeMatrix(const bool *const *const matrix);

};



#endif // graphml.h