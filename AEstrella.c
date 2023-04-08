// 1635101 FRANCESCO TEDESCO

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <values.h>


#define PI 3.14159265358979323846 
#define MAXARST 20

typedef struct {
	char carrer[12]; //cada element del vector es correspon a un caràcater del id
	unsigned numnode ; // fa referència a la posició al vector de nodes
} infoaresta ;


typedef struct {
	long int id; //identificació del node
	double latitud , longitud; 
	int narst; // numero d'arestes que surten del node
	infoaresta arestes[MAXARST]; //vector d'arestes del node
	int obert; // ho pots fer directament amb le cost infinit...
	unsigned pare_numnode; //
	double distancia; // distancia acumulada
	double heuristica; 
	double cost; 

} node;

/* Estructures per la cua oberta */
typedef struct element {
	unsigned num_node; 
	struct element *seguent; 
} elementcua ;

typedef struct {
	elementcua *inicial;
}Cua;    


/* Aquesta pila serà útil pel mostracamí */ 
typedef struct ElementPila {
unsigned int num_node;
struct ElementPila *anterior;
} PilaElem;


typedef PilaElem * Pila;

/* funcions per la cua oberta */
int treuelprimer ( Cua *cua );
void reencua_amb_prioritat(unsigned num_node, Cua *oberta, node * llista_nodes); 
int afegeix_amb_prioritat(unsigned num_node, Cua *oberta, node * llista_nodes); 

/* en el nostre cas, només necessitarem apilar elements en la pila */
int apila( unsigned int h, Pila *St); 
	
/* per saber l'índex del node donada la identificació del node */
int buscapunt ( long int ident , node llista_nodes[] , unsigned nnodes ); 

/* per calcular la distància en línia recta entre dos nodes */
double distancia ( node node0, node node1); 

/* per mostrar el camí òptim trobat */
void mostracami(unsigned index_node_actual, node *llista_nodes); 

