#include "easy_dial.hpp"

/* Construeix un easy_dial a partir de la 
informació continguda en el call_registry donat. El
prefix en curs queda indefinit. */

//Cost  O(n*m*log(n*m))
//n = nombre de telefons, m = longitud d'un telefon
easy_dial::easy_dial(const call_registry& R) throw(error){
    vector<phone> v;
    R.dump(v);
    mergeSort(v);
	_freq = 0;
	_root = nullptr;

    for (int i = 0; i < v.size(); ++i) {
        _root = insereix(_root, 0, v[i].nom()+'\0', v[i]);
		if (_root->_c != '\0') _root->_c = '\0';
		_freq += v[i].frequencia();
    }
    _ind = true;
    _pref = "";
	_act = nullptr;
	_itant = 0;
	for (nat i = 0; i < 1000; ++i) {
		_ant[i] = nullptr;
	}
	_lm = 0.0;
	if(_freq != 0){
		for(int i = 0; i < v.size(); ++i){
			nat puls = pulsacions(v[i].nom(), _root, 0, 0, false);
			_lm += (v[i].frequencia() * 1.0) / _freq * puls;
		}
	}
}


/* Tres grans. Constructor per còpia, operador d'assignació i destructor. */
//Cost O(n); n = quantitat de nodes
easy_dial::easy_dial(const easy_dial& D) throw(error){
  	_root = copia(D._root, D._root->_p);
	_act = D._act;
	for (nat i = 0; i < 1000; ++i) {
		if (D._ant[i] != nullptr) _ant[i] = D._ant[i];
		else break;
	}
	_itant = D._itant;
	_pref = D._pref;
	_ind = D._ind;
	_freq = D._freq;
	_lm = D._lm;
	_ds = D._ds;
}

//Cost O(n); n = nombre de nodes
easy_dial& easy_dial::operator=(const easy_dial& D) throw(error){
	easy_dial aux(D);
	swap(aux._root, _root);
	swap(aux._act, _act);
	swap(aux._ant, _ant);
	swap(aux._itant, _itant);
	swap(aux._pref, _pref);
	swap(aux._ind, _ind);
	swap(aux._freq, _freq);
	swap(aux._lm, _lm);
	swap(aux._ds, _ds);
	
	return *this;
}

//Cost O(n); n = nombre de nodes
easy_dial::~easy_dial() throw(){
	esborra_nodes(_root);
}
	
/* Inicialitza el prefix en curs a buit. Retorna el nom de F(S, '');
si F (S, '') no existeix llavors retorna l'string buit. */
//Cost O(1)
string easy_dial::inici() throw(){
	_ind = false;
	_ds = false;
	_pref = "";
	_itant = 0;
	_act = _root;
	if(_root != nullptr){
		return _root->_p.nom();
	}else{
		return "";
	}
	
}

/* Retorna el nom de F(S, p') on p' és el prefix resultant d'afegir
el caràcter c al final del prefix en curs p i
fa que el nou prefix en curs sigui p'.
Si F(S, p) existeix però F(S, p') no existeix llavors retorna 
l'string buit. 
Si no existeix F(S, p) (i per tant tampoc pot existir F(S, p'))
llavors es produeix un error i el prefix en curs queda indefinit. 
Naturalment, es produeix un error si el prefix en curs inicial p 
fos indefinit. */

//Cost O(n); n = longitud de la cadena
string easy_dial::seguent(char c) throw(error){	
	if(not _ind and _root != nullptr and not _ds){
		if (_act != nullptr) {
			_pref += c;
			_ant[_itant] = _act;
			++_itant;
			_act = actualitzaSeguent(_root->_sg, _pref, 0);
			if(_act == nullptr) {
				_ds = true;
				return "";
			}
			return _act->_p.nom();
		} else {
			_ind = true;
			throw error(ErrNoExisteixTelefon);
		}
	}else{
		_ind = true;
		throw error(ErrPrefixIndef);
	}
}


/* Elimina l'últim caràcter del prefix en curs p = p' · a
(a és el caràcter eliminat). Retorna el nom F(S, p') 
i fa que el nou prefix en curs sigui p'. 
Es produeix un error si p fos buida i si es fa que el prefix en curs
quedi indefinit. Òbviament, també es produeix un error 
si p fos indefinit. */
//Cost O(1)
string easy_dial::anterior()throw(error){
	_ds = false;
	if (not _ind){
		if (_pref != "") {
			_pref.pop_back();
			--_itant;
			_act = _ant[_itant];
			return _act->_p.nom();
		} else {
			_ind = true;
			throw error(ErrNoHiHaAnterior);
		}
	} else {
		_ind = true;
		throw error(ErrPrefixIndef);
	}
}

/* Retorna el número de telèfon de F(S, p), sent p
el prefix en curs. Es produeix un error si p és indefinit o si
no existeix F(S, p). */
//Cost O(1)
nat easy_dial::num_telf() const throw(error){
	if (not _ind) {
		if (_act != nullptr) {
			return _act->_p.numero();
		}
		throw error(ErrNoExisteixTelefon);
	} else {
		throw error(ErrPrefixIndef);
	}
}


/* Retorna en el vector result tots els noms dels contactes de 
telèfon que comencen amb el prefix pref, en ordre lexicogràfic creixent. */
//Cost O(n); n = nombre de nodes
void easy_dial::comencen(const string& pref, vector<string>& result) const throw(error){
	extreu(_root, pref, result);
	mergeSortLexic(result);
}



