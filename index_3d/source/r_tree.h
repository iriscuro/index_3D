#ifndef RTREE_H
#define RTREE_H
#include<bits/stdc++.h>

using namespace std;

//Pokemon
int const MAXR = 10;

class Pokemon {
  public:
    float a, d, s;//T x,y,z; //attack, defense, speed.
    string name;
    
    Pokemon(float a, float b, float c, string name_) : a(a), d(b), s(c), name(name_) {}
    Pokemon() : a(-1), d(-1), s(-1), name("") {}
    bool operator==(const Pokemon p) {
      return this->a == p.a and this->d == p.d and this->s == p.s;
    }
    void print() { cout <<"Name: "<<name<< " (" << a << "," << d << "," << s << ")\n"; }
  };


template <class T> 
class Point {
  public:
    T x, y, z;//height_m, hp y weight_kg_std.
    Point(T a, T b, T c) : x(a), y(b), z(c) {}
    Point() : x(-1), y(-1), z(-1) {}
    bool operator==(const Point<T> p) {
      return this->x == p.x and this->y == p.y and this->z == p.z;
    }
    void print() { cout << "(" << x << "," << y << "," << z << ")\n"; }
  };

template < int ndim >
class Point2 {
    public:
        //T x,y,z; //attack, defense, speed.
        vector<float> dim = {0};
        Point2 (vector<float> dim_) {
            for(int i=0; i<ndim; i++) dim[i] = dim_[i];
        }   
        Point2 () {}  
        bool operator == (Point2 < ndim > p){
            bool es = (this->dim[0] == p.dim[0]) ;
            for(int i=1; i<ndim; i++) 
                es = es and (this->dim[i] == p.dim[i]);
            return es;
        }
        void print(){
            cout << "(";
            for(int i=0; i<ndim; i++)
                cout << dim[i] << "," ; 
            cout<<")\n";
        }   
};

template< typename T, int ndim >
class Record {
    public:
    T object; //aqui el pokemon (aqui está su tamaño)
    Point2<ndim> tuple; //coordenada donde está el pokemon
    Record();
    Record(T& obj, Point2<ndim>& tup) : object(obj), tuple(tup) {}
};

template< typename T, int ndim >
class Node { //Region //Rectangle //MBR
    private:
        vector< Record<T,ndim> > records;//si es hoja
        vector< Node<T,ndim>* > children;//nodos hijo
        Point2<ndim> bottomLeft;
        Point2<ndim> upperRight;
        int maxRecords = MAXR;
        int countRecords;
        bool isLeaf;
    public:
        Node();
        Node(int maxRecords, bool isLeaf);
        bool insert(const Record<T,ndim> &record);
        //insert utilizado principalmente por RTree
        bool insert(Node *node1, Node *node2);
        pair< Node*, Node*> split();
        void updateRegion();
};

template< typename T, int ndim >
class RTree {
    private:
      Node<T,ndim> *root;
      
    public:
      int maxRecords = MAXR;

      RTree();
      void insert( const Record<T,ndim> &rec );
      void insert2( const Record<T,ndim> &rec );
};
//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
template<typename T, int ndim>
Node<T, ndim>::Node(int maxRecords, bool isLeaf) :
    records(),
    children(),
    bottomLeft(Point2<ndim>()),
    upperRight(Point2<ndim>()),
    maxRecords(maxRecords),
    countRecords(0),
    isLeaf(isLeaf) {}

template< typename T, int ndim >
pair<Node<T,ndim>*, Node<T,ndim>*>
Node<T,ndim>::split() {
    // Crear nuevos nodos para dividir el contenido del nodo actual
    Node<T,ndim> *newNode1 = new Node<T,ndim>();
    Node<T,ndim> *newNode2 = new Node<T,ndim>();
    
    // Distribuir los registros entre los dos nuevos nodos
    int mid = countRecords / 2;
    for (int i = 0; i < countRecords; i++) {
        if (i < mid) {
            newNode1->records.push_back(records[i]);
        } else {
            newNode2->records.push_back(records[i]);
        }
    }
    
    // Limpiar los registros del nodo original
    records.clear();
    
    // Insertar los nuevos nodos como hijos del nodo actual
    children.clear();
    children.push_back(newNode1);
    children.push_back(newNode2);
    
    // Actualizar las regiones de los nuevos nodos
    newNode1->updateRegion();
    newNode2->updateRegion();
    
    // Devolver los nuevos nodos creados
    return make_pair(newNode1, newNode2);
}


