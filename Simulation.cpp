#include <sstream>
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/regex.hpp>
#include <boost/property_tree/ptree.hpp>
#include <algorithm>
#include <queue>

#include "Circuit.h"
#include "Simulation.h"

int Transition::GlobalId = 0;

void Transition::Apply()
{
	if (!IsValid())
		throw std::runtime_error("Gate output should not transition to the same value");
	gate->SetOutput(newOutput);
}
/*
boost::property_tree::ptree Probe::GetJson()
{
	boost::property_tree::ptree children;
	boost::property_tree::ptree pt;
	pt.put_value(time);
	children.push_back(std::make_pair("", pt));

	pt.put_value(gateName);
	children.push_back(std::make_pair("", pt));

	pt.put_value(newValue);
	children.push_back(std::make_pair("", pt));

	return children;
}
*/
std::string Probe::GetFileOutput()
{
    std::string shift = "\t\t";
    std::string a="\n"+shift+"[\n";
    a = a+ shift+"\t\""+std::to_string(time)+"\",\n";
    a = a+ shift+"\t\""+gateName+"\",\n";
    a = a+ shift+"\t\""+std::to_string(newValue)+"\"\n";
    a = a+ shift+"]";
    return a;
}


void Simulation::AddTransition(std::string gateName, int outputValue, int outputTime)
{
	Gate* pGate = m_circuit->GetGate(gateName);
	m_inTransitions.emplace_back(Transition{ pGate, outputValue, outputTime });
}

std::unique_ptr<Simulation> Simulation::FromFile(std::ifstream& is)
{
	auto simulation = std::make_unique<Simulation>();
    auto* circut = simulation->GetCircut();
    std::string line;
    /*
     *  I replaced for(;;){
     */
    while (std::getline(is, line))
	{
		boost::char_separator<char> sep(" ");
		boost::tokenizer< boost::char_separator<char>> tokens(line, sep);
		std::vector<std::string> command;
		for (const auto& v : tokens)
			command.emplace_back(v);
		if (command.empty())
			continue;
		if (command[0] == "table")
		{
			std::vector<int> outputs;
			for (size_t i = 2; i < command.size(); ++i)
				outputs.emplace_back(std::stoi(command[i]));
			circut->AddTruthTable(command[1], outputs);
		}
		else if (command[0] == "type")
		{
			if (command.size() != 4)
				throw std::runtime_error("Invalid number of arguments for gate type");
			circut->AddGateType(command[1], command[2], std::stoi(command[3]));
		}
		else if (command[0] == "gate")
		{
			std::vector<std::string> inputs;
			for (size_t i = 3; i < command.size(); ++i)
				inputs.emplace_back(command[i]);
			circut->AddGate(command[1], command[2], inputs);
		}
		else if (command[0] == "probe")
		{
			if (command.size() != 2)
				throw std::runtime_error("Invalid number of arguments for probe type");
			circut->AddProbe(command[1]);
		}
		else if (command[0] == "flip")
		{
			if (command.size() != 4)
				throw std::runtime_error("Invalid number of arguments for flip type");
			simulation->AddTransition(command[1], std::stoi(command[2]), std::stoi(command[3]));
		}
		else if (command[0] == "done")
			break;

	}
	return simulation;
}

void Simulation::LayoutFromFile(std::ifstream& is)
{
	std::string temp;
	while (std::getline(is, temp))
	{
		if (temp == "layout")
			break;
	}
	
	boost::regex xmlRegex("^<\\?xml.*\\?>\n");
	boost::regex docTypeRegex("^<!DOCTYPE.*?>\n");
	std::ostringstream sstr;
	sstr << is.rdbuf();
	std::string str = sstr.str();
	str = boost::regex_replace(str, xmlRegex, "");
	str = boost::regex_replace(str, docTypeRegex, "");
	m_layout = str;
}

