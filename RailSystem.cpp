#pragma warning (disable:4786)
#pragma warning (disable:4503)

#include "RailSystem.h"

void RailSystem::reset(void) {
	map<string, City*>::iterator cityIter;
	for (cityIter = cities.begin(); cityIter != cities.end(); cityIter++)
	{
		cityIter->second->total_fee = INT_MAX;
		cityIter->second->total_distance = INT_MAX;
		cityIter->second->from_city = "";
		cityIter->second->visited = false;
	}
}

RailSystem::RailSystem(string const &filename) {
    
    load_services(filename);
}

void RailSystem::load_services(string const &filename) {

	ifstream inf(filename.c_str());
	string from, to;
	int fee, distance;

	while ( inf.good() ) {

		// Read in the from city, to city, the fee, and distance.
		inf >> from >> to >> fee >> distance;

		if ( inf.good() ) {
		
			City* newCity = new City(from);
			cities.insert(pair<string, City*>(from, newCity));

			map<string, list<Service*> >::iterator serviceIter = outgoing_services.find(from);
			if (serviceIter == outgoing_services.end())
			{
				list<Service*> newServiceList;
				newServiceList.push_back(new Service(to, fee, distance));
				outgoing_services.insert(map<string, list<Service*> >::value_type(from, newServiceList));
			}
			else
			{
				serviceIter->second.push_back(new Service(to, fee, distance));
			}
		}
	}

	inf.close();
}

RailSystem::~RailSystem(void) {
	map<string, City*>::iterator cityIter;
	map<string, list<Service*> >::iterator serviceIter;
	list<Service*>::iterator listIter;
	list<Service*> tempListService;

	for (serviceIter = outgoing_services.begin(); 
		serviceIter != outgoing_services.end(); serviceIter++)
	{
		tempListService = serviceIter->second;
		for (listIter = tempListService.begin();
			listIter != tempListService.end(); listIter++)
		{
			delete *listIter;
		}
		tempListService.clear();
	}
	outgoing_services.clear();

	for (cityIter = cities.begin();
		cityIter != cities.end(); cityIter++)
	{
		delete cityIter->second;
	}
	cities.clear();
}

void RailSystem::output_cheapest_route(const string& from,
                const string& to, ostream& out) {

	reset();
	pair<int, int> totals = calc_route(from, to);

	if (totals.first == INT_MAX) {
		out << "There is no route from " << from << " to " << to << "\n";
	} else {
		out << "The cheapest route from " << from << " to " << to << "\n";
		out << "costs " << totals.first << " euros and spans " << totals.second
			<< " kilometers\n";
        cout << recover_route(to) << "\n\n";
	}
}

bool RailSystem::is_valid_city(const string& name) {

	return cities.count(name) == 1;
}

pair<int, int> RailSystem::calc_route(string from, string to) {

	priority_queue<City*, vector<City*>, Cheapest> candidates;
	//priority_queue<T*,vector<T*>,Cheapest> q;
	//City* obj = new City();
  //q.insert(obj);

    // TODO: Implement Dijkstra's shortest path algorithm to
    // find the cheapest route between the cities
    this->reset();
		City* city_temp = cities[from];
		city_temp->total_distance = 0;
		city_temp->total_fee = 0;
		candidates.push(city_temp);

		int num_cities = cities.size();
		for (int i = 0; i < num_cities; i++)
		{
			city_temp = candidates.top();
			candidates.pop();
			city_temp->visited = true;
			if (city_temp->name == to)
			{
				break;
			}

			list<Service*> serviceList = outgoing_services[city_temp->name];
			list<Service*>::iterator serviceListIter;
			for (serviceListIter = serviceList.begin(); 
				serviceListIter != serviceList.end(); serviceListIter++)
			{
				string toCity = (*serviceListIter)->destination;
				if (cities[toCity]->visited)
				{
					continue;
				}
				if (city_temp->total_fee + (*serviceListIter)->fee < cities[toCity]->total_fee)
				{
					cities[toCity]->total_fee = city_temp->total_fee + (*serviceListIter)->fee;
					cities[toCity]->total_distance = city_temp->total_distance + (*serviceListIter)->distance;
					cities[toCity]->from_city = city_temp->name;
					candidates.push(cities[toCity]);
				}
			}
		}

	// Return the total fee and total distance.
	// Return (INT_MAX, INT_MAX) if not path is found.
	if (cities[to]->visited) {
		return pair<int,int>(cities[to]->total_fee, cities[to]->total_distance);
	} else {
		return pair<int,int>(INT_MAX, INT_MAX);
 	}

}

string RailSystem::recover_route(const string& city) {
	
	// TODO: walk backwards through the cities
	// container to recover the route we found
	string destination = city;
	string route = city;
	
	while(true)
	{
		route = cities[route]->from_city;
		if (route == "")
		{
			break;
		}
		destination = route + "to" + destination;
	}
	return destination;
}
