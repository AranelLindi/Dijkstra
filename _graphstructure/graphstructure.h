#ifndef GRAPHSTRUCTURE_H
#define GRAPHSTRUCTURE_H

#include "graphml.h"

// Bekommt ein GraphML Objekt und bildet daraus eine effizientere Struktur für weiteres Verarbeiten.
class GraphStructure
{

    GraphStructure(const GraphML &graph); // Konstruktor
    ~GraphStructure(void);                // Destruktor

    // Setter

    // Getter
    const bool *const *const getAdjacencyMatrix(void);
    
};

#endif // graphstructure.h