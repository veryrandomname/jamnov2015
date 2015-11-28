typedef struct sa sa;
void randolution(sa** d, short p){
	for(byte i = 0; i < NPERCEPTIONS; ++i){
	  for(byte j = 0; j < NSTATES; ++j){
	    int r = rand()%100;
	    if(r <= p){
	      d[j][i].s = (char)rand()%NSTATES;
	    }
	    else{
	      d[j][i].a = (char)rand()%NACTIONS;
	    }
	  }	
	}
}
