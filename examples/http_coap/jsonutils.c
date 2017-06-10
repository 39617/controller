#include "jsonutils.h"


void jsonparse_search(struct jsonparse_state *parserState, const char *str){
	int parseStatus;
	while((parseStatus = jsonparse_next(parserState)) != 0 || parseStatus != JSON_TYPE_ERROR){
		if( parseStatus == JSON_TYPE_PAIR_NAME )
			//retorna zero em caso de serem iguais
			if(jsonparse_strcmp_value(parserState, str) == 0){
				char temp[parserState->vlen + 1];
				printf("tamananho aqui %d\n", parserState->vlen);
				if(jsonparse_copy_value(parserState, &temp, parserState->vlen + 1) != 0){
					printf("foi encontrada a string procurada: %s \n", temp);
					parseStatus = jsonparse_next(parserState);
					printf("o valor do par e do tipo %c\n", parseStatus);
					if(parseStatus == JSON_TYPE_NUMBER)
						printf("numero! o valor e %d\n", jsonparse_get_value_as_int(parserState));
				}
				//return pos to zero
				parserState->pos = 0;
				break;
			}
	}
}