/*
int Simulation::Step()
{
    int stepTime = m_queue.min()->time;
	std::vector<Transition> transitions;
    while (m_queue.len() > 0 && m_queue.min()->time == stepTime)//finds the one having a minimum time
	{
        auto transition = m_queue.pop();//pops the one having a minimum time
		if (!transition->IsValid())
			continue;
		transition->Apply();
		if (transition->gate->IsProbed())
			m_probes.emplace_back(Probe{ transition->time, transition->gate->GetName(), transition->newOutput });
		transitions.emplace_back(*transition);
	}


	for (const auto transition : transitions)
	{
		for (auto* gate : transition.gate->GetOutGates())
		{
			auto output = gate->GetTransitionOutput();
			auto time = gate->GetTransitionTime(stepTime);
            m_queue.append(Transition(gate, output, time));
		}
	}
	return stepTime;
}
*/

void Simulation::Run()
{
    //std::sort(m_inTransitions.begin(), m_inTransitions.end());
    /*
     * second problem may be fixed  Monday, May 29, 2023 (GMT+12) am new Zealand time
     * after discovering this I kept trying to get rid of the O(3) - it's a test so I did not proceed with too many changes
     * a tree for the queue would've decreased o(1)=n to log(n)
     * the program is able to simulate stable state machines like controllers and memory. Simplifying it to finite loops would've limited it to data paths
     * std sort is probably n*log(n) using a merge sort or a binary tree sort.
    */

    //int i=0 ;
    //for (const auto& t : m_inTransitions){
    //    m_queue.append(t);
    //}
    std::priority_queue<Transition, std::vector<Transition>, std::greater<Transition>>
            m_queue(m_inTransitions.begin(), m_inTransitions.end());

    while (m_queue.size() > 0){
        int stepTime =  m_queue.top().time;
        std::vector<Transition> transitions;
        while (m_queue.size() > 0 && m_queue.top().time == stepTime)//m_queue.min() returns the smallest time
        {
            auto transition = m_queue.top();
            m_queue.pop();//pops the smallest time
            if (!transition.IsValid()){
                continue;
            }
            transition.Apply();
            if (transition.gate->IsProbed())
                m_probes.emplace_back(Probe{ transition.time, transition.gate->GetName(), transition.newOutput });
            transitions.emplace_back(transition);
        }

        for (const auto transition : transitions)
        {
            for (auto* gate : transition.gate->GetOutGates())
            {
                auto output = gate->GetTransitionOutput();
                auto time = gate->GetTransitionTime(stepTime);
                m_queue.push(Transition(gate, output, time));
            }
        }
    }
    //std::sort(m_probes.begin(), m_probes.end());
}

void Simulation::UndoProbeAllGates()
{
	for (auto* gate : m_undoLog)
	{
		gate->UndoProbe();
	}
	m_undoLog.clear();
}
/*
boost::property_tree::ptree Simulation::GetJson()
{
    std::string a = "";
    boost::property_tree::ptree pt;
    pt.add_child("cuircuit", m_circuit->GetJson());
	boost::property_tree::ptree probes;
	for (auto& p : m_probes)
		probes.push_back(std::make_pair("",p.GetJson()));
	pt.add_child("trace", probes);
	pt.add("layout", m_layout);
	return pt;
}
*/
std::string Simulation::GetFileOutput()
{
    std::string shift = "\t";
    std::string a = "";
    a = a + m_circuit->GetFileOutput();
    a = a + shift+"\"trace\":\n";
    a = a + shift +"[";
    int i=0;
    for (auto& p : m_probes){
        a = a+ p.GetFileOutput();
        if(i<m_probes.size()-1){
            a = a+ ",\n";
        }else{
            a = a+ "\n";
        }
    }
    a = a + shift+"],\n";

    // s is our escaped output string
    std::string s = "";
    // loop through all characters
    for(char c : m_layout)
    {
        if (c != '\\' and c != '"') {
          s = s+c;
        }else{
           s= s+"\\";
           s = s+c;
        }
    }

    a = a + "\t\"layout\": \""+ s+"\"";
    //pt.add_child("trace", probes);
    //pt.add("layout", m_layout);
    return a;
}

void Simulation::PrintProbes(std::ostream& os)
{

	for (const auto& probe : m_probes)
	{
        //related to request 1
        //if (!m_circuit->GetGate(probe.gateName)->IsProbed())
        //    continue;

		os << probe.time << " " << probe.gateName << " " << probe.newValue << std::endl;
	}
		
}