int main(int argc, char *argv[]){
	long int origen, desti; 
	
	/* carreguem l'origen i del final */
	origen = atol(argv[1]);
	desti = atol(argv[2]);
	
	if (argc != 3) { 
		printf("No s'ha introduït el nombre correcte d'arguments"); 
		return 1; }
	
	
	FILE * dades; 
    FILE * dades_carrers; 
    node * llista_nodes; // apuntador al vector de nodes 
    int lin=0, ll, ll1; // per comptar les línies del fitxer 
    int i;  

	/* obrim el fitxer de Nodes */
    dades=fopen("Nodes.csv","r");
    if(dades==NULL){
        printf("\nNo s'ha accedit al fitxer de dades\n");
        return 1;
    }
    /* comptem les línies */
    while((ll=fgetc(dades)) != EOF){
        if(ll=='\n'){lin++;} 
    }
    
    /* malloc pel vector de nodes */
	if((llista_nodes = (node *) malloc(lin * sizeof(node))) == NULL){
        printf ("\nNo es possible assignar la memoria necessaria...\n\n");
        return 1;
    } 
    rewind(dades);
    
	/* variables per comprobar si els nodes introduïts estan a la llista de nodes */
	unsigned origen_trobat = 0; 
    unsigned desti_trobat = 0; 
        
    /* llegeixo la identificació del node, la latitud i longitud,   
     * també aprofito per inicialitzar els valors que calguin. */  
    for (i=0; i<lin; i++) {
		fscanf(dades, "%ld", &llista_nodes[i].id);
		fgetc(dades);
		fscanf(dades, "%lf", &llista_nodes[i].latitud);
		fgetc(dades);
		fscanf(dades, "%lf", &llista_nodes[i].longitud);
		llista_nodes[i].narst = 0; 
		llista_nodes[i].distancia = 0.0; 
		llista_nodes[i].cost = UINT_MAX; 
		llista_nodes[i].heuristica = 0.0; 
		if (llista_nodes[i].id == desti) {
			desti_trobat = 1; } 
		if (llista_nodes[i].id == origen) {
			origen_trobat = 1; }	
    }
    fclose(dades);
    
	/* parem el programa si els nodes introduïts no estan a la llista del nodes */
    if (origen_trobat == 0|| desti_trobat == 0){
		printf("Els nodes introduïts no formen part del mapa \n");
		return 1;  
	}
    
    /* obrim el ftixer de Carrers */
    dades_carrers=fopen("Carrers.csv","r");
    if(dades_carrers==NULL) {
        printf("\nNo s'ha accedit al fitxer de dades\n");
        return 1;
    }
    

	rewind(dades_carrers);

    char id_carr_actual[12]; 
    
    long int id_node_actual; 
    
    long int id_node_seguent; 
    
    int index_node_actual; 
    int index_node_anterior; 
    int index_node_seguent;
    
	/* llegim el fitxer de Carrers per afegir les arestes de cada node */
    while((ll1=fgetc(dades_carrers)) != EOF){
		/* llegim la identificaió el carrer actual i els dos primers nodes del carrrr */
		/* (Estem suposant que com a mínim cada carrer tindrà dos nodes) */
		fscanf(dades_carrers, "d=%[0-9]", id_carr_actual);
		fgetc(dades_carrers); 
		fscanf(dades_carrers, "%ld", &id_node_actual); 
		fgetc(dades_carrers); 
		fscanf(dades_carrers, "%ld", &id_node_seguent); 	
		
		/* busquem l'index d'aquests dos primers nodes */
		index_node_actual = buscapunt(id_node_actual, llista_nodes, lin); 
		index_node_seguent = buscapunt(id_node_seguent, llista_nodes, lin); 

		/* Afegim al vector del primer node, l'aresta (que serà el segon node llegit) */
		strcpy(llista_nodes[index_node_actual].arestes[llista_nodes[index_node_actual].narst].carrer,id_carr_actual);  
		llista_nodes[index_node_actual].arestes[llista_nodes[index_node_actual].narst].numnode = index_node_seguent; 
		/* cada cop que afegim un node, haurem d'augmentar el nombre d'arestes */
		llista_nodes[index_node_actual].narst ++; 
		
		
		/* per fer el bucle i/o posar l'aresta de l'altre extrem del carrer, l'índex del primer node llegit serà el node "anterior" 
		* i el node actual serà el segon node llegit */
		index_node_anterior = index_node_actual; 
		index_node_actual = index_node_seguent; 
		
		/* mentre que el pròxim caràcter no sigui el salt de línia */
         while(fgetc(dades_carrers) !='\n'){ 
			/* llegim i busquem l'índex del node següent */
			fscanf(dades_carrers, "%ld", &id_node_seguent); 
			index_node_seguent = buscapunt(id_node_seguent, llista_nodes, lin); 
			
			/* afegim a les arestes del node actual, l'anterior i el següent */
			strcpy(llista_nodes[index_node_actual].arestes[llista_nodes[index_node_actual].narst].carrer,id_carr_actual);  
			llista_nodes[index_node_actual].arestes[llista_nodes[index_node_actual].narst].numnode = index_node_seguent; 
			llista_nodes[index_node_actual].narst ++; 
			
			strcpy(llista_nodes[index_node_actual].arestes[llista_nodes[index_node_actual].narst].carrer,id_carr_actual);  
			llista_nodes[index_node_actual].arestes[llista_nodes[index_node_actual].narst].numnode = index_node_anterior; 
			llista_nodes[index_node_actual].narst ++; 
			
			/* per tant, per passar al següent iterat, el node actual serà l'anterior i el node seguent passarà a ser l'actual. */
			index_node_anterior = index_node_actual; 
			index_node_actual = index_node_seguent;

		}
		/* quan hi hagi el salt de línia, el node afegirem al node actual l'aresta node anterior */
		fscanf(dades_carrers, "%ld", &id_node_seguent); 
		strcpy(llista_nodes[index_node_actual].arestes[llista_nodes[index_node_actual].narst].carrer,id_carr_actual);  
		llista_nodes[index_node_actual].arestes[llista_nodes[index_node_actual].narst].numnode = index_node_anterior; 
		llista_nodes[index_node_actual].narst ++; 
	}
     
	/* inicialitzem la cua oberta */
	Cua oberta = {NULL}; 
	/* busquem l'índex dels nodes orígen i destí */
	int index_node_inicial = buscapunt(origen, llista_nodes, lin); 
	int index_node_desti = buscapunt(desti, llista_nodes, lin); 
	/* inicialitzem la distància acumulada */
	llista_nodes[index_node_inicial].distancia = 0.0;
	/* el cost del 1r node és només la heursítica, ja que la distància acumulada a l'origen és 0 */
	llista_nodes[index_node_inicial].cost = distancia(llista_nodes[index_node_inicial], llista_nodes[index_node_desti]);
	/* el pare del 1r node */
	llista_nodes[index_node_inicial].pare_numnode = UINT_MAX; 
	/* afegim aquest node a la cua oberta */
	afegeix_amb_prioritat(index_node_inicial, &oberta, llista_nodes); 

	/* variable auxiliar que servira per guardar el cost */
	double cost_aux; 
	
	while (oberta.inicial != NULL){
		/* treiem el 1r element de la cua */
		index_node_actual = treuelprimer(&oberta); 
		
		/* si el 1r node extret de la cua coincideix amb el final, imprimirem el camí òptim calculat */
		if (index_node_actual == index_node_desti){
			printf("# La distancia de %ld a %ld es de %.10f metres. \n", origen, desti, 
			llista_nodes[index_node_desti].distancia);
			printf("# Cami optim: \n");
			mostracami(index_node_actual, llista_nodes);   
			return 0; 
		}
		/* fem un recorregut per les arestes del node extret de la cua */		
		for (i = 0; i < llista_nodes[index_node_actual].narst; i++){
			/* si la heuristica del node adjacent és 0 vol dir que no l'hem calculat, per tant ho farem ... */
			if (llista_nodes[llista_nodes[index_node_actual].arestes[i].numnode].heuristica == 0) {
				llista_nodes[llista_nodes[index_node_actual].arestes[i].numnode].heuristica =   
				+ distancia(llista_nodes[llista_nodes[index_node_actual].arestes[i].numnode], llista_nodes[index_node_desti]) ;
			}

			/* calculem el cost = distància acumulada fins el node adjacent + la heurística */
			cost_aux = distancia(llista_nodes[index_node_actual], llista_nodes[llista_nodes[index_node_actual].arestes[i].numnode]) 
			+ llista_nodes[index_node_actual].distancia +  llista_nodes[llista_nodes[index_node_actual].arestes[i].numnode].heuristica; 

			/* si el cost trobat és més petit del que ja estava, actualitzem el node */
			if (cost_aux < llista_nodes[llista_nodes[index_node_actual].arestes[i].numnode].cost) {
				/* actualitzem la distància */
				llista_nodes[llista_nodes[index_node_actual].arestes[i].numnode].distancia = llista_nodes[index_node_actual].distancia +
				distancia(llista_nodes[index_node_actual], llista_nodes[llista_nodes[index_node_actual].arestes[i].numnode]); 
				/* actualitzem el cost */
				llista_nodes[llista_nodes[index_node_actual].arestes[i].numnode].cost = cost_aux; 
				/* actualitzem el pare del node */
				llista_nodes[llista_nodes[index_node_actual].arestes[i].numnode].pare_numnode = index_node_actual; 	
				
				/* si el node ja ha esta obert, el reencuarem amb prioritat, altrament l'afegirem amb prioritat */
				if (llista_nodes[llista_nodes[index_node_actual].arestes[i].numnode].obert == 1){					    
					reencua_amb_prioritat(llista_nodes[index_node_actual].arestes[i].numnode, &oberta, llista_nodes);
				} else {							
					afegeix_amb_prioritat(llista_nodes[index_node_actual].arestes[i].numnode, &oberta, llista_nodes); 
				}				
			}						
		}
	}
	printf("No s'ha pogut trobar un camí que uneixi aquests dos nodes \n"); 		
}






