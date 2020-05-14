#ifndef GRAPHML_H
#define GRAPHML_H

// Standardbibliotheken
#include <cstdint>

// eigener Code
#include <GraphMLObjects.h>

class ReadingGraphML
{
    private: // Enthält nur Variablen
    uint8_t key_counter, node_counter, edge_counter; // Anzahl keys/nodes/edges des Graphen

    uint8_t data_counter_node, data_counter_edge;

    char* stringcopy(char* val);

    public:
    struct GRAPH* graph;
    struct KEY* keys;

    // Funktionen
    ReadingGraphML(char* filepath); // Konstruktor
    bool plausibilitaet(void); // Prüft ob GraphML-Dokument d. Spezifikation genügt und/oder Dinge doppelt definiert sind
    ~ReadingGraphML(void); // Destruktor

    uint8_t numberofkeys(void);
    uint8_t numberofnodes(void);
    uint8_t numberofedges(void);

    uint8_t** adjazenzmatrix(void);
    void freeadjazenzmatrxi(uint8_t** matrix);
};



#endif // GraphML.h