#ifndef RGRAPHML_H
#define RGRAPHML_H

#include <cstdint>

struct ReadingGraphMLClass {
    ReadingGraphMLClass(const char* filepath); // Konstruktor
    ~ReadingGraphMLClass(void); // Destruktor

    bool valid(void); // kann für Validierung des Graphen verwendet werden. Wirft Exceptions!

    uint8_t getkeys(void); // Anzahl key-Elemente
    uint8_t getnodes(void); // Anzahl Knoten
    uint8_t getedges(void); // Anzahl Kanten

    bool*const*const getAdjacencyMatrix(void);
    void freeMatrix(const bool& matrix);


};


#endif // rgraphml.h