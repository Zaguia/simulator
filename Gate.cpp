#include <stdexcept>
#include "Gate.h"


namespace
{
	void RaiseError(const char* c) { throw std::runtime_error(c); }
}
void Gate::ConnectInput(int i, Gate* target)
{
	if (m_inGates.find(i) != m_inGates.end())
		throw std::runtime_error("input terminal already connected");
	m_inGates.insert({ i, target });
	target->AddOutput(this);
}

void Gate::AddOutput(Gate* target)
{
	m_outGates.emplace_back(target);
}

void Gate::Probe() noexcept
{
	if (m_probed)
		RaiseError("Gate already probed");
		
	m_probed = true;
}

std::string Gate::bool_cast(const bool b) {
    return b ? "true" : "false";
}

std::string Gate::GetFileOutput(){
    std::string shift = "\t\t";
    std::string a = shift+"[\n";


    a = a+ shift+"\t\"gateId\": \""+ m_name+"\",\n";
    a = a+ shift+"\t\"table\": \""+ m_type->GetTruthTableName()+"\",\n";
    a = a+ shift+"\t\"type\": \""+ m_type->GetType()+"\",\n";
    a = a+ shift+"\t\"probed\": \""+ bool_cast(m_probed)+"\",\n";

    a = a+ shift+"\t\"inputs\":";
    if(m_inGates.size()==0){
        a = a + "\"\",\n";
    }else{
        a = a +"\n"+shift+"\t[\n";
        int i=0;
        for (auto& [k,v] : m_inGates)
        {
            a = a+ shift+"\t\t\""+v->GetName()+"\"";
            if(i<m_inGates.size()-1)
                a = a+ ",\n";
            i++;
        }
        a = a+ "\n"+shift+"\t],\n";
    }
    a = a+ shift+"\t\"outputs\":";
    if(m_outGates.size()==0){
        a = a + "\"\",\n";
    }else{
        a = a+ "\n"+shift+"\t[\n";
        int i=0;
        for (auto& v : m_outGates)
        {
            a = a+ shift+"\t\t\""+v->GetName()+"\"";
            if(i<m_inGates.size()-1)
                a = a+ ",\n";
            i++;
        }
        a = a+ shift+"\t]\n";
    }
    a = a+ shift+"]";
    return a;
}
/*
boost::property_tree::ptree Gate::GetJson()
{
	boost::property_tree::ptree pt;

    pt.add("id", m_name);
	pt.add("table", m_type->GetTruthTableName());
	pt.add("type", m_type->GetType());
	pt.add("probed", m_probed);

	boost::property_tree::ptree inputs, outputs;
	for (auto& [k,v] : m_inGates)
	{
		boost::property_tree::ptree pt;
		pt.put_value(v->GetName());
		inputs.push_back(std::make_pair("", pt));
	}
	pt.add_child("inputs", inputs);
	 
	for (auto& v : m_outGates)
	{
		boost::property_tree::ptree pt;
		pt.put_value(v->GetName());
		outputs.push_back(std::make_pair("", pt));
	}
	pt.add_child("outputs", outputs);
	return pt;
}
*/
int Gate::GetTransitionOutput() const
{
	std::vector<int> inputs;
	for (const auto& [k, v] : m_inGates)
		inputs.emplace_back(v->GetOutput());
	return m_type->GetOutput(inputs);
}

int Gate::GetTransitionTime(int time) const
{
	return time + m_type->GetDelay();
}

void Gate::UndoProbe()
{
	m_probed = false;
}
