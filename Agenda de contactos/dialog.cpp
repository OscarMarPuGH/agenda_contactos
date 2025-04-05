#include "dialog.hpp"

/* Retorna en el vector answers els resultats obtinguts al processar
  els successius caràcters de l'string input, i en numtelf retorna 
  el número de telèfon corresponent a l'últim nom obtingut o un 0 si
  no existeix aquest nom de telèfon. Si durant el processament de la
  seqüència de tecles representada en input es produís un error
  llavors a answers es registra el missatge d'error associat a
  l'excepció, numtelf és un 0 i finalitza el procés. */
  //Cost O(m(log n)); m = longitud de input; n = nombre de nodes de easy
void dialog::dialog(easy_dial& easy, const string& input, vector<string>& answers, nat& numtelf) throw() {
    bool trobat = false;
    string par = easy.inici();
    answers.push_back(par);
    if(par == ""){
        answers.push_back("No hi ha telefons amb el prefix en curs.");
        numtelf = 0;
    }
        
    try {
    
        for (unsigned int i = 0; i < input.size(); ++i) {
            char c = input[i];
            if (c == '<') {
                answers.push_back(easy.anterior());
            } else if (c == '|') {
                answers.push_back(easy.seguent('\0'));
            } else {
                par = easy.seguent(c);

                if (par.empty()) {
                    answers.push_back(" No hi ha telefons amb el prefix en curs.");
                    numtelf = 0;
                    easy.anterior();
                    trobat = true;
                    break;
                } else {
                    answers.push_back(par);
                }
            }
            
        }
        if (not trobat) numtelf = easy.num_telf(); 
    } catch (error e) {
        answers = { par, e.mensaje() };
        numtelf = 0;
    }
}
