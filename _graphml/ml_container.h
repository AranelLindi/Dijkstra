/*
* Enthält Container für GraphML die XML nah
* orientiert sind um leicht befüllt zu werden.
*/

#ifndef ML_CONTAINER_H
#define ML_CONTAINER_H

#include <cstdint> // Integer-Typen

namespace nsGraphML
{
    enum EdgeType
    {
        directed,
        undirected
    }; // lt. sizeof() 4 Bytes
    
    enum GraphObject
    {
        graph,
        node,
        edge,
        all
    }; // lt. sizeof() 4 Bytes

    struct Key
    {
        char *id;              // 8 Bytes
        char *attrname;        // 8 Bytes
        char *attrtype;        // 8 Bytes
        char *_default;        // 8 Bytes
        enum GraphObject _for; // n.def. in Standart (sizeof() ergab 4 Bytes)
        //unsigned:4; // 4 Padding-Bytes

        // Destruktor
        ~Key(void)
        {
            free(this->id);
            free(this->attrname);
            free(this->attrtype);
            free(this->_default);
        }
    }; // 40 Bytes (inkl. 4 Padding-Bytes)

    struct Data
    {
        char *key;   // 8 Bytes
        char *value; // 8 Bytes

        ~Data(void)
        {
            free(this->key);
            free(this->value);
        }
    }; // 16 Bytes

    struct Node
    {
        // id des Knotens
        char *id; // 8 Bytes
        // Array Data-Objekte
        struct Data *datas; // 8 Bytes
        // Anzahl data-Elements
        uint8_t dcount; // 1 Byte
        // fortlaufende Nummer
        uint8_t no; // 1 Byte
        //unsigned:6; // 6 Padding Bytes

        // Destruktor
        ~Node(void)
        {
            free(id);

            // Data Objekte freigeben!
            // Dazu durch alle Data's iterieren und Destruktor aufrufen
            for (uint8_t i = 0; i < this->dcount; i++)
            {
                Data &data_ref = this->datas[i]; // Hilfsreferenz

                // für jede Data den Destruktor aufrufen um Member freizugeben:
                data_ref.~Data();

                // danach das gesamte Struct freigeben:
                free(&data_ref);
            }

            // Zum Schluss Zeigerarray freigeben:
            free(datas);
        }
    }; // 24 Bytes

    struct Edge
    {
        // id der Kante
        char *id; // 8 Bytes
        // id Quellknoten
        char *source; // 8 Bytes
        // id Zielknoten
        char *target; // 8 Bytes
        // Array Data-Objekte
        struct Data *datas; // 8 Bytes
        // Anzahl data-Elements
        uint8_t dcount; // 1 Byte
        // Gerichtet/Ungerichtet
        enum EdgeType type; // n. def. in Standart (sizeof() ergab 4 Bytes)
        //unsigned:7; // 7 Padding-Bytes

        // Destruktor
        ~Edge(void)
        {
            free(this->id);
            free(this->source);
            free(this->target);

            // Data Objekte freigeben!
            // Dazu durch alle Data's iterieren und Destruktor aufrufen
            for (uint8_t i = 0; i < this->dcount; i++)
            {
                Data &data_ref = this->datas[i]; // Hilfsreferenz

                // für jede Data den Destruktor aufrufen um Member freizugeben:
                data_ref.~Data();

                // danach das gesamte Struct freigeben:
                free(&data_ref);
            }

            // Zum Schluss Zeigerarray freigeben:
            free(this->datas);
        }
    }; // 40 Bytes (7 Padding Bytes)
} // namespace nsGraphML

#endif // ml_container.h