/* funció que serveix per trobar l'índex del node a partir de la seva identificació */
int buscapunt ( long int ident , node llista_nodes[] , unsigned nnodes ){
	/* com que les identificacions estan ordenades, podrem implementar el mètode de cerca binària */
	int sup=nnodes+1;
	int inf=0;
	int cont=0; /* nombre de divisions que anirem fent al vector */
	int meitat; 
	
	while (cont<((nnodes/2)+1)){
	   meitat=(inf+sup)/2;
		
		/* si la identifiació del node és igual a la que estem buscant retornem la "meitat", 
		 * que serà l'índex del node */ 
	   if ((llista_nodes[meitat].id)==ident){ return meitat; }
	  
	   /* si la identificació és més gran respecte la que estem buscant, 
	    * l'extrem superior serà la meitat actual */ 
	   if ((llista_nodes[meitat].id)>ident){ sup=meitat; }
	   
	   /* si la identificació és més petita respecte la que estem buscant, 
	    * l'extrem inferior serà la meitat actual */ 
	   if ((llista_nodes[meitat].id)<ident){ inf=meitat; }
	   
	 cont++;
	 }
	printf("El id no apereix a la llista\n");
	return -1; 
}	



/* funció per calcular la distància que hi ha entre dos nodes */
double distancia ( node node0, node node1) {
  double x0, x1, y0, y1, z0, z1; 
  double latitud0, latitud1, longitud0, longitud1; 
  double R = 6371.0; 
  double f_conv; 
  f_conv = PI/180.0; 
  
  /* aplico el factor de conversió a les latituds i longituds */
  latitud0 = node0.latitud*f_conv; 
  latitud1 = node1.latitud*f_conv;  
  longitud0 = node0.longitud*f_conv;  
  longitud1 = node1.longitud*f_conv;  
  
  /* passo a coordenades cartesianes les latituds i longituds */
  x0 = R*cos(longitud0)*cos(latitud0); 
  x1 = R*cos(longitud1)*cos(latitud1); 
  y0 = R*sin(longitud0)*cos(latitud0); 
  y1 = R*sin(longitud1)*cos(latitud1); 
  z0 = R*sin(latitud0); 
  z1 = R*sin(latitud1); 
	
	/* retorno el mòdul (distància), que hi ha entre els dos nodes */
	/* *1000 ja que volem la distància en metres */
  return sqrt(pow(x0 - x1, 2) + pow(y0 - y1, 2) + pow(z0 - z1, 2))*1000; 
  
}

