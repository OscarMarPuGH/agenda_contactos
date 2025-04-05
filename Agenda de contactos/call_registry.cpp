#include "call_registry.hpp"


//Metodes Publics

/* Construeix un call_registry buit. */
//Cost O(1)
call_registry::call_registry() throw(error){
    _arrel = nullptr;
    _size = 0;
}

/* Constructor per còpia, operador d'assignació i destructor. */
//Cost O(n); n = nombre de nodes
call_registry::call_registry(const call_registry& R) throw(error){
    _size = R._size;
    _arrel = copia(R._arrel);
}

//Cost O(n); n = nombre de nodes
call_registry& call_registry::operator=(const call_registry& R) throw(error){
    call_registry aux(R);
    node *n = _arrel;
    _arrel = aux._arrel;
    aux._arrel = n;
    nat s = _size;
    _size = aux._size;
    aux._size = s;
    return *this;
}

//Cost O(n); n = nombre de nodes
call_registry::~call_registry() throw(){
    esborra(_arrel);
}

/* Registra que s'ha realitzat una trucada al número donat, 
incrementant en 1 el comptador de trucades associat. Si el número no 
estava prèviament en el call_registry afegeix una nova entrada amb 
el número de telèfon donat, l'string buit com a nom i el comptador a 1. */
//Cost O(log n); n = nombre de nodes
void call_registry::registra_trucada(nat num) throw(error){
    node *n = cerca(_arrel, num);
    if (n == nullptr){
	++_size;
	phone p(num, "", 1);
	_arrel = insereix_avl(_arrel, p);
    }else {
    	++n->_p;
    }
}

/* Assigna el nom indicat al número donat.
Si el número no estava prèviament en el call_registry, s'afegeix
una nova entrada amb el número i nom donats, i el comptador 
de trucades a 0. 
Si el número existia prèviament, se li assigna el nom donat. */
//Cost O(log n); n = nombre de nodes
void call_registry::assigna_nom(nat num, const string& name) throw(error){
    node *n = cerca(_arrel, num);
    if (n == nullptr){
        ++_size;
        phone p(num, name, 0);
        _arrel = insereix_avl(_arrel, p);
    } else {
    	phone p(num, name, n->_p.frequencia());
    	n->_p = p;
    }
}

/* Elimina l'entrada corresponent al telèfon el número de la qual es dóna.
Es produeix un error si el número no estava present. */
//Cost O(log n); n = nombre de nodes
void call_registry::elimina(nat num) throw(error){ 
    if(cerca(_arrel, num) != nullptr){
        --_size;
        _arrel = elimina(_arrel, num);
    }else{
        throw error(ErrNumeroInexistent);
    }
}

/* Retorna cert si i només si el call_registry conté un 
telèfon amb el número donat. */
//Cost O(log n); n = nombre de nodes
bool call_registry::conte(nat num) const throw(){
    return (cerca(_arrel, num) != nullptr);
}

/* Retorna el nom associat al número de telèfon que s'indica.
Aquest nom pot ser l'string buit si el número de telèfon no
té un nom associat. Es produeix un error si el número no està en
el call_registry. */
//Cost O(log n); n = nombre de nodes
string call_registry::nom(nat num) const throw(error){
    node *n = cerca(_arrel, num);
    if (n == nullptr) { 
        throw error(ErrNumeroInexistent, "call_registry", "Numero inexistent.");
    }
    return n->_p.nom();
}

/* Retorna el comptador de trucades associat al número de telèfon 
indicat. Aquest número pot ser 0 si no s'ha efectuat cap trucada a
aquest número. Es produeix un error si el número no està en el 
call_registry. */
//Cost O(log n); n = nombre de nodes
nat call_registry::num_trucades(nat num) const throw(error){
    node *n = cerca(_arrel, num);
    if (n == nullptr) {
       throw error(ErrNumeroInexistent);
    }
    return n->_p.frequencia();
}

/* Retorna cert si i només si el call_registry està buit. */
//Cost O(1)
bool call_registry::es_buit() const throw(){
    return(_arrel==nullptr);
}

/* Retorna quants números de telèfon hi ha en el call_registry. */
//Cost O(1)
nat call_registry::num_entrades() const throw(){
    return _size;
}