/* Retorna el número mitjà de pulsacions necessàries para obtenir un
telèfon. Formalment, si X és el conjunt de noms emmagatzemats en
el easy_dial i t(s) és el número de pulsacions mínimes
necessàries (= número de crides a l'operació seguent) per
obtenir el telèfon el nom del qual és s. La funció retorna la suma
    Pr(s) · t(s)
per tots els telèfons s del conjunt X, sent Pr(s) la probabilitat de
telefonar a s. La probabilitat s'obté dividint la freqüència de s per
la suma de totes les freqüències. */
//Cost O(1)
double easy_dial::longitud_mitjana() const throw(){
    return _lm;
}

//**Metodes privats**

//Cost O(1)
easy_dial::node::node(const char &c, phone p, node* pf, node* sg) : _c(c), _p(p), _pf(pf), _sg(sg){}

//Pre: cert
//Post: Insereix l'string k en el diccionari. Si ja hi era, no fa res.
//Cost O(n); n = longitud de k
easy_dial::node* easy_dial::insereix(node *t, nat i, const string &k, const phone &p) {
	if (t == nullptr) {
		t = new node(k[i], p);
	} else {
		if (t->_c != k[i]) {
			t->_sg = insereix(t->_sg, i, k, p);
		} else {
			t->_pf = insereix(t->_pf, i + 1, k, p);
		}
	}
	return t;
}

/* Pre: cert */
/* Post: no fa res si m és NULL, sinó allibera
   espai dels nodes de l'arbre binari apuntat per m */
//Cost O(n); n = nombre de nodes
void easy_dial::esborra_nodes(node* t) {
	if (t != nullptr) {
		esborra_nodes(t->_pf);
		esborra_nodes(t->_sg);
		delete t;
	}
}

/* Pre: cert */
/* Post: si m és NULL, el resultat és NULL; sinó,
   el resultat apunta al primer node d'un arbre binari
   de nodes que són còpia de l'arbre apuntat per m */
//Cost O(n); n = longitud del nom
easy_dial::node* easy_dial::copia(node* m, const phone &p){
    node *n = nullptr;
    if(m != nullptr) {
        n = new node(m->_c, p);
        try{
            n->_pf = copia(m->_pf, m->_pf->_p);
            n->_sg = copia(m->_sg, m->_sg->_p);
        } catch(error) {
            esborra_nodes(n);
        }
    }
	
    return n;
}

/*Pre: cert
Post: retorna el node que correspon al prefix pref, si no existeix 
retorna nullptr*/
//Cost O(n); n = longitud de la cadena
easy_dial::node* easy_dial::actualitzaSeguent(node *n, string pref, int i){
	if (n != nullptr) {
		if (pref[i] == n->_c) {
			if (i < pref.size() - 1) n = actualitzaSeguent(n->_pf, pref, i + 1);
		} else {
			n = actualitzaSeguent(n->_sg, pref, i);
		}
	}
	return n;
}


/*Pre:cert
Post: retorna el node on es troba s, si no existeix, retorna nullptr*/
//Cost O(log n); n = nombre de nodes
easy_dial::node* easy_dial::cerca(const string &s, node *n, nat i){
    if (n != nullptr and i < s.size()) {
		if(n->_c == s[i]){
			n = cerca(s, n->_pf, i+1);  
        }else{
			n = cerca(s, n->_sg, i);
        }
	}else{
		if(n->_c != '\0'){
			n = cerca(s, n->_sg, i);
		}
	}
	return n;
}

// Pre = true
// Post = insereix a res totes les paraules que hi ha en el
// subarbre n.
//Cost O(n); n = nombre de nodes
void easy_dial::extreu(node *n, const string &par, vector<string> &res){
	if(n != nullptr){
		string aux;
		for(int i = 0; i < par.size(); ++i){
			aux += n->_p.nom()[i];
		}
		if(aux == par) {
			res.push_back(n->_p.nom());
		}
		extreu(n->_pf, par, res);
		extreu(n->_sg, par, res);
	}
}


/*Pre: s no és buit, puls = 0, sg = false, i = 0
Post: retorna el nombre de pulsacions necessaries per a cada nom
del call_registry*/
//Cost O(m); m = longitud de s
nat easy_dial::pulsacions(string s, node *n, nat i, nat puls, bool sg){
	if (s != n->_p.nom()) {
		if (s[i] != n->_c) {
			if (not sg) ++puls;
			puls = pulsacions(s, n->_sg, i, puls, true);
		} else {
			puls = pulsacions(s, n->_pf, i + 1, puls + 1, true);
		}
	}
	return puls;
}

//Pre: cert
//Post: Ordena el vector de phones V per frequencies, de forma descendent
//Cost O(n(log n)); n = nombre d'elements de v
void easy_dial::mergeSort(vector<phone>& V) {
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
            if (lh[i] > rh[j]) {
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
//Post: Ordena lexicogràficament el vector de phones V de forma ascendent
//Cost O(n(log n)); n = nombre d'elements de v
void easy_dial::mergeSortLexic(vector<string>& V) {
    if (V.size()>1) {
        int mid = V.size()/2;
        
        vector<string> lh;
        vector<string> rh;
        
	for(unsigned int i = 0; i < mid; ++i) {
		lh.push_back(V[i]);
	}
	for(unsigned int i = mid; i < V.size(); ++i) {
		rh.push_back(V[i]);
	}
        mergeSortLexic(lh);
        mergeSortLexic(rh);

        unsigned int i = 0;
        unsigned int j = 0;
        unsigned int k = 0;
        while (i < lh.size() && j < rh.size()) {
            if (lh[i] < rh[j]) {
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