/* funció per treure el primer element de la cua */
int treuelprimer ( Cua *cua ){
	/* declarem un element de la on guardarem el 1r element d'aquesta */
	elementcua *aux = cua->inicial; 
	/* per retornar el primer element, el guardem en una variable */
	int index_primer_node = aux->num_node; 
	/* modifiquem l'inici de la cua */
	cua->inicial=(cua->inicial)->seguent; 
	/* borrem de la memòria l'element auxiliar que ja no esn serveix */
	free(aux); 
	/* retornem l'índex del node que es troba en la 1a posició de la cua */
	return index_primer_node; 
}


/* NOTA: Les funcions reencua_amb_prioritat, afegeix_amb_prioritat estan pensades
 * a partir de la implementació que es troba en la transparència de la presentació 
 * Shortest paths algorithms in weighted graphs */ 
 

/* funció per reencuar un element */
void reencua_amb_prioritat(unsigned num_node, Cua *oberta, node * llista_nodes) {		
		/* apuntador a elementcua per recorrer la cua */
		elementcua * prepv;
		
		/*si l'element que volem reencuar es troba en la 1a posició, no caldrà fer cap modificació a la cua
		* perquè vol dir que aquest node serà el següent en sser extret de la cua i per tant tindrà el cost més baix */ 
		if((*oberta).inicial->num_node == num_node) return;
		
		/* bucle per recorrer la cua fins trobar l'element l'element que coincideixi amb el node que volem reencuar */
		for(prepv = (*oberta).inicial; prepv->seguent && prepv->seguent->num_node != num_node; prepv = prepv->seguent); 
		
		/* Per poder eliminar l'element de la cua que conté el node a reencuar, creem un apuntador element cua 
		 * que apuntarà a l'element que volem treue */ 
		elementcua * pv = prepv->seguent;
		
		/* ara el seguent de l'element que volem treure passarà a apuntar al "seguent del seguent" */
		prepv->seguent = pv->seguent;
		/* d'aquesta manera podrem treure l'element de la cua, sense trencar aqeusta... */
		free(pv);
		/* un cop eliminat l'element de la cua, el l'afegim amb prioritat cridant la següent funció. */
		afegeix_amb_prioritat(num_node, oberta, llista_nodes); 
}