/* Fa un bolcat de totes les entrades que tenen associat un
nom no nul sobre un vector de phone.
Comprova que tots els noms dels telèfons siguin diferents; 
es produeix un error en cas contrari. */
//Cost O(n(log n)); n = mida del vector v
void call_registry::dump(vector<phone>& V) const throw(error){
    dump(_arrel, V);
    mergeSort(V);
    if(V.size() != 0){
        for (int i = 0; i < V.size()-1; ++i) {
            if (V[i].nom() == V[i+1].nom()) {
                throw error(ErrNomRepetit);
            }
        }
    }
    
}

//Metodes Privats

//Cost O(1)
call_registry::node::node(const phone &p , node* esq , node* dret) : _p(p), f_esq(esq), f_dret(dret), _altura(1) {
}

//Pre: cert
//Post: Retorna 0 si n = nullptr, sino retorna la altura de n
//Cost O(1)
int call_registry::altura(node *n)
{
    if (n == nullptr) return 0;
    return n->_altura;
}

//Pre: cert
//Post: Retorna 0 si n = nullptr, sino retorna la resta de les altures del fill esquerre menys el fill dret
//Cost O(1)
int call_registry::factor_equilibri(node *n)
{
    if (n == nullptr) return 0;
    return altura(n->f_esq) - altura(n->f_dret);
}
//Pre: y != nullptr
//Post: El subarbre del node retornat 
//Cost O(1)
call_registry::node* call_registry::rotacio_dreta(node *y){
    node *x = y->f_esq;
    if (x != nullptr) {
    	node *T2 = x->f_dret;
    	
    	x->f_dret = y;
    	y->f_esq = T2;
    	
    	x->_altura = max(altura(x->f_esq ), altura(x->f_dret )) + 1;
    	y->_altura = max(altura(y->f_esq ), altura(y->f_dret )) + 1;
    	return x;
    }
    return y;
}

//Pre: x != nullptr
//Post: Retorna el node actualitzat
//Cost O(1)
call_registry::node* call_registry::rotacio_esquerra(node *x)
{
    node *y = x->f_dret ;
    if (y != nullptr) {
    	node *T2 = y->f_esq ;
    	
    	y->f_esq = x ;
    	x->f_dret = T2 ;
    	
    	y->_altura = max(altura(y->f_esq), altura(y->f_dret )) + 1;
    	x->_altura = max(altura(x->f_esq), altura(x->f_dret )) + 1;
    	return y;
    }
    return x;
}

//Pre: cert
//Post: insereix el nou telefon p, si ja hi era no fa res
//i equilibra l'arbre en conseqüencia
//Cost  O(log n); n = nombre de nodes
call_registry::node* call_registry::insereix_avl(node *n, const phone &p){
    if (n == nullptr){
        return new node(p);
    }else if (p.numero() < n->_p.numero()) {
	n->f_esq = insereix_avl(n->f_esq, p);
    } else if (p.numero() > n->_p.numero()) {
        n->f_dret = insereix_avl(n->f_dret, p);
    } else {
    	n->_p = p;
    	return n;
    }
    n->_altura = max(altura(n->f_esq), altura(n->f_dret)) + 1;
    int fe = factor_equilibri(n);
    if (fe > 1 and p.numero() < n->f_esq->_p.numero())
    	return rotacio_dreta(n);
    if (fe < -1 and p.numero() > n->f_dret->_p.numero())
    	return rotacio_esquerra (n);
    if (fe > 1 and p.numero() > n->f_esq->_p.numero()){
        n->f_esq = rotacio_esquerra (n->f_esq);
        return rotacio_dreta(n);
    }
    if (fe < -1 and p.numero() < n->f_dret->_p.numero()){
        n->f_dret = rotacio_dreta (n->f_dret);
        return rotacio_esquerra(n);
    }
    return n;
}

