#pragma once
#include <fstream>
#include <memory>
#include "Circuit.h"

struct Transition
{
	Transition(Gate* g, int output, int t) : gate(g), newOutput(output), time(t) {}
	bool operator<(const Transition& other) const noexcept
	{
		if (time == other.time)
			return objectId < other.objectId;
		return time < other.time ;
	}
    bool operator>(const Transition& other) const noexcept
    {
        if (time == other.time)
            return objectId > other.objectId;
        return time > other.time ;
    }
	bool IsValid() const noexcept {  return gate->GetOutput() != newOutput; }
	void Apply();
	Gate* gate;
	int newOutput;
	int time;
	int objectId = ++GlobalId;
	static int GlobalId;
};

struct Probe
{
	bool operator<(const Probe& other) const noexcept
	{
		if (time == other.time)
			return newValue < other.newValue;
		return time < other.time;
	}
    std::string GetFileOutput();
	boost::property_tree::ptree GetJson();
	int time{};
	std::string gateName;
	int newValue{};
};



struct CompareTransitions {
    bool operator()(Transition const & t1, Transition const & t2) {
        // return "true" if "p1" is ordered before "p2", for example:
        return t1.time < t2.time;
    }
};


class Simulation
{
public:
    Simulation() : m_circuit(new Circuit()) {}
	static std::unique_ptr<Simulation> FromFile(std::ifstream& is);
	void LayoutFromFile(std::ifstream& is);
	void AddTransition(std::string gateName, int outputValue, int outputTime);
	Circuit* GetCircut() { return m_circuit.get(); }
    int Step();
    void Run();
	void ProbeAllGates() { m_undoLog = m_circuit->ProbeAllGates(); }
	void UndoProbeAllGates();
	boost::property_tree::ptree GetJson();
	void PrintProbes(std::ostream& os);
    std::string GetFileOutput();
private:
	std::unique_ptr<Circuit> m_circuit;
	std::string m_layout;
	std::vector<Transition> m_inTransitions;
    //PriorityQueue<Transition> m_queue;

	std::vector<Probe> m_probes;
    std::vector<Gate*> m_undoLog;
};
