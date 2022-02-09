
#include "reductio.h"

closure(firstprime,lastprime)
int firstprime,lastprime;

{

	/* Find the chains implied by current primes */

	pset implicata;
	int peso,closed,membri;
	int i,j,k;
	int startchain,endchain;

	implicata = set_new (ns);
	 
	/*
	printf("\n");
	printf("~~~~~~~~~\n");
	printf("*CLOSURE*\n");
	printf("~~~~~~~~~\n");
	*/
	/* weight[i] stores the cardinality of the chain of the i-th
	   prime */
	MYREALLOC (int,firstchain.weight, firstchain.weight_size, lastprime);
	firstchain.implied = sf_new (0, ns);
	firstchain.cover = sf_new (0, ns);
	for (i=firstprime; i<lastprime; i++) firstchain.weight[i] = 0;

	/* peso is the cumulative weight of all the chains */
	peso = -1; 
	for (i=0; i<firstprime; i++) peso += firstchain.weight[i];

	/* loop on the maximal compatibles */
	for (i=firstprime; i<lastprime; i++)
	{
	  /*
	  printf("COMPUTATION OF THE CHAIN n. = %d\n",i+1); 
	  */
	  /* compute the chain generated by the i-th prime */ 

	  /* put in front of the chain the generating prime */
	  /* 1) update  the weight of the i-th chain */
	  firstchain.weight[i]++;
	  /*printf("firstchain.weight[i] = %d\n", firstchain.weight[i]);*/
	  /* 2) update the cumulative weight */
	  peso++;
	  /*printf("peso = %d\n",peso);*/
	  /* 3) update the chain and its cover */
	  sf_insert_set (firstchain.implied, peso, GETSET (primes, i));
	  sf_insert_set (firstchain.cover, i, GETSET (primes, i));

	  closed = 0; /* closed is the number of the classes of the i-th chain
					 whose implications have been computed */
	  /* while the closed classes are fewer than the generating classes
		 linked in the i-th chain */
	  while (closed < firstchain.weight[i])
	  {
		closed++; /* closed is the relative address in the i-th chain of the class
					 whose implications are currently computed */

		/* a new class in the i-th chain is closed */
		/* loop on all the inputs */
		for (k=0; k<ni; k++)
		{
		  /* find the class implied by the current generating implicant class 
			 under the current input */

		  generate(i,closed,k,implicata,&firstchain);

		  /* if implicata has more  than one state and has not been already
			 included in the chain */

		  membri = set_ord (implicata); /* count the states in implicata */

		  if ( membri >1 && exist(i,implicata,&firstchain) != 1)
		  {
			/* add the implied class to the chain of the i-th maximal compatible */

			/* update the weight of the i-th chain */
			firstchain.weight[i]++;
			/* update the cumulative weight */
			peso++;
			/* update the chain and its cover */
			sf_insert_set (firstchain.implied, peso, implicata);
			assert (i < firstchain.cover->count);
			set_or (GETSET (firstchain.cover, i),
				GETSET (firstchain.cover, i), implicata);

		  }
		}

		startchain = 0;
		for (j=0; j<i; j++) startchain += firstchain.weight[j];
		endchain = startchain + firstchain.weight[i];
	  }

	  startchain = 0;
	  for (j=0; j<i; j++) startchain += firstchain.weight[j];
	  endchain = startchain + firstchain.weight[i];
	}

	set_free (implicata);
}
