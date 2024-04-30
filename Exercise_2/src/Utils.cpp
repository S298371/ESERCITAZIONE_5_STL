#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>


namespace PolygonalLibrary {
bool ImportMesh(const string& filepath,
                PolygonalMesh& mesh)
{
    if(!ImportCell0Ds(filepath + "/Cell0Ds.csv",
                       mesh))
    {
        return false;
    }
    else
    {
        cout << "Cell0D marker:" << endl;

        for(auto it = mesh.Cell0DMarkers.begin(); it != mesh.Cell0DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }
    }

    if(!ImportCell1Ds(filepath + "/Cell1Ds.csv",
                       mesh))
    {
        return false;
    }
    else
    {
        cout << "Cell1D marker:" << endl;
        for(auto it = mesh.Cell1DMarkers.begin(); it != mesh.Cell1DMarkers.end(); it++)
        {
            cout << "key:\t" << it -> first << "\t values:";
            for(const unsigned int id : it -> second)
                cout << "\t" << id;

            cout << endl;
        }
    }

    if(!ImportCell2Ds(filepath + "/Cell2Ds.csv",
                       mesh))
    {
        return false;
    }
    else
    {
        // Test vertici non coincidono:
        for(unsigned int c = 0; c < mesh.NumberCell2D; c++)
        {
            auto poligono = mesh.Cell2DVerticles[c];
            for (unsigned int v=0; v< poligono.size(); v++){
                auto vertice = poligono[v];
                for (unsigned int w=v+1; w < poligono.size();w++){
                    if(mesh.Cell0DCoordinates[vertice]==mesh.Cell0DCoordinates[poligono[w]]){
                        cerr << "non tutti i lati dei poligoni sono diversi da zero"<<endl;
                        return 2;
                    }
                }
            }
        }
        cout<<"i lati del poligono sono tutti diversi da zero"<<endl;

        //test lati sono collegati

        for(unsigned int p = 0; p < mesh.NumberCell2D; p++){
            auto poligono=mesh.Cell2DVerticles[p];
            auto archi=mesh.Cell2DEdges[p];
            for (unsigned int v=0; v<poligono.size(); v++){
                auto a=poligono[v]; //id vertice a
                auto b= poligono[(v+1)%poligono.size()]; //id vertice b
                Vector2i arco1;
                arco1[0]=a;
                arco1[1]=b;
                Vector2i arco2;
                arco2[0]=b;
                arco2[1]=a;
                auto lato=archi[v]; //id del lato;

                if (arco1!=mesh.Cell1DVertices[lato] && arco2!=mesh.Cell1DVertices[lato] ){
                    cout<<arco1<<endl;
                    cout<<arco2<<endl;
                    cout<<mesh.Cell1DVertices[lato]<<endl;
                    cerr<< "i lati non sono collegati"<<endl;
                    return 3;
                }

            }
        }
        cout<<"tutti i poligono sono chiusi e con area diversa da zero"<<endl;

    }

    return true;

}
bool ImportCell0Ds(const string &filename,
                   PolygonalMesh& mesh)
{

    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line)){
        listLines.push_back(line);
    }

    file.close();

    listLines.pop_front();

    mesh.NumberCell0D = listLines.size();

    if (mesh.NumberCell0D == 0)
    {
        cerr << "There is no cell 0D" << endl;
        return false;
    }

    mesh.Cell0DId.reserve(mesh.NumberCell0D);
    mesh.Cell0DCoordinates.reserve(mesh.NumberCell0D);

    for (const string& line : listLines)
    {
        vector <float> elementi;
        istringstream iss (line);
        string elemento;
        while (getline(iss, elemento, ';'))
            elementi.push_back(stof(elemento));

        unsigned int id;
        unsigned int marker;
        Vector2d coord;
        id=int(elementi[0]);
        marker=int(elementi[1]);
        coord[0]=elementi[2];
        coord[1]=elementi[3];

        mesh.Cell0DId.push_back(id);
        mesh.Cell0DCoordinates.push_back(coord);

        if( marker != 0)
        {

            auto ret = mesh.Cell0DMarkers.insert({marker, {id}});
            if(!ret.second)
                (ret.first)->second.push_back(id);
        }

    }
    file.close();
    return true;
}

bool ImportCell1Ds(const string &filename,
                   PolygonalMesh& mesh)
{

    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    listLines.pop_front();

    mesh.NumberCell1D = listLines.size();

    if (mesh.NumberCell1D == 0)
    {
        cerr << "There is no cell 1D" << endl;
        return false;
    }

    mesh.Cell1DId.reserve(mesh.NumberCell1D);
    mesh.Cell1DVertices.reserve(mesh.NumberCell1D);

    for (const string& line : listLines)
    {
        vector <float> elementi;
        istringstream iss (line);
        string elemento;
        while (getline(iss, elemento, ';'))
            elementi.push_back(stof(elemento));

        unsigned int id;
        unsigned int marker;
        Vector2i vertices;

        id=int(elementi[0]);
        marker=int(elementi[1]);
        vertices[0]=int(elementi[2]);
        vertices[1]=int(elementi[3]);

        mesh.Cell1DId.push_back(id);
        mesh.Cell1DVertices.push_back(vertices);

        if( marker != 0)
        {

            auto ret = mesh.Cell1DMarkers.insert({marker, {id}});
            if(!ret.second)
                (ret.first)->second.push_back(id);
        }
    }

    file.close();

    return true;
}
bool ImportCell2Ds(const string &filename,
                   PolygonalMesh& mesh)
{

    ifstream file;
    file.open(filename);

    if(file.fail())
        return false;

    list<string> listLines;
    string line;
    while (getline(file, line))
        listLines.push_back(line);

    listLines.pop_front();

    mesh.NumberCell2D = listLines.size();

    if (mesh.NumberCell2D == 0)
    {
        cerr << "There is no cell 2D" << endl;
        return false;
    }

    mesh.Cell2DId.reserve(mesh.NumberCell2D);
    mesh.Cell2DVerticles.reserve(mesh.NumberCell2D);
    mesh.Cell2DEdges.reserve(mesh.NumberCell2D);
    int count=0;
    for (const string& line : listLines) {
        vector<unsigned int> Verticles = {};
        vector<unsigned int> Edges = {};
        vector <int> elementi;
        istringstream iss (line);
        string elemento;
        while (getline(iss, elemento, ';'))
            elementi.push_back(stoi(elemento));

        unsigned int id;
        unsigned int NumVerticles;
        unsigned int NumEdges;

        id=elementi[0];
        NumVerticles=elementi[2];
        for(unsigned int i=0; i<NumVerticles; i++ ){
            Verticles.push_back(elementi[3+i]);
        }

        NumEdges=elementi[3+NumVerticles];
        for(unsigned int i=0; i<NumEdges; i++ ){
            Edges.push_back(elementi[NumVerticles+4+i]);
        }


        mesh.Cell2DId.push_back(id);
        count++;

        mesh.Cell2DVerticles.push_back(Verticles);
        mesh.Cell2DEdges.push_back(Edges);
        /* stampa il vettore
        for(const auto& v: mesh.Cell2DVerticles){
            for(const auto& elemento : v){
                cout<< elemento<< " ";
            }
            cout<<endl;
        }
        */
    }
    file.close();

    return true;
}
}