/* funció per afegir un element a la cua amb prioritat */
int afegeix_amb_prioritat(unsigned num_node, Cua *oberta, node * llista_nodes){
	/* apuntador a element cua que anirà passat pels element de la cua */
	elementcua * q;
	
	/* apuntador a un element auxiliar, que serà el que voldrem afegir a la cua */
	elementcua *aux = (elementcua *) malloc(sizeof(elementcua));
	if(aux == NULL) return 0;
	
	/* carreguem l'element de la cua amb l'índex del node que volem afegir */
	aux->num_node = num_node;
	
	/* indiquem que el node esta en la cua oberta */
	llista_nodes[num_node].obert = 1;
	/*si la cua està buida o si el primer element ja té un cost major respecte el node que volem afegir; 
	 * afegirm el node a la cua
	 */ 
	if( (*oberta).inicial == NULL || !(llista_nodes[num_node].cost > llista_nodes[(*oberta).inicial->num_node].cost) ) {
		/* Per per tant l'element auxiliar haurà de ser el primer elemnet, per aquest mositu el seguent apuntarà a l'antic element de 
		 * la cua, i l'inci de la cua passara a apuntar aquest nou element auxiliar. */ 
		aux->seguent = (*oberta).inicial; (*oberta).inicial = aux;
		return 1;
	}
	/* si encara no s'ah pogut afegir l'element a la cua, farem un recorregut mentre que el cost de l'element de la cua 
	 * sigui més petit que el cost del node que volem afegir */    
	for(q = (*oberta).inicial; q->seguent && llista_nodes[q->seguent->num_node].cost < llista_nodes[num_node].cost; q = q->seguent );
		/* Quan sortim del bucle, "estarem" en l'element de la cua (q) que anirà just abans de l'element de voleme afegir (aux), per això 
		 * el seguent de l'element que volem afegir serà el que abans era el seguent de (q), i per tant el seguent de q, serà (aux). */ 	
	aux->seguent = q->seguent; q->seguent = aux;	 
	return 1;
}


/* funció per posar un element a la pila */
int apila( unsigned int h, Pila *Pl){
	/* apuntador a un element pila que voldrem afegir */
	PilaElem *aux = (PilaElem *) malloc(sizeof(PilaElem));
	if(aux == NULL) return 0;
	/* carreguem el valor a aquest element pila */ 
	aux->num_node = h;
	/* fem que l'anterior de l'element que volem afegir, apunti a l'apuntador de la Pila
	 * i que aquest passi a apuntar a l'auxiliar que hem creat */
	aux->anterior = *Pl;
	*Pl = aux;
	return 1;
}
	
/* funció per mostar el camí òptim trobat */	
void mostracami(unsigned index_node_actual, node *llista_nodes){
	
	Pila cami = NULL; /* inicialitzem la cua */ 
 
	/* bucle fins que no hi hagi pare del node */
	while (llista_nodes[index_node_actual].pare_numnode != UINT_MAX) {
		/* anirem apilant els nodes a la pila */ 
		apila(index_node_actual, &cami); 
		index_node_actual = llista_nodes[index_node_actual].pare_numnode; 
	}		
		
	/* apilem l'origen */
	apila(index_node_actual, &cami); 
	
	/* imprimim l'origen */
	printf("Id=%10.ld | %.6f | %.6f | Dist=%.6f \n", llista_nodes[cami->num_node].id, 
	llista_nodes[cami->num_node].latitud, llista_nodes[cami->num_node].longitud, 
	llista_nodes[cami->num_node].distancia); 
	
	/* mentre que l'element de la pila no sigui nul, anirem recorrent la pila per 
	 *imprimir els nodes */  
	while (cami->anterior != NULL) {
		cami = cami->anterior; 
		printf("Id=%10.ld | %.6f | %.6f | Dist=%.6f \n", llista_nodes[cami->num_node].id, 
		llista_nodes[cami->num_node].latitud, llista_nodes[cami->num_node].longitud, 
		llista_nodes[cami->num_node].distancia); 
	}	
}






















