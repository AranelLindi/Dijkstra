#include "graphml.h" // HEADER

using namespace rapidxml;  // für rapidxml
using namespace nsGraphML; // für GraphML-Container (ml_container.h)

GraphML::GraphML(const char *filepath)
{
    // Variablen initialisieren
    key_counter = 0, node_counter = 0, edge_counter = 0, data_counter = 0;

    uint16_t data_counter_node = 0, data_counter_edge = 0;

    // Definitionen für den Try-Block (damit Bereinigungen vorgenommen werden können)
    xml_document<> doc;
    xml_node<> *root_node;
    std::ifstream file;

    try
    {
        // ************************************
        // Prüfen ob GraphML überhaupt physisch exisitert:
        if (filepath == nullptr || (!(std::filesystem::exists(filepath))))
        {
            // Datei existiert nicht
            throw "File not found! (1)";
        }
        // ************************************

        // ####################################
        // 1. Mit dem Einlesen von XML beginnen     EINLESEN/START
        // ####################################

        // XML Datei mit Graphen öffnen:
        file.open(filepath);
        // dazu: vector mit Iterator anlegen, ...
        std::vector<char> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        // ... der XML komplett einliest:
        buffer.push_back('\0');
        // Parst den Vector in das xmldoc:
        doc.parse<0>(&buffer[0]);
        // Den root-Knoten finden:
        root_node = doc.first_node("graphml");

        // ####################################
        // 2. Durch alle Keys iterieren             KEY
        // ####################################

        this->keys = (Key *)calloc(0, sizeof(Key)); // key_counter = 0 bei erstem Durchgang
        // Zur Erklärung: realloc benötigt einen bereits allokierten Zeiger. Dieser
        // hier zeigt auf einen 0-Length-Memory und muss vor erneutem Einsatz
        // reallokiert werden.

        // durch key-Knoten itierieren
        for (xml_node<> *key_node = root_node->first_node("key"); key_node; key_node = key_node->next_sibling("key"))
        {
            // Erstes Element gefunden! Speicher des Arrays vergrößern und per Referenz Objekt füllen:
            this->keys = (Key *)realloc(this->keys, (this->key_counter + 1) * sizeof(Key));
            // Referenz als Hilfsobjekt:
            Key &key_ref = this->keys[key_counter];
            // Anzahl keys korrigieren:
            key_counter++;

            // Standartwerte setzen: Bei allen hier aufgelisteten kann sicher davon ausgegangen werden, dass ein Knoten/Attribut vorhanden ist
            key_ref.id = nullptr;
            key_ref._for = (GraphObject)0;
            key_ref.attrname = nullptr;
            key_ref.attrtype = nullptr;
            key_ref._default = nullptr; // free(nullptr) unternimmt laut Standart nichts!

            // XML: Attribute erfassen:
            // - id:
            key_ref.id = copyCharArray(key_node->first_attribute("id")->value());
            // - for:
            uint8_t _for_placeholder = 0;
            char *attr = key_node->first_attribute("for")->value();
            if (strcmp(attr, "graph") == 0)
                _for_placeholder = 0;
            else if (strcmp(attr, "node") == 0)
                _for_placeholder = 1;
            else if (strcmp(attr, "edge") == 0)
                _for_placeholder = 2;
            else if (strcmp(attr, "all") == 0)
                _for_placeholder = 4;
            key_ref._for = (GraphObject)_for_placeholder;
            // - attrname:
            key_ref.attrname = copyCharArray(key_node->first_attribute("attr.name")->value());
            // - attrtype:
            key_ref.attrtype = copyCharArray(key_node->first_attribute("attr.type")->value());
            // - default:
            //   Es muss nicht zwangsläufig ein 'default'-Knoten vorhanden sein, deswegen zuerst auf Existenz prüfen:
            xml_node<> *default_node = key_node->first_node("default");
            if (default_node != 0)
            {
                key_ref._default = copyCharArray(default_node->value());
            }
        }
        if (this->key_counter == 0)
        {
            // Sind keine Key-Knoten vorhanden, die entsprechende Referenz wieder auf einen nullptr zeigen lassen.
            // Der bisherige Zeiger, zeigt auf ein Speicherobjekt der Größe 0, das weder gelesen noch beschrieben
            // werden kann und soll. (undefiniertes Verhalten)
            // Va. im Destruktor wird dies Fehler verursachen.
            keys = nullptr;
        }

        // ####################################
        // 3. Graph Attribute abrufen               GRAPH
        // ####################################

        // Enthält den "graph"-Node, der die weitere Definition des Graphen enthält:
        xml_node<> *graph_node = root_node->first_node("graph");

        // - id
        this->id = copyCharArray(graph_node->first_attribute("id")->value());
        // - edgedefault
        this->edgedefault = (strcmp(graph_node->first_attribute("edgedefault")->value(), "directed") == 0 ? (EdgeType)0 : (EdgeType)1);

        // ####################################
        // 4. Durch alle Knoten iterieren           NODE
        // ####################################

        // Speicher allokieren:
        this->nodes = (Node *)calloc(0, sizeof(Node)); // selbes Prinzip wie bei keys

        // Durch Knoten iterieren:
        for (xml_node<> *nodes = graph_node->first_node("node"); nodes; nodes = nodes->next_sibling("node"))
        {
            // Speicher des übergeordneten Arrays reallokieren:
            this->nodes = (Node *)realloc(this->nodes, (node_counter + 1) * sizeof(Node)); // 1 mehr als vorher
            // Referenz als Hilfsvariable und auf letztes Element (aktuelles) referenzieren:
            Node &node_ref = this->nodes[this->node_counter];

            // Standartwerte setzen: (siehe Key-Objekt darüber)
            node_ref.id = nullptr;
            node_ref.datas = nullptr;
            node_ref.no = this->node_counter; // Nummer zuweisen

            this->node_counter++; // Anzahl Knoten inkrementieren (beginnt bei 0)

            // XML Attribute erfassen:
            node_ref.id = copyCharArray(nodes->first_attribute("id")->value());

            //      ####################################
            //      4.1 Durch alle Data- Knoten iterieren  NODE DATA
            //      ####################################
            {
                // Vorkehrungen für data-Array treffen:
                this->data_counter = 0; // Wichtig! Hat nichts mit Initialisierung zu tun, sondern beginnt bei jedem Knoten von vorne (also von 0)
                // Speicher allokieren:
                node_ref.datas = (Data *)calloc(0, sizeof(Data *));

                // durch data-Knoten innerhalb der node-Knoten iterieren:
                for (xml_node<> *data_nodes = nodes->first_node("data"); data_nodes; data_nodes = data_nodes->next_sibling("data"))
                {
                    // Speicher reallokieren:
                    node_ref.datas = (Data *)realloc(node_ref.datas, (this->data_counter + 1) * sizeof(Data)); // 1 mehr als vorher
                    // Referenz als Hilfsvariable und auf letztes Element (aktuelles) referenzieren:
                    Data &data_ref = node_ref.datas[this->data_counter];
                    // Anzahl Data-Knoten inkrementieren:
                    this->data_counter++;

                    // Standartwerte setzen: (Erklärung siehe Key-Standartwerte)
                    data_ref.key = nullptr;
                    data_ref.value = nullptr;

                    // XML Attribute erfassen:
                    // - key
                    data_ref.key = copyCharArray(data_nodes->first_attribute("key")->value());
                    // - value
                    data_ref.value = copyCharArray(data_nodes->value()); // Node value
                }
                if (this->data_counter == 0)
                {
                    // Erklärung: selbe Stelle bei Key!
                    node_ref.datas = nullptr;
                }

                // Zum Schluss, Anzahl data-Knoten merken
                node_ref.dcount = this->data_counter; // wird in nächster Iteration wieder auf 0 resettet
            }
        }
        if (this->node_counter == 0)
        {
            // Erklärung: selbe Stelle bei Key!
            this->nodes = nullptr;
        }

        // ####################################
        // 5. Durch alle Kanten iterieren       EDGES
        // ####################################

        // Speicherverwaltung (Siehe bei Nodes/Keys/Data)
        this->edges = (Edge *)calloc(0, sizeof(Edge));

        // durch edge-Knoten iterieren:
        for (xml_node<> *nodes = graph_node->first_node("edge"); nodes; nodes = nodes->next_sibling("edge"))
        {
            // Speicher reallokieren: (Erklärung siehe andere For-Schleifen darüber)
            this->edges = (Edge *)realloc(this->edges, (this->edge_counter + 1) * sizeof(Edge)); // 1 mehr als vorher
            // Referenz als Hilfsvariable und auf letztes Element (aktuelles) referenzieren:
            Edge &edge_ref = this->edges[this->edge_counter];
            // Anzahl Kanten-Knoten inkrementieren:
            this->edge_counter++;

            // Standartwerte setzen: (Erklärung siehe Key-Standartwerte)
            edge_ref.id = nullptr;
            edge_ref.source = nullptr;
            edge_ref.target = nullptr;
            edge_ref.datas = nullptr;

            // XML Attribute erfassen:
            // - id
            edge_ref.id = copyCharArray(nodes->first_attribute("id")->value());
            // - source
            edge_ref.source = copyCharArray(nodes->first_attribute("source")->value());
            // - target
            edge_ref.target = copyCharArray(nodes->first_attribute("target")->value());
            // - directed
            if (nodes->first_attribute("directed") != 0)
            {
                edge_ref.type = strcmp(nodes->first_attribute("directed")->value(), "true") == 0 ? (EdgeType)1 : (EdgeType)0;
            }

            //      ####################################
            //      5.1 Durch alle Data- Knoten iterieren  NODE DATA
            //      ####################################
            {
                // Vorkehrungen für data-Array treffen:
                data_counter = 0; // Wichtig! Keine Initialisierung, sondern Zurücksetzung. Wird für jede Kante seperat aufgeführt und muss zurückgesetzt werden
                // Speicher allokieren:
                edge_ref.datas = (Data *)calloc(0, sizeof(Data *));

                // durch Data-Knoten iterieren:
                for (xml_node<> *data_nodes = nodes->first_node("data"); data_nodes; data_nodes = data_nodes->next_sibling("data"))
                {
                    // Speicher des übergeordneten Arrays reallokieren:
                    edge_ref.datas = (Data *)realloc(edge_ref.datas, (this->data_counter + 1) * sizeof(Data)); // 1 mehr als vorher
                    // Referenz als Hilfsvariable und auf letztes Element (aktuelles) referenzieren:
                    Data &data_ref = edge_ref.datas[this->data_counter];
                    // Anzahl Kanten-Knoten inkrementieren:
                    this->data_counter++;

                    // Standartwerte setzen (Erklärungen bei gleichen Prodzeduren oben)
                    data_ref.key = nullptr;
                    data_ref.value = nullptr;

                    // XML Attribute lesen:
                    // - key
                    data_ref.key = copyCharArray(data_nodes->first_attribute("key")->value());
                    // - value
                    data_ref.value = copyCharArray(data_nodes->value());
                }
                if (this->data_counter == 0)
                {
                    // Erklärung selbe Stelle bei Key
                    edge_ref.datas = nullptr;
                }

                // Zum Schluss, Anzahl data-Knoten merken
                edge_ref.dcount = this->data_counter; // wird in nächster Iteration wieder auf 0 resettet
            }
        }
        if (this->edge_counter == 0)
        {
            // selbe Erklärung bei Keys!
            this->edges = nullptr;
        }

        // An dieser Stelle sind wir fertig, der Graf wurde vollständig eingelesen!
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    // Bereinigungen vornehmen:
    file.close();
}

GraphML::~GraphML(void)
{
    // Alle Objekte freigeben:
    // da neue Objekte mit malloc-basierten Funktionen allokiert wurden,
    // muss der Destruktor sämtlicher Objekte selbst aufgerufen werden:

    // 1. Nodes freigeben
    // dazu durch alle iterieren:
    for (int8_t i = 0; i < this->node_counter; i++)
    {
        Node &node_ref = this->nodes[i]; // Hilfsreferenz

        // für jeden Node den Destruktor aufrufen um Member freizugeben:
        // (ruft damit verbunden auch den Destruktor für jedes Data Objekt auf! (siehe Destruktor Node))
        node_ref.~Node();

        // danach das gesamte Struct freigeben:
        free(&node_ref);
    }

    // 2. Edges freigeben
    // dazu durch alle iterieren:
    for (uint8_t i = 0; i < this->edge_counter; i++)
    {
        Edge &edge_ref = this->edges[i]; // Hilfsreferenz

        // für jede Edge den Destruktr aufrufen um Member freizugeben:
        // (ruft damit verbunden auch den Destruktor für jedes Data Objekt auf! (siehe Destruktor Edge))
        edge_ref.~Edge();

        // danach das gesamte Struct freigeben:
        free(&edge_ref);
    }

    // Alle Kanten und Knoten freigeben
    free(this->edges);
    free(this->nodes);

    free(this->id);
}

bool GraphML::valid(void)
{
    // Verschiedene Dinge prüfen, bei einem Verstoß eine Exception werfen und false zurückgeben
    // Dabei auf grundlegende Dinge beschränken, eine logische Überprüfung erfolgt später in
    // graphstructure.

    try
    {
        /* ******************************************************************
        *   ANZAHL KNOTEN & KANTEN:
        *   Es müssen mindestens so viele Kanten wie Knoten vorhanden sein, jedoch höchstens v*(v-1)/2.
        *  *****************************************************************/
        if (this->edge_counter < (this->node_counter - 1) || (this->edge_counter > (this->edge_counter * (this->edge_counter - 1) / 2)))
        {
            throw "Unzulässige Anzahl Kanten im Verhältnis zu Knoten";
        }

        /* ******************************************************************
        *   ZEIGERVERBINDUNGEN:
        *   Prüfen ob bei untergeordneten Elementen Nullzeiger vorhanden sind
        *  *****************************************************************/

        // Lambda-Funktion zum Handling mit Nullzeigern
        auto inline func = [](void *ptr) -> void {
            if (ptr == nullptr)
                throw "Nullzeiger entdeckt!";
        };

        // *********************************
        // Keys:
        //
        // durch Keys iterieren...
        for (uint8_t i = 0; i < this->key_counter; i++)
        {
            // ... bei jedem Key die Eigenschaften anschauen
            Key &key_ref = this->keys[i]; // Hilfsreferenz

            func(key_ref.id);
            func(key_ref.attrname);
            func(key_ref.attrtype);
            func(key_ref._default);
        }

        // *********************************
        // Nodes:
        //
        // durch Nodes iterieren...
        for (uint8_t i = 0; i < this->node_counter; i++)
        {
            // ... bei jedem Node die Eigenschaften anschauen
            Node &node_ref = this->nodes[i]; // Hilfsreferenz

            // dabei: durch die Data-Eigenschaften eines Nodes iterieren:
            for (uint8_t j = 0; j < node_ref.dcount; j++)
            {
                Data &data_ref = node_ref.datas[j]; // Hilfsreferenz

                func(data_ref.key);
                func(data_ref.value);
            }

            // jetzt die Node Eigenschaften anschauen:
            func(node_ref.id);
        }

        // *********************************
        // Edges:
        //
        // durch Edge iterieren...
        for (uint8_t i = 0; i < this->edge_counter; i++)
        {
            // ... bei jedem Edge die Eigenschaften anschauen
            Edge &edge_ref = this->edges[i]; // Hilfsreferenz

            // dabei: durch die Data-Eigenschaften eines Edge iterieren:
            for (uint8_t j = 0; j < edge_ref.dcount; j++)
            {
                Data &data_ref = edge_ref.datas[j]; // Hilfsreferenz

                func(data_ref.key);
                func(data_ref.value);
            }

            // jetzt die Edge Eigenschaften anschauen:
            func(edge_ref.id);
            func(edge_ref.source);
            func(edge_ref.target);
        }

        /* ******************************************************************
        *   Verbindungen der Kanten:
        *   1.) Grundsätzlich: Prüfen ob jede ID auch tatsächlich auf einen gültigen Knoten zeigt, dieser also vorhanden ist.
        *  *****************************************************************/
        // Dazu: Durch alle Knoten iterieren und ihre id's in eine Liste aufnehmen.
        // Anschließend durch alle Edges iterieren und deren source/target Attribut
        // mit dieser Liste vergleichen:
        std::vector<char *> n_ids; // enthält die Zeiger auf die id's der Knoten
        // durch Nodes iterieren:
        for (uint8_t i = 0; i < this->node_counter; i++)
        {
            Node &node_ref = this->nodes[i]; // Hilfsreferenz

            // id in vector aufnehmen:
            n_ids.push_back(node_ref.id);
        }

        // durch Edges iterieren:
        for (uint8_t i = 0; i < this->edge_counter; i++)
        {
            Edge &edge_ref = this->edges[i]; // Hilfsreferenz

            bool sourcefound = false;
            bool targetfound = false;

            // source & target mit vector abgleichen
            for (char *const &value : n_ids)
            {
                if (strcmp(value, edge_ref.source) == 0)
                    sourcefound = true;
                if (strcmp(value, edge_ref.target) == 0)
                    targetfound = true; // hier nicht mit else if unterscheiden! Denn:
                // zu diesem Zeitpunkt wäre es noch gestattet, eine Schleife, also Ausgangs- und Zielknoten identisch, zu haben.
                // dies wird erst in graphstructure ausgeschlossen.
            }

            // Ist zu diesem Zeitpunkt eine der beiden oberen Variablen auf 'false' existiert der Knoten nicht, zu
            // dem die Kante hin/weg verläuft. Dann eine Exception werfen.
            throw "Ungültige Knotenreferenz";

            // in der nächsten Iteration werden die beiden oberen bool-Variablen wieder auf 'false' zurückgesetzt und
            // Suche beginnt von neuem.
        }

        /* ******************************************************************
        *   Keys:
        *   1.) "attr.name must be unique among all GraphML-Attributes declared in the document."
        *  *****************************************************************/
        // Alle attr.name in eine Liste aufnehmen und vorher prüfen ob sie schon Teil deren ist:
        // Durch Keys iterieren:
        std::vector<char *> k_attrname; // Enthält Zeiger auf key-Id's

        // durch alle Key iterieren:
        for (uint8_t i = 0; i < this->key_counter; i++)
        {
            Key &key_ref = this->keys[i]; // Hilfsreferenz

            // jedes Element des Vectors anschauen und prüfen ob das aktuelle key_ref bereits aufgenommen ist:
            for (char *const &value : k_attrname)
            {
                if (strcmp(value, key_ref.attrname) == 0)
                {
                    // Es befindet sich bereits ein key-Attribut mit gleicher id im Graphen.
                    // Das ist nicht zulässig, da GraphML vorschreibt, dass ID für key im
                    // gesamte Graphen einzigartig sein müssen.
                    throw "Doppeltes Key-Attribut entdeckt";
                }
            }
        }

        /* ******************************************************************
        *   Graph:
        *   1.) Attribute müssen korrekt ausgefüllt sein
        *  *****************************************************************/
        // Nur die id des Graphen prüfen:
        func(this->id);
    }
    catch (const std::exception &e)
    {
        // Bei einer Exception wird hier reingesprungen, der Fehler ausgegeben und false zurückgegeben.
        // Führt dazu, dass die Fehlersuche bei fehlerhaftem XML länger dauert, weil nur ein Fehler pro
        // Iteration von valid() ausgegeben werden kann.
        std::cerr << e.what() << std::endl;
        return false;
    }

    // Wurde bis hierhin gelangt, ist der Graph in Ordnung:
    return true;
}

uint8_t inline GraphML::getkeys(void)
{
    return this->key_counter;
}

uint8_t inline GraphML::getnodes(void)
{
    return this->node_counter;
}

uint8_t inline GraphML::getedges(void)
{
    return this->edge_counter;
}

const bool *const *const GraphML::getAdjacencyMatrix(void)
{
    // Hier kann davon ausgegangen werden, dass der Graph wohldefiniert ist und keine Fehler vorhanden sind:

    bool **matrix;

    matrix = (bool **)calloc(this->node_counter, sizeof(bool *)); // Spalten
    for (uint8_t i = 0; i < this->node_counter; i++)
    { // Zeilen
        matrix[i] = (bool *)calloc(this->node_counter, sizeof(bool));
    }

    // Alle Matrizeneinträge auf 'false' setzen:
    for (uint8_t i = 0, i; this->node_counter; i++)
    {
        for (uint8_t j = 0; j < this->node_counter; j++)
        {
            matrix[i][j] = false;
        }
    }

    // Werte setzen, dazu alle edges durchlaufen und bei Verbindungen in der Matrix 'true' setzen:
    for (uint8_t i = 0; i < this->edge_counter; i++)
    {
        // Für jeden Edge muss folgendes überprüft werden:
        // - Identifizierung der beiden Knoten über deren ID (NODE.no ist aufsteigende Nummer -> Arrayadresse)

        // Referenz als Hilfsvariable. Referenziert immer aktuelle Kanten die betrachtet wird:
        Edge &edge_ref = this->edges[i];

        uint8_t source = 0, target = 0;

        for (int j = 0; j < this->node_counter; j++)
        { // alle Knoten durchlaufen:
            // Referenz als Hilfsvariable. Referenziert immer aktuellen Knoten der gerade betrachtet wird:
            Node &node_ref = this->nodes[j];

            // Bevor teuer auf Verbindung geprüft wird, schauen, ob bereits Verbindung eingetragen ist (kann höchstens bei ungerichteten Graphen (siehe unten) passiert sein)
            if (matrix[source][target] == true)
                continue;

            if (strcmp(node_ref.id, edge_ref.source) == 0)
                source = node_ref.no; // Quelle gefunden
            else if (strcmp(node_ref.id, edge_ref.target) == 0)
                target = node_ref.no; // Ziel gefunden
            // Kurze Anmerkung: Beides für eine Knoten ist ausgeschlossen, deswegen hier nur auf eines prüfen
        }

        // Verbindung eintragen:
        matrix[source][target] = true;

        // Ist die Kante ungerichtet, dann auch in umgekehrter Richtung markieren:
        if (edge_ref.type == (EdgeType)0)
            matrix[target][source] = true;

        // Matrix unveränderlich machen:
        const bool *const *const constantMatrix = matrix;

        return constantMatrix;
    }
}

// Übergebene Matrix freigeben. (Statische Methode!)
void GraphML::freeMatrix(const bool *const *const AdjM, uint8_t nodes)
{
    // Übergebenes Const-Objekt "entconsten"
    bool **matrix = const_cast<bool **>(AdjM);

    // Durch Knoten iterieren und Zeilenweise freigeben:
    for (uint8_t i = 0; i < nodes; i++)
    {
        free(matrix[i]);
    }

    // Zum Schluss, Zeigerspalte freigeben
    free(matrix);
}