template<typename T, int ndim>
bool Node<T, ndim>::insert(const Record<T, ndim>& record) {
    // Si el nodo es una hoja, inserta el registro en el vector de registros.
    if (isLeaf) {
        // Construye un objeto Record sin argumentos.
        Record<T, ndim> newRecord;
        // Establece los campos del nuevo registro.
        newRecord.object = record.object;
        newRecord.tuple = record.tuple;
        // Agrega el registro al vector de registros.
        records.push_back(newRecord);
        countRecords++;
        // Si el número de registros en el nodo supera el límite, se debe realizar una división.
        if (countRecords > maxRecords) {
            pair<Node*, Node*> newNodes = split();
            return false;
        }
        return true;
    }
    // Si el nodo no es una hoja, busca el hijo adecuado y llama a su método insert.
    // ...
}


template< typename T, int ndim >
bool Node<T,ndim>::insert(Node<T,ndim> *node1, Node<T,ndim> *node2) {
    // Actualizar los límites inferior y superior de la región del nodo actual
    bottomLeft = node1->bottomLeft;
    upperRight = node2->upperRight;

    // Verificar si el nodo actual ha alcanzado su capacidad máxima
    if (countRecords > maxRecords) {
        // Dividir el nodo y devolver true para indicar que se ha realizado un split
        split();
        return true;
    }

    // Actualizar la región del nodo y devolver false para indicar que no se ha realizado un split
    updateRegion();
    return false;
}


template<typename T, int ndim>
void Node<T, ndim>::updateRegion() {
    if (records.empty()) {
        // Si no hay registros, la región del nodo es indefinida
        bottomLeft = Point2<ndim>();
        upperRight = Point2<ndim>();
        return;
    }

    // Establecemos bottomLeft y upperRight en el primer registro
    bottomLeft = records[0].tuple;
    upperRight = records[0].tuple;

    // Iteramos sobre los demás registros para ampliar la región del nodo
    for (const auto &record : records) {

      // Actualizar las coordenadas bottomLeft y upperRight
        for (int i = 0; i < ndim; i++) {
            bottomLeft.dim[i] = min(bottomLeft.dim[i], record.tuple.dim[i]);
            upperRight.dim[i] = max(upperRight.dim[i], record.tuple.dim[i]);
        }
    
      
    }
}

template<typename T, int ndim>
RTree<T, ndim>::RTree() : root(new Node<T, ndim>(MAXR, true)) {}


template<typename T, int ndim>
void RTree<T, ndim>::insert2(const Record<T, ndim> &rec) {
    if (root == nullptr) {
        root = new Node<T, ndim>(MAXR, true);
    }
    root->insert(rec);
}

template< typename T, int ndim >
void RTree<T,ndim>::insert( const Record<T,ndim> &rec ) {
    if (root == nullptr) {
        root = new Node<T,ndim>(MAXR, true);
    }

    // Intentamos insertar el registro en el nodo raíz.
    // Si no hay suficiente espacio, se hace un split y se crean dos nuevos nodos.
    // Luego se intenta insertar el registro nuevamente en el nodo raíz.
    if (!root->insert(rec)) {
        pair< Node<T,ndim>*, Node<T,ndim>* > nodes = root->split();
        Node<T,ndim>* newNode1 = nodes.first;
        Node<T,ndim>* newNode2 = nodes.second;
        root->insert(newNode1, newNode2);
        root->insert(rec);
    }
}
#endif