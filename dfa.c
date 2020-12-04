#include <stdio.h>
#include <stdlib.h>

enum dfa_state {
  DFA_START = 0
};

typedef struct dfa dfa;

struct dfa_rule {
  int current_state;
  char input_char;
  int result_state;
};

typedef struct dfa_rule dfa_rule;

struct dfa_rules {
  dfa_rule *data;
  int size;
};

typedef struct dfa_rules dfa_rules;

struct dfa {
  int state;
  dfa_rules *rules;
  int (*transition_func)(dfa *pdfa, 
			 char input_char);	
  void (*do_func)(dfa *pdfa, 
		  void *pdata /* reserved */ );
};

int 
state_transition(dfa *pdfa,
		 char input_char)
{
  int i;

  for (i = 0; i < pdfa->rules->size; i++) {
    dfa_rule current_rule = pdfa->rules->data[i];
    if (pdfa->state == current_rule.current_state 
	&& input_char == current_rule.input_char) {
      pdfa->state = current_rule.result_state;
      pdfa->do_func(pdfa, NULL);
      return 0;
    }
  }
  /* not found transition rule */
  return -1;
}

/* dump function */
void print_func(dfa *pdfa, 
		void *pdata)
{
  switch(pdfa->state) {
  case 0:
    printf("the current state is started state: %d, do sth.\n", pdfa->state);
    break;
  default:
    printf("the current state is %d, do sth.\n", pdfa->state);
    break;
  }
}

int init_rules(dfa_rules *rules)
{  
  rules->data[0].current_state = 0; 
  rules->data[0].input_char = 'a'; 
  rules->data[0].result_state = 1;

  rules->data[1].current_state = 1;
  rules->data[1].input_char = 'b';
  rules->data[1].result_state = 1;

  rules->data[2].current_state = 1;
  rules->data[2].input_char = 'a';
  rules->data[2].result_state = 2;

  rules->data[3].current_state = 2;
  rules->data[3].input_char = 'b';
  rules->data[3].result_state = 1;
  
  rules->data[4].current_state = 2;
  rules->data[4].input_char = 'a';
  rules->data[4].result_state = 1;
}


int main(int argc, char *argv[])
{
  const int rules_num = 5;
  
  dfa *pdfa = (dfa *)malloc(sizeof(dfa));
  
  dfa_rules *rules = (dfa_rules *)malloc(sizeof(dfa_rules));
  rules->size = rules_num;
  rules->data = (dfa_rule *)malloc(sizeof(dfa_rule) * rules->size);

  const char *test_seqs[] = {
    "abba",
    "abbab",
    "abbaaaaa",
    "ac",
    "cb"
  };

  int i, j;


  /* prepare rules*/
  init_rules(rules);


  /* prepare dfa */
  pdfa->rules = rules;
  pdfa->transition_func = state_transition;
  pdfa->do_func = print_func;

  for (i = 0; i < sizeof(test_seqs) / sizeof(const char *); i++) {
    const char *pc = test_seqs[i];
    char cc;
    int rc;

    /* every time is loaded, important */
    pdfa->state = DFA_START;
    printf("start test sequences %d: %s\n", i, test_seqs[i]);

    while((cc = *pc++) != '\0') {
      rc = pdfa->transition_func(pdfa, cc);
      if (rc < 0) {
	fprintf(stderr, "The test of string %s is FAILED.\n", test_seqs[i]);      
	break;
      }
    }    
    if(rc == 0) {
      fprintf(stdout, "The test of string %s is SUCCESSFUL.\n", test_seqs[i]);
    }
  }

  free(rules->data);
  free(rules);
  free(pdfa);
  
  return 0;
}
