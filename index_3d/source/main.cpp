#include <vtkActor.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkVertexGlyphFilter.h>

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include "rtree.h"
using namespace std;

template <class T>
pair<vector<string>, vector<vector<T>>> readPoints() {
  ifstream input("pokemon.csv");
  vector<vector<T>> coordenada; //record
  vector<string> names;
  string name;
  int num;
  float attack, defense, speed, height_m, hp, weight_kg_std;

  while (input >> num >> name >> attack >> defense >> speed >> height_m >> hp >> weight_kg_std) {
    vector<float> tmp{attack, defense, speed, height_m, hp, weight_kg_std};
    coordenada.emplace_back(tmp);
    names.push_back(name);
  }
  cout << "Ready points!" << endl;

  return make_pair(names, coordenada);
}

int main(int argc, char* argv[]) {
  cout << "Hola VTK" << std::endl;
  pair<vector<string>, vector<vector<float>>> info = readPoints<float>();
  RTree<Pokemon, 3> rtree;
  insertPoints<Pokemon,3, float>(rtree, info);
  cout << "Finish!\n";
  return 0;
}