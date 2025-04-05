#include "phone.hpp"

//Cost O(n); n = mida de name
phone::phone(nat num, const string& name, nat compt) throw(error){
	numtlf = num;
	this->name = name;
	this->compt = compt;
	
	for(int i = 0; i<name.size(); ++i){
		char c = name[i];
		if(c == '<' or c == '|' or c == '\0'){
			throw error(ErrNomIncorrecte);
		}
	}
	
	
}

//Cost O(1)
phone::phone(const phone& T) throw(error){
	numtlf = T.numtlf;
	name = T.name;
	compt = T.compt;
}

//Cost O(1)
phone& phone::operator=(const phone& T) throw(error){
	numtlf = T.numtlf;
	name = T.name;
	compt = T.compt;
	return *this;
}

//Cost O(1)
phone::~phone() throw(){}

//Cost O(1)
nat phone::numero() const throw(){
	return numtlf;
}

//Cost O(1)
string phone::nom() const throw(){
	return name;
}

//Cost O(1)
nat phone::frequencia() const throw(){
	return compt;
}

//Cost O(1)
phone& phone::operator++() throw(){
	++compt;
	return *this;
}

//Cost O(1)
phone phone::operator++(int) throw(){
	phone tlf(*this);
	compt++;
	return tlf;
}

//Cost O(1)
bool phone::operator==(const phone& T) const throw(){
	return (compt == T.compt and name == T.name);
}

//Cost O(1)
bool phone::operator!=(const phone& T) const throw(){
	if (compt == T.compt) return (name != T.name);
	return (compt != T.compt);
}

//Cost O(1)
bool phone::operator<(const phone& T) const throw(){
	if (compt == T.compt) return (name < T.name);
	return (compt < T.compt);
}

//Cost O(1)
bool phone::operator>(const phone& T) const throw(){
	if (compt == T.compt) return (name > T.name);
	return (compt > T.compt);
}

//Cost O(1)
bool phone::operator<=(const phone& T) const throw(){
    if (compt == T.compt) return (name <= T.name);
    return (compt <= T.compt);
}

//Cost O(1)
bool phone::operator>=(const phone& T) const throw(){
    if (compt == T.compt) return (name >= T.name);
    return (compt >= T.compt);
}