//Pre: cert
//Post: Elimina el node que conte num i equilibra l'arbre en consecuencia
//si no hi es, no fa res.
//Cost O(log n); n = nombre de nodes
call_registry::node* call_registry::elimina(node *n, nat num){
    
    if (n!= nullptr) {
        if(n->_p.numero() == num){
            if (n->f_esq != nullptr and n->f_dret != nullptr) {
            	node *successor = succ(n->f_dret);
            	n->_p = successor->_p;
            	n->f_dret = elimina(n->f_dret, n->_p.numero());           	
            } else if (n->f_esq == nullptr and n->f_dret != nullptr) {
            	node *pelim = n;
            	n = n->f_dret;
            	delete pelim;
            } else if (n->f_esq != nullptr and n->f_dret == nullptr) {
            	node *pelim = n;
            	n = n->f_esq;
            	delete pelim;
            } else {
            	node *pelim = n;
            	n = nullptr;
            	delete pelim;
            }
        } else if (n->_p.numero() > num) {
            n->f_esq = elimina(n->f_esq, num);
        } else {
            n->f_dret = elimina(n->f_dret, num);
        }
    }

    if (n != nullptr) {
        n->_altura = max(altura(n->f_esq), altura(n->f_dret)) + 1;
        int fe = factor_equilibri(n);
        if(fe > 1 and num < n->f_esq->_p.numero()){
            return rotacio_dreta(n);
        }    
        if(fe < -1 and num > n->f_dret->_p.numero()){
            
            return rotacio_esquerra(n);
        }   
        if(fe > 1 and num > n->f_esq->_p.numero()){
            n->f_esq = rotacio_esquerra (n ->f_esq );
            return rotacio_dreta (n);
        }
        if(fe < -1 and num < n->f_dret->_p.numero()){
            n->f_dret = rotacio_dreta (n->f_dret);
            return rotacio_esquerra(n);
        }
    }
    return n;
}

/* Pre: cert */
/* Post: si m és NULL, el resultat és NULL; sinó,
   el resultat apunta al primer node d'un arbre binari
   de nodes que són còpia de l'arbre apuntat per m */

//Cost O(n); n = nombre de nodes
call_registry::node* call_registry::copia(node* m) {
    node *n = nullptr;
    if(m != nullptr) {
        n = new node(m->_p);
        try{
            n->f_esq = copia(m->f_esq);
            n->f_dret = copia(m->f_dret);
        } catch(error) {
            esborra(n);
        }
    }
    return n;
}

/* Pre: cert */
/* Post: no fa res si m és NULL, sinó allibera
   espai dels nodes de l'arbre binari apuntat per m */
//Cost O(n); n = nombre de nodes
void call_registry::esborra(node* m) {
  if (m != nullptr) {
    esborra(m->f_esq);
    esborra(m->f_dret);
    delete m;
  }
}

//Pre: cert
//Post: Retorna el node on es troba num, si no ho troba retorna nullptr
//Cost O(log n); n = nombre de nodes
call_registry::node* call_registry::cerca(node *n, nat num){
    if (n != nullptr) {
        if (num == n->_p.numero()) {
            return n; 
        } else if (num < n->_p.numero()) {
            return cerca(n->f_esq, num);
        } else {
            return cerca(n->f_dret, num);
        }
    } else {
        return n;
    }
}

//Pre: cert
//Post: Fa un bolcat de totes les entrades que tenen associat un 
//nom no nul sobre un vector de phone
//Cost O(n); n = nombre de nodes
void call_registry::dump(node *n, vector<phone>& V) {
    if (n != nullptr) {
        if (n->_p.nom() != "") {
            V.push_back(n->_p);
        }
        dump(n->f_esq, V);
        dump(n->f_dret, V);
    }
}

//Pre: cert
//Post: Ordena lexicogràficament el vector de phones V
//Cost O(n(log n)); n = mida del vector v
void call_registry::mergeSort(vector<phone>& V) {
    if (V.size()>1) {
        int mid = V.size()/2;
        
        vector<phone> lh;
        vector<phone> rh;
        
	for(unsigned int i = 0; i < mid; ++i) {
		lh.push_back(V[i]);
	}
	for(unsigned int i = mid; i < V.size(); ++i) {
		rh.push_back(V[i]);
	}
        mergeSort(lh);
        mergeSort(rh);

        unsigned int i = 0;
        unsigned int j = 0;
        unsigned int k = 0;
        while (i < lh.size() && j < rh.size()) {
            if (lh[i].nom() < rh[j].nom()) {
                V[k]=lh[i];
                i++;
            } else {
                V[k] = rh[j];
                j++;
            }
            k++;
        }

        while (i<lh.size()) {
            V[k] = lh[i];
            i++;
            k++;
        }

        while (j<rh.size()) {
            V[k]=rh[j];
            j++;
            k++;
        }

    }
}

//Pre: cert
//Post: Retorna el successor de n
//Cost O(log n); n = nombre de nodes
call_registry::node* call_registry::succ(node *n) {
	if (n->f_esq != nullptr) {
		n = succ (n->f_esq);
	}
	return n;	
}

