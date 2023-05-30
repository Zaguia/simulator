// Simulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <fstream>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>



#include "Simulation.h"

int main(int argc, char** argv)
{
    /*
     * started to work on the project 28 of may 2023 at 1:44 am new zealand time
     * the setup of boost took too much time (on a mac)
    */
    //argc = 3;
    //argv[1] = strdup("/Users/machd/simulator/tests/3xor.in");
    //argv[2] = strdup("json");

    if (argc < 2)
    {
        std::cout << "Simulator.exe <simfile> [json]" << std::endl;
        std::cout << "simulation output is in circuit.jsonp" << std::endl;
        exit(0);
    }
    bool json = (argc >= 3 && "json" == std::string(argv[2]));
    std::ifstream input(argv[1], std::ios::in);
    auto simulation = Simulation::FromFile(input);

    //related to request 1
    //if (json)
    //{
        simulation->LayoutFromFile(input);
        // probe all gates should only be executed when
        // json output is on 
        simulation->ProbeAllGates();
    //}
        /*
         * first problem fixed the  28 of may 2023 at 2:25 am new zealand time
         * the debugger is refusing to work
        */




    simulation->Run();
    if (json)
        simulation->UndoProbeAllGates();
    if (argc >= 3 && "json" == std::string(argv[2]))
    {
        //boost::property_tree::ptree simResult = simulation->GetJson();
        std::string simultionResult = simulation->GetFileOutput();
        std::ofstream output("circuit.jsonp", std::ios::out);
        //std::ofstream output2("circuit2.jsonp", std::ios::out);
        //output << "onJsonp(";
        //boost::property_tree::write_json(output, simResult);
        //output << ");\n";
        output << "onJsonp(\n";
        output << simultionResult;
        output << "\n);\n";
    }
    simulation->PrintProbes(std::cout);
}
