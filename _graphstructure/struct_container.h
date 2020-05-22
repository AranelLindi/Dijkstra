/*
* Enthält Container für GraphML die aufbereitet sind
* um sich gut durch Code mit dem Graphen bearbeiten 
* zu lassen und logische Prüfungen vorzunehmen, die möglichst
* geringe Kosten und Laufzeit haben.
*/

#ifndef STRUCT_CONTAINER_H
#define STRUCT_CONTAINER_H

#include "../_graphml/ml_container.h" // Verweise auf nsGraphML-Objekte

namespace gsObject
{
    class Node {
        // Variablen


        // Funktionen
        Node(const mlObject::Node& node);
        // Konstruktor empfängt als Argument einen Knoten
        // aus dem XML-nahen Konstrukt um sich zu initialisieren.
        
        ~Node(void);

        
    };
}

#endif // struct_container.h